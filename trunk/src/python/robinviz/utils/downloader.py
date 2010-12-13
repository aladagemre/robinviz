
"""Taken from python-jake project which is protected under GPL and modified
http://sourceforge.net/projects/python-jake
"""

import sys, os
from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtNetwork import *
class Downloader(QProgressDialog):
    def __init__(self, url, target=None, parent=None):
        QProgressDialog.__init__(self, parent)
        self.parent = parent
        self.url = url
        if not target:
            target = url.split("/")[-1]
        self.downloadPath = target

        self.setWindowModality(Qt.ApplicationModal)
        self.setWindowTitle("Downloading, please wait...")
        self.text = "Downloading:\n%s\n" % url
        self.setLabelText(self.text)
        self.manager = QNetworkAccessManager()
        self.resize(self.size().width()*2, self.size().height())
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
        self.setMaximum(total)
        self.setValue(done)
        
        d = done/1024
        t = total/1024

        if total == -1:
            t = "Unknown"

        self.setLabelText(self.text + "%s KB / %s KB (%s KB/s)" % (d, t, self.speed))

    def downloadFinished(self, reply):
        redirect = reply.attribute(QNetworkRequest.RedirectionTargetAttribute).toUrl()
        error = reply.attribute(QNetworkRequest.HttpStatusCodeAttribute).toInt()
        #print "ERROR: " + str(error)
        #print "HEADER: " + str(reply.header(2).toString())
        
        if error[0] == 404:
            errorDialog = QErrorMessage()
            errorDialog.showMessage("File does not exist:\n%s" % str(self.url.toString()))
            errorDialog.exec_()
            self.reject()
            return


        if not redirect.isEmpty():
            print "REDIRECT: " + str(redirect)
            self.reply = self.manager.get(QNetworkRequest(redirect))
            self.reply.downloadProgress.connect(self.updateDataReadProgress)
        else:
            self.updateFile = QFile(self.downloadPath)
            self.updateFile.open(QIODevice.WriteOnly)
            self.updateFile.write(self.reply.readAll())
            self.updateFile.close()
            self.reply.deleteLater()
            self.manager.deleteLater()
        reply.deleteLater()

        self.accept()
        

    def cancelDownload(self):
        self.reply.abort()

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    window = Downloader("http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.jcvi_Aphagocytophilum.gz?rev=HEAD", "/home/emre/Desktop/gene_association.jcvi_Aphagocytophilum.gz")
    sys.exit(app.exec_())