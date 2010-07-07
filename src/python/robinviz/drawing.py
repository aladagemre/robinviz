# encoding: utf-8

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from pygml import Graph
from os.path import normcase
import os
from PyQt4 import QtWebKit
from bicluster import BiclusterWindow
import math


GRAPH_LAYOUTS = {}
layoutFile = open("layouts.ini")
for line in layoutFile:
    if line.startswith("//"):
        continue # this is a comment
    name, filename = line.strip().split(":")
    GRAPH_LAYOUTS[name] = filename

CATEGORY_COLORS = {}
colorFile = open("outputs/colors_func.txt")
lineNum = 0
for line in colorFile:
    name, r, g, b = line.strip().split()
    CATEGORY_COLORS[str(lineNum)] = name.replace("_", " ")
    lineNum+=1
    

class Scene(QGraphicsScene):
    def __init__(self, parent=None):
        QGraphicsScene.__init__(self, parent)
        
        # Object Creation
        # ---------------------

        # Default Values
        # ---------------------
        self.directed = False
        self.menu = None

        # Setup Operations
        # ---------------------
        

    #----------- Event Methods ------------------
    def mouseMoveEvent(self, event):
        QGraphicsScene.mouseMoveEvent(self, event)
        self.emit(SIGNAL("sceneMouseMove"), QPointF(event.scenePos()))

    #----------- Data Structural Methods ------------------

    def loadGraph(self, filename):
        self.nodeDict = {}
        self.edgeDict = {}
        self.filename = filename
        self.constructGraph()
        for view in self.views():
            view.setDragMode(QGraphicsView.ScrollHandDrag)
            view.update()

    def reloadGraph(self, filename):
        # First clear
        self.clear()
        self.setSceneRect(self.itemsBoundingRect())

        # Then load the graph
        self.loadGraph(filename)
        self.setSceneRect(self.itemsBoundingRect())

    def constructGraph(self):
    	g = Graph()
        #gp = GMLParser(self.filename, g)
        g.read_gml(self.filename)
        g.prepare()

        for node in g.nodes:
            self.addNode(node)

        edgeWidthMin = g.edges[0].graphics.width
        edgeWidthMax = g.edges[0].graphics.width
        for edge in g.edges:
            if edge.graphics.width < edgeWidthMin:
                edgeWidthMin = edge.graphics.width
            elif edge.graphics.width > edgeWidthMax:
                edgeWidthMax = edge.graphics.width

        for edge in g.edges:
            edge.minWidth = edgeWidthMin
            edge.maxWidth = edgeWidthMax
            self.addEdge(edge)

        self.g = g

    def addEdge(self, edge):
        source = edge.u
        target = edge.v

        path = []
        # If has segments, add to path list.
        if hasattr(edge.graphics, "Line"):
            for Point in edge.graphics.Line:
                path.append(QPointF(Point.x, Point.y))
        else:
            path = [QPointF(source.graphics.x, source.graphics.y), QPointF(target.graphics.x, target.graphics.y)]

        startItem = self.nodeDict.get(source.id)
        endItem = self.nodeDict.get(target.id)

        #print startItem, endItem, path, edge
        if startItem and endItem:
            e = EdgeItem(startItem, endItem, path, edge)
            self.addItem(e)
            startItem.addEdge(e)
            endItem.addEdge(e)
        else:
            pass
            # TODO: Fix here
            #print "start:",getattr(source, "id") or None
            #print "end:", getattr(target, "id") or None

    #----------- GUI / Geometric Methods ------------------
    # Not yet
    # ------------------------------
    



class MainScene(Scene):
    def __init__(self, parent=None):
        Scene.__init__(self, parent)

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
        item = RectNode(node)
        self.addItem(item)
        self.nodeDict[node] = item
        self.nodeDict[node.id] = item
        item.associateWithNode(node)


