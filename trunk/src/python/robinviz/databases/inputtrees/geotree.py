# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import urllib
import os

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap
from utils.compression import download_file_to, ungz

class GEOSelector(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        self.filename = ap("geodata/geoTreeView.txt")
        self.checkIndex()
        self.setupGUI()
        
    def checkIndex(self):
	if os.path.exists(self.filename):
	    return
	url = "http://robinviz.googlecode.com/svn/data2/expressions/geoTreeView.txt"
	download_file_to(url, self.filename)
		
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(3)
	treeWidget.setHeaderLabels(("Accession", "Series Platform ID","Description",))
	treeWidget.setSortingEnabled(True)
	
	self.parseFile()
	self.resizeColumns()
	    
	layout.addWidget(treeWidget)
	#button = QPushButton("Report Selected")
	#button.clicked.connect(self.getCheckedItems)        
        #layout.addWidget(button)

        self.setLayout(layout)
        self.setWindowTitle("GEO Tree")
        
    def resizeColumns(self):
	"""Resizes columns appropriately to content size."""
	for i in range(3):
	    self.treeWidget.resizeColumnToContents(i)
	    
    def getCheckedItems(self):
	checkedItems = set()
	
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    for c in range(topLevelItem.childCount()):
		item = topLevelItem.child(c)
		if item.checkState(0) == Qt.Checked:
		    checkedItems.add(str(item.filename))
	    
	if not checkedItems:
            print "No Gene Expression data source selected, using the most recent preferences."
            return checkedItems
        
	f = open(ap("geodata/selected_geo.txt"), "w")
	f.write("\n".join( sorted(checkedItems) ) )
	f.close()
        return checkedItems
        
    def downloadCheckedGEOs(self):
	files = self.getCheckedItems()
	base_url = "http://robinviz.googlecode.com/svn/data2/expressions/gse"
	for filename in files:
	    local_path = ap("geodata/%s" % filename)
	    if not os.path.exists( local_path ):
		remote_gz = "%s/%s.gz" % (base_url, filename)
		local_gz = "%s.gz" % local_path
		download_file_to(remote_gz, local_gz)
		ungz(local_gz)
	
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
