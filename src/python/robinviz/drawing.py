# -*- coding: utf-8 -*-
# encoding: utf-8

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtCore import qFuzzyCompare
from pygml import Graph
from os.path import normcase
import os
from PyQt4 import QtWebKit
from bicluster import BiclusterWindow
import math
from utils.info import root, ap

COLORS18 = {
"A": "#CB2838",
"B": "#FEC9BD",
"C": "#5EF6DA",
"D": "#321B28",
"E": "#CD9BB4",
"F": "#28E4F2",
"G": "#BEF98A",
"H": "#6C9627",
"I": "#F481F0",
"J": "#8B7F82",
"K": "#4003C0",
"L": "#499411",
"M": "#E6C8BE",
"N": "#DB26B2",
"O": "#E4C645",
"P": "#87D29A",
"R": "#BC34F3",
"S": "#5D9154",
"X": "#808080",
}

GRAPH_LAYOUTS = {}
layoutFile = open("%s/layouts.ini" % root)
for line in layoutFile:
    if line.startswith("//"):
        continue # this is a comment
    name, filename = line.strip().split(":")
    GRAPH_LAYOUTS[name] = filename

CATEGORY_COLORS = {}
colorFile = open("%s/outputs/colors_func.txt" % root)
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
        # Commenting coordinate information for performance.
        self.emit(SIGNAL("sceneMouseMove"), QPointF(event.scenePos()))

    def focusOutEvent(self, event):
        if hasattr(self, 'focusEllipse'):
            if self.focusEllipse.scene() == self:
                self.removeItem(self.focusEllipse)
                self.update()
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
    	self.g = g = Graph()
        #gp = GMLParser(self.filename, g)
        g.read_gml(self.filename)
        g.prepare()

        self.numNodes = len(g.nodes)
        self.numEdges= len(g.edges)
        self.numElements = self.numNodes + self.numEdges
        
        for node in g.nodes:
            self.addNode(node)

	if not g.edges:
	    return
	    
	edgeWidthMin = int(g.edges[0].parameter)
        edgeWidthMax = int(g.edges[0].parameter)
        for edge in g.edges:
            weight = int(edge.parameter)
            if weight < edgeWidthMin:
                edgeWidthMin = weight
            elif weight > edgeWidthMax:
                edgeWidthMax = weight
        
        for edge in g.edges:
            edge.minWidth = edgeWidthMin
            edge.maxWidth = edgeWidthMax
            self.addEdge(edge)

        

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
        item = PiechartNode(node, parent=None, scene=self)
        #self.addItem(item)
        self.nodeDict[node] = item
        self.nodeDict[node.id] = item
        #item.associateWithNode(node)

    """def mousePressEvent(self, event):
        print dir(event)
        item = self.itemAt(event.scenePos())
        print item"""
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
        self.findBoundingRect()
        self.update()

    def toggleHighlight(self):
        """Toggles the boolean if the highlight is selected."""
        self.highlighted ^= 1
        self.update()

    def paint(self, painter, option, widget):
        """Defines how to paint the edge item."""
        # --------- Set the pen settings -----------
        thickPen = QPen()
        #thickPen.setWidth(10*self.edge.parameter / self.edge.minWidth)
        #thickPen.setColor(QColor(self.edge.graphics.fill))
        if self.highlighted:
            thickPen.setColor(QColor(255,0,0))
        else:
            if self.edge.parameter == "10":
                thickPen.setColor(QColor("#8f8989"))
            else:
                thickPen.setColor(QColor(0,0,0))

        thicknessRange = 5
        if self.edge.maxWidth == self.edge.minWidth:
            newWidth = 2
        else:
            newWidth = 2 + thicknessRange*( (int(self.edge.parameter) - self.edge.minWidth) / (self.edge.maxWidth - self.edge.minWidth) )
            #newRatio = ((self.edge.parameter - self.edge.minWidth)/(self.edge.maxWidth - self.edge.minWidth)) * newRange + 30

        thickPen.setWidthF(newWidth)
        #thickPen.setColor(QColor(0,0,0, newRatio))
        painter.setPen(thickPen)

        # --------- Now draw the line(s) -----------
        
        
        line = None
        for i in range( len(self.path) - 1 ): # take first n-1 line segments
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
    def __init__(self, parent=None, scene=None):
        QGraphicsItem.__init__(self, parent, scene)
        self.highlighted = False
    
    #----------- Event Methods ------------------
    def hoverEnterEvent(self, event):
        """When hovered on the node, we make the scene unable to be moved by dragging."""
        for view in self.scene().views():
            view.setDragMode(QGraphicsView.NoDrag)

        if len(self.scene().items()) > 1000:
            # If we have lots of items in the scene, do not apply highligting.
            return
        #if not self.isSelected():
        self.toggleHighlight()
        
    def hoverLeaveEvent(self, event):
        """When hovering is off the node, we make the scene able to be moved by dragging."""
        for view in self.scene().views():
            view.setDragMode(QGraphicsView.ScrollHandDrag)
        if len(self.scene().items()) > 1000:
            return
        if not self.isSelected():
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
        self.highlighted ^= 1
        self.update()

    def qMin(self, a1,  a2):
        if a1 >= a2:
            return a2
        else:
            return a1
    def qMax(self, a1,  a2):
        if a1 <= a2:
            return a2
        else:
            return a1

    def qt_graphicsItem_highlightSelected(self, item, painter, option):
        murect = painter.transform().mapRect(QRectF(0, 0, 1, 1))
        """if abs(self.qMax(murect.width(), murect.height()) + 1 - 1) < 0.00001:
            return"""

        if qFuzzyCompare(self.qMax(murect.width(), murect.height()) + 1, 1):
            return
        mbrect = painter.transform().mapRect(item.boundingRect())
        if self.qMin(mbrect.width(), mbrect.height()) < 1.0:
            return
        itemPenWidth = 1.0
        pad = itemPenWidth / 2
        penWidth = 0 # cosmetic pen

        fgcolor = option.palette.windowText().color()
        red = 0 if fgcolor.red() > 127 else 255
        green = 0 if fgcolor.green() > 127 else 255
        blue = 0 if fgcolor.blue() > 127 else 255
        bgcolor = QColor ( # ensure good contrast against fgcolor
        red, green, blue)

        painter.setPen(QPen(bgcolor, penWidth, Qt.SolidLine))
        painter.setBrush(Qt.NoBrush)
        painter.drawRect(item.boundingRect().adjusted(pad, pad, -pad, -pad))
        painter.setPen(QPen(option.palette.windowText(), 0, Qt.DashLine))
        painter.setBrush(Qt.NoBrush)
        painter.drawRect(item.boundingRect().adjusted(pad, pad, -pad, -pad))
        
