# -*- coding: utf-8 -*-
import sys
from PyQt4.QtGui import QWizard, QWizardPage, QApplication, QLabel, QVBoxLayout,QDialog,QWidget
#from PyQt4.QtCore import 
from databases.inputtrees.ppitree import PPISelector
from databases.inputtrees.gotree import GOSelector
from databases.inputtrees.geotree import GEOSelector
from databases.inputtrees.assoctree import AssociationSelector
from databases.inputtrees.selectors import *
from utils.info import ap, rp
from utils.compression import untar
import shutil
import os


class InputWizard(QWizard):
    def __init__(self, parent=None):
	QWizard.__init__(self, parent)
	self.setWindowTitle("Execution Wizard")
	#os.chdir("src/python/robinviz/databases/inputtrees")

        self.PreSelectionPage = PreSelectionPage()
        self.ConfirmationSelectionPage = ConfirmationSelectionPage()        
        self.ColorSelectionPage = ColorSelectionPage()
        self.EdgeWeightSelectionPage = EdgeWeightSelectionPage()
        self.NodeWeightSelectionPage = NodeWeightSelectionPage()
	self.PPISelectionPage = PPISelectionPage()
	self.AssociationSelectionPage = AssociationSelectionPage()
	self.GOSelectionPage = GOSelectionPage()
	self.GEOSelectionPage = GEOSelectionPage()
        self.BiclusteringSelectionPage = BiclusteringSelectionPage()

        self.addPage(self.PreSelectionPage)
        self.addPage(self.PPISelectionPage )
        
        self.addPage(self.ConfirmationSelectionPage )
        self.addPage(self.ColorSelectionPage )

        # Co-Ontology
        self.addPage(self.GOSelectionPage )
        self.addPage(self.AssociationSelectionPage )


        #self.addPage(self.EdgeWeightSelectionPage)
        # Co-Expression
	self.addPage(self.GEOSelectionPage )
        self.addPage(self.BiclusteringSelectionPage )
        self.addPage(self.NodeWeightSelectionPage)
        #self.setStartId(0)
        #print self.ConfirmationSelectionPage.wizard()
	
	#self.setWindowModality(QWidget.modal)
	self.setModal(True)
    def showHelp(self):
	pass

class PreSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Start Page")
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)


        self.last= QRadioButton("Use the last settings")
        self.manual = QRadioButton("Define your manual settings")
        self.preconfigured = QRadioButton("Use preconfigured settings")

        self.settingsbox = QComboBox()
        self.conf_layout = QHBoxLayout()
        self.conf_layout.addWidget(self.preconfigured)
        self.conf_layout.addWidget(self.settingsbox)

        self.last.toggled.connect(self.emitChange)
        self.preconfigured.toggled.connect(self.emitChange)
        self.manual.toggled.connect(self.emitChange)
        
        self.manual.setChecked(True)

        self.layout.addLayout(self.conf_layout)
        self.layout.addWidget(self.last)
        self.layout.addWidget(self.manual)

        self.loadConfigurations()

    def loadConfigurations(self):
        directory = rp("sources/preconfigurations")
        files = filter( lambda name: name.endswith(".tar.gz"), os.listdir(directory) )
        names = map ( lambda filename : filename[:-7] , files)
        self.settingsbox.addItems(sorted(names))
        
    def emitChange(self):
        if self.manual.isChecked():
            self.setFinalPage(False)
        else:
            self.setFinalPage(True)
        
    def validatePage(self):
        if self.preconfigured.isChecked():
            selected = self.settingsbox.currentText() + ".tar.gz"
            source_file = rp("sources/preconfigurations/" + selected)
            target_file = rp(selected)
            shutil.copy( source_file, target_file)
            untar( target_file )
            os.remove( target_file )
            
        return True

    def nextId(self):        
        if self.manual.isChecked():
            return 1
        else:
            return -1


class ConfirmationSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Verification Concept")
	topLabel = QLabel("In this page, you need to select the confirmation type.")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = ConfirmationSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)

    def validatePage(self):
	self.confirmation = selection = self.selector.getSelection()
        if selection == "Co-Expression":
            print "Co-Expression selected"
            f = open(ap("godata/selected_terms.txt"), "w")
            f.close()
            f = open("outputs/resultparams.txt","w")
            f.write("co_expression.exe")
            f.close()
        else:
            print "Co-Ontology selected"
            f = open("outputs/resultparams.txt","w")
            f.write("co_ontologies.exe")
            f.close()

        # self.wizard().EdgeWeightSelectionPage.setupGUI()
        if selection == "Co-Expression":
            self.wizard().NodeWeightSelectionPage.setupGUI()
	return True

class ColorSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Color Assignment")
	topLabel = QLabel("In this page, you need to select the way nodes are colored. \n" +
        "Nodes will have colors according to the category type you select.")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = ColorSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)

    def nextId(self):
        n = QWizardPage.nextId(self)
        confirmation = self.wizard().ConfirmationSelectionPage.confirmation
        if confirmation == "Co-Ontology":
            return n # Move to GO Tree
        else:  # Co-Expression
            return n+1 # Move to Association part
        
    def validatePage(self):
	self.flag = flag = self.selector.getSelection()
        filename = rp("outputs/resultparams.txt")
        exe = open(filename, "r").read().strip().split(" ")[0]

        f = open(filename, "w")
        f.write("%s %s" % (exe, flag ))
        f.close()
	return True


class EdgeWeightSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)

    def initializePage(self):
        self.selector.initialize()
        
    def setupGUI(self):
	self.setTitle("Edge Weights / Ratio")
	layout = QVBoxLayout()
        self.setLayout(layout)
        confirmation = self.wizard().ConfirmationSelectionPage.confirmation
        if confirmation == "Co-Expression":
            confirmation = "CoExpression"
        elif confirmation == "Co-Ontology":
            confirmation = "CoFunctionality"

        self.confirmation = confirmation
	self.selector = EdgeWeightSelector(confirmation)
	layout.addWidget(self.selector)

    def nextId(self):
        n = QWizardPage.nextId(self)
        if self.confirmation == "CoFunctionality":
            return n + 1
        else:
            return n
    def validatePage(self):
        self.selector.saveSettings()
	return True

class NodeWeightSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)

    def initializePage(self):
        self.selector.initialize()
        
    def setupGUI(self):
	self.setTitle("Central Node Weights")
	layout = QVBoxLayout()
        self.setLayout(layout)
        
        # ==== SET CONFIRMATION ====
        self.confirmation = confirmation = self.wizard().ConfirmationSelectionPage.confirmation
        if confirmation == "Co-Expression":
            confirmation = "CoExpression"
        elif confirmation == "Co-Ontology":
            confirmation = "CoFunctionality"

        # ==== CREATE WIDGET =======
	self.selector = NodeWeightSelector(confirmation)

	layout.addWidget(self.selector)

    def validatePage(self):
        self.selector.saveSettings()
        return True

class AssociationSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("GO Association Sources")
	topLabel = QLabel("In this page, you need to select the association sources you'd like to use for Gene-Category mapping")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = AssociationSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)

    def nextId(self):
        n = QWizardPage.nextId(self)
        if self.wizard().ConfirmationSelectionPage.confirmation == "Co-Expression":
            return n
        else:
            return -1

    def validatePage(self):
	self.selector.mergeSelectedAssociations()
	return True
	
class GOSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Central Nodes from GO Categories")
	topLabel = QLabel("In this page, you need to select the GO terms you would like to use.")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = GOSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)
    
    def validatePage(self):
	self.selector.getCheckedItems()
	# TODO: Do this after last page
	#os.chdir("../../../../..")
	return True
	
class PPISelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("PPI Network")
	topLabel = QLabel("In this page, you need to select the PPI sources you'd like to visualize")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = PPISelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)
    
    def validatePage(self):
	self.selector.getCheckedItems()
	return True
	
class GEOSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("GEO Expression Matrix")
	topLabel = QLabel("In this page, you need to select the Gene Expression source you'd like to use for biclustering")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = GEOSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)

    def initializePage(self):
        self.selector.initialize()

    def validatePage(self):
	self.selector.downloadCheckedGEOs()
        self.selector.saveSettings()
	return True

class BiclusteringSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Biclustering Algorithm")
	topLabel = QLabel("In this page, you need to select the biclustering algoritm and define parameters to be applied on Gene Expression source you selected.")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = BiclusteringSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)

    def initializePage(self):
        self.selector.initialize()
        
    def validatePage(self):
	self.selector.saveSettings()
	return True

def runWizard():
    app = QApplication(sys.argv)
    mainWindow = InputWizard()
    mainWindow.showMaximized()
    sys.exit(app.exec_())

if __name__ == "__main__":
    runWizard()