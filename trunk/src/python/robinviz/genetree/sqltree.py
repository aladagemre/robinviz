# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import sqlite3

class TermSelector(QMainWindow):
    def __init__(self, filename):
        QMainWindow.__init__(self)
        self.filename = filename
        self.connectDB()
        self.setupGUI()
        
    def connectDB(self):
	self.conn = sqlite3.connect(self.filename)
	self.curs = self.conn.cursor()
	
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(2)
	treeWidget.setHeaderLabels(("Accession", "Name"))
	treeWidget.setSortingEnabled(True)
	treeWidget.itemDoubleClicked.connect(self.itemDoubleClicked) # Define what shall happen when double clicked on an item.
	treeWidget.itemExpanded.connect(self.itemExpanded) # Define what shall happen when double clicked on an item.
	
	self.loadTopLevelGO() # Load first level
	self.resizeFirstColumn()
	
	button = QPushButton("Report Selected")
	button.clicked.connect(self.getCheckedItems)
	
        layout.addWidget(treeWidget)
        layout.addWidget(button)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("Generic Tree")
    
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
	
	f = open("selected_terms.txt", "w")
	f.write("\n".join( sorted(checkedItems) ) )
	f.close()
        
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
    mainWindow = TermSelector("sqlite.db")
    mainWindow.showMaximized()
    sys.exit(app.exec_())
