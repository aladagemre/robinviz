from core import *
from bicluster import *
from drawing import *


class SingleMainViewWindow(QMainWindow):
    def __init__(self, scene=None):
        QMainWindow.__init__(self)
        if scene:
            self.scene = scene
            self.view = MainView(self.scene)
            self.setupGUI()

    def loadGraph(self, filename):
        self.scene = MainScene()
        self.scene.loadGraph(filename)
        self.view = MainView(self.scene)

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
    def setDisplayGrid(self, value):
        self.scene.gridActive = value
        self.scene.update()

    def createActions(self):
        exit = QAction('Exit', self)
        exit.setShortcut('Ctrl+Q')
        exit.setStatusTip('Exit application')
        self.connect(exit, SIGNAL('triggered()'), SLOT('close()'))


        displayGrid = QAction('Display Grid', self)
        displayGrid.setCheckable(True)
        displayGrid.setChecked(self.scene.gridActive)
        displayGrid.setShortcut('Ctrl+G')
        displayGrid.setStatusTip('Display or Hide the grid')
        self.connect(displayGrid, SIGNAL('toggled(bool)'), self.setDisplayGrid)

        aboutDialog = QAction('About', self)
        aboutDialog.setStatusTip('About RobinViz')
        self.connect(aboutDialog, SIGNAL('triggered()'), self.displayAboutDialog)

        
        self.statusBar()

        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(exit)

        viewMenu = menubar.addMenu('&View')
        viewMenu.addAction(displayGrid)

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
    def __init__(self, scene=None):
        SingleMainViewWindow.__init__(self, scene)

    def loadGraph(self, filename):
        self.scene = PeripheralScene()
        self.scene.loadGraph(filename)
        self.view = PeripheralView(self.scene)
        self.setupGUI()


class MainView(View):
    def __init__(self, parent=None):
        View.__init__(self, parent)
        self.main = True


    def newWindow(self):
        if not hasattr(self, 'specialWindow'):
            self.specialWindow = SingleMainViewWindow(self.scene())
            #self.specialWindow.loadGraph(self.scene().filename)
        self.specialWindow.showMaximized()


class PeripheralView(View):
    def __init__(self, parent=None):
        View.__init__(self, parent)
        self.preview = False
        
    def setPreview(self, value=True):
        self.preview = value
        
    def mousePressEvent(self, event):
        QGraphicsView.mousePressEvent(self, event)
        if hasattr(self.scene(), "id"):
            self.emit(SIGNAL("viewSelected"), self.scene().id)

    def newWindow(self):
        if not hasattr(self, 'specialWindow'):
            self.specialWindow = SinglePeripheralViewWindow(self.scene())
            #self.specialWindow.loadGraph(self.scene().filename)
            #self.specialWindow.scene.setId(self.scene().id)
        if len(self.scene().items())>30:
            self.specialWindow.showMaximized()
        else:
            view = self.specialWindow.view
            view.fitInView(view.sceneRect())
            self.specialWindow.show()


    
    def focusInEvent(self, event):
        """Highlights the view with color yellow when focused."""
        View.focusInEvent(self, event)
        if self.preview:
            if not hasattr(self, 'normalBackground'):
                self.normalBackground = self.backgroundBrush()
            self.setBackgroundBrush(QBrush(QColor(Qt.yellow)))
    def focusOutEvent(self, event):
        """Removes yellow highlight when focused out."""
        View.focusOutEvent(self, event)
        if self.preview:
            self.setBackgroundBrush(self.normalBackground)
        

            
class MainScene(Scene):
    def __init__(self, parent=None):
        Scene.__init__(self, parent)
        self.readSettings()

        # By default, Scene is NOT directed.
        # But main scene can be directed or undirected.
        if self.parameters["edgesBetween"]:
            self.directed = True
            self.onlyUp = True
        else:
            self.directed = False

            
    def loadGraph(self, filename):
        Scene.loadGraph(self, filename)
        self.determineScoring()
        
    def addNode(self, node):
        item = CircleNode(node)
        self.addItem(item)
        self.nodeDict[node] = item
        self.nodeDict[node.id] = item
        
    def readSettings(self):
        """Read settings from the settings file(s)"""
        self.parameters = {}
        f = open("settings.ini")
        # TODO: handle spaces!
        for line in f:
            (parameterName, parameterValue) = line.strip().split()
            # Try to convert it to int or float
            try:
                parameterValue = int(parameterValue)
            except:
                try:
                    parameterValue = float(parameterValue)
                except:
                    pass
            self.parameters[parameterName] = parameterValue
            
    def determineScoring(self):
        """Assigns scoring name and value to the tooltips of the nodes."""
        # Set scoring name

        if self.parameters["hvalueWeighting"]:
            self.scoringName = "H-Value"
        elif self.parameters["enrichmentWeighting_o"]:
            self.scoringName = "Enrichment Ratio"
        elif self.parameters["enrichmentWeighting_f"]:
            self.scoringName = "Enrichment Ratio"
        elif self.parameters["ppihitratioWeighting"]:
            self.scoringName = "PPI Hit Ratio"

        f = open("outputs/biclusters/scoring.txt")
        f.readline() # for the first line (scoring scheme : blabla)
        for line in f:
            ( biclusterstr, id, value ) = line.strip().split()
            item = self.nodeDict[int(id)]
            tip = "%s: %s\nCategory: %s" % (self.scoringName, value, CATEGORY_COLORS[item.node.parameter])
            item.setToolTip(tip)
            


    def mouseDoubleClickEvent(self, event):
        """When double clicked on a node, signals the node id so that
        matching PPI graph can be displayed on pViews."""
        clickedItem = self.itemAt(event.scenePos())
        if not clickedItem or isinstance(clickedItem, EdgeItem):
            return

        if isinstance(clickedItem, QGraphicsTextItem):
            clickedItem = clickedItem.root
            
        if not isinstance(clickedItem, CircleNode):
            return
        nodeId = clickedItem.node.id
        self.emit(SIGNAL("nodeDoubleClicked"), nodeId)

    def mousePressEvent(self, event):
        """When selected a node, signals the node id so that matching
        pView catches focus"""
        QGraphicsScene.mousePressEvent(self, event)
        clickedItem = self.itemAt(event.scenePos())
        if clickedItem:
            if isinstance(clickedItem, QGraphicsTextItem):
                clickedItem = clickedItem.root
                
            if not isinstance(clickedItem, CircleNode):
                return

            clickedNode = clickedItem.node
            nodeId = clickedNode.id
            
            self.emit(SIGNAL("nodeSelected"), nodeId)
            
class PeripheralScene(Scene):
    def __init__(self, filename=None, parent=None):
        Scene.__init__(self, parent)

    def setId(self, id):
        self.id = id

    def addNode(self, node):
        item = SquareNode(node)
        self.addItem(item)
        self.nodeDict[node] = item
        self.nodeDict[node.id] = item
        item.associateWithNode(node)