class EdgeItem(QGraphicsItem):
    def __init__(self, start, end, path, edge):
        QGraphicsItem.__init__(self, None, None)
        self.start = start          # Source GraphicsItem (node)
        self.end   = end            # Target GraphicsItem (node)
        self.path  = path           # path contains all the points of an edge.
        self.edge = edge            # corresponding graph edge

        # Object creation
        self.arrowHead = QPolygonF()    # arrow head

        # Default Values
        self.onlyUpMiddle = True        # won't show arrow head if target is downwards.
        self.highlighted = False        # if edge is highlighted
        self.rect = self.findBoundingRect()
        self.setZValue(-10)

        # Setup operations
        # TODO: test if the order poses a problem.
        self.__avoidCrossingNodeBorder()  # update path to avoid crossing node borders


    def startPoint(self):
        """Returns the point which is the last point of a line
        drawn from the second element of the path. Aim is to avoid
        the edge, crossing the source node border."""
        
        return self.start.intersectionPoint(self.path[1])

    def endPoint(self):
        """Returns the point which is the last point of a line
        drawn from 'the second element from the end of the path'. Aim
        is to avoid the edge, crossing the target node border."""
        return self.end.intersectionPoint(self.path[-2])

    def __avoidCrossingNodeBorder(self):
        """Updates the edge path so that no node border crossing occurs."""
        intersectionPoint = self.startPoint()
        self.path[0] = intersectionPoint

        intersectionPoint = self.endPoint()
        self.path[-1] = intersectionPoint

    def replaceEdge(self, path, edge):
        self.path = path
        self.edge = edge

        self.__avoidCrossingNodeBorder()  # update path to avoid crossing node borders
        self.rect = self.findBoundingRect()

    def updatePosition(self):
        self.__avoidCrossingNodeBorder()  # update path to avoid crossing node borders
        self.update()

    def toggleHighlight(self):
        """Toggles the boolean if the highlight is selected."""
        self.highlighted ^= 1
        self.update()

    def paint(self, painter, option, widget):
        """Defines how to paint the edge item."""
        # --------- Set the pen settings -----------
        thickPen = QPen()
        #thickPen.setWidth(10*self.edge.graphics.width / self.edge.minWidth)
        #thickPen.setColor(QColor(self.edge.graphics.fill))
        if self.highlighted:
            thickPen.setColor(QColor(255,0,0))
        else:
            thickPen.setColor(QColor(0,0,0))

        thicknessRange = 10
        if self.edge.maxWidth == self.edge.minWidth:
            newWidth = 2
        else:
            newWidth = 2 + (thicknessRange * (self.edge.graphics.width - self.edge.minWidth)) / (self.edge.maxWidth - self.edge.minWidth)
            #newRatio = ((self.edge.graphics.width - self.edge.minWidth)/(self.edge.maxWidth - self.edge.minWidth)) * newRange + 30

        thickPen.setWidthF(newWidth)
        #thickPen.setColor(QColor(0,0,0, newRatio))
        painter.setPen(thickPen)

        # --------- Now draw the line(s) -----------
        
        
        line = None
        for i in range(len(self.path) - 1): # take first n-1 line segments
            s = self.path[i]                # starting point of the segment.
            e = self.path[i+1]              # end point of the segment
            lastLine = line                 # keeping the last line for placing arrow head
            line = QLineF(s.x(), s.y(), e.x(), e.y())
            painter.drawLine(line)

        if not self.scene().directed:       # if scene is not directed, stop here!
            return

        # --------- Now add the arrow head-----------

        if line.p2().y() < line.p1().y():
            if self.scene().onlyUp:
                # If the line is going upwards, then draw the arrows.
                if len(self.path) > 3:
                    # If more than 3 segments, better use 2nd from the end.
                    arrowBase = lastLine
                else:
                    # If less than 3 segments, use the last segment.
                    arrowBase = line

                centerPoint = (arrowBase.p1() + arrowBase.p2()) / 2.0

                line = QLineF(arrowBase.p1(), centerPoint)


            # Now draw the arrow
            arrowSize = 30.0
            myColor = QColor(Qt.black)
            painter.setBrush(myColor)
            angle = math.acos(line.dx() / line.length())
            if line.dy() >= 0:
                angle = (math.pi * 2.0) - angle

            arrowP1 = line.p2() - QPointF(math.sin(angle + math.pi / 3.0) * arrowSize,
                                            math.cos(angle + math.pi / 3) * arrowSize)
            arrowP2 = line.p2() - QPointF(math.sin(angle + math.pi - math.pi / 3.0) * arrowSize,
                                            math.cos(angle + math.pi - math.pi / 3.0) * arrowSize)

            self.arrowHead.clear()
            for point in [line.p2(), arrowP1, arrowP2]:
                self.arrowHead.append(point)

            #painter.drawLine(line)
            painter.drawPolygon(self.arrowHead)

        # --------- Arrow drawing has finished -----------
        
        """
        if self.isSelected():
            painter.setPen(QPen(myColor, 1, Qt.DashLine))
            myLine = QLineF(line)
            myLine.translate(0, 4.0)
            painter.drawLine(myLine)
            myLine.translate(0,-8.0)
            painter.drawLine(myLine)"""


    def findBoundingRect(self):
        """Finds the bounding rect of the edge item (covering all the bends)"""
        min_x = float("inf")
        max_x = float("-inf")
        min_y = float("inf")
        max_y = float("-inf")
        for point in self.path:
            if point.x() < min_x:
                min_x = point.x()
            if point.x() > max_x:
                max_x = point.x()

            if point.y() < min_y:
                min_y = point.y()
            if point.y() > max_y:
                max_y = point.y()
        return QRectF(min_x, min_y, max_x - min_x, max_y - min_y)
    
    def boundingRect(self):
        """Returns the boundingRect of the Edge item."""
        return self.rect