class CircleNode(NodeItem):
    """
    NodeItem with circle shape
    """
    def __init__(self, node, parent=None, scene=None, label=None):
        NodeItem.__init__(self, parent, scene)
        # Object Creation
        # ---------------------
        
        # Default Values
        # ---------------------
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        try:
            # available only in Qt 4.6
            #self.setFlag(QGraphicsItem.ItemSendsScenePositionChanges, True)
            self.setFlag( QGraphicsItem.ItemSendsGeometryChanges)
        except:
            # no need to do this in Qt 4.5
            pass

        self._scene = scene
        self.label = label
        self.setAcceptsHoverEvents(True)
        self.edges = []
        self.setOpacity(0.5)
        self.noProperties = False

        # Setup Operations
        # ---------------------
        self.associateWithNode(node)


    #----------- Event Methods ------------------
    def setNoProperties(self):
        self.noProperties = True
        
    def contextMenuEvent(self, event):
        # TODO: Workaround here. Fix it.
        # We can setNoProperties to prevent properties action to be added.
        menu = QMenu()
        goTable = menu.addAction("GO Table")
        
        # ======== DETAILED INFORMATION =======
        detailedInformation = None
        if self.label:
            detailedInformation = menu.addAction("Detailed Information (Online)")

        # ======== PROPERTIES =================
        propertiesAction = None
        if not self.noProperties:
            propertiesAction = menu.addAction("Properties")

        # ======== EXECUTE ====================
        action = menu.exec_(event.screenPos())
        if action == propertiesAction:
            if not hasattr(self, 'biclusterWindow'):
                self.biclusterWindow = BiclusterWindow(self.node.id)

            self.biclusterWindow.showMaximized()
        elif action == goTable:
            self.showGOTable()
        elif action == detailedInformation:
            self.showDetailedInformation()

    def showGOTable(self):
        path = normcase("outputs/go/gobicluster%d.html" % self.node.id)
        if os.path.exists(path):
            self.GOTable= QtWebKit.QWebView()
            self.GOTable.setUrl(QUrl(path))
            self.GOTable.show()
        else:
            QMessageBox.information(None, 'GO Table not found.',
     "You need to run the program with the Gene Ontology File option in Biological Settings Tab checked and provide the GO file.")

    def showDetailedInformation(self):
        code = open(ap("assocdata/category_codes.txt")).readlines()[self.node.id].strip()
        #TODO: fix this. get the filename from settings.yaml
        html = """
        <html><head></head><body>
        <form id="form" action="http://amigo.geneontology.org/cgi-bin/amigo/search.cgi" method="post">
        <input type="hidden" name="search_query" value="%(code)s"/>
        <input type="hidden" name="search_constraint" value="term"/>
        <input type="hidden" value="454amigo1288983527" name="session_id"/>

        <input type="hidden" value="new-search" name="action"/>
        
        <input type="submit" value="Press here to continue"/>
        </form>

        <script type="text/javascript">
        function myfunc () {
        var frm = document.getElementById("form");
        frm.submit();
        }
        window.onload = myfunc;
        </script>
        </body></html>
        """ % { 'code': code,                
                }

        self.detailBrowser = QtWebKit.QWebView()
        self.detailBrowser.setHtml(html)
        self.detailBrowser.showMaximized()

    def updateEdges(self):
        for edge in self.edges:
            edge.updatePosition()
            
    def mousePressEvent(self, event):
        QGraphicsItem.mousePressEvent(self, event)
        self.updateEdges()            
        self.stopAnimation()
        
    def mouseReleaseEvent(self, event):
        QGraphicsItem.mouseReleaseEvent(self, event)
        localPos = event.pos()
        scenePos = event.scenePos()

        newPos = QPointF(scenePos.x() - localPos.x(), scenePos.y() - localPos.y())
        self.setPos(newPos)

        self.updateEdges()
        self._scene.update()
        self.setupAnimation()
        self.toggleAnimation()
        
    def toggleAnimation(self):
        action = {
                    QTimeLine.Running : self.stopAnimation,
                    QTimeLine.NotRunning: self.startAnimation
                }

        action[self.timeline.State()]()
            
    def itemChange(self, change, value):        
        if change == QGraphicsItem.ItemPositionChange or change == QGraphicsItem.ItemTransformHasChanged:
            self.updateEdges()
        elif change == QGraphicsItem.ItemSelectedChange:
            if self.isSelected():
                self.stopAnimation()
                self.toggleHighlight()
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()
                self.scene().update()
            else:
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()
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
    
    def setPercentageColors(self, colors):
        if colors:
            self.percentColors = colors
            self.lighterColors = [ ( color[0].lighter(), color[1] ) for color in colors]
        else:
            self.percentColors = [ ( QColor(COLORS18.get("X")) , 100) ]
            self.lighterColors = [ ( QColor(COLORS18.get("X")).lighter() , 100)]

        self.num_colors = len(self.percentColors)

    def paint(self, painter, option, widget):
        rectangle = QRectF(0.0, 0.0, self.w, self.w)
        startAngle = 0

        # selection box
        if option.state & QStyle.State_Selected:
            self.qt_graphicsItem_highlightSelected(self, painter, option)

        if self.highlighted:
            colors = self.lighterColors
        else:
            colors = self.percentColors
            
        if self.num_colors == 1:
            painter.setBrush(QBrush(colors[0][0]))
            painter.drawEllipse(rectangle)
            return
        for color in colors:
            angle = color[1] * 360 * 16
            painter.setBrush(QBrush(color[0]))
            painter.drawPie(rectangle, startAngle, angle)
            startAngle += angle
            
    def boundingRect(self):
        return QRectF(0, 0, self.w, self.w)

    def associateWithNode(self, node):
        # Store node data
        self.node = node
        self.w = self.h = node.graphics.w


        # =============== COLORS ===================
        f = open("outputs/enrich/pie%d.txt" % node.id)
        colors = []
        for line in f:
            letter, percentage = line.split(" ")
            percentage = float(percentage)
            code = COLORS18.get(letter)
            if not code:
                code = COLORS18.get("X")
            colors.append( (QColor(code), percentage) )
        self.setPercentageColors(colors)

        # Set position of the node:
        self.setPos(QPointF(node.graphics.x - self.w/2, node.graphics.y - self.w/2))
        

        """# ============== ID INSIDE NODE ==================
        # Construct the text.
        self.text = QGraphicsTextItem(self)
        self.text.root = self
        textFont = QFont()
        textFont.setBold(True)
        #textFont.setPointSize(20)
	textFont.setPixelSize(self.w/4)
        self.text.setFont(textFont)
        if self.defaultColor < 250 or self.defaultColor in (Qt.red, Qt.green, Qt.blue, Qt.black, QColor("#0000CD")):
            self.text.setDefaultTextColor(QColor(Qt.white))

        # Set node id as text.
        self.text.setPlainText(str(node.id))
        self.text.contextMenuEvent = self.contextMenuEvent
        # Define bounding rect
        boundRect = self.text.boundingRect()

        # Align text to the center.
        self.text.setPos((self.w - boundRect.width()) / 2, (self.w - boundRect.height()) / 2)"""

        # =============== LABEL ==================
        # Set label
        if self.label:
            self.labelText = QGraphicsTextItem(self)
            self.labelText.root = self
            labelFont = QFont()
            labelFont.setBold(False)
            labelFont.setPixelSize(self.w/4)
            self.labelText.setFont(labelFont)
            self.labelText.setPlainText(self.label)
        else:
            print "no label for %d" % node.id

        # Position the label test
        self.labelText.setPos(  (self.w + 3), -1    )

        self.setupAnimation()

    def setupAnimation(self):
        """Sets up how the animation shall be. Calculates positions."""
        self.originalPos = self.scenePos()
        animationPeriod = 1500 + (self._scene.numElements - 50) * 3
        self.timeline = QTimeLine(animationPeriod)
        self.timeline.setCurveShape(QTimeLine.SineCurve)
        self.timeline.setFrameRange(0, 24)
        self.timeline.setLoopCount(0)

        self.animation = QGraphicsItemAnimation()
        self.animation.setItem(self)
        self.animation.setTimeLine(self.timeline)
        self.timeline.stateChanged.connect(self.updateEdges)
        #self.timeline.frameChanged.connect(self.frameChanged) # consuming so much cycles!

        #rect = self.boundingRect()
        #self.refreshFrame = QRectF(rect.x()-30, rect.y()+30, rect.width()+60, rect.height()+60)
        
        for i in range(100):
            newxPos = self.originalPos.x() - (0.01 * i) * (self.w /2)
            newyPos = self.originalPos.y() - (0.01 * i) * (self.w /2)

            self.animation.setPosAt(i/100.0, QPointF(newxPos, newyPos))
            self.animation.setScaleAt(i/100.0, 1 + 0.01 * i, 1 + 0.01 * i)
        
    def frameChanged(self):
        if self.timeline.currentFrame() == self.timeline.endFrame()-1:
            self._scene.update() # self.refreshFrame)

    def startAnimation(self):
        """Starts the selected node animation."""
        for edge in self.edges:
            edge.prepareGeometryChange()
        self.timeline.start()

    def stopAnimation(self):
        """Stops the selected node animation."""
        if hasattr(self, 'animation'):
            # to avoid fake nodes (TODO: should remove these lines later)
            self.animation.setStep(0)
            self.timeline.stop()


