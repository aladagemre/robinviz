#!/usr/bin/python
# -*- coding: utf-8 -*-
from multiview import *

def main():
    app = QApplication(sys.argv)
    #style = open("src/python/robinviz/style.css").read()
    #app.setStyleSheet(style)
    mainWindow = MultiViewWindow()
    mainWindow.showMaximized()
    #mainWindow.showFullScreen()
    sys.exit(app.exec_())

if __name__ == "__main__":
    #import cProfile
    #cProfile.run('main()')
    main()
    