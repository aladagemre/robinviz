# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os


if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap, rp
from utils.compression import download_file_to, ungz
from utils.settingswrite import write_values
import yaml

class GEOSelector(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        self.filename = ap("geodata/geoTreeView.txt")
        self.radio2item = {}

    def initialize(self):
        self.loadSettings()
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
	treeWidget.setColumnCount(4)
	treeWidget.setHeaderLabels(("", "Accession", "Series Platform ID","Description",))
	treeWidget.setSortingEnabled(True)
        treeWidget.setSelectionMode(QAbstractItemView.SingleSelection)

	self.parseFile()
        self.setItemWidgets()
	self.resizeColumns()
	    
	layout.addWidget(treeWidget)
	#button = QPushButton("Report Selected")
	#button.clicked.connect(self.getCheckedItems)        
        #layout.addWidget(button)

        self.setLayout(layout)
        self.setWindowTitle("GEO Tree")
        
    def resizeColumns(self):
	"""Resizes columns appropriately to content size."""
	for i in range(1,4):
	    self.treeWidget.resizeColumnToContents(i)

        self.treeWidget.setColumnWidth(0, 65)
    def getCheckedItems(self):
        for key in self.radio2item.keys():
            if key.isChecked():
                item_filename = self.radio2item[key].filename
                f = open(ap("geodata/selected_geo.txt"), "w")
                f.write("%s\n" % item_filename)
                f.close()
                return [item_filename]
	    
        print "No Gene Expression data source selected, using the most recent preferences."
        return []
        
    def downloadCheckedGEOs(self):
	files = self.getCheckedItems()
	base_url = "http://robinviz.googlecode.com/svn/data2/expressions/gse"
	for filename in files:
	    local_path = ap("geodata/%s" % filename)
	    if not os.path.exists( local_path ):
		remote_gz = "%s/%s.gz" % (base_url, filename)
		local_gz = "%s.gz" % local_path
		download_file_to(remote_gz, local_gz, binary=True)
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
                # leave blank for radio button
		data = ("", "%s (%s)" % tuple(cols) ,) # Homo Sapiens (9606)
		item = QTreeWidgetItem(self.treeWidget, data )
                #item.setFlags( Qt.ItemIsSelectable | Qt.ItemIsEnabled)
		self.treeWidget.insertTopLevelItem(0, item)
		lastNodeAtLevel[0] = item # save the last root
	    else: # If inner level
		parent = lastNodeAtLevel[numt-1] # find who the parent is.
		cols = line.strip().split("\t") # split into columns
                temp = [""] # for radio button
                temp.extend(cols[1:])
		data = tuple(temp) # leave the first column (filename)
		item = QTreeWidgetItem(parent, data )
		item.filename = cols[0] # assign filename to the item object secretly.

		parent.addChild( item )
		lastNodeAtLevel[numt] = item	# assign itself as the last root at its level
                #item.setFlags( Qt.ItemIsSelectable | Qt.ItemIsEnabled)
	    
    def setItemWidgets(self):
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    for c in range(topLevelItem.childCount()):
		item = topLevelItem.child(c)
                radio = QRadioButton(self)
                self.radio2item[radio] = item
                self.treeWidget.setItemWidget(item, 0, radio)
                if self.last_selected == item.filename:
                    radio.setChecked(True)

    def loadSettings(self):
        stream = file(rp('settings.yaml'), 'r')
        self.complete_params = yaml.load(stream)
        self.params = self.complete_params["Confirmation"]["CoExpression"]
        self.last_selected = self.params["Input"]["dataName_bic"].split("/")[-1]

    def saveSettings(self):
        # ==GET CHECKED INFORMATION===
        checked = self.getCheckedItems()
        
        """dataName_bic: src/python/robinviz/databases/inputtrees/geodata/GSE23741_series_matrix.txt
            dataName2_bic: src/python/robinviz/databases/inputtrees/geodata/GSE23741_series_matrix.txt"""
        self.params["Input"]["dataName_bic"] = "%s/%s" % ("src/python/robinviz/databases/inputtrees/geodata", checked[0])
        self.params["Input"]["dataName2_bic"] = "%s/%s" % ("src/python/robinviz/databases/inputtrees/geodata", checked[0])
        
        # ===== PLACE PARAMETERS ====
        # We put partial dict back to the complete dict.
        self.complete_params["Confirmation"]["CoExpression"] = self.params
        #print self.complete_params
        # ==== WRITE PARAMETERS =====
        f = open(rp('settings.yaml'), "w")
        f.write(write_values(self.complete_params))
        f.close()
        
if __name__ == "__main__":
    app = QApplication(sys.argv)
    mainWindow = GEOSelector()
    mainWindow.showMaximized()
    sys.exit(app.exec_())