class TinyNode(QGraphicsEllipseItem, NodeItem):
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
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        try:
            # available only in Qt 4.6
            #self.setFlag(QGraphicsItem.ItemSendsScenePositionChanges, True)
            self.setFlag( QGraphicsItem.ItemSendsGeometryChanges)
        except:
            # no need to do this in Qt 4.5
            pass

        self._scene = scene
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
    def mouseReleaseEvent(self, event):
        """When released the button (stopped moving), update the edges/scene."""
        QGraphicsItem.mouseReleaseEvent(self, event)
        self.updateEdges()
        self.updateLabel()
        self.scene().update()
    def setupAnimation(self):
        pass
    
    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionChange or change == QGraphicsItem.ItemTransformHasChanged:
            self.updateEdges()
        elif change == QGraphicsItem.ItemSelectedChange:
            if self.isSelected():
                self.toggleHighlight()
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()
                self.scene().update()
            else:
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()

        return QVariant(value)

    def contextMenuEvent(self, event):
        menu = QMenu()
        detailedInformation = menu.addAction("Detailed Information (Online)")
        displayNeighborsAction = menu.addAction("Display Neighbors in the whole PPI")
        action = menu.exec_(event.screenPos())
        if action == displayNeighborsAction:
	    from windows import SinglePeripheralViewWindow
	    self.specialWindow = SinglePeripheralViewWindow(self.scene().views()[0].__class__ , scene=None)
            geneId = self.node.label.split("_")[0]
	    neihgboringFilename = "outputs/graphs/%s.gml" % geneId
	    if not os.path.exists(neihgboringFilename):
		os.system("./proteinScreen.exe %s TXT" % self.node.label)
	    self.specialWindow.loadGraph(neihgboringFilename)
            self.specialWindow.showMaximized()
	elif action == detailedInformation:
	    url = "http://thebiogrid.org/search.php?search=%s&organism=all" % geneId
	    self.detailBrowser = QtWebKit.QWebView()
            self.detailBrowser.setUrl(QUrl(url))
            self.detailBrowser.showMaximized()
            
    #----------- Data Structural Methods ------------------

    #----------- GUI / Geometric Methods ------------------
    def updateLabel(self):
        self.text.setPos(self.pos().x() + self.w, self.pos().y() - self.w)
        
    def intersectionPoint(self, startPoint):
        """Gives the intersection point when a line is drawn into the center
        of the circle node from the given startPoint."""

        xdiff = self.centerPos().x() - startPoint.x()
        ydiff = self.centerPos().y() - startPoint.y()

        if ydiff == 0:
            # inside it.
            return

        intersectPoint = QPointF()

        # End point
        r = self.w/2

        r_over_h = r / (math.sqrt(ydiff**2 + xdiff**2))

        a = xdiff * r_over_h
        c = ydiff * r_over_h

        intersectPoint = self.centerPos() + QPointF(-a,-c)

        return intersectPoint

    def associateWithNode(self, node):
        # Store node data
        self.node = node
    
	self.setSize(20)
	try:
	    self.setCategoryInformation()
	except:
	    pass # do nothing if category information shall not be provided.	
	    

        # Set Color
        self.defaultColor = QColor(node.graphics.outline)
        self.currentColor = self.defaultColor
        self.highlightedColor = self.defaultColor.lighter(150)
        self.setBrush(self.defaultColor)

        # Set position of the node:
        self.setPos(QPointF(node.graphics.x - self.w/2, node.graphics.y - self.w/2))
        self.setRect(0, 0, self.w, self.w)

        # Construct the text.
        self.text = QGraphicsTextItem(node.label, None, self._scene)
        self.text.root = self
        self.text.contextMenuEvent = self.contextMenuEvent
        # Define bounding rect
        
        # Align text.
        self.updateLabel()
        
    def setSize(self, width=20):
	self.w = self.h = width
	
    def setCategoryInformation(self):
        """Sets category information to the node as tooltip."""
        tip = "Category: %s" % CATEGORY_COLORS[self.node.parameter]
        self.setToolTip(tip)


