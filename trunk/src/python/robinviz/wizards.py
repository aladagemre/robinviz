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
import os


class InputWizard(QWizard):
    def __init__(self, parent=None):
	QWizard.__init__(self, parent)
	
	#os.chdir("src/python/robinviz/databases/inputtrees")

        self.ConfirmationSelectionPage = ConfirmationSelectionPage()
        self.ColorSelectionPage = ColorSelectionPage()
        self.EdgeWeightSelectorPage = EdgeWeightSelectorPage()
        self.NodeWeightSelectorPage = NodeWeightSelectorPage()
	self.PPISelectionPage = PPISelectionPage()
	self.AssociationSelectionPage = AssociationSelectionPage()
	self.GOSelectionPage = GOSelectionPage()
	self.GEOSelectionPage = GEOSelectionPage()

        self.setPage(0, self.ConfirmationSelectionPage )
        self.setPage(1, self.ColorSelectionPage )
        self.setPage(2, self.EdgeWeightSelectorPage)
        self.setPage(3, self.NodeWeightSelectorPage)
	self.setPage(4, self.PPISelectionPage )
	self.setPage(5, self.GOSelectionPage )
        self.setPage(6, self.AssociationSelectionPage )
	self.setPage(7, self.GEOSelectionPage )
	
	self.setStartId(0)
	#self.setWindowModality(QWidget.modal)
	self.setModal(True)
    def showHelp(self):
	pass

class ConfirmationSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Confirmation type selection")
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
            
	return True

class ColorSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Color selection")
	topLabel = QLabel("In this page, you need to select the way nodes are colored. \n" +
        "Nodes will have colors according to the category type you select.")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = ColorSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)

    def validatePage(self):
	self.flag = flag = self.selector.getSelection()
        filename = rp("outputs/resultparams.txt")
        exe = open(filename, "r").read().strip().split(" ")[0]

        f = open(filename, "w")
        f.write("%s %s" % (exe, flag ))
        f.close()
	return True


class EdgeWeightSelectorPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Edge Weights / Ratio")
	layout = QVBoxLayout()
        self.setLayout(layout)
        
	self.selector = EdgeWeightSelector()
	layout.addWidget(self.selector)

    def validatePage(self):
	return True

class NodeWeightSelectorPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Node Weights calculation method")
	layout = QVBoxLayout()
        self.setLayout(layout)

	self.selector = NodeWeightSelector()
	layout.addWidget(self.selector)

    def validatePage(self):
	return True

class AssociationSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Gene Ontology Association source selection")
	topLabel = QLabel("In this page, you need to select the association sources you'd like to use for Gene-Category mapping")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = AssociationSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)
    
    def validatePage(self):
	self.selector.mergeSelectedAssociations()
	return True
	
class GOSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
	self.setTitle("Gene Ontology data selection")
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
	self.setTitle("PPI data selection")
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
	self.setTitle("GEO data selection")
	topLabel = QLabel("In this page, you need to select the Gene Expression sources you'd like to use for biclustering")
	topLabel.setWordWrap(True)
	layout = QVBoxLayout()
	self.selector = GEOSelector()
	layout.addWidget(self.selector)
	self.setLayout(layout)
    
    def validatePage(self):
	self.selector.downloadCheckedGEOs()
	return True
	
def runWizard():
    app = QApplication(sys.argv)
    mainWindow = InputWizard()
    mainWindow.showMaximized()
    sys.exit(app.exec_())

if __name__ == "__main__":
    runWizard()