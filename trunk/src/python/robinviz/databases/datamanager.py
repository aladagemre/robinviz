import os
import sys
import shutil

sys.path.append("..")
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from utils.info import rp, ap, latest_osprey_dir
from utils.compression import unzip_file_into_dir, untar
from utils.BeautifulSoup import BeautifulSoup
from utils.downloader import Downloader
from inputtrees.assoctree import *
from inputtrees.ppi_downloader import download_organism
#import urllib

# TODO: uncomment above, fix urls


import shutil

class Manager(QWidget):
    finished = pyqtSignal(int)
    
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.force = True
        self.name = "the"
    
    def setForce(self, value):
        self.force = value
        
    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)
        self.mybutton = QPushButton("Download %s Data" % self.name)
        self.mybutton.clicked.connect(self.download)
        self.layout.addWidget(self.mybutton)
        
    def isNeeded(self):
        if self.filename and os.path.exists(self.filename):
            return False
        return True

    def succeed(self):
        self.finished.emit(1)

    def fail(self):
        self.finished.emit(0)

class IdentifierManager(Manager):
    
    IDENTIFIER_PATH = rp("src/python/robinviz/databases/identifier.db")
    TARNAME = "identifier.db.tar.gz"
    
    def __init__(self):
        Manager.__init__(self)
        self.filename = self.IDENTIFIER_PATH
        self.name = "Identifier"
        self.setupGUI()
        
    def run(self):
        self.download()

    def downloaded(self):
        if os.path.exists(self.TARNAME):
            if self.untar():
                self.succeed()
                return
        self.fail()

    def download(self):
        #url = "http://garr.dl.sourceforge.net/project/robinviz/identifier/identifier.db.tar.gz"
        url = "http://www.emrealadag.com/dosyalar/identifier.db.tar.gz"
        self.d = Downloader(url)
        self.d.finished.connect(self.downloaded)
        self.d.exec_()

    def untar(self):
        if not os.path.exists(self.IDENTIFIER_PATH) and os.path.exists(self.TARNAME):
            try:
                print "Uncompressing %s" % self.TARNAME
                untar(self.TARNAME)
                shutil.move("identifier.db", self.IDENTIFIER_PATH)
                os.remove(self.TARNAME)
                return True
            except:
                print "Could not untar %s" % self.TARNAME

        return False


class GOManager(Manager):
    def __init__(self):
        Manager.__init__(self)
        self.filename = ap("godata/goinfo.sqlite3")
        self.name = "GO"
        self.setupGUI()
        
    def run(self):
        if not self.force and not self.isNeeded():
            print "No need to run GO Manager, file exists."
            self.succeed()
            return
        self.download()

    def download(self):
        url = "http://robinviz.googlecode.com/svn/data2/go/goinfo.sqlite3"
        self.d = Downloader(url, self.filename)
        self.d.finished.connect(self.downloaded)
        self.d.exec_()

    def downloaded(self):
        if os.path.exists(self.filename):
            self.succeed()
        else:
            self.fail()


class OspreyManager(Manager):
    def __init__(self):
        Manager.__init__(self)
        self.filename = latest_osprey_dir()
        self.name = "Osprey PPI"
        self.setupGUI()
    
    def run(self):
        self.download()
        
    def downloaded(self, successful):
        if successful:
            if self.unzip():
                self.succeed()
                return
        self.fail()

    def unzip(self):
        self.osprey_dir = ap("ppidata/%s" % self.directory )
        ziplocation = "%s.zip" % self.directory

        if os.path.exists(self.osprey_dir) and os.path.exists(ziplocation):
            shutil.rmtree(self.osprey_dir)
            
        if os.path.exists(ziplocation):
            try:
                print "Unzipping",ziplocation
                unzip_file_into_dir(ziplocation, self.osprey_dir)
                os.remove(ziplocation)
                return True
            except:
                print "Could not unzip", ziplocation

        return False

    def download(self):
        self.version = version = self.detectLatestVersionOnline()
        self.directory = "BIOGRID-OSPREY_DATASETS-%s.osprey" % version
        #url = "http://thebiogrid.org/downloads/archives/Release%20Archive/" + \
        url = "http://localhost/~emre/" + \
        "BIOGRID-%s/BIOGRID-OSPREY_DATASETS-%s.osprey.zip" % (version, version)
        print "Downloading osprey dataset..."
        self.d = Downloader(url)
        self.d.finished.connect(self.downloaded)
        self.d.exec_()
        
    def detectLatestVersionOnline(self):
        try:
            html = urllib.urlopen("http://thebiogrid.org/").read()
            soup = BeautifulSoup(html)
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


class DataManager(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.o = OspreyManager()
        self.setupGUI()

    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.mybutton = QPushButton("Run Managers")
        self.mybutton.clicked.connect(self.download_all)
        self.layout.addWidget(self.mybutton)
        self.setLayout(self.layout)

    def run_managers(self, managers):
        for i in range(len(managers) - 1):
            managers[i].finished.connect(managers[i+1].run)

        managers[0].run()

    def download_all(self):
        self.o = OspreyManager()

        self.g = GOManager()
        self.g.setForce(False)

        self.i = IdentifierManager()
        
        self.run_managers([self.o, self.g, self.i])
        """
        self.download_goinfo()
        self.download_identifiers()
        self.download_osprey()
        self.download_hitpredicts()
        self.download_associations()"""

    def download_goinfo(self):
        pass

    def download_identifiers(self):
        pass



    def download_hitpredicts(self):
        organisms = os.listdir(latest_osprey_dir())
        map (download_organism, organisms)

    def download_associations(self):
        pass

    def setup(self, result):
        self.osprey_dir = ap("ppidata/%s" % self.directory )
        ziplocation = "%s.zip" % self.directory

        if not os.path.exists(self.osprey_dir) and os.path.exists(ziplocation):
            unzip_file_into_dir(ziplocation, self.osprey_dir)
            os.remove(ziplocation)

        # ======IDENTIFIERS========
        if not os.path.exists(self.IDENTIFIER_PATH) and os.path.exists("identifier.db.tar.gz"):
            print "Uncompressing identifier.db.tar.gz"
            untar("identifier.db.tar.gz")
            shutil.move("identifier.db", self.IDENTIFIER_PATH)
            os.remove("identifier.db.tar.gz")

        # =========================

        self.readPPIData()
	self.useDictionary(self.organism_experiments)

    def assureIdentifiersExists(self):
	if not os.path.exists(self.IDENTIFIER_PATH):
            url = "http://garr.dl.sourceforge.net/project/robinviz/identifier/identifier.db.tar.gz"
            print "Identifiers DB does not exist. Downloading it..."
            self.iden = Downloader(url)
            self.iden.finished.connect(self.setup)
            qApp.processEvents()
            self.iden.exec_()
        else:
            self.setup(1)


def main():
    app = QApplication(sys.argv)
    mainWindow = DataManager()
    mainWindow.show()
    sys.exit(app.exec_())

main()