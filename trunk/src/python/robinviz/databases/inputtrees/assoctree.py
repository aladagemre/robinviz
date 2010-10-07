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

class AssociationSelector(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        self.filename = ap("assocdata/index.txt")
        self.checkIndex()
        self.setupGUI()
        
    def checkIndex(self):
	if os.path.exists(self.filename):
	    return
	from complete_goa import *
	
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(2)
	treeWidget.setHeaderLabels(("Organism", "Source"))
	treeWidget.setSortingEnabled(True)
	
	self.parseFile()
	self.resizeColumns()
	    
	layout.addWidget(treeWidget)
	button = QPushButton("Report Selected")
	button.clicked.connect(self.downloadCheckedAssociation)        
        layout.addWidget(button)

        self.setLayout(layout)
        self.setWindowTitle("Gene Association Sources")
        
    def resizeColumns(self):
	"""Resizes columns appropriately to content size."""
	for i in range(2):
	    self.treeWidget.resizeColumnToContents(i)
	    
    def getCheckedItems(self):
	checkedItems = set()
	
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    if topLevelItem.checkState(0) == Qt.Checked:
		checkedItems.add(str(topLevelItem.url))
	
	f = open(ap("assocdata/selected_assoc.txt"), "w")
	f.write("\n".join( sorted(checkedItems) ) )
	f.close()
        return checkedItems
        
    def downloadCheckedAssociation(self):
	urls = self.getCheckedItems()
	for url in urls:
	    filename = url.split("/")[-1][:-12]
	    local_path = ap("assocdata/%s" % filename)
	    if not os.path.exists( local_path ):
		remote_gz = url
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
	    # Count the number of preceding tabs to define depth.
	    numt = tcount(line)
	    cols = line.strip().split("\t")
	    if numt == 0: # If top level,
		item = QTreeWidgetItem(self.treeWidget, (cols[0],) )
		lastNodeAtLevel[0] = item # save the last root
		self.treeWidget.insertTopLevelItem(0, item)
	    else: # If inner level
		parent = lastNodeAtLevel[numt-1] # find who the parent is.
		data = tuple(cols[0:2])
		item = QTreeWidgetItem(parent, data )
		item.url = cols[2]
		parent.addChild( item )
		lastNodeAtLevel[numt] = item	# assign itself as the last root at its level
 
	    item.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsTristate)
	    item.setCheckState(0, Qt.Unchecked)
	    
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = AssociationSelector()
    mainWindow.showMaximized()
    sys.exit(app.exec_())
