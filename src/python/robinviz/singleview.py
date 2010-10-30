#from extensions import *
from confirmation import CoFunctionalityPeripheralView
from windows import SingleMainViewWindow, SinglePeripheralViewWindow
import sys
from PyQt4.QtGui import QApplication
import os
from utils.info import rp

def main():
    app = QApplication(sys.argv)

    mainWindow = SinglePeripheralViewWindow(CoFunctionalityPeripheralView)
    mainWindow.loadGraph(os.path.normcase(rp("outputs/graphs/graph7.gml")))

    #mainWindow = SingleMainViewWindow()
    #mainWindow.loadGraph(os.path.normcase("../outputs/graphs/maingraph.gml"))
    
    mainWindow.showMaximized()
    #mainWindow.show()

    #mainWindow.setGeometry(app.desktop().availableGeometry())
    #QWidget::setGeometry(QApplication::desktop()->availableGeometry());
    #mainWindow.showFullScreen()


    sys.exit(app.exec_())


if __name__ == "__main__":
    #import cProfile
    #cProfile.run('main()')
    main()

