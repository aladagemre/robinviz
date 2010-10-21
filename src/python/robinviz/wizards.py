# -*- coding: utf-8 -*-
import sys
from PyQt4.QtGui import QWizard, QWizardPage, QApplication, QLabel, QVBoxLayout,QDialog,QWidget
#from PyQt4.QtCore import 
from databases.inputtrees.ppitree import PPISelector
from databases.inputtrees.gotree import GOSelector
from databases.inputtrees.geotree import GEOSelector
from databases.inputtrees.assoctree import AssociationSelector
import os


class InputWizard(QWizard):
    def __init__(self, parent=None):
	QWizard.__init__(self, parent)
	
	#os.chdir("src/python/robinviz/databases/inputtrees")

	self.PPISelectionPage = PPISelectionPage()
	self.AssociationSelectionPage = AssociationSelectionPage()
	self.GOSelectionPage = GOSelectionPage()
	self.GEOSelectionPage = GEOSelectionPage()
	
	self.setPage(0, self.PPISelectionPage )
	self.setPage(1, self.GOSelectionPage )
        self.setPage(2, self.AssociationSelectionPage )
	self.setPage(3, self.GEOSelectionPage )
	
	self.setStartId(0)
	#self.setWindowModality(QWidget.modal)
	self.setModal(True)
    def showHelp(self):
	pass

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