# -*- coding: utf-8 -*-
"""Gene Ontology Association Source selector dialog"""
from PyQt4.QtGui import QWidget, QTreeWidget, QTreeWidgetItem, QApplication, QHBoxLayout, QProgressDialog
from PyQt4.QtCore import Qt, QProcess,QString
import sys
import os
import sqlite3

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap, rp
from utils.compression import download_file_to, ungz
from databases.genequery import GeneDB
from databases.translator import AssociationTranslator
import windows

from complete_goa import generate_index

class AssociationSelector(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        self.filename = ap("assocdata/index.txt")
        self.checkIndex()
        self.setupGUI()
        
    def checkIndex(self):
	if os.path.exists(self.filename):
	    return
	generate_index()
	
    def setupGUI(self):
	layout = QHBoxLayout()
	
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(3)
	treeWidget.setHeaderLabels(("Organism", "Source DB", "Gene #",))
	treeWidget.itemExpanded.connect(self.resizeColumns)
	treeWidget.setSortingEnabled(True)
	treeWidget.sortItems(0, Qt.AscendingOrder)
	self.parseFile()	    
	layout.addWidget(treeWidget)
	
	#button = QPushButton("Report Selected")
	#button.clicked.connect(self.mergeSelectedAssociations)
        #layout.addWidget(button)

        self.setLayout(layout)
        self.setWindowTitle("Gene Association Sources")
        
        self.resizeColumns()
        
    def resizeColumns(self):
	"""Resizes columns appropriately to content size."""
	for i in range(2):
	    self.treeWidget.resizeColumnToContents(i)
	    
        
    def getCheckedItems(self):
	checkedItems = set()
	
	def traverse(item):
	    # Add the term into checked items list if it's checked.
	    if item.checkState(0) == Qt.Checked:
		checkedItems.add(str(item.url))	
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
            for c in range( topLevelItem.childCount() ):
		child = topLevelItem.child(c)
		traverse(child)

        if not checkedItems:
            #checkedItems = open(ap("assocdata/selected_assoc.txt")).read().strip().split("\n")
            return checkedItems

	checkedItems = sorted(checkedItems)
	f = open(ap("assocdata/selected_assoc.txt"), "w")
	f.write("\n".join( checkedItems ) )
	f.close()
	return checkedItems
	
        


    def translate_files(self, files):
        translator = AssociationTranslator()
        for filename in files:
            translator.set_filename(filename)
            #translator.translate_biogrids()
            translator.translate(from_type=None, to_type="OFFICIAL_SYMBOL")
        
    def mergeSelectedAssociations(self):
	"""Merges selected association data files into one single assocdata/input_go.txt"""
        items = self.getCheckedItems()
        if not items:
            print "No association source selected, using the most recent preferences."
            return
        
        # Run the process here
        self.process = QProcess(self.parent())

        """self.progress = QProgressDialog("Downloading &| Merging Association Files. Please wait...", "Abort", 0, 0, self.parent())
        self.progress.setWindowModality(Qt.WindowModal)
        self.progress.canceled.connect(self.process.kill)
        self.progress.forceShow()"""

        self.log_window = windows.LogWindow()
        self.log_window.show()

        self.process.setWorkingDirectory(ap(""))
        self.process.readyReadStandardOutput.connect(self.readOutput)
        self.process.readyReadStandardError.connect(self.readError)
        self.process.finished.connect(self.log_window.close)
        arguments = [ "assocmerger.py" ]

        self.process.start("python", arguments)
        self.process.waitForFinished()
        
    def readOutput(self):
        msg = QString(self.process.readAllStandardOutput())
        print msg,
        self.log_window.addMessage(msg)

    def readError(self):
        msg = QString(self.process.readAllStandardOutput())
        print msg,
        self.log_window.addMessage(msg)

    def parseFile(self):
	"""Parses the index file and generates TreeWidget."""
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
		data = tuple(cols[0:3])
		item = QTreeWidgetItem(parent, data )
		try:
		    item.url = cols[3]
		except:
		    print cols
		parent.addChild( item )
		lastNodeAtLevel[numt] = item	# assign itself as the last root at its level
 
	    item.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsTristate)
	    item.setCheckState(0, Qt.Unchecked)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    selector = AssociationSelector()
    selector.showMaximized()
    sys.exit(app.exec_())
