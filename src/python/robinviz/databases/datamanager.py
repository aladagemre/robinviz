import os
import sys
import shutil
#import urllib
# TODO: uncomment above, fix urls
from functools import partial

sys.path.append("..")
from PyQt4.QtGui import *
from PyQt4.QtCore import *

from utils.info import rp, ap, pp, latest_osprey_dir
from utils.compression import Extractor
from utils.BeautifulSoup import BeautifulSoup
from utils.downloader import Downloader, MultiDownloader

from inputtrees.assoctree import *
from inputtrees.ppi_downloader import get_hc_url, get_lc_url, get_ss_url
from inputtrees.hp_to_tabbedppi import tabify

from databases.osprey2biogrid import ConverterThread, OSPREY_ORGANISMS

class Manager(QWidget):
    finished = pyqtSignal(int)
    downloadFinished = pyqtSignal(int)
    status = pyqtSignal(QString)
    
    def __init__(self, parent=None):
        QWidget.__init__(self, parent=None)
        self.force = True
        self.name = "the"
    
    def setForce(self, value):
        self.force = value
        
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
    
    def __init__(self, parent=None, datamanager=None):
        Manager.__init__(self, parent=None)
        self.filename = self.IDENTIFIER_PATH
        self.name = "Identifier"
        self.datamanager = datamanager
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
            self.extractor_thread.finished.connect(self.datamanager.o.convert2biogrid)
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
        #os.remove(self.TARNAME)
        
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
        Manager.__init__(self, parent=None)
        self.filename = ap("godata/goinfo.sqlite3")
        self.name = "GO"
        
    def run(self):
        if not self.force and not self.isNeeded():
            print "No need to run GO Manager, file exists."
            self.succeed()
            return
        self.download()

    def download(self):
        self.status.emit("Downloading...")
        #url = "http://robinviz.googlecode.com/svn/data2/go/goinfo.sqlite3"
        url = "http://localhost/~emre/goinfo.sqlite3"
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
    def __init__(self, parent=None, datamanager=None):
        Manager.__init__(self, parent=None)
        self.filename = latest_osprey_dir()
        self.name = "Osprey PPI"
        self.datamanager = datamanager

        self.extractor_thread = Extractor()
        #self.extractor_thread.finished.connect(self.succeed)
        self.extractor_thread.terminated.connect(self.fail)
        
    def run(self):
        self.download()

    def succeed(self):
        Manager.succeed(self)
        try:
            os.remove(self.ziplocation)
        except:
            pass

    def fail(self):
        Manager.fail(self)
        try:
            os.remove(self.ziplocation)
        except:
            pass

    def convert2biogrid(self):
        if latest_osprey_dir() and os.path.exists(IdentifierManager.IDENTIFIER_PATH):
            self.status.emit("Converting PPI data to Biogrid annotation...")
            self.converter_thread = ConverterThread()
            self.converter_thread.status.connect(self.status.emit)
            self.converter_thread.done.connect(self.succeed)
            self.converter_thread.start()
            self.status.emit("Conversion Finished...")
            self.succeed()
        else:
            self.fail()
            self.status.emit("Could not convert PPI data to Biogrid annotation. Check Identifier and Osprey data.")
            
        
    def downloaded(self, successful):
        self.osprey_dir = ap("ppidata/%s" % self.directory )
        self.ziplocation = "%s.zip" % self.directory

        if successful and os.path.exists(self.ziplocation):
            self.status.emit("Downloaded")
            self.downloadFinished.emit(1)

            if os.path.exists(self.osprey_dir):
                shutil.rmtree(self.osprey_dir)

            self.extractor_thread.setup(self.ziplocation, self.osprey_dir)
            self.extractor_thread.finished.connect(self.extracted)
            self.extractor_thread.extract()
            self.status.emit("Extracting...")
        else:
            self.status.emit("Could not download")
            self.fail()

    def extracted(self):
        self.status.emit("Extracted.")
        if not self.datamanager.i.extractor_thread.isRunning():
            self.status.emit("Converting PPI data to Biogrid annotation...")
            self.convert2biogrid()
        
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

