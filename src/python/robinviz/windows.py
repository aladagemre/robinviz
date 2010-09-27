# -*- coding: utf-8 -*-
from PyQt4.QtGui import *
from PyQt4.QtCore import *

import os.path
#from extensions import *
#from core import MainScene, PeripheralScene
from confirmation import CoExpressionMainView, CoExpressionMainScene
from settings import SettingsDialog
from search import ComprehensiveSearchWidget, ProteinSearchWidget
import os
from os.path import normcase
from wizards import InputWizard

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
        self.view.fitInView(self.scene.itemsBoundingRect(),Qt.KeepAspectRatio)

        layout = QHBoxLayout()
        layout.addWidget(self.view)

        self.widget = QWidget()
        self.widget.setLayout(layout)

        self.setCentralWidget(self.widget)
        self.setWindowTitle("RobinViz - Single View")

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
        if not confirmationType:
            confirmationType = str(self.coGroup.checkedAction().text())
            
        if confirmationType == "Co-Expression":
            self.mainViewType = CoExpressionMainView
            self.peripheralViewType = CoExpressionPeripheralView
            self.mainSceneType = CoExpressionMainScene
            pass
        elif confirmationType == "Co-Functionality":
            self.mainViewType = CoFunctionalityMainView
            self.peripheralViewType = CoFunctionalityPeripheralView
            self.mainSceneType = CoFunctionalityMainScene
            pass
        elif confirmationType == "Co-Localization":
            pass
        else:
            print "No such confirmation type:", confirmationType
            print "Setting Co-Expression as default."
            confirmationType = "Co-Expression"
            

        self.confirmationType = confirmationType
        self.menuBar().clear()
        self.setupGUI()

        self.checkCurrentConfirmationType()

    def checkCurrentConfirmationType(self):

        actionDict = {"Co-Expression": self.coExpressionAction,
                      "Co-Functionality": self.coFunctionalityAction,
                      "Co-Localization": self.coLocalizationAction,
                      }
        actionDict[self.confirmationType].setChecked(True)
        
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
        if not os.path.exists(normcase("outputs/graphs/maingraph.gml")):
            QMessageBox.information(self, 'No recent results',
     "Results not found. Please report this issue.")
            return
        self.pScenes = {} # Peripheral scenes
        self.mainScene = self.mainSceneType()
        self.mainScene.loadGraph(normcase("outputs/graphs/maingraph.gml"))
        self.mainView.setScene(self.mainScene)
        self.mainView.setSceneRect(self.mainScene.sceneRect())
        self.mainView.fitInView(self.mainScene.sceneRect(),Qt.KeepAspectRatio)

        if self.confirmationType == "Co-Expression":
            self.keyList = map(lambda d: "Bicluster %0d" % d, range(len(self.mainScene.g.nodes)))

        elif self.confirmationType == "Co-Functionality":
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

    def viewSelected(self, id):
        self.mainScene.clearSelection()
        self.mainScene.nodeDict[id].setSelected(True)
        self.nodeSelected(id)

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
                scene.loadGraph(normcase('outputs/graphs/graph%d.gml' % id ))
                scene.setId(id)
                self.pScenes[id] = scene
            except IOError:
                # If we can't create the scene, means that there's no such graph file.
                QMessageBox.information(self, 'Empty Bicluster',
                    "No interactions found in this bicluster")
                self.mainScene.nodeDict[id].blowUp()
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
            elif not view.scene():
                # If t
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

        # ======= PREPARE ========
        if not self.confirmationType:
            print "No confirmation type specified, won't run."
            return
        self.clearViews()
        
        exe_files = {
            'Co-Expression' : './co_expression.exe',
            'Co-Functionality' : './co_functional.exe',
        }
        print "Starting operation"
        self.setWindowTitle("RobinViz - Please wait, window might not respond for a while...")
        self.setCursor(Qt.WaitCursor)

        # ======= CLEANUP ==========
        errorFile = "outputs/error.txt"
        if os.path.exists(errorFile):
            os.remove(errorFile)
        import clean
        clean.clean()

	# ======= INPUT SELECTION ===

        # ======== RUN ==============
        failed = os.system(normcase(exe_files[self.confirmationType])) # returns 0 for success

        # ======== DISPLAY ==========
        if not failed:
            self.loadMainScene()
            self.connectSlots()

            # Write the confirmation type so that we can recognize what type
            # of confirmation has been applied.
            with open("outputs/resultparams.txt", "w") as resultparams:
                resultparams.write(self.confirmationType)
            
        else:
            if os.path.exists(normcase(errorFile)):
                message = open(normcase(errorFile)).read()
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
            self.clearViews()
            os.system(normcase("./session.exe load %s" % fileName))
            QMessageBox.information(self, "Session loaded", "The session you provided has been loaded. You may now start working.")

    def saveSession(self):
        fileName = QFileDialog.getSaveFileName(self, "Save Session File",
                                                 "sessions/", "Session File (*.ses)");
        if filename:
            os.system(normcase("./session.exe save %s %d" % ( fileName, len(self.mainView.scene().g.nodes) )))

    def detectLastConfirmationType(self):
        try:
            with open("outputs/resultparams.txt") as resultparams:
                confirmationType = resultparams.read().strip()
                self.setConfirmationType(confirmationType)
        except:
            print "No confirmation type found, using Co-Expression."
            self.setConfirmationType("Co-Expression")

    def displayLast(self):
        self.clearViews()

        if not os.path.exists(normcase("outputs/graphs/maingraph.gml")):
            QMessageBox.information(self, 'No recent results',
            "No recent results not found. Please run the program.")
        else:
            self.detectLastConfirmationType()
            self.loadMainScene()
            self.connectSlots()

    def displaySettings(self):
        self.settingsDialog = SettingsDialog()
        self.settingsDialog.show()

    def selectInputs(self):
	#os.chdir("src/python/robinviz/databases/inputtrees")
	self.inputWizard = InputWizard()
	self.inputWizard.show()
	#os.chdir("../../../../..")
	
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
        self.manualDialog.setUrl(QUrl("http://hacivat.khas.edu.tr/~robinviz/manuals/manual.html"))
        self.manualDialog.show()

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
        run = QAction('Run', self)
        run.setShortcut('Ctrl+R')
        run.setStatusTip('Confirmation by Co-Expression')
        self.connect(run, SIGNAL('triggered()'), self.run)


        confirmationMenu = QMenu('Confirmation', self.menuBar())
        self.coGroup = coGroup = QActionGroup(confirmationMenu)
        coGroup.setExclusive(True)
        
        self.coExpressionAction = coExpression = QAction('Co-Expression', coGroup)
        self.coFunctionalityAction = coFunctionality = QAction('Co-Functionality', coGroup)
        self.coLocalizationAction = coLocalization = QAction('Co-Localization', coGroup)


        coActions = (coExpression, coFunctionality, coLocalization)
        # Add actions to confirmation menu.
        map(confirmationMenu.addAction, coActions)
        # Set checkable
        map(lambda action: action.setCheckable(True), coActions)
        # Connect signals (sets confirmation type)
        map(lambda action: self.connect(action, SIGNAL('triggered()'), self.setConfirmationType) , coActions)


        loadSession = QAction('L&oad Session', self)
        loadSession.setShortcut('Ctrl+O')
        loadSession.setStatusTip('Load a saved session.')
        self.connect(loadSession, SIGNAL('triggered()'), self.loadSession)

        saveSession = QAction('&Save Session', self)
        saveSession.setShortcut('Ctrl+S')
        saveSession.setStatusTip('Save your session.')
        self.connect(saveSession, SIGNAL('triggered()'), self.saveSession)

        displayLast = QAction('Display R&ecent Results', self)
        displayLast.setShortcut('Ctrl+E')
        displayLast.setStatusTip('Display Recent Results without running the program again.')
        self.connect(displayLast, SIGNAL('triggered()'), self.displayLast)



        settings = QAction('Se&ttings', self)
        settings.setShortcut('Ctrl+T')
        settings.setStatusTip('Program Settings')
        self.connect(settings, SIGNAL('triggered()'), self.displaySettings)


	selectInput = QAction('&Input', self)
	selectInput.setShortcut('Ctrl+I')
	settings.setStatusTip('Input Selection Wizard')
	self.connect(selectInput, SIGNAL('triggered()'), self.selectInputs)
	
        exit = QAction('E&xit', self)
        exit.setShortcut('Ctrl+Q')
        exit.setStatusTip('Exit application')
        self.connect(exit, SIGNAL('triggered()'), SLOT('close()'))


        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(run)
        fileMenu.addMenu(confirmationMenu)
        fileMenu.addSeparator()
        map(fileMenu.addAction, (loadSession, saveSession, displayLast, settings, selectInput))
        fileMenu.addSeparator()
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
        viewMenu.addAction(goto)
        viewMenu.addAction(refresh)
        viewMenu.addAction(clearViews)
        viewMenu.addAction(showFullscreen)


        # HELP MENU

        manualDialog = QAction('&Manual', self)
        manualDialog.setStatusTip('RobinViz Documentation')
        self.connect(manualDialog, SIGNAL('triggered()'), self.displayManualDialog)

        updateDialog = QAction('&Update', self)
        updateDialog.setStatusTip('Update RobinViz')
        self.connect(updateDialog, SIGNAL('triggered()'), self.displayUpdateDialog)

        aboutDialog = QAction('&About', self)
        aboutDialog.setStatusTip('About RobinViz')
        self.connect(aboutDialog, SIGNAL('triggered()'), self.displayAboutDialog)

        helpMenu = menubar.addMenu('&Help')
        helpMenu.addAction(manualDialog)
        helpMenu.addAction(updateDialog)
        helpMenu.addSeparator()
        helpMenu.addAction(aboutDialog)

from confirmation import * # to avoid circular import. Need to fix it.
# windows requires confirmation->extensions. extensions requires windows (to open a new window)