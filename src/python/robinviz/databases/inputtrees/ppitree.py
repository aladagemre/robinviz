# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap
from utils.compression import *


class PPISelector(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        self.assureOspreyDirExists()
        
        dir_prefix = "BIOGRID-OSPREY_DATASETS"
        #print os.listdir("ppidata")
        biogrid_dirname = filter(lambda filename: filename.startswith(dir_prefix), os.listdir("ppidata") )[0]
        
        self.biogridVersion = biogrid_dirname[len(dir_prefix)+1:-7]
        self.osprey_dir = ap("ppidata/%s-%s.osprey" % (dir_prefix, self.biogridVersion))
        self.setupGUI()
    def assureOspreyDirExists(self):
	dir_prefix = "BIOGRID-OSPREY_DATASETS"
        dirs = filter(lambda filename: filename.startswith(dir_prefix), os.listdir("ppidata") )
        if len(dirs) == 0:
	    if not os.path.exists(ap("ppidata/BIOGRID-OSPREY_DATASETS-3.0.68.osprey")):
		download_file("http://thebiogrid.org/downloads/archives/Release%20Archive/BIOGRID-3.0.68/BIOGRID-OSPREY_DATASETS-3.0.68.osprey.zip")
	    unzip_file_into_dir("BIOGRID-OSPREY_DATASETS-3.0.68.osprey.zip", ap("ppidata/BIOGRID-OSPREY_DATASETS-3.0.68.osprey"))
	    
	    os.remove("BIOGRID-OSPREY_DATASETS-3.0.68.osprey.zip")
	
    def setupGUI(self):	
	layout = QVBoxLayout()
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(1)
	treeWidget.setHeaderLabels(("PPI",))
	
	#button = QPushButton("Report Selected PPIs")
	#button.clicked.connect(self.getCheckedItems)
	
        layout.addWidget(treeWidget)
        #layout.addWidget(button)
	
	self.setLayout(layout)

        #self.widget = QWidget()
        #self.widget.setLayout(layout)
        

        #self.setCentralWidget(self.widget)
        #self.setWindowTitle("PPI Selection Tree")
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
													
		checkedItems.add(ap(filename))
		
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    for c in range( topLevelItem.childCount() ):
		child = topLevelItem.child(c)
		traverse(child)
	
	f = open(ap("ppidata/selected_ppis.txt"), "w")
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
	    
def main():
    app = QApplication(sys.argv)
    mainWindow = PPISelector()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()