from extensions import *

def main():
    app = QApplication(sys.argv)

    #mainWindow = SinglePeripheralViewWindow()
    #mainWindow.loadGraph(os.path.normcase("../outputs/graphs/graph0.gml"))

    mainWindow = SingleMainViewWindow()
    mainWindow.loadGraph(os.path.normcase("../outputs/graphs/maingraph.gml"))
    
    mainWindow.showMaximized()
    #mainWindow.show()

    #mainWindow.setGeometry(app.desktop().availableGeometry())
    #QWidget::setGeometry(QApplication::desktop()->availableGeometry());
    #mainWindow.showFullScreen()


    sys.exit(app.exec_())


if __name__ == "__main__":
    import cProfile
    cProfile.run('main()')

    