# -*- coding: utf-8 -*-
import sys
from PyQt4.QtGui import QWizard, QWizardPage, QApplication, QLabel, QVBoxLayout,QDialog,QWidget
#from PyQt4.QtCore import 
from databases.inputtrees.ppitree import PPISelector
from databases.inputtrees.gotree import GOSelector
import os


class InputWizard(QWizard):
    def __init__(self, parent=None):
	QWizard.__init__(self, parent)
	
	os.chdir("src/python/robinviz/databases/inputtrees")

	self.PPISelectionPage = PPISelectionPage()
	self.GOSelectionPage = GOSelectionPage()
	
	self.setPage(0, self.PPISelectionPage )
	self.setPage(1, self.GOSelectionPage )
	
	self.setStartId(0)
	#self.setWindowModality(QWidget.modal)
	self.setModal(True)
    def showHelp(self):
	pass
    
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
	os.chdir("../../../../..")
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

	
def runWizard():
    app = QApplication(sys.argv)
    mainWindow = InputWizard()
    #mainWindow = QWizard()
    #mainWindow.addPage(PPISelectionPage())
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    runWizard()