class NodeItem(QGraphicsItem):
    """Common methods for NodeItem of various shapes."""

    #----------- Event Methods ------------------
    def hoverEnterEvent(self, event):
        """When hovered on the node, we make the scene unable to be moved by dragging."""
        self.scene().views()[0].setDragMode(QGraphicsView.NoDrag)
        self.toggleHighlight()
        
    def hoverLeaveEvent(self, event):
        """When hovering is off the node, we make the scene able to be moved by dragging."""
        self.scene().views()[0].setDragMode(QGraphicsView.ScrollHandDrag)
        self.toggleHighlight()
        
    #----------- Data Structural Methods ------------------
    def addEdge(self, e):
        """Adds the EdgeItem e to edges list."""
        self.edges.append(e)

    def updateEdges(self):
        for edge in self.edges:
            edge.updatePosition()

    #----------- GUI / Geometric Methods ------------------
    def centerPos(self):
        """Returns the center coordinate of the item."""
        return QPointF(self.x() + self.w/2, self.y() + self.h/2)

    def blowUp(self):
        """Paints the node as it was blown up (burned)."""
        radialGrad = QRadialGradient (QPointF(25, 25), 30)
        radialGrad.setColorAt(0, Qt.black)
        radialGrad.setColorAt(0.5, Qt.white)
        radialGrad.setColorAt(1, Qt.black)
        self.defaultColor = radialGrad
    def toggleHighlight(self):
        if self.currentColor == self.defaultColor:
            self.currentColor = self.highlightedColor
        else:
            self.currentColor = self.defaultColor
        self.setBrush(self.currentColor)
        

class RectNode(QGraphicsPolygonItem, NodeItem):
    def __init__(self, node, parent=None, scene=None):
        QGraphicsPolygonItem.__init__(self, parent, scene)
        path = QPainterPath()
        path.moveTo(0,0)
        path.addRect(0, 0, 130, 40)
        polygon = path.toFillPolygon()
        self.setPolygon(polygon)
        self.setOpacity(0.5)
        self.defaultColor = Qt.blue


        self.setAcceptHoverEvents(True)
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        try:
            # available only in Qt 4.6
            #self.setFlag(QGraphicsItem.ItemSendsScenePositionChanges, True)
            self.setFlag( QGraphicsItem.ItemSendsGeometryChanges)
        except:
            # no need to do this in Qt 4.5
            pass
        
        self.setAcceptsHoverEvents(True)
        self.edges = []
        self.associateWithNode(node)

    def paint(self, painter, option,widget):
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setBrush(self.currentColor)
        painter.drawRoundedRect(0, 0, self.w, self.h, 5, 5)

    def mouseReleaseEvent(self, event):
        """When released the button (stopped moving), update the edges/scene."""
        QGraphicsItem.mouseReleaseEvent(self, event)
        self.updateEdges()
        self.scene().update()
    
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

    #========= Events =====================        
    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionChange:
            self.updateEdges()
        return QVariant(value)

    def associateWithNode(self, node):
        self.defaultColor = QColor(node.graphics.outline)
        self.highlightedColor = self.defaultColor.lighter(150)
        self.currentColor = self.defaultColor
        self.setBrush(self.currentColor) # Give the category color.
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

