# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from xmlparser import getTermDict
import sys


class TermSelector(QMainWindow):
    def __init__(self, filename):
        QMainWindow.__init__(self)
        self.filename = filename
        self.setupGUI()
        
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(1)
	treeWidget.setHeaderLabels(("Gene Term",))
	
	self.parseFile()
	
	button = QPushButton("Report Selected")
	button.clicked.connect(self.getCheckedItems)
	
        layout.addWidget(treeWidget)
        layout.addWidget(button)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("Generic Tree")
        
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
        
    def parseFile(self):
	lastNodeAtLevel = {}
	
	def tcount(text):
	    return text.count("\t")
	    
	
	for line in open(self.filename):
	    numt = tcount(line)
	    if numt == 0:
		item = QTreeWidgetItem(self.treeWidget, (line.strip(),) )
		self.treeWidget.insertTopLevelItem(0, item)
		lastNodeAtLevel[0] = item
	    else:
		parent = lastNodeAtLevel[numt-1]
		item = QTreeWidgetItem(parent, (line.strip(),) )
		parent.addChild( item )
		lastNodeAtLevel[numt] = item
 
	    item.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled)
	    item.setCheckState(0, Qt.Unchecked)
	    
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = TermSelector("cities")
    mainWindow.showMaximized()
    sys.exit(app.exec_())