class HitpredictManager(Manager):
    HITPREDICT_DIR = ap("ppidata/hitpredict")
    
    def __init__(self, parent=None):
        Manager.__init__(self, parent=None)
        self.extractor_thread = Extractor()

    def run(self):
        """pool = Pool(processes = 4)
        pool.map(download_organism, OSPREY_ORGANISMS)"""
        self.download_organism(OSPREY_ORGANISMS[0])
        
    def download_organism(self, organism_name):
        targzs = []
        urls = []
        for function in (get_hc_url, get_lc_url, get_ss_url):
            # Small scale should be last! hc will define max confidence. ss will take that value instead of t
            url = function(organism_name)
            urls.append(url)
            targzs.append(url.split("/")[-1])

        self.md = MultiDownloader()
        self.md.set_files(urls)
        self.md.finished.connect(lambda files: self.process_organism(files, organism_name))
        self.md.start()

    def process_organism(self, files, organism_name):
        self.status.emit("Downloaded %s, extracting..." % organism_name)
        for file in files:
            self.extractor_thread.setup(file)
            self.extractor_thread.extract()

    def tabify(self):
        self.status.emit("Processing %s..." % organism_name)
        # TODO: make tabify a process and connect finished/terminated to succeeded and failed
        tabify(map(lambda f: f[:-7], files), organism_name)
        self.status.emit("Processing %s finished." % organism_name)
        
    def succeed(self):
        Manager.succeed(self)
        self.cleanup()
    def fail(self):
        Manager.fail(self)
        self.cleanup()
        
    def cleanup(self):
        files = os.listdir(self.HITPREDICT_DIR)
        residues = filter(lambda filename: filename.endswith("_interactions") or filename.endswith(".tar.gz"), files)
        for residue in residues:
            os.remove(residue)
        
    def delete(self):
        try:
            files = os.listdir(self.HITPREDICT_DIR)
            map(os.remove, files)
        except:
            pass


class StatusLight(QLabel):
    def __init__(self, parent=None):
        QLabel.__init__(self,parent)
        self.setFixedSize(32, 32)
        
    def fill_color(self, color):
        pixmap = QPixmap(self.size())
        pixmap.fill(color)
        self.setPixmap(pixmap)

    def go_green(self):
        self.fill_color(Qt.green)
    def go_red(self):
        self.fill_color(Qt.red)

    def reflect_result(self, result):
        if result:
            self.go_green()
        else:
            self.go_red()
        
