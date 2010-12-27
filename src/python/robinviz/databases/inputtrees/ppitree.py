import os.path
# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap,rp, latest_osprey_dir
from utils.compression import *
from utils.downloader import Downloader
from databases.translator import BiogridOspreyTranslator
from ppi_downloader import download_organism
from utils.BeautifulSoup import BeautifulSoup
import urllib

def get_soup(url):
    html = urllib.urlopen(url).read()
    soup = BeautifulSoup(html)
    return soup

class PPISelector(QWidget):
    IDENTIFIER_PATH = rp("src/python/robinviz/databases/identifier.db")
    
    def __init__(self):
        QWidget.__init__(self)
        self.assureOspreyDirExists()
        
    def setup(self, result):
        self.osprey_dir = ap("ppidata/%s" % self.directory )
        ziplocation = "%s.zip" % self.directory
            
        if not os.path.exists(self.osprey_dir) and os.path.exists(ziplocation) and os.path.getsize(ziplocation):
            unzip_file_into_dir(ziplocation, self.osprey_dir)
            os.remove(ziplocation)      

        # ======IDENTIFIERS========
        TARGZ = "identifier.db.tar.gz"
        if not os.path.exists(self.IDENTIFIER_PATH) or not os.path.getsize(self.IDENTIFIER_PATH):
            # If identifier.db does not exist, check if targz is downloaded
            if not os.path.exists(TARGZ) or not os.path.getsize(TARGZ):
                # if targz is not downloaded, warn
                QMessageBox.information(self,"Download not completed", "Download has to be completed before moving any further. Please go back and forth.")
                return
            else:
                # if targz is downloaded and has some size
                print "Uncompressing identifier.db.tar.gz"
                untar("identifier.db.tar.gz")
                shutil.move("identifier.db", self.IDENTIFIER_PATH)
                os.remove("identifier.db.tar.gz")
        # =========================

        self.readPPIData()
	self.useDictionary(self.organism_experiments)

    def detectLatestVersionOnline(self):
        try:
            soup = get_soup("http://thebiogrid.org/")
            main_div = soup.findAll(id='maintext')
            try:
                strongs = main_div[0].findAll('strong')
                for strong in strongs:
                    if strong.text.count(".") == 2:
                        # 3.1.71
                        return str(strong.text)
            except:
                print "HTML structure of thebiogrid.org has changed. No div tag with maintext id!"
        except:
            print "Could not connect to the biogrid web site. Please try again later."

        # if any error is to happen, return 3.1.71
        print "Will use 3.1.71 as the default version"
        return "3.1.71"
        
        
    
    def assureOspreyDirExists(self):
	dir_prefix = "BIOGRID-OSPREY_DATASETS"
        dirs = filter(lambda filename: filename.startswith(dir_prefix), os.listdir(ap("ppidata")) )
        if len(dirs) == 0:
            # ============================
            self.version = version = self.detectLatestVersionOnline()
            self.directory = "BIOGRID-OSPREY_DATASETS-%s.osprey" % version
	    #if not os.path.exists(ap("ppidata/BIOGRID-OSPREY_DATASETS-3.0.68.osprey")):
            url = "http://thebiogrid.org/downloads/archives/Release%20Archive/" + \
            "BIOGRID-%s/BIOGRID-OSPREY_DATASETS-%s.osprey.zip" % (version, version)
            print "Osprey dataset does not exist. Downloading it..."
            self.d = Downloader(url)
            self.d.finished.connect(self.assureIdentifiersExists)
            qApp.processEvents()
            self.d.exec_()
            self.setupGUI()
            
            """else:
                self.setupGUI()
                self.assureIdentifiersExists()"""
        else:
            self.setupGUI()
            self.directory = latest_osprey_dir()
            self.version = self.directory.split("-")[-1][:-7]
            self.assureIdentifiersExists()

    def assureIdentifiersExists(self):
	if not os.path.exists(self.IDENTIFIER_PATH) or not os.path.getsize(self.IDENTIFIER_PATH):
            url = "http://garr.dl.sourceforge.net/project/robinviz/identifier/identifier.db.tar.gz"            
            print "Identifiers DB does not exist. Downloading it..."
            self.iden = Downloader(url)
            self.iden.finished.connect(self.setup)
            qApp.processEvents()
            self.iden.exec_()
        else:
            self.setup(1)
    
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
		  "version": self.version,
		}
													
		checkedItems.add(ap(filename))
		
	    
	for i in range( self.treeWidget.topLevelItemCount() ):
	    topLevelItem = self.treeWidget.topLevelItem(i)
	    for c in range( topLevelItem.childCount() ):
		child = topLevelItem.child(c)
		traverse(child)

        if not checkedItems:
            return checkedItems
        
	f = open(ap("ppidata/selected_ppis.txt"), "w")
	f.write("\n".join( sorted(checkedItems) ) )
	f.close()

	self.mergePPIFiles(checkedItems)
        return checkedItems

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
        interactions = {}
        
	for organism in organisms:
	    """For each organism, create a dictionary holding (p1,p2)=>confidence
	    and store them in confidence_dicts"""
	    d = {} # to store 
            #interactions = {} # dunno why I put it here. it causes a logical bug!
            
	    for line in open(ap("ppidata/hitpredict/%s.txt" % organism )):
		cols = line.strip().split("\t")
                #print cols
                # TODO: make a normalization here. remove old normalization
                interaction = cols[0], cols[1]
                confidence = cols[2]
		d[interaction] = confidence
                # get the confidence list for the interaction
		i = interactions.get( interaction )
		if not i:
                    # if no confidence recorded yet, create an empty list
		    i = []
                i.append( float(confidence) )
                interactions[interaction] = i
                
	    confidence_dicts[organism] = d
	
	
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
                    # ignore self-interactions
		    continue

                # Find the interaction and its confidence if exists.
		interaction = cols[0], cols[1]
		confidence = confidence_dicts[organism].get(interaction)
		if not confidence:
                    # if no confidence, assign 0.1 (biogrid interaction without confidence)
                    # and add this confidence to the list as it hasn't been added
                    # in the hitpredict part above.
		    confidence = 0.1

                    # get the confidence list for the interaction
                    i = interactions.get( interaction )
                    if i:
                        # if some confidence value has been assigned,
                        # no need to add confidence 0.1 to the list
                        # as there are already more reliable confidence
                        # scores.
                        continue

                    # ELSE: (if no confidence values recorded in the hitpredict part,
                    # if no confidence recorded yet, create an empty list
                    # put the new confidence and save the new list.
                    i = [ float(confidence) ]                    
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