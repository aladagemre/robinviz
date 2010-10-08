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
from databases.genequery import GeneDB

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
	#treeWidget.setSortingEnabled(True)
	
	self.parseFile()	    
	layout.addWidget(treeWidget)
	
	button = QPushButton("Report Selected")
	button.clicked.connect(self.mergeSelectedAssociations)        
        layout.addWidget(button)

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
	    # For all children, traverse them too.
	    for c in range( item.childCount() ):
		child = item.child(c)
		traverse(child)
		
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    traverse(topLevelItem)
	
	checkedItems = sorted(checkedItems)
	f = open(ap("assocdata/selected_assoc.txt"), "w")
	f.write("\n".join( checkedItems ) )
	f.close()
	return checkedItems
	
        
    def downloadCheckedAssociation(self):
	urls = self.getCheckedItems()
	uncompressed_files = []
	for url in urls:
	    # Find the uncompressed filename
	    filename = url.split("/")[-1]
	    filename = filename[:filename.find(".gz")]
	    # get the absolute path of this file.
	    local_path = ap("assocdata/%s" % filename)
	    # add this file to uncompressed_files so that we'll return all of them
	    uncompressed_files.append(local_path)
	    # if this file doesn't exist yet, we have to download it.
	    if not os.path.exists( local_path ):
		print local_path, "does not exist, will download it."
		remote_gz = url
		local_gz = "%s.gz" % local_path
		download_file_to(remote_gz, local_gz)
		ungz(local_gz)
		# now we have local_path!
	    else:
		print local_path, "exists, skipping."
		
	return uncompressed_files

    def detectAnnotationType(self, filename):
	"""Detects and returns the annotation type for given filename
	Uses a quick workaround. If first record seems to be only one type, then
	just return it. If more than one, take first 10 records and returns their
	intersection as the annotation type."""
	
	f = open(filename)
	sets = []
	counter = 0
	for line in f:
	    if line[0] == "!":
		continue
	    
	    cols = line.split("\t")
	    if cols[2]:
		name = cols[2]
	    else:
		name = cols[1]
	    
	    bids = self.db.value2biogrids(name)
	    if len(bids) == 1:
		# if only one type, just return it.
		# as intersection will be this one.
		return bids[0][1]
		
	    if bids:
		# for each convertable
		counter += 1
		sets.append(set([bid[1] for bid in bids]))
	    if counter == 10:
		break
		
	#print sets
	if not sets:
	    return None
	    
	# Now find the intersection. Start with the first one.
	# Iterate over the set cumulatively.
	s = sets[0]
	for i in range(len(sets)-1):
	    s = s.intersection(sets[i+1])
	
	# Intersection should be one item!
	assert len(s)==1, "Length of annotation set is not 1 but %d" %len(s)
	
	return s.pop() 
		
    def mergeSelectedAssociations(self):
	"""Merges selected association data files into one single assocdata/go_slim.txt"""
	files = self.downloadCheckedAssociation()
	o = open(ap("assocdata/go_slim.txt"),"w")
	converted = 0
	not_converted = 0
	go_dict = {}
	
	self.db = GeneDB()
	
	# ===============================================
	for filename in files:
	    atype = self.detectAnnotationType(filename)
	    print filename.split("/")[-1]
	    print "Annotation type:",atype
	    f = open(filename)
	    for line in f:
		if line[0] == "!":
		    continue
		cols = line.split("\t")
		protein_name1 = cols[1]
		protein_name2 = cols[2]
		go_term = cols[4]
		
		l = go_dict.get(go_term)
		if not l:
		    l = []
		if protein_name2:
		    #protein_name2 = self.db.value2value(protein_name2,"OFFICIAL_SYMBOL") # to convert OFFICIAL_SYMBOL
		    #protein_name2 = [record[0] for record in self.db.value2biogrids(protein_name2)] # to convert possible biogrids, we have to use l.extend for this.
		    protein_name2 = self.db.value2biogrid(protein_name2, atype)
		    if protein_name2:
			converted+=1
			l.append(protein_name2)
		    else:
			not_converted+=1
		    
		else:
		    protein_name1 = self.db.value2biogrid(protein_name1, atype)
		    if protein_name1:
			converted+=1
			l.append(protein_name1)
		    else:
			not_converted+=1
		    
		go_dict[go_term] = l
	# ===============================================
	
	for key in sorted(go_dict.keys()):
	    try:
		o.write("%s\t%s\n" % ( key, "\t".join( map(str, go_dict[key]) ) ) )
	    except:
		raise Exception, "problem here. key: %s; values = %s" % (key, go_dict[key])
	o.close()
    
	print "Converted:",converted
	print "Could not convert:",not_converted
	
	
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
    mainWindow = AssociationSelector()
    mainWindow.showMaximized()
    sys.exit(app.exec_())