class DataManager(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent=None)
        self.setWindowTitle("Data Manager")
        self.createManagers()

        # TODO: Move this part into the managers as attributes
        self.data_dict = {
            'i': [self.i.IDENTIFIER_PATH],
            'o': [ap("ppidata/%s" % latest_osprey_dir())],
            'g': [ap('godata/goinfo.sqlite3')],
            #'h': map( lambda x: ap('ppidata/hitpredict')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('ppidata/hitpredict')))),
            #'a': map( lambda x: ap('assocdata')+"/"+ x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('assocdata') )  ) ),
            #'geo': map( lambda x: ap('geodata')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('geodata') )  ) ),
        }
        self.setupGUI()
        self.refresh_all()

    def isDownloadNeeded(self):
        for path in self.data_dict.values():
            if not os.path.exists(path[0]):
                return True
        return False


                
    def createManagers(self):
        self.i = IdentifierManager(datamanager=self)
        self.o = OspreyManager(datamanager=self)
        self.g = GOManager()
    def setupGUI(self):
        self.layout = QGridLayout()
        # ========= HEADER =========================
        self.info = QLabel("""This tool will download the essential data from the databases and process
them afterwards.""")

        self.label_all = QLabel("All data sources")

        self.button_refresh_all= QToolButton()
        self.button_refresh_all.setIcon(QIcon(pp("misc/images/refresh.png")))
        self.button_refresh_all.setIconSize(QSize(32,32))
        self.button_refresh_all.setToolTip("Refresh the status of all the databases")
        self.button_refresh_all.clicked.connect(self.refresh_all)


        self.button_download_all= QToolButton()
        self.button_download_all.setIcon(QIcon(pp("misc/images/download.png")))
        self.button_download_all.setIconSize(QSize(32,32))
        self.button_download_all.setToolTip("Download all the databases")
        self.button_download_all.clicked.connect(self.download_all)

        self.button_delete_all= QToolButton()
        self.button_delete_all.setIcon(QIcon(pp("misc/images/delete.png")))
        self.button_delete_all.setIconSize(QSize(32,32))
        self.button_delete_all.setToolTip("Delete all the databases")
        self.button_delete_all.clicked.connect(self.delete_all)

        
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
        # Create 3 light objects
        self.i_light, self.o_light, self.g_light =  map(StatusLight, [None]*3 )

        for i, status in enumerate( [self.i_light, self.o_light, self.g_light] ):
            status.go_red()
            self.layout.addWidget(status, i+3, 0)
        # ==========================================
        

        # ========= DATA SOURCE NAMES ==============

        self.layout.addWidget(QLabel("Identifier translation database"), 3, 1)
        self.layout.addWidget(QLabel("Osprey PPI Network"), 4, 1)
        self.layout.addWidget(QLabel("GO Tree Information"), 5, 1)
        #self.layout.addWidget(QLabel("Hitpredict PPI Network"), 6, 1)
        #self.layout.addWidget(QLabel("Association Data"), 7, 1)

        # =========================================
        
        # ========== STATUS INFORMATION ===========
        # Create 3 status label objects
        self.i_status, self.o_status, self.g_status = map(QLabel, [None]*3 )

        for i, status in enumerate( [self.i_status, self.o_status, self.g_status] ):
            status.setStyleSheet("QLabel { color : blue; }");
            status.setWordWrap(True)
            self.layout.addWidget(status, i+3, 2)
            
        # =========================================

        # ======= Download Buttons ===============
        # Create 3 download button objects
        self.i_download, self.o_download, self.g_download = map(QToolButton, [None]*3 )
        
        for i, letter in enumerate( ["i","o","g"] ):
            download = self.__dict__["%s_download" % letter]
            slot = partial(self.download_one, letter=letter)
            download.clicked.connect(slot)
            download.setIcon(QIcon(pp("misc/images/download.png")))
            download.setIconSize(QSize(32,32))
            download.setToolTip("Download this data")
            self.layout.addWidget(download, i+3, 3)
        
        # =========================================

        # ======= Delete Buttons ===============
        # Create 5 delete button objects
        self.i_delete, self.o_delete, self.g_delete = map(QToolButton, [None]*3 )

        for i, letter in enumerate( ["i","o","g"] ):
            delete = self.__dict__["%s_delete" % letter]
            slot = partial(self.delete_one, letter=letter)
            delete.clicked.connect(slot)
            delete.setIcon(QIcon(pp("misc/images/delete.png")))
            delete.setIconSize(QSize(32,32))
            delete.setToolTip("Delete this data")
            self.layout.addWidget(delete, i+3, 4)

        # =========================================

        self.setLayout(self.layout)


    def download_one(self, letter):
        manager = self.__dict__[letter]
        light = self.__dict__["%s_light" % letter]
        status = self.__dict__["%s_status" % letter]

        manager.finished.connect(light.reflect_result)
        manager.status.connect(status.setText)
        manager.run()

    def delete_one(self, letter):
        #manager = self.__dict__[letter]
        light = self.__dict__["%s_light" % letter]
        status = self.__dict__["%s_status" % letter]

        files = self.data_dict[letter]

        
        for file in files:
            try:
                if os.path.isdir(file):
                    shutil.rmtree(file)
                else:
                    os.remove(file)
            except:
                print "Could not remove", file

        light.go_red()
        status.setText("Deleted.")

        
    def operation_finished(self):
        self.i.finished.connect(lambda: None)
        self.o.finished.connect(lambda: None)
        self.g.finished.connect(lambda: None)
        self.i.downloadFinished.connect(lambda: None)
        self.o.downloadFinished.connect(lambda: None)
        self.g.finished.connect(lambda: None)

        self.button_download_all.setEnabled(True)
        self.button_delete_all.setEnabled(True)
        self.button_refresh_all.setEnabled(True)


    def refresh_all(self):
        for letter, files in self.data_dict.iteritems():
            light = self.__dict__.get("%s_light" % letter)
            if not light:
                continue
            status = self.__dict__["%s_status" % letter]

            if letter in ("i", "o", "g"):
                if os.path.exists(files[0]):
                    light.go_green()
                    status.setText("Exists.")
                else:
                    light.go_red()
                    status.setText("Does not exist.")
            elif letter == "h":
                pass
            elif letter == "a":
                pass

    def delete_all(self):
        response= QMessageBox.warning(self, 'Update Local Data',
     "This operation will remove the local data. You will have to perform a re-download. "+ \
     "Hitpredict PPI and Gene Expression data will also be removed. \n\n" + \
     "Are you sure you want to do this?", buttons=QMessageBox.Yes|QMessageBox.No)

        if response == QMessageBox.Yes:
            # do the operation
            data = [ self.i.IDENTIFIER_PATH,
                     ap('godata/goinfo.sqlite3')
            ]
            osprey_dir = latest_osprey_dir()
            if osprey_dir:
                data.append( ap(  "ppidata/%s" % osprey_dir ) )
            data += map( lambda x: ap('assocdata')+"/"+ x, filter ( lambda x: not x.startswith(".") and not os.path.isdir(x),  os.listdir( ap('assocdata') )  ) )
            data += map( lambda x: ap('ppidata/hitpredict')+"/"+x, filter ( lambda x: not x.startswith(".") and not os.path.isdir(x),  os.listdir( ap('ppidata/hitpredict') )  ) )
            data += map( lambda x: ap('geodata')+"/"+x, filter ( lambda x: not x.startswith(".") and not os.path.isdir(x),  os.listdir( ap('geodata') )  ) )

            data = set(data) - set(["/home/emre/robinviz/trunk/src/python/robinviz/databases/inputtrees/assocdata/index.txt",
                                    "/home/emre/robinviz/trunk/src/python/robinviz/databases/inputtrees/geodata/geoTreeView.txt"])
            
            for i in data:
                print "Removing",i
                try:
                    if os.path.isdir(i):
                        shutil.rmtree(i)
                    else:
                        os.remove(i)
                except:
                    print "Could not remove",i

            self.refresh_all()
    def download_all(self):
        self.button_download_all.setEnabled(False)
        self.button_delete_all.setEnabled(False)
        self.button_refresh_all.setEnabled(False)

        self.i.finished.connect(self.i_light.go_green)
        self.o.finished.connect(self.o_light.go_green)
        self.g.finished.connect(self.g_light.go_green)

        self.i.status.connect(self.i_status.setText)
        self.o.status.connect(self.o_status.setText)
        self.g.status.connect(self.g_status.setText)
        

        # After Identifiers are downloaded, download osprey
        self.o.downloadFinished.connect(self.g.run)
        # After osprey is downloaded, download goinfo.sqlite3
        self.g.downloadFinished.connect(self.i.run)
        # After goinfo is downloaded, download hitpredict
        pass # replace goinfo&hitpredict
        # After hitpredict is downloaded,

        self.i.finished.connect(self.operation_finished)
        self.o.run()


def main():
    app = QApplication(sys.argv)
    mainWindow = DataManager()
    mainWindow.show()
    """hp = HitpredictManager()
    hp.run()"""
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()