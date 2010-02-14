from core import *
from bicluster import *


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
        
    def setPreview(self, value):
        self.preview = True
        
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

            nodeId = clickedItem.node.id
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

class CircleNode(QGraphicsEllipseItem):
    def __init__(self, node, parent=None, scene=None):
        QGraphicsEllipseItem.__init__(self, parent, scene)

        #self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        self.setAcceptsHoverEvents(True)
        self.arrows = []
        self.color = QColor("#52C6FF")
        self.selectedColor = QColor("#E6FF23")
        self.setBrush(self.color)
        self.associateWithNode(node)

    def hoverEnterEvent(self, event):
        self.scene().views()[0].setDragMode(QGraphicsView.NoDrag)
    def hoverLeaveEvent(self, event):
        self.scene().views()[0].setDragMode(QGraphicsView.ScrollHandDrag)
        
    def addEdge(self, e):
        self.arrows.append(e)
		
    def updateEdges(self):
        for arrow in self.arrows:
            arrow.updatePosition()

    def contextMenuEvent(self, event):
        menu = QMenu()
        goTable = menu.addAction("GO Table")
        propertiesAction = menu.addAction("Properties")
        action = menu.exec_(event.screenPos())
        if action == propertiesAction:
            if not hasattr(self, 'biclusterWindow'):
                self.biclusterWindow = BiclusterWindow(self.node.id)
                
            self.biclusterWindow.showMaximized()
        elif action == goTable:
            self.showGOTable()

    def showGOTable(self):
        path = normcase("outputs/go/gobicluster%d.html" % self.node.id)
        if os.path.exists(path):
            self.GOTable= QtWebKit.QWebView()
            self.GOTable.setUrl(QUrl(path))
            self.GOTable.show()
        else:
            QMessageBox.information(None, 'GO Table not found.',
     "You need to run the program with the Gene Ontology File option in Biological Settings Tab checked and provide the GO file.")
     
    def intersectionPoint(self, startPoint):
        """Gives the intersection point when a line is drawn into the center
        of the circle node from the given startPoint."""

        xdiff = self.centerPos().x() - startPoint.x()
        ydiff = self.centerPos().y() - startPoint.y()

        if ydiff == 0:
            return

        intersectPoint = QPointF()
        #intersectPoint2 = QPointF()

        # End point
        r = self.w/2

        a = (r * ydiff) / (math.sqrt(ydiff**2 + xdiff**2))
        c = (a * xdiff) / ydiff

        intersectPoint = self.centerPos() + QPointF(-c,-a)

        """## Start Point
        r = self.w/2

        a = (r * ydiff) / (math.sqrt(ydiff**2 + xdiff**2))
        c = (a * xdiff) / ydiff

        intersectPoint2 = startPoint.centerPos() + QPointF(c,a)"""

        return intersectPoint
    
    def associateWithNode(self, node):
        # Store node data
        self.node = node
        self.w = node.graphics.w

        # Set Color
        self.color = QColor(node.graphics.outline)
        
        """if  self.color.value() > 230 or self.color.value() < 10:
            # If it is too bright or dark:
            self.selectedColor = QColor(Qt.yellow)
        else:
            # If it is dark enough to highlight with a lighter color,"""
        self.selectedColor = self.color.lighter(150)
                
        self.setBrush(self.color)
        
        # Set position of the node:
        self.setPos(QPointF(node.graphics.x - self.w/2, node.graphics.y - self.w/2))
        
        self.setRect(0, 0, self.w, self.w)
        
        # Construct the text.
        self.text = QGraphicsTextItem(self)
        self.text.root = self
        textFont = QFont()
        textFont.setBold(True)
        #textFont.setPointSize(20)
	textFont.setPixelSize(self.w/2)
        self.text.setFont(textFont)
        if self.color < 250 or self.color in (Qt.red, Qt.green, Qt.blue, Qt.black, QColor("#0000CD")):
            self.text.setDefaultTextColor(QColor(Qt.white))
        
        # Set node id as text.
        self.text.setPlainText(str(node.id))
        self.text.contextMenuEvent = self.contextMenuEvent
        # Define bounding rect
        boundRect = self.text.boundingRect()
        # Align text to the center.
        self.text.setPos((self.w - boundRect.width()) / 2, (self.w - boundRect.height()) / 2)

        self.setupAnimation()

    def centerPos(self):
        """Returns the center coordinate of the item."""
        return QPointF(self.x() + self.w/2, self.y() + self.w/2)


    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionChange:
            for arrow in self.arrows:
                arrow.updatePosition()
                
        elif change == QGraphicsItem.ItemSelectedChange:
            if self.isSelected():
                self.setBrush(self.color)
                self.stopAnimation()
            else:
                self.setBrush(self.selectedColor)
                """if self.selectedColor == Qt.yellow:
                    self.text.setDefaultTextColor(QColor(Qt.black))"""
                self.startAnimation()

        return QVariant(value)

    def setupAnimation(self):
        self.originalPos = self.scenePos()
        self.timeline = QTimeLine(1000)
        self.timeline.setCurveShape(QTimeLine.SineCurve)
        self.timeline.setFrameRange(0, 100)
        self.timeline.setLoopCount(0)

        self.animation = QGraphicsItemAnimation()
        self.animation.setItem(self)
        self.animation.setTimeLine(self.timeline)
        self.timeline.stateChanged.connect(self.updateEdges)

        for i in range(100):
            newxPos = self.originalPos.x() - (0.01 * i) * (self.w /2)
            newyPos = self.originalPos.y() - (0.01 * i) * (self.w /2)

            self.animation.setPosAt(i/100.0, QPointF(newxPos, newyPos))
            self.animation.setScaleAt(i/100.0, 1 + 0.01 * i, 1 + 0.01 * i)
    def startAnimation(self):
        """Starts the selected node animation."""
        self.timeline.start()

    def stopAnimation(self):
        """Stops the selected node animation."""
        self.animation.setStep(0)
        self.timeline.stop()