class PiechartNode(NodeItem):
    def __init__(self, node, parent=None, scene=None, label=None):
        NodeItem.__init__(self, parent, scene)

        # Default Values
        # ---------------------
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        try:
            # available only in Qt 4.6
            #self.setFlag(QGraphicsItem.ItemSendsScenePositionChanges, True)
            self.setFlag( QGraphicsItem.ItemSendsGeometryChanges)
        except:
            # no need to do this in Qt 4.5
            pass

        self._scene = scene
        self.label = label
        self.setAcceptsHoverEvents(True)
        self.edges = []
        self.setOpacity(0.5)
        self.w = 20.0
        self.setColors( map(QColor, [Qt.green, Qt.red, Qt.blue, Qt.yellow]) )

        # Setup Operations
        # ---------------------
        if node:
            self.associateWithNode(node)

    def setColors(self, colors):
        if colors:
            self.colors = colors
        else:
            # if no color provided, assume that's an X. (unknown)
            self.colors = [ QColor(COLORS18.get("X")) ]
            
        self.num_colors = len(self.colors)
        if len(self.colors) > 1:
            self.angle_per_color = 16* (360 / self.num_colors)
        
    def toggleHighlight(self):
        for i, color in enumerate(self.colors):
            if self.isSelected():
                self.colors[i] = color.darker(100)
            else:
                self.colors[i] = color.lighter(100)

        self.update(self.boundingRect())

    def paint(self, painter, option, widget):
        rectangle = QRectF(0.0, 0.0, self.w, self.w)
        startAngle = 0

        if option.state & QStyle.State_Selected:
            self.qt_graphicsItem_highlightSelected(self, painter, option)
            
        if self.num_colors == 1:
            painter.setBrush(QBrush(self.colors[0]))
            painter.drawEllipse(rectangle)
            return
        for color in self.colors:
            painter.setBrush(QBrush(color))
            painter.drawPie(rectangle, startAngle, self.angle_per_color)
            startAngle += self.angle_per_color
                        
    def mouseReleaseEvent(self, event):
        """When released the button (stopped moving), update the edges/scene."""
        QGraphicsItem.mouseReleaseEvent(self, event)
        self.updateEdges()
        self.updateLabel()
        self.scene().update()

    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionChange or change == QGraphicsItem.ItemTransformHasChanged:
            self.updateEdges()
        elif change == QGraphicsItem.ItemSelectedChange:
            if self.isSelected():
                self.toggleHighlight()
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()
                self.scene().update()
            else:
                for edge in self.edges:
                    edge.toggleHighlight()
                    edge.end.toggleHighlight()

        return QVariant(value)

    def boundingRect(self):
        return QRectF(0, 0, self.w, self.w)

    def contextMenuEvent(self, event):
        menu = QMenu()
        detailedInformation = menu.addAction("Detailed Information (Online)")
        displayNeighborsAction = menu.addAction("Display Neighbors in the whole PPI")
        action = menu.exec_(event.screenPos())

        geneId = self.node.label.split("_")[0]
        
        if action == displayNeighborsAction:
	    from windows import SinglePeripheralViewWindow
	    self.specialWindow = SinglePeripheralViewWindow(self.scene().views()[0].__class__ , scene=None)
	    neihgboringFilename = "outputs/graphs/%s.gml" % self.node.label
	    if not os.path.exists(neihgboringFilename):
		os.system("./proteinScreen.exe %s TXT" % self.node.label)
	    self.specialWindow.loadGraph(neihgboringFilename)
            self.specialWindow.showMaximized()
	elif action == detailedInformation:
	    url = "http://thebiogrid.org/search.php?search=%s&organism=all" % geneId
	    self.detailBrowser = QtWebKit.QWebView()
            self.detailBrowser.setUrl(QUrl(url))
            self.detailBrowser.showMaximized()
    #----------- GUI / Geometric Methods ------------------
    def updateLabel(self):
        self.labelText.setPos(  (self.w + 3), -1    )

    def intersectionPoint(self, startPoint):
        """Gives the intersection point when a line is drawn into the center
        of the circle node from the given startPoint."""

        xdiff = self.centerPos().x() - startPoint.x()
        ydiff = self.centerPos().y() - startPoint.y()

        if ydiff == 0:
            # inside it.
            return

        intersectPoint = QPointF()

        # End point
        r = self.w/2

        r_over_h = r / (math.sqrt(ydiff**2 + xdiff**2))

        a = xdiff * r_over_h
        c = ydiff * r_over_h

        intersectPoint = self.centerPos() + QPointF(-a,-c)

        return intersectPoint

    def associateWithNode(self, node):
        # Store node data
        self.node = node

	self.setSize(50)
	try:
	    self.setCategoryInformation()
	except:
	    pass # do nothing if category information shall not be provided.


        """# Set Color
        self.defaultColor = QColor(node.graphics.outline)
        self.currentColor = self.defaultColor
        self.highlightedColor = self.defaultColor.lighter(150)
        self.setBrush(self.defaultColor)"""

        # Set position of the node:
        self.setPos(QPointF(node.graphics.x - self.w/2, node.graphics.y - self.w/2))

        # Construct the text.
        #self.text = QGraphicsTextItem(node.label, None, self._scene)
        #self.text.root = self
        #self.text.contextMenuEvent = self.contextMenuEvent
        # Define bounding rect

        if node.label:
            label, colors = node.label.split("_")
            self.labelText = QGraphicsTextItem(self)
            self.labelText.root = self
            labelFont = QFont()
            labelFont.setBold(False)
            labelFont.setPixelSize(self.w/4)
            self.labelText.setFont(labelFont)
            self.labelText.setPlainText(label)
             # find hex codes for colors
            letters = colors.split(":")
            codes = filter(lambda color: color is not None, map(COLORS18.get, letters) )
            colors = map(QColor, codes)
            self.setColors(colors)
        else:
            print "no label"
        
        # Position the label text
        self.updateLabel()

    def setSize(self, width=20):
	self.w = self.h = width

    def setCategoryInformation(self):
        """Sets category information to the node as tooltip."""
        tip = "Category: %s" % CATEGORY_COLORS[self.node.parameter]
        self.setToolTip(tip)
