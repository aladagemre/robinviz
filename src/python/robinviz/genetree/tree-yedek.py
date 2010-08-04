# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
from xmlparser import getTermDict
import sys


class Window(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupGUI()
        
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(1)
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
        self.setWindowTitle("Tree test")
        
    def test(self):
	item = QTreeWidgetItem(self.treeWidget, ("Parent",) )
	self.treeWidget.insertTopLevelItem(0, item)
	
	subitem1 = QTreeWidgetItem(item, ("Item1", ) )
	subitem2 = QTreeWidgetItem(item, ("Item2", ) )
	subitem3 = QTreeWidgetItem(item, ("Item3", ) )
	
	item.addChildren( ( subitem1, subitem2, subitem3 ) )
    def populate(self):
	self.termDict = getTermDict()
	
	def traverse(termName, parentItem):
	    item = QTreeWidgetItem(parentItem, (termName, ) )
	    item.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled| Qt.ItemIsTristate)
	    item.setCheckState(0, Qt.Unchecked)
	    try:
		parentItem.addChild(item)
	    except AttributeError:
		parentItem.addTopLevelItem(item)
	    
	    children = self.termDict[str(termName)]
	    for child in children:
		traverse(child, item)
	
	for parent in ("GO:0008150",):#, "GO:0005575", "GO:0003674"):
	    traverse(parent, self.treeWidget)
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
	#print checkedItems
	#return checkedItems
	
	
	    

    
    


        
    """def parseFile(self):
	
	def tcount(text):
	    return text.count("\t")
	    
	f = open("data.yaml")
	lastParent = None
	lines = f.readlines()
	for i, line in enumerate(lines):
	    numt = tcount(line)
	    item = QTreeWidgetItem(self.treeWidget, line.strip())
	    if numt == 0:
		self.treeWidget.insertTopLevelItem(0, item)
		lastParent = item
		lastNumt = 0"""
	    
		
		
"""	
QTreeWidget *treeWidget = new QTreeWidget();
 treeWidget->setColumnCount(1);
 QList<QTreeWidgetItem *> items;
 for (int i = 0; i < 10; ++i)
     items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
 treeWidget->insertTopLevelItems(0, items);
 """
 
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = Window()
    mainWindow.showMaximized()
    sys.exit(app.exec_())
