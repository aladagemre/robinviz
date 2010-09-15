# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys, os, re
import sqlite3
import gene2goparser
#from termdbparser import generateTermDict
from utils.file.ungz import *
import shutil

def grep(string,list):
    expr = re.compile(string)
    return [elem for elem in list if expr.match(elem)]


class GOSelector(QMainWindow):
    def __init__(self, filename="godata/goinfo.sqlite3"):
        QMainWindow.__init__(self)
        self.filename = filename
        if not os.path.exists(self.filename):
	    print self.filename, "does not exist. Downloading it..."
	    #generateTermDict()
	    download_file("http://www.emrealadag.com/dosyalar/goinfo.sqlite3")
	    shutil.move("goinfo.sqlite3", self.filename)
	    
        self.connectDB()
        self.setupGUI()
        
    def connectDB(self):
	self.conn = sqlite3.connect(self.filename)
	self.curs = self.conn.cursor()
	
    def setupGUI(self):
	layout = QVBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(2)
	treeWidget.setHeaderLabels(("Accession", "Name"))
	treeWidget.setSortingEnabled(True)
	treeWidget.itemDoubleClicked.connect(self.itemDoubleClicked) # Define what shall happen when double clicked on an item.
	treeWidget.itemExpanded.connect(self.itemExpanded) # Define what shall happen when double clicked on an item.
	
	self.loadTopLevelGO() # Load first level
	self.resizeFirstColumn()

        layout.addWidget(treeWidget)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("Gene Ontology Tree")
    
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
	    if item.checkState(0) == Qt.Checked:
		checkedItems.add(str(item.text(0)))
	    # For all children, traverse them too.
	    for c in range( item.childCount() ):
		child = item.child(c)
		traverse(child)
		
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    traverse(topLevelItem)
	
	checkedItems = sorted(checkedItems)
	f = open("godata/selected_terms.txt", "w")
	f.write("\n".join( checkedItems ) )
	f.close()
        
        # Now use complete go mapping to produce a sub-go mapping
        if not os.path.exists("godata/go_mapping.txt"):
	    # if go mapping has not been created yet, do it.
	    ggp = gene2goparser.Gene2GOParser(input_file="godata/gene2go",output_file="godata/go_mapping.txt", terms=None)
	
	# filter complete go mapping.
	go_term_list = open("godata/go_mapping.txt").readlines()
	output = open("godata/sub_go_mapping.txt","w")
	for checkedItem in checkedItems:
	    x = grep(checkedItem, go_term_list)
	    #print checkedItem, x
	    if x:
		output.write(x[0])
	    else:
		output.write(checkedItem+"\tNULL\n")
		
	output.close()
	    
        
        
    def getRecord(self, record_id):
	self.curs.execute("SELECT * FROM terms WHERE id=%d;" % record_id)
	return self.curs.fetchone()
	
    def loadTopLevelGO(self, record_id=3674): # molecular function=3674
	record_id, name, children = self.getRecord(record_id)
	
	parent = QTreeWidgetItem(self.treeWidget, ("GO:%07d"% record_id, name) )
	self.treeWidget.insertTopLevelItem(0, parent)
	
	parent.setFlags( Qt.ItemIsSelectable | Qt.ItemIsEnabled)
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
