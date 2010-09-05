# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from xmlparser import getTermDict
import sys
class Term:
    def __init__(self, accession=None):
	self.accession = accession
	self.name = None
	self.children = []
	
    def add_child(self, term):
	self.children.append(term)
	
    def __repr__(self):
	return "%s (%s) - %s" % (self.accession, self.name, self.children)

class Window(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupGUI()
        
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(2)
	treeWidget.setHeaderLabels( ("Accession", "Name") )
	#parent = QTreeWidgetItem(treeWidget, "Test")
	#treeWidget.insertTopLevelItems(0, [parent])
	self.populate()
	
	button = QPushButton("Report")
	button.clicked.connect(self.getCheckedItems)
	
        layout.addWidget(treeWidget)
        layout.addWidget(button)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("Gene Selection")

    def populate(self):
	self.termDict = getTermDict()
	
	def traverse(term, parentItem):
	    x = QStringList()
	    x.append( term.accession )
	    x.append( term.name or "-" )
	    item = QTreeWidgetItem(parentItem, x )
	    item.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled)
	    item.setCheckState(0, Qt.Unchecked)
	    try:
		parentItem.addChild(item)
	    except AttributeError:
		parentItem.addTopLevelItem(item)
	    
	    for child in term.children:
		#print "Considering child %s of %s" % (child.accession, term.accession)
		traverse(child, item)
	
	for topCategory in (map(lambda term: self.termDict[term], ("GO:0008150", "GO:0005575", "GO:0003674"))):
	    traverse(topCategory, self.treeWidget)
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
	
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = Window()
    mainWindow.showMaximized()
    sys.exit(app.exec_())
