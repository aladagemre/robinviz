# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os
import re
import sqlite3

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap
from utils.compression import *
from utils.downloader import Downloader
#from termdbparser import generateTermDict

def grep(string,list):
    expr = re.compile(string)
    return [elem for elem in list if expr.match(elem)]

class GOSelector(QMainWindow):
    def __init__(self, filename=ap("godata/goinfo.sqlite3")):
        QMainWindow.__init__(self)
        self.filename = filename
        if not os.path.exists(self.filename):
	    print self.filename, "does not exist. Downloading it..."
	    #generateTermDict()
            url = "http://robinviz.googlecode.com/svn/data2/go/goinfo.sqlite3"
            self.d = Downloader(url, filename)
            self.d.finished.connect(self.setup)
            qApp.processEvents()
            self.d.exec_()
            self.setupGUI()
                 
        else:
            self.setupGUI()
            self.setup(1)
        
    def setup(self, result):
        if result:
            print "Download result is positive, loading the list."
            self.loadData()
        else:
            print "Download result is negative, not loading the list."
        
    def connectDB(self):
	self.conn = sqlite3.connect(self.filename)
	self.curs = self.conn.cursor()
	
    def setupGUI(self):
	layout = QVBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(2)
	treeWidget.setHeaderLabels(("Accession", "Name"))
	treeWidget.setSortingEnabled(True)
        treeWidget.sortItems(1, Qt.AscendingOrder)
	treeWidget.itemDoubleClicked.connect(self.itemDoubleClicked) # Define what shall happen when double clicked on an item.
	treeWidget.itemExpanded.connect(self.itemExpanded) # Define what shall happen when double clicked on an item.

        layout.addWidget(treeWidget)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("Gene Ontology Tree")
        
    def loadData(self):
        self.connectDB()
        self.loadTopLevelGO(3674) # Load first level
        self.loadTopLevelGO(5575) # Load first level
        self.loadTopLevelGO(8150) # Load first level
	self.resizeFirstColumn()
        
    def resizeFirstColumn(self):
	self.treeWidget.resizeColumnToContents(0)
	
    def itemExpanded(self, item):
	self.resizeFirstColumn()
	
    def itemDoubleClicked(self, item, column):
	"""Defines what will happen when double clicked on a record.
	If item does not have any child, it shall search for its children
	and add them below the item."""
	
	# If no item was selected or item already has child(ren) in the tree, or term does not have any child in the database, skip.
	if not item or item.childCount() != 0 or not item.font(0).bold():
	    return
	
	# Get the term id selected.
	record_id = int(item.text(0).split("GO:")[1])
	
	# Find its children.
	record_id, name, children = self.getRecord(record_id)
	
	    
	for child in children.split(","):
	    # For each child,, create a new QTreeWidgetItem
	    cid, cname, cchild = self.getRecord(int(child))
	    citem = QTreeWidgetItem(item, ("GO:%07d"% cid, cname))
	    citem.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled)
	    citem.setCheckState(0, Qt.Unchecked)
	    
	    if cchild: 
		# If we have children, make it bold so that user can understand it.
		font = QFont()
		font.setBold(True)
		citem.setFont(0, font)
	    
	    item.addChild( citem )
	
	self.resizeFirstColumn()
	
    def getCheckedItems(self):
	checkedItems = set()
	
	def traverse(item):
	    # Add the term into checked items list if it's checked.
	    if item.checkState(0) == Qt.Checked and item.parent():
		checkedItems.add(str(item.text(0)))
	    # For all children, traverse them too.
	    for c in range( item.childCount() ):
		child = item.child(c)
		traverse(child)
		
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    traverse(topLevelItem)

        if not checkedItems:
            return checkedItems

        
	checkedItems = sorted(checkedItems)
	f = open(ap("godata/selected_terms.txt"), "w")
	f.write("\n".join( checkedItems ) )
	f.close()
        return checkedItems
        
    def getRecord(self, record_id):
	self.curs.execute("SELECT * FROM terms WHERE id=%d;" % record_id)
	return self.curs.fetchone()
	
    def loadTopLevelGO(self, record_id=3674): # molecular function=3674
	record_id, name, children = self.getRecord(record_id)
	
	parent = QTreeWidgetItem(self.treeWidget, ("GO:%07d"% record_id, name) )
	self.treeWidget.insertTopLevelItem(0, parent)
	
	parent.setFlags( Qt.ItemIsTristate | Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled)
	parent.setCheckState(0, Qt.Unchecked)
	
	for child in children.split(","):
	    cid, cname, cchild = self.getRecord(int(child))
	    item = QTreeWidgetItem(parent, ("GO:%07d"% cid, cname))
	    item.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled)
	    item.setCheckState(0, Qt.Unchecked)
	    
	    if cchild: 
		# If we have children, make it bold so that user can understand it.
		font = QFont()
		font.setBold(True)
		item.setFont(0, font)
	    
	    parent.addChild( item )
	   
	    
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = GOSelector()
    mainWindow.showMaximized()
    sys.exit(app.exec_())
