
"""Taken from python-jake project which is protected under GPL and modified
http://sourceforge.net/projects/python-jake
"""

import sys, os, urllib
from threading import Thread
from functools import partial
from compression import Extractor

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtNetwork import *
class Downloader(QProgressDialog):
    def __init__(self, url, target=None, parent=None):
        QProgressDialog.__init__(self, parent)
        self.parent = parent
        self.url = url
        if not target:
            target = url.split("/")[-1].split("?")[0]
        self.downloadPath = target

        self.setWindowModality(Qt.ApplicationModal)
        self.setWindowTitle("Downloading, please wait...")
        self.text = "Downloading:\n%s\n" % url
        self.setLabelText(self.text)
        self.manager = QNetworkAccessManager()
        self.resize(self.size().width(), self.size().height())
        self.manager.finished.connect(self.downloadFinished)
        self.canceled.connect(self.cancelDownload)
        self.download()

    def download(self):
        print "Downloading: %s" % self.url
        self.timer = QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.updateSpeed)
        self.last_done_size = 0
        self.speed = 0.0
        
        self.url = QUrl(self.url)
        self.reply = self.manager.get(QNetworkRequest(self.url))
        self.reply.downloadProgress.connect(self.updateDataReadProgress)

        self.timer.start()
        
    def updateSpeed(self):
        current_size = self.value()
        self.speed = (current_size - self.last_done_size)/1024
        self.last_done_size = current_size
        
    def updateDataReadProgress(self, done, total):
        #print "UPDATE: " + str(done) + " of " + str(total)
        self.total = total
        self.setMaximum(total)
        self.setValue(done)
        
        d = done/1024
        t = total/1024

        if total == -1:
            t = "Unknown"

        self.setLabelText(self.text + "%s KB / %s KB (%s KB/s)" % (d, t, self.speed))

    def downloadFinished(self, reply):
        print "downloadFinished"
        redirect = reply.attribute(QNetworkRequest.RedirectionTargetAttribute).toUrl()
        error = reply.attribute(QNetworkRequest.HttpStatusCodeAttribute).toInt()
        #print "ERROR: " + str(error)
        #print "HEADER: " + str(reply.header(2).toString())

        if error[0] == 404:
            errorDialog = QErrorMessage()
            errorDialog.showMessage("File does not exist:\n%s" % str(self.url.toString()))
            errorDialog.exec_()
            reply.deleteLater()
            self.reject()
            return


        if not redirect.isEmpty():
            print "REDIRECT: " + str(redirect)
            self.reply = self.manager.get(QNetworkRequest(redirect))
            self.reply.downloadProgress.connect(self.updateDataReadProgress)
            reply.deleteLater()
            self.reject()
        else:
            self.updateFile = QFile(self.downloadPath)
            self.updateFile.open(QIODevice.WriteOnly)
            self.updateFile.write(self.reply.readAll())
            self.updateFile.close()
            self.reply.deleteLater()
            self.manager.deleteLater()
            reply.deleteLater()
            downloaded_size = os.path.getsize(self.downloadPath)
            if downloaded_size and self.total != -1 and downloaded_size == self.total:
                print "Finished."
                self.accept()
            else:
                self.reject()
        
    def cancelDownload(self):
        print "Download aborted."
        self.reply.abort()
        try:
            os.remove(self.downloadPath)
        except IOError,e :
            print "%s was not saved, won't delete it." % self.downloadPath

class DownloadAndExtract(QThread):
    def __init__(self, parent = None):
        QThread.__init__(self, parent)
        self.d = None
        self.e = Extractor()

    def __del__(self):
        print "deleting d&e"
        self.e.wait()
        #self.wait()

    def run(self):
        self.d = Downloader(self.url)
        self.d.finished.connect(self.extract)
        self.d.show()

    def extract(self, successful):
        if successful:
            filename = self.url.split("/")[-1].split("?")[0]
            self.e.setup(filename)
            self.e.extract()
        
    def start(self, url):
        self.url = url
        self.run()

class MultiDownloader(QObject):
    finished = pyqtSignal('QList<QString>')
    
    def __init__(self):
        QObject.__init__(self)
        self.files = []
        self.d = None
        
    def set_files(self, files):
        self.files = files

    def download(self, index):
        del self.d
        print index
        try:
            file = self.files[index]
        except:
            self.finished.emit([file.split("/")[-1] for file in self.files] )
            return
        self.d = Downloader(file)
        self.d.show()
        print "creating func"
        func = partial(self.download, index=index+1)
        print "connecting"
        self.d.finished.connect(func)
        print "connected"
        
    def start(self):
        self.download(0)

class MultiDownloadAndExtract(QThread):
    finished = pyqtSignal('QList<QString>')

    def __init__(self, parent = None):
        QThread.__init__(self, parent)
        self.files = []
        self.threads = []

    def set_files(self, files):
        self.files = files

    def run(self):
        for file in self.files:
            d = DownloadAndExtract()
            self.threads.append(d)
            d.start(file)

    def __del__(self):
        for thread in self.threads:
            thread.wait()



def test(sonuc):
    print sonuc
if __name__ == "__main__":
    app = QApplication(sys.argv)
    #window = Downloader("http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.jcvi_Aphagocytophilum.gz?rev=HEAD", "/home/emre/Desktop/gene_association.jcvi_Aphagocytophilum.gz")
    #window = Downloader("http://sourceforge.net/projects/python-jake/files/betas/Jake-PyQT4-0.1.zip/download", "test.zip")
    #window = Downloader("http://localhost/~emre/identifier.db.tar.gz")
    #window = MultiDownloader()
    #window.download("http://localhost/~emre/identifier.db.tar.gz", "/home/emre/Desktop/identifier.db.tar.gz")
    #window.finished.connect(test)

    m = MultiDownloadAndExtract()
    m.set_files(["http://localhost/~emre/identifier.db.tar.gz", "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.jcvi_Aphagocytophilum.gz?rev=HEAD"])
    m.run()

    #d = DownloadAndExtract()
    #d.start("http://localhost/~emre/identifier.db.tar.gz")
    sys.exit(app.exec_())