# -*- coding: utf-8 -*-
from PyQt4.QtGui import *
from PyQt4.QtCore import *

import os.path
from confirmation import CoExpressionMainView, CoExpressionMainScene
from settings import SettingsDialog
from search import ComprehensiveSearchWidget, ProteinSearchWidget
from misc.legend import LegendWidget
from drawing import read_category_information
import os
import shutil

from wizards import InputWizard
from utils.info import id2cat, runcommand, rp
from utils.compression import compressdir, untar

class SingleMainViewWindow(QMainWindow):
    def __init__(self, mainViewType, mainSceneType, scene=None):
        QMainWindow.__init__(self)
        self.mainViewType = mainViewType
        self.mainSceneType = mainSceneType

        if scene:
            self.scene = scene
            self.view = self.mainViewType(self.scene)
            self.setupGUI()

    def loadGraph(self, filename):
        self.scene = self.mainSceneType()
        self.scene.loadGraph(filename)
        self.view = self.mainViewType(self.scene)

        self.setupGUI()

    def setupGUI(self):
        #self.view.setViewport(QGLWidget());
        self.view.setRenderHints(QPainter.Antialiasing)
        self.view.setSceneRect(self.scene.sceneRect())
        #self.view.fitInView(self.scene.itemsBoundingRect(),Qt.KeepAspectRatio)
        self.view.refresh()

        layout = QHBoxLayout()
        layout.addWidget(self.view)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("RobinViz - Central View")

        self.createActions()
        self.statusBar()
        self.connectSlots()
    def connectSlots(self):
        self.connect(self.scene, SIGNAL('sceneMouseMove'), self.displayCoordinate)

    def displayCoordinate(self, point):
        self.statusBar().showMessage("(%s, %s)" % (point.x(), point.y()))

    def createActions(self):
        exit = QAction('Exit', self)
        exit.setShortcut('Ctrl+Q')
        exit.setStatusTip('Exit application')
        self.connect(exit, SIGNAL('triggered()'), SLOT('close()'))

        refresh = QAction('Refresh', self)
        refresh.setShortcut('F5')
        refresh.setStatusTip('Refresh the view')
        self.connect(refresh, SIGNAL('triggered()'), self.view.refresh)

        legend = QAction('Legend', self)
        legend.setShortcut('F6')
        legend.setStatusTip('Display the legend')
        self.connect(legend, SIGNAL('triggered()'), self.view.legend)

        aboutDialog = QAction('About', self)
        aboutDialog.setStatusTip('About RobinViz')
        self.connect(aboutDialog, SIGNAL('triggered()'), self.displayAboutDialog)


        # ====== File Menu ========
        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(exit)
        
        # ====== View Menu ========
        self.viewMenu = viewMenu = menubar.addMenu('&View')
        viewMenu.addAction(refresh)
        viewMenu.addAction(legend)

        # ====== Help Menu ========
        helpMenu = menubar.addMenu('&Help')
        #helpMenu.addAction(Manual)
        helpMenu.addAction(aboutDialog)

    def displayAboutDialog(self):
        from misc.about import Ui_AboutDialog
        self.AboutDialog = QDialog()
        ui = Ui_AboutDialog()
        ui.setupUi(self.AboutDialog)
        self.AboutDialog.show()
    def resizeEvent(self, event):
        #self.view.setSceneRect(self.scene.sceneRect())
        self.view.fitInView(self.scene.itemsBoundingRect(),Qt.KeepAspectRatio)
        #self.view.refresh()



