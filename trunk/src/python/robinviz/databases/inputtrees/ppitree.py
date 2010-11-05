# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap
from utils.compression import *
from databases.translator import BiogridOspreyTranslator
from ppi_downloader import download_organism

class PPISelector(QWidget):
    def __init__(self):
        QWidget.__init__(self)
        self.assureOspreyDirExists()
        
        dir_prefix = "BIOGRID-OSPREY_DATASETS"
        #print os.listdir("ppidata")
        biogrid_dirname = filter(lambda filename: filename.startswith(dir_prefix), os.listdir(ap("ppidata")) )[0]
        
        self.biogridVersion = biogrid_dirname[len(dir_prefix)+1:-7]
        self.osprey_dir = ap("ppidata/%s-%s.osprey" % (dir_prefix, self.biogridVersion))
        self.setupGUI()
    def assureOspreyDirExists(self):
	dir_prefix = "BIOGRID-OSPREY_DATASETS"
        dirs = filter(lambda filename: filename.startswith(dir_prefix), os.listdir(ap("ppidata")) )
        if len(dirs) == 0:
	    if not os.path.exists(ap("ppidata/BIOGRID-OSPREY_DATASETS-3.0.68.osprey")):
		download_file("http://thebiogrid.org/downloads/archives/Release%20Archive/BIOGRID-3.0.68/BIOGRID-OSPREY_DATASETS-3.0.68.osprey.zip")
	    unzip_file_into_dir("BIOGRID-OSPREY_DATASETS-3.0.68.osprey.zip", ap("ppidata/BIOGRID-OSPREY_DATASETS-3.0.68.osprey"))
	    
	    os.remove("BIOGRID-OSPREY_DATASETS-3.0.68.osprey.zip")
	
    def setupGUI(self):	
	layout = QVBoxLayout()
	self.treeWidget = treeWidget = QTreeWidget()
	treeWidget.setColumnCount(1)
	treeWidget.setHeaderLabels(("PPI",))
	layout.addWidget(treeWidget)
	
	#button = QPushButton("Report Selected PPIs")
	#button.clicked.connect(self.getCheckedItems)
	#layout.addWidget(button)
        
	self.setLayout(layout)
        self.setWindowTitle("PPI Selection Tree")
        self.setMinimumSize(300,400)
        
        self.readPPIData()
	self.useDictionary(self.organism_experiments)
        
    def readPPIData(self):
	
	#organisms = filter(lambda filename: filename[0]!=".", os.listdir(self.osprey_dir))
	organisms = ["Arabidopsis_thaliana",
	"Caenorhabditis_elegans",
	"Drosophila_melanogaster",
	"Escherichia_coli_K12_MG1655",
	"Homo_sapiens",
	"Mus_musculus",
	"Rattus_norvegicus",
	"Saccharomyces_cerevisiae",
	"Schizosaccharomyces_pombe"]
	
	self.organism_experiments = {} # organism_name : experiments...

	for organism in organisms:
	    experiment_files = filter(lambda filename: filename[0]!="." and filename.endswith(".txt"), os.listdir(self.osprey_dir + "/" + organism))
	    organism_txt_length = len(organism)
	    experiment_names = sorted(map(lambda fn:fn[15:-19-organism_txt_length], experiment_files))
	    
	    self.organism_experiments[organism] = experiment_names
	    
    def getCheckedItems(self):
	checkedItems = set()
	
	def traverse(item):
	    # Add the term into checked items list if it's checked.
	    if item.checkState(0) == Qt.Checked:
		filename = "%(osprey_dir)s/%(organism)s/BIOGRID-OSPREY-%(experiment)s-%(organism)s-%(version)s.osprey.txt" % \
		{ "osprey_dir": self.osprey_dir,
		  "organism": str(item.parent().text(0)),
		  "experiment": str(item.text(0)),
		  "version": self.biogridVersion,
		}
													
		checkedItems.add(ap(filename))
		
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    for c in range( topLevelItem.childCount() ):
		child = topLevelItem.child(c)
		traverse(child)

        if not checkedItems:
            print "No PPI data source selected, using the most recent preferences."
            return checkedItems
        
	f = open(ap("ppidata/selected_ppis.txt"), "w")
	f.write("\n".join( sorted(checkedItems) ) )
	f.close()

	self.mergePPIFiles(checkedItems)

    def useDictionary(self, dictionary):
	topLevelItems = sorted(dictionary.keys())
	for topLevelItem in topLevelItems:
	    parentItem = QTreeWidgetItem(self.treeWidget, (topLevelItem,))
	    for child in dictionary[topLevelItem]:
		childItem = QTreeWidgetItem(parentItem, (child,))
		childItem.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsTristate)
		childItem.setCheckState(0, Qt.Unchecked)
		
		parentItem.addChild(childItem)
		
	    
	    parentItem.setFlags( Qt.ItemIsUserCheckable | Qt.ItemIsSelectable | Qt.ItemIsEnabled | Qt.ItemIsTristate)
	    parentItem.setCheckState(0, Qt.Unchecked)

    def convert_path_to_organism_name(self, path):
	if os.name == "posix":
	    seperator = "/"
	else:
	    seperator = "\\"
	# TODO: could be better
	return path.split(seperator)[-2]
	
    def findSelectedOrganisms(self, checkedPaths):
	organisms = set()
	for path in checkedPaths:
	    organismName = self.convert_path_to_organism_name(path)
	    organisms.add(organismName)
	    
	return list(organisms)

    def mergePPIFiles(self, files):
	"""Merges given PPI files."""
	confidence_dicts = {}
	
	organisms = self.findSelectedOrganisms(files)
	map(download_organism, organisms) # Download HitPredict data and generate (p1 p2 confidence) file.

        # TODO: find the max and min values of the files.

	for organism in organisms:
	    """For each organism, create a dictionary holding (p1,p2)=>confidence
	    and store them in confidence_dicts"""
	    d = {}
	    for line in open(ap("ppidata/hitpredict/%s.txt" % organism )):
		cols = line.strip().split("\t")
                #print cols
                # TODO: make a normalization here. remove old normalization
		d[cols[0], cols[1]] = cols[2]
	    confidence_dicts[organism] = d
	
	interactions = {}
	for filename in files:
            bfilename = "%s-BIOGRID" % filename
            if not os.path.exists(bfilename):
                b = BiogridOspreyTranslator(filename)
                b.translate()
            filename = bfilename

	    organism = self.convert_path_to_organism_name(filename)
	    for line in open(filename):
		cols = line.strip().split("\t")
		if cols[0] == cols[1]:
		    continue
		
		interaction = cols[0], cols[1]
		confidence = confidence_dicts[organism].get(interaction)
		if not confidence:
		    continue
		
		i = interactions.get( interaction )
		if not i:
		    i = []
		i.append( float(confidence) )
		interactions[ interaction ] = i
	
	# Now we have an interactions dict, containing (p1,p2) => [conf1, conf2, ...]
	# I'll take average of them. But I don't know if this is a good idea.
	single_ppi_file = open(ap("ppidata/ppi.txt"), "w")
	
	for key in interactions.keys():
	    p1, p2 = key
	    confidence_values = interactions[key]
	    confidence = sum(confidence_values) / len(confidence_values)
	    single_ppi_file.write("\t\t".join( (p1, p2, str(confidence)) ) + "\n")
	single_ppi_file.close()
		
		
		
	    
def main():
    app = QApplication(sys.argv)
    mainWindow = PPISelector()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()