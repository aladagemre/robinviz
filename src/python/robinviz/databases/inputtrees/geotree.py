# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import urllib
import os

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap

class GEOSelector(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        self.filename = ap("geodata/geoTreeView.txt")
        self.checkIndex()
        self.setupGUI()
        
    def checkIndex(self):
	if os.path.exists(self.filename):
	    return
	url = "http://robinviz.googlecode.com/svn/data/expressions/geoTreeView.txt"
	txt = urllib.urlopen(url).read()
	with open(self.filename, "w") as f:
	    f.write(txt)
		
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(3)
	treeWidget.setHeaderLabels(("Accession", "Series Platform ID","Description",))
	
	self.parseFile()
	layout.addWidget(treeWidget)	
	#button = QPushButton("Report Selected")
	#button.clicked.connect(self.getCheckedItems)        
        #layout.addWidget(button)

        self.setLayout(layout)
        self.setWindowTitle("GEO Tree")
        
    def getCheckedItems(self):
	checkedItems = set()
	
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    for c in range(topLevelItem.childCount()):
		item = topLevelItem.child(c)
		if item.checkState(0) == Qt.Checked:
		    checkedItems.add(str(item.filename))
	    
	
	f = open(ap("geodata/selected_geo.txt"), "w")
	f.write("\n".join( sorted(checkedItems) ) )
	f.close()
        
    def parseFile(self):
	lastNodeAtLevel = {}
	
	def tcount(text):
	    i = 0
	    while text[i] == '\t':
		i+=1
	    return i
	    
	
	for line in open(self.filename):
	    # If empty line, skip it.
	    if len(line) <= 2:
		continue
	    
	    # Count the number of preceding tabs to define depth.
	    numt = tcount(line)
	    if numt == 0: # If top level,
		cols = line.strip().split("\t")
		data = ("%s (%s)" % tuple(cols) ,) # Homo Sapiens (9606)
		item = QTreeWidgetItem(self.treeWidget, data )
		self.treeWidget.insertTopLevelItem(0, item)
		lastNodeAtLevel[0] = item # save the last root
	    else: # If inner level
		parent = lastNodeAtLevel[numt-1] # find who the parent is.
		cols = line.strip().split("\t") # split into columns
		data = tuple(cols[1:]) # leave the first column (filename)
		
		item = QTreeWidgetItem(parent, data )
		item.filename = cols[0] # assign filename to the item object secretly.
		
		parent.addChild( item )
		lastNodeAtLevel[numt] = item	# assign itself as the last root at its level
 
	    item.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsTristate)
	    item.setCheckState(0, Qt.Unchecked)
	    
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = GEOSelector()
    mainWindow.showMaximized()
    sys.exit(app.exec_())
