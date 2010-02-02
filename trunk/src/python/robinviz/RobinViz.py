#!/usr/bin/python

if __name__ == "__main__":
    from multiview import *

    app = QApplication(sys.argv)
    mainWindow = MultiViewWindow()
    mainWindow.showMaximized()
    #mainWindow.showFullScreen()
    sys.exit(app.exec_())

