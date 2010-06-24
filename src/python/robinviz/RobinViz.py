#!/usr/bin/python
# -*- coding: utf-8 -*-

if __name__ == "__main__":
    from multiview import *

    app = QApplication(sys.argv)
    #style = open("src/python/robinviz/style.css").read()
    #app.setStyleSheet(style)
    mainWindow = MultiViewWindow()
    mainWindow.showMaximized()
    #mainWindow.showFullScreen()
    sys.exit(app.exec_())

