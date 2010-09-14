# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os

class PPISelector(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.biogridVersion = "3.0.67"
        self.osprey_dir = "BIOGRID-OSPREY_DATASETS-%s.osprey" % self.biogridVersion
        self.setupGUI()
        
    def setupGUI(self):	
	layout = QVBoxLayout()
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(1)
	treeWidget.setHeaderLabels(("PPI",))
	
	button = QPushButton("Report Selected PPIs")
	button.clicked.connect(self.getCheckedItems)
	
        layout.addWidget(treeWidget)
        layout.addWidget(button)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("PPI Selection Tree")
        self.setMinimumSize(400,600)
        
        self.readPPIData()
	self.useDictionary(self.organism_experiments)
        
    def readPPIData(self):
	
	organisms = filter(lambda filename: filename[0]!=".", os.listdir(self.osprey_dir))
	self.organism_experiments = {} # organism_name : experiments...

	for organism in organisms:
	    experiment_files = filter(lambda filename: filename[0]!=".", os.listdir(self.osprey_dir + "/" + organism))
	    organism_txt_length = len(organism)
	    experiment_names = sorted(map(lambda fn:fn[15:-19-organism_txt_length], experiment_files))
	    
	    self.organism_experiments[organism] = experiment_names
	    
    def getCheckedItems(self):
	checkedItems = set()
	
	def traverse(item):
	    # Add the term into checked items list if it's checked.
	    if item.checkState(0) == Qt.Checked:
		filename = "%(osprey_dir)s/%(organism)s/BIOGRID-OSPREY-%(experiment)s-%(organism)s-%(version)s.osprey.txt" % \
		{ "osprey_dir": self.osprey_dir,
		  "organism": str(item.parent().text(0)),
		  "experiment": str(item.text(0)),
		  "version": self.biogridVersion,
		}
													
		checkedItems.add(filename)
		
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    for c in range( topLevelItem.childCount() ):
		child = topLevelItem.child(c)
		traverse(child)
	
	f = open("selected_ppis.txt", "w")
	f.write("\n".join( sorted(checkedItems) ) )
	f.close()
	
    def useDictionary(self, dictionary):
	topLevelItems = sorted(dictionary.keys())
	for topLevelItem in topLevelItems:
	    parentItem = QTreeWidgetItem(self.treeWidget, (topLevelItem,))
	    for child in dictionary[topLevelItem]:
		childItem = QTreeWidgetItem(parentItem, (child,))
		childItem.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsTristate)
		childItem.setCheckState(0, Qt.Unchecked)
		
		parentItem.addChild(childItem)
		
	    
	    parentItem.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsTristate)
	    parentItem.setCheckState(0, Qt.Unchecked)
	    
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = PPISelector()
    mainWindow.show()
    sys.exit(app.exec_())
