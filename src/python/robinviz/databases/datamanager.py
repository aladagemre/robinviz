import os
import sys
import shutil

sys.path.append("..")
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from utils.info import rp, ap, dp, latest_osprey_dir
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
    status = pyqtSignal(QString)
    
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
        self.status.emit("Succeeded")

    def fail(self):
        self.finished.emit(0)
        self.status.emit("Failed")

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
            self.status.emit("Downloaded...")
            self.downloadFinished.emit(1)
            self.extractor_thread.setup(self.TARNAME, self.DATABASE_PATH)
            self.extractor_thread.extract()
            self.status.emit("Extracting...")
        else:
            self.status.emit("Could not download!")
            self.fail()

    def succeed(self):
        Manager.succeed(self)
        os.remove(self.TARNAME)

    def fail(self):
        Manager.fail(self)
        os.remove(self.TARNAME)
        
    def download(self):
        self.status.emit("Downloading...")
        #url = "http://garr.dl.sourceforge.net/project/robinviz/identifier/identifier.db.tar.gz"
        #url = "http://www.emrealadag.com/dosyalar/identifier.db.tar.gz"
        url = "http://localhost/~emre/identifier.db.tar.gz"
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
        self.status.emit("Downloading...")
        url = "http://robinviz.googlecode.com/svn/data2/go/goinfo.sqlite3"
        self.d = Downloader(url, self.filename)
        self.d.finished.connect(self.downloaded)
        self.d.exec_()

    def downloaded(self, successful):
        if successful and os.path.exists(self.filename):
            self.status.emit("Downloaded")
            self.downloadFinished.emit(1)
            self.succeed()
        else:
            self.status.emit("Could not download!")
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
            self.status.emit("Downloaded")
            self.downloadFinished.emit(1)

            if os.path.exists(self.osprey_dir):
                shutil.rmtree(self.osprey_dir)

            self.extractor_thread.setup(ziplocation, self.osprey_dir)
            self.extractor_thread.extract()
            self.status.emit("Extracting...")
        else:
            self.status.emit("Could not download")
            self.fail()

    def download(self):
        self.status.emit("Determining the latest version...")
        self.version = version = self.detectLatestVersionOnline()
        self.directory = "BIOGRID-OSPREY_DATASETS-%s.osprey" % version
        #url = "http://thebiogrid.org/downloads/archives/Release%20Archive/" + \
        url = "http://localhost/~emre/" + \
        "BIOGRID-%s/BIOGRID-OSPREY_DATASETS-%s.osprey.zip" % (version, version)
        print "Downloading osprey dataset..."
        self.status.emit("Downloading...")
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
        # ========= HEADER =========================
        self.info = QLabel("""This tool will download the essential data from the databases and process
them afterwards.""")

        self.label_all = QLabel("All data sources")

        self.button_refresh_all= QToolButton()
        self.button_refresh_all.setIcon(QIcon(dp("images/refresh.png")))
        self.button_refresh_all.setIconSize(QSize(32,32))
        self.button_refresh_all.setToolTip("Refresh the status of all the databases")
        #self.button_refresh_all.clicked.connect(self.refresh_all)


        self.button_download_all= QToolButton()
        self.button_download_all.setIcon(QIcon(dp("images/download.png")))
        self.button_download_all.setIconSize(QSize(32,32))
        self.button_download_all.setToolTip("Download all the databases")
        self.button_download_all.clicked.connect(self.download_all)

        self.button_delete_all= QToolButton()
        self.button_delete_all.setIcon(QIcon(dp("images/delete.png")))
        self.button_delete_all.setIconSize(QSize(32,32))
        self.button_delete_all.setToolTip("Delete all the databases")

        hr = QFrame(self )
        hr.setFrameStyle( QFrame.Sunken + QFrame.HLine )
        hr.setFixedHeight( 12 )


        self.layout.addWidget(self.info, 0, 0, 1, 4)

        self.layout.addWidget(self.button_refresh_all, 1, 0)
        self.layout.addWidget(self.label_all, 1, 1)
        self.layout.addWidget(self.button_download_all, 1, 3)
        self.layout.addWidget(self.button_delete_all, 1, 4)
        
        
        self.layout.addWidget(hr, 2, 0, 1, 4)

        # ==========================================


        # ========== LIGHTS =======================
        self.i_light = StatusLight()
        self.o_light = StatusLight()
        self.g_light = StatusLight()
        self.h_light = StatusLight()
        self.a_light = StatusLight()

        for i, status in enumerate( [self.i_light, self.o_light, self.g_light, self.h_light, self.a_light] ):
            status.go_red()
            self.layout.addWidget(status, i+3, 0)
        # ==========================================
        

        # ========= DATA SOURCE NAMES ==============

        self.layout.addWidget(QLabel("Identifier translation database"), 3, 1)
        self.layout.addWidget(QLabel("Osprey PPI Network"), 4, 1)
        self.layout.addWidget(QLabel("GO Tree Information"), 5, 1)
        self.layout.addWidget(QLabel("Hitpredict PPI Network"), 6, 1)
        self.layout.addWidget(QLabel("Association Data"), 7, 1)

        # =========================================
        
        # ========== STATUS INFORMATION ===========        
        self.i_status = QLabel()
        self.o_status = QLabel()
        self.g_status = QLabel()
        self.h_status = QLabel()
        self.a_status = QLabel()



        for i, status in enumerate( [self.i_status, self.o_status, self.g_status, self.h_status, self.a_status] ):
            status.setStyleSheet("QLabel { color : blue; }");

            self.layout.addWidget(status, i+3, 2)
            
        # =========================================

        # ======= Download Buttons ===============
        self.i_download = QToolButton()
        self.o_download = QToolButton()
        self.g_download = QToolButton()
        self.h_download = QToolButton()
        self.a_download = QToolButton()

        for i, download in enumerate( [self.i_download, self.o_download, self.g_download, self.h_download, self.a_download] ):
            download.setIcon(QIcon(dp("images/download.png")))
            download.setIconSize(QSize(32,32))
            download.setToolTip("Download this data")
            self.layout.addWidget(download, i+3, 3)
        
        # =========================================

        # ======= Delete Buttons ===============
        self.i_delete = QToolButton()
        self.o_delete = QToolButton()
        self.g_delete = QToolButton()
        self.h_delete = QToolButton()
        self.a_delete = QToolButton()

        for i, delete in enumerate( [self.i_delete, self.o_delete, self.g_delete, self.h_delete, self.a_delete] ):
            delete.setIcon(QIcon(dp("images/delete.png")))
            delete.setIconSize(QSize(32,32))
            delete.setToolTip("Delete this data")
            self.layout.addWidget(delete, i+3, 4)

        # =========================================

        self.setLayout(self.layout)

    def run_managers(self, managers):
        for i in range(len(managers) - 1):
            managers[i].finished.connect(managers[i+1].run)

        managers[0].run()

    def operation_finished(self):
        self.button_download_all.setEnabled(True)
        self.button_delete_all.setEnabled(True)
        self.button_refresh_all.setEnabled(True)
    
    def download_all(self):
        self.button_download_all.setEnabled(False)
        self.button_delete_all.setEnabled(False)
        self.button_refresh_all.setEnabled(False)
        
        self.i = IdentifierManager()
        self.o = OspreyManager()
        self.g = GOManager()

        self.i.finished.connect(self.i_light.go_green)
        self.o.finished.connect(self.o_light.go_green)
        self.g.finished.connect(self.g_light.go_green)

        self.i.status.connect(self.i_status.setText)
        self.o.status.connect(self.o_status.setText)
        self.g.status.connect(self.g_status.setText)
        

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