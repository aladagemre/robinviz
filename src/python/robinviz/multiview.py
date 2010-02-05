import os.path
#! /usr/bin/python

from extensions import *
from settings import SettingsDialog
import os
from os.path import normcase


class MultiViewWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.setupGUI()
        self.setWindowFlags(Qt.Window|Qt.FramelessWindowHint)

    def setupGUI(self):
        desktop = QDesktopWidget().availableGeometry()
        brPoint = desktop.bottomRight()
        self.windowWidth = brPoint.x()
        self.windowHeight = brPoint.y() - 25
        self.mainView = MainView()
        self.mainView.setRenderHints(QPainter.Antialiasing)

        #self.loadMainScene()
        layout = QVBoxLayout()

        self.pViews = []
        self.widgets = []
        
        for i in range(4):
            pView,widget = self.createTopView(i+1)
            self.pViews.append(pView)
            self.widgets.append(widget)

        for i in range(4,12):
            pView,widget = self.createSideView(i+1)
            self.pViews.append(pView)
            self.widgets.append(widget)

        for i in range(12,16):
            pView,widget = self.createTopView(i+1)
            self.pViews.append(pView)
            self.widgets.append(widget)

        topBand = QHBoxLayout()
        for i in range(0,4):
            topBand.addWidget(self.widgets[i])
        layout.addLayout(topBand)

        centerBand = QHBoxLayout()
        centerLeftLayout = QVBoxLayout()
        centerRightLayout = QVBoxLayout()
        for i in range(4,8):
            centerLeftLayout.addWidget(self.widgets[i])
        centerBand.addLayout(centerLeftLayout)
        centerBand.addWidget(self.mainView)
        for i in range(8,12):
            centerRightLayout.addWidget(self.widgets[i])
        centerBand.addLayout(centerRightLayout)
        layout.addLayout(centerBand)

        self.mainView.setMinimumHeight(self.windowHeight * 0.70 - 35)
        self.mainView.setMaximumHeight(self.windowHeight * 0.70 - 35)
        self.mainView.setMinimumWidth(self.windowWidth * 0.50 + 20)


        bottomBand = QHBoxLayout()
        for i in range(12,16):
            bottomBand.addWidget(self.widgets[i])

        layout.addLayout(bottomBand)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("RobinViz")

        self.createActions()




    def loadMainScene(self):
        """Loads the main scene after the results are found."""
        if not os.path.exists(normcase("outputs/graphs/maingraph.gml")):
            QMessageBox.information(self, 'No recent results',
     "Results not found. Please report this issue.")
            return
        self.pScenes = {} # Peripheral scenes
        self.mainScene = MainScene()
        self.mainScene.loadGraph(normcase("outputs/graphs/maingraph.gml"))
        self.mainView.setScene(self.mainScene)
        self.mainView.setSceneRect(self.mainScene.sceneRect())
        self.mainView.fitInView(self.mainScene.sceneRect(),Qt.KeepAspectRatio)

    def connectSlots(self):
        self.connect(self.mainScene, SIGNAL('nodeDoubleClicked'), self.nodeDoubleClicked)
        self.connect(self.mainScene, SIGNAL('nodeSelected'), self.nodeSelected)
        self.connect(self.mainScene, SIGNAL('sceneMouseMove'), self.displayCoordinate)
        for view in self.pViews:
            self.connect(view, SIGNAL('viewSelected'), self.viewSelected)

    def viewSelected(self, id):
        self.mainScene.clearSelection()
        self.mainScene.nodeDict[id].setSelected(True)

    def displayCoordinate(self, point):
        self.statusBar().showMessage("(%s, %s)" % (point.x(), point.y()))

    def nodeSelected(self, id):
        scene = self.pScenes.get(id)
        if not scene:
            return
        for view in self.pViews:
            if view.scene() == scene:
                view.setFocus()

    def nodeDoubleClicked(self, id):
        scene = self.pScenes.get(id)
        if not scene:
            scene = PeripheralScene()
            scene.loadGraph(normcase('outputs/graphs/graph%d.gml' % id ))
            scene.setId(id)
            self.pScenes[id] = scene

        for view in self.pViews:
            if view.scene() == scene:
                view.setScene(None)
                print view.scene()
                break
            elif not view.scene():
                view.setScene(scene)
                view.fitInView(scene.sceneRect(), Qt.KeepAspectRatio)
                break
        else:
            # If no break occurs, means no place left :(
            QMessageBox.information(self, 'No space left',
     "You need to clear one of the peripheral views to add a new one.")




    def createTopView(self, id):

        widget = QWidget()

        ratio = 0.15
        margin = 5
        widget.setMinimumHeight(self.windowHeight * ratio)
        widget.setMaximumHeight(self.windowHeight * ratio)
        widget.setMinimumWidth(self.windowWidth * 0.25 - margin)
        widget.setMaximumWidth(self.windowWidth * 0.25 - margin)
        l = QHBoxLayout(widget)

        view = PeripheralView()
        view.setPreview(True)
        #scene = Scene('graph_gml%d.gml' % id)
        #view.setScene(scene)
        view.setRenderHints(QPainter.Antialiasing)

        #view.setSceneRect(scene.sceneRect())
        #view.fitInView(scene.sceneRect(), Qt.KeepAspectRatio)
        l.addWidget(view)

        return view, widget
    def createSideView(self, id):

        widget = QWidget()

        ratio = 0.70
        margin = 35
        widget.setMinimumHeight((self.windowHeight * ratio)/4  - margin)
        widget.setMaximumHeight((self.windowHeight * ratio - margin)/4)

        l = QHBoxLayout(widget)

        view = PeripheralView()
        view.setPreview(True)
        view.setRenderHints(QPainter.Antialiasing)

        l.addWidget(view)

        return view, widget


    def clearViews(self):
        for view in self.pViews:
            view.setScene(None)
        self.mainView.setScene(None)
        if hasattr(self, 'pScenes'):
            for scene in self.pScenes:
                del scene
        if hasattr(self, 'mainScene'):
            del self.mainScene
        
    def setDisplayGrid(self, value):
        self.mainScene.gridActive = value
        self.mainScene.update()

    def displaySettings(self):
        self.settingsDialog = SettingsDialog()
        self.settingsDialog.show()

    def run(self):
        print "Starting operation"
        self.setWindowTitle("RobinViz - Please wait, window might not response for a while...")
        self.setCursor(Qt.WaitCursor)
        errorFile = "outputs/error.txt"
        if os.path.exists(errorFile):
            os.remove(errorFile)
        import clean
        clean.clean()
        failed = os.system(normcase("./layered.exe")) # returns 0 for success
        if not failed:
            self.loadMainScene()
            self.connectSlots()
        else:
            if os.path.exists(normcase(errorFile)):
                message = open(normcase(errorFile)).read()
            else:
                message = "Unknown error occured. Please report the debug messages on the console"

            QMessageBox.information(self, 'Failed', message)
        self.unsetCursor()
        self.setWindowTitle("RobinViz")
    def displayLast(self):
        if not os.path.exists(normcase("outputs/graphs/maingraph.gml")):
            QMessageBox.information(self, 'No recent results',
            "No recent results not found. Please run the program.")
        else:
            self.loadMainScene()
            self.connectSlots()
        
    def displayAboutDialog(self):
        from misc.about import Ui_AboutDialog
        self.AboutDialog = QDialog()
        ui = Ui_AboutDialog()
        ui.setupUi(self.AboutDialog)
        self.AboutDialog.show()

    def setFullScreen(self, value):
        if value:
            self.showFullScreen()
        else:
            self.showMaximized()
            
    def keyPressEvent(self, event):
        key = event.key()
        """if key == Qt.Key_F11:
            self.toggleFullScreen()"""
            
    def createActions(self):

        # FILE MENU
        run = QAction('Run', self)
        run.setShortcut('Ctrl+R')
        run.setStatusTip('Run the program')
        self.connect(run, SIGNAL('triggered()'), self.run)

        settings = QAction('Settings', self)
        settings.setShortcut('Ctrl+T')
        settings.setStatusTip('Program Settings')
        self.connect(settings, SIGNAL('triggered()'), self.displaySettings)

        
        displayLast = QAction('Display Recent Results', self)
        displayLast.setShortcut('Ctrl+E')
        displayLast.setStatusTip('Display Recent Results without running the program again.')
        self.connect(displayLast, SIGNAL('triggered()'), self.displayLast)
        
        exit = QAction('Exit', self)
        exit.setShortcut('Ctrl+Q')
        exit.setStatusTip('Exit application')
        self.connect(exit, SIGNAL('triggered()'), SLOT('close()'))


        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(run)
        fileMenu.addSeparator()
        fileMenu.addAction(displayLast)
        fileMenu.addAction(settings)
        fileMenu.addSeparator()
        fileMenu.addAction(exit)


        # VIEW MENU
        displayGrid = QAction('Display Grid', self)
        displayGrid.setCheckable(True)
        #displayGrid.setChecked(self.mainScene.gridActive)
        displayGrid.setShortcut('Ctrl+G')
        displayGrid.setStatusTip('Display or Hide the grid')
        self.connect(displayGrid, SIGNAL('toggled(bool)'), self.setDisplayGrid)
        self.statusBar()
        
        clearViews = QAction("Clear Views", self)
        clearViews.setShortcut('Ctrl+L')
        clearViews.setStatusTip('Clear the views in the window.')
        self.connect(clearViews, SIGNAL('triggered()'), self.clearViews)

        showFullscreen = QAction("Fullscreen", self)
        showFullscreen.setCheckable(True)
        showFullscreen.setShortcut("F11")
        showFullscreen.setStatusTip('Display the window in fullscreen')
        self.connect(showFullscreen, SIGNAL('toggled(bool)'), self.setFullScreen)

        viewMenu = menubar.addMenu('&View')
        viewMenu.addAction(clearViews)
        viewMenu.addAction(displayGrid)
        viewMenu.addAction(showFullscreen)


        # HELP MENU
        aboutDialog = QAction('About', self)
        aboutDialog.setStatusTip('About RobinViz')
        self.connect(aboutDialog, SIGNAL('triggered()'), self.displayAboutDialog)


        helpMenu = menubar.addMenu('&Help')
        #helpMenu.addAction(Manual)
        helpMenu.addAction(aboutDialog)

if __name__ == "__main__":

    app = QApplication(sys.argv)
    mainWindow = MultiViewWindow()
    #mainWindow.setGeometry(100, 100, 800, 500)
    mainWindow.showMaximized()
    #mainWindow.showFullScreen()


    sys.exit(app.exec_())