class SquareNode(QGraphicsPolygonItem):
    def __init__(self, node, parent=None, scene=None):
        QGraphicsPolygonItem.__init__(self, parent, scene)
        path = QPainterPath()
        path.moveTo(0,0)
        path.addRect(0, 0, 130, 40)
        polygon = path.toFillPolygon()
        self.setPolygon(polygon)
        

        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        self.setAcceptsHoverEvents(True)
        self.arrows = []
        self.associateWithNode(node)

    def hoverEnterEvent(self, event):
        self.scene().views()[0].setDragMode(QGraphicsView.NoDrag)
    def hoverLeaveEvent(self, event):
        self.scene().views()[0].setDragMode(QGraphicsView.ScrollHandDrag)
    def addEdge(self, e):
        self.arrows.append(e)

    def intersectionPoint(self, startPoint):
        """Gives the intersection point when a line is drawn into the center
        of the node from the given startPoint."""

        centerLine = QLineF(startPoint, self.centerPos()) # The line
        p1 = self.polygon().first() + self.pos() # Take the first point

        intersectPoint = QPointF() # Define the intersection point.
        for i in self.polygon(): # For each point in the polygon,
            p2 = i + self.pos() # Determine that point's coords.
            polyLine = QLineF(p1, p2) # Imagine the edge between p1 and p2
            intersectType = polyLine.intersect(centerLine, intersectPoint)
            if intersectType == QLineF.BoundedIntersection: # If they intersect
                break # Stop. We'll use the intersectPoint.
            p1 = p2 # If not encountered an intersection, go on over other edges.

        return intersectPoint

    def centerPos(self):
        """Returns the center coordinate of the item."""
        return QPointF(self.x() + self.w/2, self.y() + self.h/2)

    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionChange:
            for arrow in self.arrows:
                arrow.updatePosition()
            if self.scene():
                self.scene().update()
        return QVariant(value)

    """def mouseDoubleClickEvent(self, event):
        print self.centerPos()"""

    def associateWithNode(self, node):
        self.color = QColor(node.graphics.outline)
        self.setBrush(self.color) # Give the category color.
        margin = 2.0
        # Construct the text.
        self.text = QGraphicsTextItem(self)
        self.text.root = self
        #if int(node.graphics.outline[1:], 16) < int("FFFFFF", 16) / 2:
            #self.text.setDefaultTextColor(QColor(Qt.white))
        if node.graphics.outline == "#000000":
            self.text.setDefaultTextColor(QColor(Qt.white))


        textFont = QFont()
        textFont.setBold(True)
        textFont.setPointSize(16)
        self.text.setFont(textFont)

        # Set node id as text.
        #self.text.setPlainText("YNL199C")
        self.text.setPlainText(node.label)


        # Define bounding rect
        boundRect = self.text.boundingRect()
        self.w = boundRect.width() + margin
        self.h = boundRect.height() + margin

        # Store node data
        self.node = node
        # Set position of the node:
        self.setPos(QPointF(node.graphics.x - self.w/2, node.graphics.y - self.h/2))
        tip = "Category: %s" % CATEGORY_COLORS[node.parameter]
        self.setToolTip(tip)
            
        # Leave some margin for the text.
        self.text.setPos(1,1)
