import os
import sys
import shutil

sys.path.append("..")
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from utils.info import rp, ap, latest_osprey_dir
from utils.compression import unzip_file_into_dir, untar, Extractor
from utils.BeautifulSoup import BeautifulSoup
from utils.downloader import Downloader
from inputtrees.assoctree import *
from inputtrees.ppi_downloader import download_organism
#import urllib

# TODO: uncomment above, fix urls


import shutil

class Manager(QWidget):
    finished = pyqtSignal(int)
    downloadFinished = pyqtSignal(int)
    
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
    DATABASE_PATH = rp("src/python/robinviz/databases")
    
    def __init__(self):
        Manager.__init__(self)
        self.filename = self.IDENTIFIER_PATH
        self.name = "Identifier"
        self.setupGUI()
        self.extractor_thread = Extractor()
        self.extractor_thread.finished.connect(self.succeed)
        self.extractor_thread.terminated.connect(self.fail)
        
    def run(self):
        self.download()

    def downloaded(self, successful):
        if successful and os.path.exists(self.TARNAME):
            self.downloadFinished.emit(1)
            self.extractor_thread.setup(self.TARNAME, self.DATABASE_PATH)
            self.extractor_thread.extract()
        else:
            self.fail()

    def succeed(self):
        Manager.succeed(self)
        os.remove(self.TARNAME)

    def fail(self):
        Manager.fail(self)
        os.remove(self.TARNAME)
        
    def download(self):
        #url = "http://garr.dl.sourceforge.net/project/robinviz/identifier/identifier.db.tar.gz"
        url = "http://www.emrealadag.com/dosyalar/identifier.db.tar.gz"
        #url = "http://localhost/~emre/identifier.db.tar.gz"
        self.d = Downloader(url)
        self.d.finished.connect(self.downloaded)
        self.d.exec_()

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

    def downloaded(self, successful):
        if successful and os.path.exists(self.filename):
            self.downloadFinished.emit(1)
            self.succeed()
        else:
            self.fail()


class OspreyManager(Manager):
    def __init__(self):
        Manager.__init__(self)
        self.filename = latest_osprey_dir()
        self.name = "Osprey PPI"
        self.setupGUI()
        
        self.extractor_thread = Extractor()
        self.extractor_thread.finished.connect(self.succeed)
        self.extractor_thread.terminated.connect(self.fail)
        
    def run(self):
        self.download()
        
    def downloaded(self, successful):
        self.osprey_dir = ap("ppidata/%s" % self.directory )
        ziplocation = "%s.zip" % self.directory

        if successful and os.path.exists(ziplocation):
            self.downloadFinished.emit(1)

            if os.path.exists(self.osprey_dir):
                shutil.rmtree(self.osprey_dir)

            self.extractor_thread.setup(ziplocation, self.osprey_dir)
            self.extractor_thread.extract()
            
            """if self.unzip():
                self.succeed()
                return"""
        else:
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


class StatusLight(QLabel):
    def __init__(self, parent=None):
        QLabel.__init__(self,parent)
        self.setFixedSize(40, 40)
        
    def fill_color(self, color):
        pixmap = QPixmap(self.size())
        pixmap.fill(color)
        self.setPixmap(pixmap)

    def go_green(self):
        self.fill_color(Qt.green)
    def go_red(self):
        self.fill_color(Qt.red)
        
class DataManager(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setWindowTitle("Data Manager")
        self.setupGUI()

    def setupGUI(self):
        self.layout = QGridLayout()
        self.info = QLabel("""This tool will download the essential data from the databases and process
them afterwards.""")
        self.button = QPushButton("Start operation")
        self.button.clicked.connect(self.download_all)
        self.layout.addWidget(self.info, 0, 0, 1, 2)
        self.layout.addWidget(self.button, 1, 0, 1, 2)
        self.layout.addWidget(QLabel("Identifier translation database"), 2, 0)
        self.layout.addWidget(QLabel("Osprey PPI Network"), 3, 0)
        self.layout.addWidget(QLabel("GO Tree Information"), 4, 0)
        self.layout.addWidget(QLabel("Hitpredict PPI Network"), 5, 0)
        self.layout.addWidget(QLabel("Association Data"), 6, 0)


        self.i_status = StatusLight()
        self.o_status = StatusLight()
        self.g_status = StatusLight()
        self.h_status = StatusLight()
        self.a_status = StatusLight()

        for i, status in enumerate( [self.i_status, self.o_status, self.g_status, self.h_status, self.a_status] ):
            status.go_red()
            self.layout.addWidget(status, i+2, 1)
        
        self.setLayout(self.layout)

    def run_managers(self, managers):
        for i in range(len(managers) - 1):
            managers[i].finished.connect(managers[i+1].run)

        managers[0].run()

    def operation_finished(self):
        self.button.setEnabled(True)
    
    def download_all(self):
        self.button.setEnabled(False)
        
        self.o = OspreyManager()
        self.i = IdentifierManager()
        self.g = GOManager()

        self.o.finished.connect(self.o_status.go_green)
        self.i.finished.connect(self.i_status.go_green)
        self.g.finished.connect(self.g_status.go_green)
        

        # After Identifiers are downloaded, download osprey
        self.i.downloadFinished.connect(self.o.run)
        # After osprey is downloaded, download goinfo.sqlite3
        self.o.downloadFinished.connect(self.g.run)
        # After goinfo is downloaded, download hitpredict
        pass # replace goinfo&hitpredict
        # After hitpredict is downloaded,

        self.g.finished.connect(self.operation_finished)
        self.i.run()

    def download_hitpredicts(self):
        organisms = os.listdir(latest_osprey_dir())
        map (download_organism, organisms)


def main():
    app = QApplication(sys.argv)
    mainWindow = DataManager()
    mainWindow.show()
    sys.exit(app.exec_())

main()