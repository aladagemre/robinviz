import os
import sys
import shutil

sys.path.append("..")
from PyQt4.QtGui import *
from PyQt4.QtCore import *
from utils.info import rp, ap, pp, latest_osprey_dir
from utils.compression import unzip_file_into_dir, untar, Extractor
from utils.BeautifulSoup import BeautifulSoup
from utils.downloader import Downloader
from inputtrees.assoctree import *
from inputtrees.ppi_downloader import download_organism
from functools import partial
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
        """self.layout = QVBoxLayout()
        self.setLayout(self.layout)
        self.mybutton = QPushButton("Download %s Data" % self.name)
        self.mybutton.clicked.connect(self.download)
        self.layout.addWidget(self.mybutton)"""
        pass
        
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
        print "Successful:", successful

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

    def succeed(self):
        Manager.succeed(self)
        os.remove(self.ziplocation)

    def fail(self):
        Manager.fail(self)
        os.remove(self.ziplocation)

    def downloaded(self, successful):
        self.osprey_dir = ap("ppidata/%s" % self.directory )
        self.ziplocation = "%s.zip" % self.directory

        if successful and os.path.exists(self.ziplocation):
            self.status.emit("Downloaded")
            self.downloadFinished.emit(1)

            if os.path.exists(self.osprey_dir):
                shutil.rmtree(self.osprey_dir)

            self.extractor_thread.setup(self.ziplocation, self.osprey_dir)
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
        self.setFixedSize(32, 32)
        
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
        self.createManagers()

        # TODO: Move this part into the managers as attributes
        self.data_dict = {
            'i': [self.i.IDENTIFIER_PATH],
            'o': [ap("ppidata/%s" % latest_osprey_dir())],
            'g': [ap('godata/goinfo.sqlite3')],
            'h': map( lambda x: ap('ppidata/hitpredict')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('ppidata/hitpredict')))),
            'a': map( lambda x: ap('assocdata')+"/"+ x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('assocdata') )  ) ),
            'geo': map( lambda x: ap('geodata')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('geodata') )  ) ),
        }
        self.setupGUI()
        self.refresh_all()

    def createManagers(self):
        self.i = IdentifierManager()
        self.o = OspreyManager()
        self.g = GOManager()
        self.h = GOManager()
        self.a = GOManager()
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
        # Create 5 light objects
        self.i_light, self.o_light, self.g_light, self.h_light, self.a_light = map(StatusLight, [None]*5 )

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
        # Create 5 status label objects
        self.i_status, self.o_status, self.g_status, self.h_status, self.a_status = map(QLabel, [None]*5 )

        for i, status in enumerate( [self.i_status, self.o_status, self.g_status, self.h_status, self.a_status] ):
            status.setStyleSheet("QLabel { color : blue; }");
            self.layout.addWidget(status, i+3, 2)
            
        # =========================================

        # ======= Download Buttons ===============
        # Create 5 download button objects
        self.i_download, self.o_download, self.g_download, self.h_download, self.a_download = map(QToolButton, [None]*5 )
        
        for i, letter in enumerate( ["i","o","g","h","a"] ):
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
        self.i_delete, self.o_delete, self.g_delete, self.h_delete, self.a_delete = map(QToolButton, [None]*5 )

        for i, letter in enumerate( ["i","o","g","h","a"] ):
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

        manager.finished.connect(light.go_green)
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
                return
            status = self.__dict__["%s_status" % letter]

            if letter in ("i", "o", "g"):
                if os.path.exists(files[0]):
                    light.go_green()
                else:
                    light.go_red()
            elif letter == "h":
                pass
            elif letter == "a":
                pass

    def delete_all(self):
        response= QMessageBox.warning(self, 'Update Local Data',
     "This operation will remove the local data. You will have to perform a re-download.\n\n"+
     "Are you sure you want to do this?", buttons=QMessageBox.Yes|QMessageBox.No)

        if response == QMessageBox.Yes:
            # do the operation
            data = [ latest_osprey_dir(),
                     ap('godata/goinfo.sqlite3')
            ]
            data += map( lambda x: ap('assocdata')+"/"+ x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('assocdata') )  ) )
            data += map( lambda x: ap('ppidata/hitpredict')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('ppidata/hitpredict') )  ) )
            data += map( lambda x: ap('geodata')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('geodata') )  ) )

            for i in data:
                print "Removing",i
                try:
                    os.remove(i)
                except:
                    print "Could not remove",i


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

if __name__ == "__main__":
    main()