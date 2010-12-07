# -*- coding: utf-8 -*-
"""Gene Ontology Association Source selector dialog"""
from PyQt4.QtGui import QWidget, QTreeWidget, QTreeWidgetItem, QApplication, QHBoxLayout
from PyQt4.QtCore import Qt
import sys
import os
import sqlite3

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap, rp
from utils.compression import download_file_to, ungz
from databases.genequery import GeneDB
from databases.translator import AssociationTranslator

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
            checkedItems = open(ap("assocdata/selected_assoc.txt")).read().split("\n")
            return checkedItems

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
                #print cols[2]
		# for each convertable increase the counter
                #print name
		counter += 1
		sets.append(set([bid[1] for bid in bids]))
	    if counter == 10:
		break
		
	#print sets
	if not sets:
	    return None
	    
	# Now find the intersection. Start with the first one.
	# Iterate over the set cumulatively.
        #print sets
	s = sets[0]
	for i in range(len(sets)-1):
	    s = s.intersection(sets[i+1])
	
	# Intersection should be one item!
        if len(s) == 1:
            return s.pop()
        else:
            print "No consensus. # of Candidates: %d" % len(s)
            return None
	#assert len(s)==1, "Length of annotation set is not 1 but %d" %len(s)

    def _extend_dictionary(self, pool, newbie):
        """Extends the pool dictionary with the newbie's contents.
        Both dictionaries have list records mapped to string keys.
        """
        for key in newbie.keys():
            l = pool.get(key)
            if not l:
                l = []
            l.extend(newbie[key])
            pool[key] = l

        #return pool

    def _get_dictionary_from_file(self, filename):
        newbie = {}
        f = open(filename)
        for line in f:
            cols = line.strip().split("\t")
            if cols[1] == "NULL":
                continue
            newbie[cols[0]] = cols[1:]
        return newbie

    def filterSelected(self):
        flag = open(rp("outputs/resultparams.txt")).read().strip().split("-")[-1]

        top_level_file = ap("godata/toplevel_%s.txt" % flag)
        
        topLevels = open(top_level_file).read().split("\n")
        selected_terms = sorted(map(lambda x:x.strip(), open(ap("godata/selected_terms.txt")).readlines()))
        united_terms = list(set(sorted(topLevels + selected_terms)))

        output = open(ap("assocdata/go_slim.txt"),"w")
        catnames = open(ap("assocdata/input_go.txt"),"w")
        catcodes = open(ap("assocdata/category_codes.txt"), "w")
        conn = sqlite3.connect(ap("godata/goinfo.sqlite3"))
        cursor = conn.cursor()

        def get_name_of_term(i):
            cursor.execute("SELECT name FROM terms WHERE id=?", (str(i),))
            name = cursor.fetchone()
            
            if name:
                return name[0]
            else:
                print "Name for term %s could not be found." % i
                return "NULL"

        name_dict = {}
        for term in united_terms:
            name_dict[term] = get_name_of_term(int(term.split(":")[-1]))

        catnames.write( "\n".join([ name_dict[term] for term in selected_terms]) )
        catcodes.write( "\n".join([ term for term in selected_terms]) )
        
        for term in united_terms:
            if term in self.go_dict:
                genes = self.go_dict[term]
                if not genes:
                    genes = ["NULL"]
                output.write("%s\t%s\t%s\n" % ( term, name_dict[term], "\t".join( genes ) ) )
            else:
                print term, "was not found in go dict"
        output.close()
        catnames.close()
    def mergeSelectedAssociations(self):
	"""Merges selected association data files into one single assocdata/input_go.txt"""
	files = self.downloadCheckedAssociation()
        if not files:
            print "No association source selected, using the most recent preferences."
            return
	o = open(ap("assocdata/go.txt"),"w")
	"""converted = 0
	not_converted = 0"""
	self.go_dict = go_dict = {}
	TARGET_ANNOTATION= "BIOGRID"
	self.db = GeneDB()
	translator = AssociationTranslator()

	# ===============================================
	for filename in files:
            target_file = ap("%s-%s" % (filename, TARGET_ANNOTATION))
            if os.path.exists( target_file ):
                print target_file,"exists, shall not re-generate it."
                newbie = self._get_dictionary_from_file(target_file)
                self._extend_dictionary( go_dict, newbie )
            else:
                translator.set_filename(filename)
                """if translator.translate(None, TARGET_ANNOTATION):
                    self._extend_dictionary( go_dict, translator.go_dict )"""
                if translator.translate_biogrids():
                    self._extend_dictionary( go_dict, translator.go_dict )
     	# ===============================================

        # ============= REMOVE DUPLICATE GENES  ==============
        for key in go_dict.keys():
            l = go_dict[key]
            go_dict[key] = set(l)
        # =====================================================
        conn = sqlite3.connect(ap("godata/goinfo.sqlite3"))
        cursor = conn.cursor()
        
        def get_name_of_term(i):
            cursor.execute("SELECT name FROM terms WHERE id=?", (str(i),))
            name = cursor.fetchone()
            if not name:
                print "Name for term %s could not be found." % i
                return "NULL"
            else:
                return name[0]

	for term in sorted(go_dict.keys()):
#	    try:
            genes = list(set(go_dict[term]))
            if genes:
                genes_str = "\t".join( map(str, genes) )
            else:
                genes_str = "NULL"

            #o.write("%s\t%s\n" % ( term,  genes_str) )
            o.write("%s\t%s\t%s\n" % ( term, get_name_of_term(int(term.split(":")[-1])), genes_str ) )
	    #except:
#		raise Exception, "problem here. term: %s; genes= %s" % (term, go_dict[term])
	o.close()


        self.filterSelected()
	
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
