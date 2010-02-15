#! /usr/bin/python

from PyQt4.QtCore import *
from PyQt4.QtGui import *

import sys
from updater import *

class UpdaterDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.setupGUI()
        self.createUpdater()
        self.connectSlots()

        
    def setupGUI(self):
        self.buttonTexts = ("Check for updates", "Install Update", "Close")
        
        self.setWindowTitle("RobinViz Updater")
        self.layout = QGridLayout(self)

        self.labelLogs = QLabel("Logs:")
        self.button = QPushButton(self.buttonTexts[0])
        self.buttonTextIndex = 0
        self.textLogs = QTextEdit()

        self.layout.addWidget(self.labelLogs, 0, 0)
        self.layout.addWidget(self.button, 0, 1)
        self.layout.addWidget(self.textLogs, 1, 0, 1, 2)

    def createUpdater(self):
        self.updater = UpdateChecker()

    def setButtonText(self):
        """Updates button text according to buttonTextIndex."""
        self.button.setText(self.buttonTexts[self.buttonTextIndex])
        
    def slotAddLog(self, message):
        self.textLogs.append(message)
        
    def slotUpdateFound(self, url):
        self.slotAddLog("Update found: " + url)

    def slotError(self, message):
        self.slotAddLog(message)
        self.buttonTextIndex = 2
        self.setButtonText()
        
    def slotButtonClicked(self):
        print "Clicked"
        if self.buttonTextIndex == 0:
            # Check for updates
            print "Lets look for updates"
            self.updater.checkUpdate()
            
        elif self.buttonTextIndex == 1:
            # Install the updates
            print "Install updates"
            self.updater.installUpdate()
        else:
            print "Bye"
            # Close the dialog
            self.close()
        
        self.buttonTextIndex += 1
        self.setButtonText()
        
    def connectSlots(self):
        print "Connecting slots"
        self.connect(self.button, SIGNAL('clicked()'), self.slotButtonClicked)
        self.connect(self.updater, SIGNAL('message(QString)'), self.slotAddLog)
        self.connect(self.updater, SIGNAL('updateFound(QString)'), self.slotUpdateFound)
        self.connect(self.updater, SIGNAL('error(QString)'), self.slotError)
        
        
if __name__ == "__main__":
    app = QApplication(sys.argv)
    ud = UpdaterDialog()
    ud.show()
    sys.exit(app.exec_())