class SinglePeripheralViewWindow(SingleMainViewWindow):
    def __init__(self, peripheralViewType, scene=None):
        QMainWindow.__init__(self)
        self.peripheralViewType = peripheralViewType
        if scene:
            self.scene = scene
            self.view = self.peripheralViewType(self.scene)
            self.view.setPreview(False)
            self.setupGUI()
            self.createDockWindows()
            if peripheralViewType == CoExpressionPeripheralView:
                self.setWindowTitle("Robinviz - Bicluster %s" % self.scene.id )
            else:
                self.setWindowTitle("Robinviz - %s" % id2cat(self.scene.id))

    def setPeripheralViewType(self, peripheralViewType):
        """Sets the peripheral view type like:
        CoExpressionPeripheralView, CoFunctionalityPeripheralView, etc."""
        self.peripheralViewType = peripheralViewType
        
    def loadGraph(self, filename):
        self.scene = PeripheralScene()
        self.scene.loadGraph(filename)
        self.view = self.peripheralViewType(self.scene)
        self.view.setPreview(False)
        self.setupGUI()
        self.createDockWindows()

    def createDockWindows(self):
        if hasattr(self, 'dock'):
            self.removeDockWidget(self.dock)
            del self.dock

        self.dock = dock = QDockWidget("Search", self)
        dock.setAllowedAreas(Qt.LeftDockWidgetArea | Qt.RightDockWidgetArea);

        self.searchPane = ProteinSearchWidget(dock, singleWindow=self)
        dock.setWidget(self.searchPane)
        self.addDockWidget(Qt.RightDockWidgetArea, dock)
        self.viewMenu.addAction(dock.toggleViewAction())
        dock.toggleViewAction().setShortcut('F3')


class MultiViewWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)
        self.detectLastConfirmationType()

    def setConfirmationType(self, confirmationType=None):
        print "Setting confirmationType:", confirmationType
            
        if confirmationType.startswith( "co_expression.exe" ):
            self.mainViewType = CoExpressionMainView
            self.peripheralViewType = CoExpressionPeripheralView
            self.mainSceneType = CoExpressionMainScene
        elif confirmationType.startswith("co_ontologies.exe"):
            self.mainViewType = CoFunctionalityMainView
            self.peripheralViewType = CoFunctionalityPeripheralView
            self.mainSceneType = CoFunctionalityMainScene
        else:
            print "No such confirmation type:", confirmationType
            print "Setting Co-Ontology as default."
            confirmationType = "co_ontologies.exe -f"
            

        self.confirmationType = confirmationType
        self.menuBar().clear()
        self.setupGUI()
            
    def setupGUI(self):
        desktop = QDesktopWidget().availableGeometry()
        brPoint = desktop.bottomRight()
        self.windowWidth = brPoint.x()
        self.windowHeight = brPoint.y()
        self.mainView = self.mainViewType()
        self.mainView.setRenderHints(QPainter.Antialiasing)
	#self.setMaximumWidth(self.windowWidth)

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

        #self.mainView.setMinimumHeight(self.windowHeight * 0.70 - 35)
        #self.mainView.setMaximumHeight(self.windowHeight * 0.70)
        self.mainView.setMaximumWidth(self.windowWidth * 0.60)


        bottomBand = QHBoxLayout()
        for i in range(12,16):
            bottomBand.addWidget(self.widgets[i])

        layout.addLayout(bottomBand)

        self.widget = QWidget()

        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("RobinViz")

        self.createActions()



    def createDockWindows(self):
        if hasattr(self, 'dock'):
            self.removeDockWidget(self.dock)
            del self.dock

        self.dock = dock = QDockWidget("Search", self)
        dock.setAllowedAreas(Qt.LeftDockWidgetArea | Qt.RightDockWidgetArea);
        
        self.searchPane = ComprehensiveSearchWidget(dock, multiView=self)
        dock.setWidget(self.searchPane)
        self.addDockWidget(Qt.RightDockWidgetArea, dock)
        self.viewMenu.addAction(dock.toggleViewAction())
        dock.toggleViewAction().setShortcut('F3')


    def loadMainScene(self):
        """Loads the main scene after the results are found."""
        path = rp("outputs/graphs/maingraph.gml")
        if not os.path.exists(path):
            QMessageBox.information(self, 'No recent results',
     "Results not found. Please report this issue.")
            return
        self.pScenes = {} # Peripheral scenes
        self.mainScene = self.mainSceneType()
        self.mainScene.loadGraph(rp("outputs/graphs/maingraph.gml"))
        self.mainView.setScene(self.mainScene)
        self.mainView.setSceneRect(self.mainScene.sceneRect().adjusted(-10, -10, 10, 10))
        #self.mainView.fitInView(self.mainScene.sceneRect(),Qt.KeepAspectRatio)
        self.mainView.refresh()

        if self.confirmationType.startswith("co_expression.exe"):
            self.keyList = map(lambda d: "Bicluster %0d" % d, range(len(self.mainScene.g.nodes)))

        elif self.confirmationType.startswith("co_ontologies.exe"):
            self.keyList = map(lambda line: line.strip(), open(self.mainScene.params["Input"]["dataName_go"]).readlines())
            

        self.createDockWindows()

    def connectSlots(self):
        self.connect(self.mainScene, SIGNAL('nodeDoubleClicked'), self.nodeDoubleClicked)
        self.connect(self.mainScene, SIGNAL('nodeSelected'), self.nodeSelected)
        self.connect(self.mainScene, SIGNAL('sceneMouseMove'), self.displayCoordinate)
        self.connect(self.searchPane, SIGNAL('graphClicked'), self.viewSelected)
        self.connect(self.searchPane, SIGNAL('graphDoubleClicked'), self.nodeDoubleClicked)
        for view in self.pViews:
            self.connect(view, SIGNAL('viewSelected'), self.viewSelected)
            self.connect(view, SIGNAL('newWindowOpened'), self.mainScene.stopSelectedAnimation)

    def viewSelected(self, id):
        if not hasattr(self, 'mainScene'):
            return
        
        self.mainScene.clearSelection()
        try:
            self.mainScene.nodeDict[id].setSelected(True)
            self.nodeSelected(id)
        except:
            print "No associated node"

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
        """Defines what will happen when double clicked on a maingraph node"""
        # Try to find the scene if it's constructed before.
        scene = self.pScenes.get(id)

        # If there's no such scene created before, create a new one.
        if not scene:
            try:
                scene = PeripheralScene()
                scene.loadGraph(rp('outputs/graphs/graph%d.gml' % id ))
                scene.setId(id)
                self.pScenes[id] = scene
            except IOError:
                # If we can't create the scene, means that there's no such graph file.
                QMessageBox.information(self, 'Empty Bicluster/Category',
                    "No associated genes/proteins found.")
                #self.mainScene.nodeDict[id].blowUp()
                del scene
                return
            except:
                # Some other error occured, display traceback.
                import traceback
                traceback.print_stack()
                traceback.print_exc()
                del scene
     
        # Now try to place/unplace the scene in peripheral views.
        for view in self.pViews:
            # Look in order,
            # If current view has a scene and
            # if we see the scene placed in that view,
            if view.scene() == scene:
                # Unplace it.
                del self.pScenes[id]
                view.setScene(None)
                view.specialWindow = None
                break
        else:
            # means no instance of that scene has been found in any of the views.
            # so we'll place the scene in the first empty view
            for view in self.pViews:
                if not view.scene():
                    view.setScene(scene)
                    #view.fitInView(scene.sceneRect(), Qt.KeepAspectRatio)
                    view.refresh()
                    break
            else:
                # If no break occurs, means no place left :(
                QMessageBox.information(self, 'No space left',
         "You need to clear one of the peripheral views to add a new one.")

    def createTopView(self, id):

        widget = QWidget()

        ratio = 0.15
        #widget.setMinimumHeight(self.windowHeight * ratio)
        widget.setMaximumHeight(self.windowHeight * ratio)
        #widget.setMinimumWidth(self.windowWidth * 0.25)
        widget.setMaximumWidth(self.windowWidth * 0.25)
        l = QHBoxLayout(widget)

        view = self.peripheralViewType()
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
        margin = self.windowHeight * 0.10 # 0.70 - 0.15*4
        widget.setMinimumHeight((self.windowHeight * ratio)/4  - margin)
        widget.setMaximumHeight((self.windowHeight * ratio - margin)/4)
        widget.setMaximumWidth(self.windowWidth * 0.20)

        l = QHBoxLayout(widget)

        view = self.peripheralViewType()
        view.setPreview(True)
        view.setRenderHints(QPainter.Antialiasing)

        l.addWidget(view)

        return view, widget


    def stopAllAnimations(self):
        # First of all, we have to stop all the animation on selected nodes.
        if hasattr(self, 'mainScene'):
            for item in self.mainScene.selectedItems():
                item.stopAnimation()

        # Then we stop layout animations.
        self.mainView.stopLayoutAnimation()
        for pView in self.pViews:
            pView.stopLayoutAnimation()

    ################## FILE MENU #######################

    def run(self):
        """Runs the analysis operation."""
        self.detectLastConfirmationType()
        
        # ======= PREPARE ========
        if not self.confirmationType:
            print "No confirmation type specified, won't run."
            return
        self.clearViews()
        
        print "Starting operation"
        self.setWindowTitle("RobinViz - Please wait, window might not respond for a while...")
        self.setCursor(Qt.WaitCursor)

        # ======= CLEANUP ==========
        errorFile = rp("outputs/error.txt")
        if os.path.exists(errorFile):
            os.remove(errorFile)
        import clean
        clean.clean()

	# ======= INPUT SELECTION ===

        # ======== RUN ==============
        failed = runcommand(self.confirmationType) #returns 0 for success

        # ======== DISPLAY ==========
        if not failed:
            read_category_information()
            self.loadMainScene()
            self.connectSlots()

            # Write the confirmation type so that we can recognize what type
            # of confirmation has been applied.
            with open(rp("outputs/resultparams.txt"), "w") as resultparams:
                print "Writing conftype", self.confirmationType
                resultparams.write(self.confirmationType)
            
        else:
            if os.path.exists(errorFile):
                message = open(errorFile).read()
            else:
                message = "Unknown error occured. Please report the debug messages on the console."

            QMessageBox.information(self, 'Failed', message)

        # ======= POST-OPERATIONS ====
        self.unsetCursor()
        self.setWindowTitle("RobinViz")


    def loadSession(self):
        fileName = QFileDialog.getOpenFileName(self, "Load Session File",
                                                 "sessions/", "Session File (*.ses)");

        if fileName:
            # Init part
            self.clearViews()
            shutil.rmtree(rp("outputs"))
            shutil.copy(fileName, rp("outputs.tar.gz"))
            untar("outputs.tar.gz")

            # Afterwards
            os.remove("outputs.tar.gz")
            shutil.move(rp("outputs/input_go.txt"), ap("assocdata/input_go.txt"))

            # Load part
            self.detectLastConfirmationType()
            print "Detected:", self.confirmationType
            QMessageBox.information(self, "Session loaded", "The session you provided has been loaded. You may now start working.")
            self.displayLast()
            
    def saveSession(self):
        fileName = QFileDialog.getSaveFileName(self, "Save Session File",
                                                 "sessions", "Session File (*.ses)");

        # Copy two files
        # =======================
        shutil.copy(ap("assocdata/input_go.txt"), rp("outputs/input_go.txt"))
        # =======================
        if fileName:
            # saving window doesnt put extension automatically on Gnome. So ensure the extension:
            if not str(fileName).endswith(".ses"):
                fileName = str(filename) + ".ses"
            compressdir("outputs", fileName)

    def detectLastConfirmationType(self):
        
        with open("outputs/resultparams.txt") as resultparams:
            confirmationType = resultparams.read().strip()
            self.setConfirmationType(confirmationType)
        """except:
            print "No confirmation type found, using Co-Expression."
            self.setConfirmationType("co_expression.exe -f")"""

    def displayLast(self):
        self.clearViews()

        if not os.path.exists(rp("outputs/graphs/maingraph.gml")):
            QMessageBox.information(self, 'No recent results',
            "No recent results not found. Please run the program.")
        else:
            self.detectLastConfirmationType()
            self.loadMainScene()
            self.connectSlots()

    def displaySettings(self):
        self.settingsDialog = SettingsDialog()
        self.settingsDialog.show()

    def act(self, result):
        """Acts according to input wizard's result. Runs if wizard succeeds.
        Does nothing if it does not."""
        if result:
            self.run()

    def selectInputs(self):
	self.inputWizard = InputWizard()
        self.inputWizard.finished.connect(self.act)
	self.inputWizard.show()


    def updateData(self):
        response= QMessageBox.warning(self, 'Update Local Data',
     "This operation will remove the local data and the data will be restored when you perform another execution.\n\n"+
     "Are you sure you want to do this?", buttons=QMessageBox.Yes|QMessageBox.No)
        
        if response == QMessageBox.Yes:
            # do the operation
            data = [ ap('ppidata/BIOGRID-OSPREY_DATASETS-3.0.68.osprey'),
                     ap('godata/goinfo.sqlite3')
            ]
            data += map( lambda x: ap('assocdata')+"/"+ x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('assocdata') )  ) )
            data += map( lambda x: ap('ppidata/hitpredict')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('ppidata/hitpredict') )  ) )
            data += map( lambda x: ap('geodata')+"/"+x, filter ( lambda x: not x.startswith("."),  os.listdir( ap('geodata') )  ) )

            for i in data:
                print "Removing",i
                try:
                    os.remove(i)
                except:
                    print "Could not remove",i
        
    ############### VIEW MENU ###################
    def goto(self):
        value, ok = QInputDialog.getInt(self, "Go to bicluster/category",
                                          "Number:", value=0, min=0, max=1000, step=1)
        if ok:
            self.searchPane.emit(SIGNAL("graphClicked"), value)

    def refreshAll(self):
        for view in self.pViews:
            view.refresh()
        self.mainView.refresh()

    def clearViews(self):
        """Clears all the views in the window."""
        self.stopAllAnimations()

        for view in self.pViews:
            view.setScene(None)
            view.setDragMode(QGraphicsView.NoDrag)

        self.mainView.setScene(None)
        self.mainView.setDragMode(QGraphicsView.NoDrag)
        if hasattr(self, 'pScenes'):
            for scene in self.pScenes:
                del scene
        if hasattr(self, 'mainScene'):
            del self.mainScene

    def legend(self):
        """Displays meanings of the colors"""
        self.legendWindow = LegendWidget()
        self.legendWindow.show()
        
    def setFullScreen(self, value):
        if value:
            self.showFullScreen()
        else:
            self.showMaximized()

    """def keyPressEvent(self, event):
        key = event.key()
        if key == Qt.Key_F11:
            self.toggleFullScreen()"""

    ############### HELP MENU ###################
    def displayManualDialog(self):
        self.manualDialog= QtWebKit.QWebView()
        self.manualDialog.setUrl(QUrl("http://code.google.com/p/robinviz/wiki/Manual"))
        self.manualDialog.setWindowTitle("Robinviz Manual")
        self.manualDialog.showMaximized()

    def displayUpdateDialog(self):
        from misc.updatergui import UpdaterDialog
        self.updateDialog = UpdaterDialog()
        self.updateDialog.show()


    def displayAboutDialog(self):
        from misc.about import Ui_AboutDialog
        self.AboutDialog = QDialog()
        ui = Ui_AboutDialog()
        ui.setupUi(self.AboutDialog)
        self.AboutDialog.show()



    def createActions(self):

        # FILE MENU
        run = QAction('E&xecute', self)
        run.setShortcut('Ctrl+X')
        run.setStatusTip('Execute the operation')
        self.connect(run, SIGNAL('triggered()'), self.selectInputs)

        loadSession = QAction('L&oad Session', self)
        loadSession.setShortcut('Ctrl+O')
        loadSession.setStatusTip('Load a saved session.')
        self.connect(loadSession, SIGNAL('triggered()'), self.loadSession)

        saveSession = QAction('&Save Session', self)
        saveSession.setShortcut('Ctrl+S')
        saveSession.setStatusTip('Save your session.')
        self.connect(saveSession, SIGNAL('triggered()'), self.saveSession)

        displayLast = QAction('&Display Last Session', self)
        displayLast.setShortcut('Ctrl+D')
        displayLast.setStatusTip('Display Recent Results without running the program again.')
        self.connect(displayLast, SIGNAL('triggered()'), self.displayLast)

        """settings = QAction('Se&ttings', self)
        settings.setShortcut('Ctrl+T')
        settings.setStatusTip('Program Settings')
        self.connect(settings, SIGNAL('triggered()'), self.displaySettings)"""

	"""selectInput = QAction('&Input', self)
	selectInput.setShortcut('Ctrl+I')
	selectInput.setStatusTip('Input Selection Wizard')
	self.connect(selectInput, SIGNAL('triggered()'), self.selectInputs)"""

        updateData = QAction('&Update local data', self)
        updateData.setShortcut('Ctrl+U')
        updateData.setStatusTip('Remove local data to be updated at the next run')
        self.connect(updateData, SIGNAL('triggered()'), self.updateData)
	
        exit = QAction('E&xit', self)
        exit.setShortcut('Ctrl+Q')
        exit.setStatusTip('Exit application')
        self.connect(exit, SIGNAL('triggered()'), SLOT('close()'))


        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(run)
        fileMenu.addSeparator()
        map(fileMenu.addAction, (loadSession, saveSession, displayLast))
        fileMenu.addSeparator()
        fileMenu.addAction(updateData)
        fileMenu.addAction(exit)


        # VIEW MENU
        self.statusBar()

        goto = QAction('Go to', self)
        goto.setShortcut('Ctrl+G')
        goto.setStatusTip('Go to bicluster/category')
        self.connect(goto, SIGNAL('triggered()'), self.goto)


        refresh = QAction('Refresh', self)
        refresh.setShortcut('F5')
        refresh.setStatusTip('Refresh the view')
        self.connect(refresh, SIGNAL('triggered()'), self.refreshAll)

        legend = QAction('Color Legend', self)
        legend.setShortcut('F6')
        legend.setStatusTip('Display the Color Legend')
        self.connect(legend, SIGNAL('triggered()'), self.legend)

        clearViews = QAction("C&lear Views", self)
        clearViews.setShortcut('Ctrl+L')
        clearViews.setStatusTip('Clear the views in the window.')
        self.connect(clearViews, SIGNAL('triggered()'), self.clearViews)

        showFullscreen = QAction("&Fullscreen", self)
        showFullscreen.setCheckable(True)
        showFullscreen.setShortcut("F11")
        showFullscreen.setStatusTip('Display the window in fullscreen')
        self.connect(showFullscreen, SIGNAL('toggled(bool)'), self.setFullScreen)

        self.viewMenu = viewMenu = menubar.addMenu('&View')
        viewMenu.addAction(legend)
        viewMenu.addAction(goto)
        viewMenu.addAction(refresh)
        viewMenu.addAction(clearViews)
        viewMenu.addAction(showFullscreen)


        # HELP MENU

        manualDialog = QAction('&Manual', self)
        manualDialog.setStatusTip('RobinViz Documentation')
        self.connect(manualDialog, SIGNAL('triggered()'), self.displayManualDialog)

        """updateDialog = QAction('&Update', self)
        updateDialog.setStatusTip('Update RobinViz')
        self.connect(updateDialog, SIGNAL('triggered()'), self.displayUpdateDialog)"""

        aboutDialog = QAction('&About', self)
        aboutDialog.setStatusTip('About RobinViz')
        self.connect(aboutDialog, SIGNAL('triggered()'), self.displayAboutDialog)

        helpMenu = menubar.addMenu('&Help')
        helpMenu.addAction(manualDialog)
        #helpMenu.addAction(updateDialog)
        helpMenu.addSeparator()
        helpMenu.addAction(aboutDialog)

    def resizeEvent(self, event):
        if self.mainView.scene():
            self.mainView.fitInView(self.mainView.scene().itemsBoundingRect(),Qt.KeepAspectRatio)
        for view in self.pViews:
            if view.scene():
                view.fitInView(view.scene().itemsBoundingRect(),Qt.KeepAspectRatio)
from confirmation import * # to avoid circular import. Need to fix it.
# windows requires confirmation->extensions. extensions requires windows (to open a new window)