class CircleNode(QGraphicsEllipseItem, NodeItem):
    """
    NodeItem with circle shape
    """
    def __init__(self, node, parent=None, scene=None):
        QGraphicsEllipseItem.__init__(self, parent, scene)
        NodeItem.__init__(self)
        # Object Creation
        # ---------------------
        
        # Default Values
        # ---------------------
        #self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        try:
            # available only in Qt 4.6
            #self.setFlag(QGraphicsItem.ItemSendsScenePositionChanges, True)
            self.setFlag( QGraphicsItem.ItemSendsGeometryChanges)
        except:
            # no need to do this in Qt 4.5
            pass


        self.setAcceptsHoverEvents(True)
        self.edges = []
        self.defaultColor = QColor("#52C6FF")
        self.highlightedColor = QColor("#E6FF23")
        self.currentColor = self.defaultColor
        self.setOpacity(0.5)

        # Setup Operations
        # ---------------------
        self.associateWithNode(node)
        self.setBrush(self.defaultColor)

    #----------- Event Methods ------------------        
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


    def itemChange(self, change, value):        
        if change == QGraphicsItem.ItemPositionChange or change == QGraphicsItem.ItemTransformHasChanged:
            self.updateEdges()
        elif change == QGraphicsItem.ItemSelectedChange:
            if self.isSelected():
                self.toggleHighlight()
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()
                self.stopAnimation()
                self.scene().update()
            else:
                self.toggleHighlight()
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()
                """if self.highlightedColor == Qt.yellow:
                    self.text.setDefaultTextColor(QColor(Qt.black))"""
                self.startAnimation()

        return QVariant(value)

        
    #----------- Data Structural Methods ------------------

    #----------- GUI / Geometric Methods ------------------        
    def intersectionPoint(self, startPoint):
        """Gives the intersection point when a line is drawn into the center
        of the circle node from the given startPoint."""

        xdiff = self.centerPos().x() - startPoint.x()
        ydiff = self.centerPos().y() - startPoint.y()

        if ydiff == 0:
            return

        intersectPoint = QPointF()

        # End point
        r = self.w/2

        a = (r * ydiff) / (math.sqrt(ydiff**2 + xdiff**2))
        c = (a * xdiff) / ydiff

        intersectPoint = self.centerPos() + QPointF(-c,-a)

        return intersectPoint

    def associateWithNode(self, node):
        # Store node data
        self.node = node
        self.w = self.h = node.graphics.w

        # Set Color
        self.defaultColor = QColor(node.graphics.outline)
        self.currentColor = self.defaultColor
        self.highlightedColor = self.defaultColor.lighter(150)
        self.setBrush(self.defaultColor)

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
        if self.defaultColor < 250 or self.defaultColor in (Qt.red, Qt.green, Qt.blue, Qt.black, QColor("#0000CD")):
            self.text.setDefaultTextColor(QColor(Qt.white))

        # Set node id as text.
        self.text.setPlainText(str(node.id))
        self.text.contextMenuEvent = self.contextMenuEvent
        # Define bounding rect
        boundRect = self.text.boundingRect()
        # Align text to the center.
        self.text.setPos((self.w - boundRect.width()) / 2, (self.w - boundRect.height()) / 2)

        self.setupAnimation()

    def setupAnimation(self):
        """Sets up how the animation shall be. Calculates positions."""
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
