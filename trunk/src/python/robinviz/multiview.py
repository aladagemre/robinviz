#! /usr/bin/python
from PyQt4.QtGui import QApplication
from windows import MultiViewWindow
import sys

if __name__ == "__main__":

    app = QApplication(sys.argv)
    mainWindow = MultiViewWindow()
    #mainWindow.setGeometry(100, 100, 800, 500)
    mainWindow.showMaximized()
    #mainWindow.show()
    #mainWindow.showFullScreen()


    sys.exit(app.exec_())

