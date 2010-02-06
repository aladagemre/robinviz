#from parser import GMLParser
#from styles import *
#import sys
from pygml import Graph
from bicluster import *

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtWebKit

from os.path import split
from os.path import normcase
import os
import math

GRAPH_LAYOUTS = {}
layoutFile = open("layouts.ini")
for line in layoutFile:
    if line.startswith("//"):
        continue # this is a comment
    name, filename = line.strip().split(":")
    GRAPH_LAYOUTS[name] = filename


class View(QGraphicsView):
    def __init__(self, parent=None):
        QGraphicsView.__init__(self, parent)
        self.printer = QPrinter(QPrinter.HighResolution)
        self.printer.setPageSize(QPrinter.A4)
        self.useAnimation = True # TODO: Add this as an option
        
    def wheelEvent(self, event):
        coords = self.mapToScene(event.pos())

        self.centerOn(coords.x(), coords.y())
        #factor = 1.41 ** (event.delta() / 240.0)
        factor = 1.05 ** (event.delta() / 240.0)
        self.scale(factor, factor)

    def contextMenuEvent(self, event):
        # If clicked on an item, skip this menu, let it handle the signal
        if not self.scene():
            return
        
        itemRightClicked = self.scene().itemAt(self.mapToScene(event.pos()))
        if itemRightClicked:
            QGraphicsView.contextMenuEvent(self, event)
            return

        menu = QMenu(self)
        newWindow = menu.addAction("Open in &new window")

        menu.addSeparator()

        saveAsImage = menu.addAction("Save as &Image")
        saveAsGML= menu.addAction("Save as &GML")
        printGraph = menu.addAction("&Print")

        actionToFunction = {newWindow       : self.newWindow,
                            saveAsImage     : self.saveAsImage,
                            printGraph      : self.printGraph,
                            saveAsGML       : self.saveAsGML
                            }

        if hasattr(self, 'main'):
            enrichmentTable = menu.addAction("Enrichment Table")
            actionToFunction[enrichmentTable] = self.showEnrichmentTable
        else:
            clearAction = menu.addAction("Clear")
            actionToFunction[clearAction] = self.clearView
            
            menu.addSeparator()

            goTable = menu.addAction("GO Table")
            actionToFunction[goTable] = self.showGOTable

            propertiesAction = menu.addAction("Properties")
            actionToFunction[propertiesAction] = self.peripheralProperties

        # Now switch to layout menu
        switchToLayoutMenu = menu.addMenu("Switch To &Layout")
        for graphLayout in sorted(GRAPH_LAYOUTS.keys()):
            # Create action
            newLayoutAction = switchToLayoutMenu.addAction(graphLayout)
            newLayoutAction.isLayoutAction = True
            # Link action to the method.
            actionToFunction[newLayoutAction] = self.switchToLayout



        action = menu.exec_(self.mapToGlobal(event.pos()))
        if not action:
            return
        elif hasattr(action, 'isLayoutAction') and action.isLayoutAction:
            actionToFunction[action](action.text())
        else:
            actionToFunction[action]()

    def peripheralProperties(self):
        if not hasattr(self, 'biclusterWindow'):
            self.biclusterWindow = BiclusterWindow(self.scene().id)

        self.biclusterWindow.showMaximized()
            
    def clearView(self):
        self.setScene(None)
        
    def showGOTable(self):
        path = normcase("outputs/go/gobicluster%d.html" % self.scene().id)
        if os.path.exists(path):
            self.GOTable= QtWebKit.QWebView()
            self.GOTable.setUrl(QUrl(path))
            self.GOTable.show()
        else:
            QMessageBox.information(self, 'GO Table not found.',
     "You need to run the program with the Gene Ontology File option in Biological Settings Tab checked and provide the GO file.")
        
    def showEnrichmentTable(self):
        self.enrichmentTable= QtWebKit.QWebView()
        self.enrichmentTable.setUrl(QUrl(normcase("outputs/enrich/result.html")))
        self.enrichmentTable.showMaximized()
        
    def restoreEdges(self):
        newGraph = self.newGraph
        # Add the edges
        edgeWidthMin = newGraph.edges[0].graphics.width
        edgeWidthMax = newGraph.edges[0].graphics.width
        for edge in newGraph.edges:
            if edge.graphics.width < edgeWidthMin:
                edgeWidthMin = edge.graphics.width
            elif edge.graphics.width > edgeWidthMax:
                edgeWidthMax = edge.graphics.width

        for edge in newGraph.edges:
            edge.minWidth = edgeWidthMin
            edge.maxWidth = edgeWidthMax
            self.scene().addEdge(edge)
        for item in self.scene().items():
            if isinstance(item, EdgeItem):
                item.onlyUpMiddle = False
                
        self.scene().update()
        self.fitInView(self.scene().sceneRect(), Qt.KeepAspectRatio)

    def startLayoutAnimation(self):
        self.timer.start()

    def switchToLayout(self, layoutName):
        if self.setupLayoutAnimation(layoutName):
            self.startLayoutAnimation()
        
    def setupLayoutAnimation(self, layoutName):
        filename = self.scene().filename
        if not hasattr(self, 'originalFileName'):
                self.originalFileName = filename

        if layoutName == "Layered":
            # Switch back to original.
            self.scene().reloadGraph(self.originalFileName)
            return False
        else:
            # Find the exe
            exename = GRAPH_LAYOUTS[str(layoutName)]
            # Run the exe
            command = "%s %s" % (exename, filename)
            if os.name == "posix":
                command = "./" + command
            os.system(normcase(command))

            if not self.useAnimation:
                self.scene().reloadGraph("%s%s.gml" % (filename.split(".")[0], exename.split(".")[0]) )
                return False
            
            # Load the graph
            self.newGraph = newGraph = Graph()
            self.newGraph.read_gml("%s%s.gml" % (filename.split(".")[0], exename.split(".")[0]))
            newGraph.prepare()

            # Clear the scene
            for item in self.scene().items():
                if isinstance(item, EdgeItem):
                    self.scene().removeItem(item)
                    del item

            # Setup animation for each node
            self.timer = QTimeLine(5000)
            self.timer.setFrameRange(0, 100)
            self.animations = []
            for node in newGraph.nodes:
                item = self.scene().nodeDict[node.id]
                #item.setPos(node.graphics.x, node.graphics.y)
                oldPos = item.scenePos()
                x1 = oldPos.x()
                y1 = oldPos.y()
                x2 = node.graphics.x
                y2 = node.graphics.y

                dx = (x2 - x1) / 200.0
                dy = (y2 - y1) / 200.0
                
                animation = QGraphicsItemAnimation()
                animation.setItem(item)
                animation.setTimeLine(self.timer)
                self.animations.append(animation)
                for i in range(0,200):
                    animation.setPosAt(i/200.0, QPointF(x1+dx*i, y1+dy*i))
            
            self.timer.finished.connect(self.restoreEdges)
            return True
            
            
    def switchToLayoutNoAnimation(self, layoutName):
        filename = self.scene().filename
        if not hasattr(self, 'originalFileName'):
                self.originalFileName = filename
                
        if layoutName == "Layered":
            # Switch back to original.
            self.scene().reloadGraph(self.originalFileName)
        else:
            # Find the exe
            exename = GRAPH_LAYOUTS[str(layoutName)]
            # Run the exe
            command = "./%s %s" % (exename, filename)
            os.system(normcase(command))
            # Load the resulting file.
            self.scene().reloadGraph("%s%s.gml" % (filename.split(".")[0], exename.split(".")[0]) )

        self.fitInView(self.scene().sceneRect(), Qt.KeepAspectRatio)
        self.scene().update(self.scene().sceneRect())

    def printGraph(self):
        dialog = QPrintDialog(self.printer)
        if dialog.exec_():
            painter = QPainter(self.printer)
            painter.setRenderHint(QPainter.Antialiasing)
            painter.setRenderHint(QPainter.TextAntialiasing)
            self.scene().clearSelection()
            self.scene().render(painter)

    def saveAsImage(self):
        filename = QFileDialog.getSaveFileName(self, "Save Image",
                self.scene().filename.split(".")[0]+".png", "Images (*.png *.xpm *.jpg)")
        if not filename:
            return
        frame = self.scene().sceneRect()
        frame.moveTo(0., 0.)
        image = QImage(frame.size().toSize(), QImage.Format_RGB32)
        painter = QPainter(image)
        #painter.setRenderHint(QPainter.Antialiasing)
        #painter.setRenderHint(QPainter.TextAntialiasing)
        painter.fillRect(frame, QBrush(Qt.white))
        self.scene().render(painter)
        image.save(filename)
        painter.end()

    def saveAsGML(self):
        from shutil import copy
        originalFile = self.scene().filename
        filename = QFileDialog.getSaveFileName(self, "Save As GML",
                split(originalFile)[1], "Graphlet GML (*.gml)")
        if not filename:
            return
        
        copy(originalFile, filename)



class Scene(QGraphicsScene):
    def __init__(self, parent=None):
        QGraphicsScene.__init__(self, parent)
        self.gridActive = False
    
    def loadGraph(self, filename):
        self.nodeDict = {}
        self.edgeDict = {}
        self.filename = filename
        self.constructGraph()
    def mouseMoveEvent(self, event):
        QGraphicsScene.mouseMoveEvent(self, event)
        self.emit(SIGNAL("sceneMouseMove"), QPointF(event.scenePos()))
        
    def reloadGraph(self, filename):
        # First clear
        self.clear()
        #self.clear2()
        self.setSceneRect(self.itemsBoundingRect())
        
        # Then load the graph
        self.loadGraph(filename)
        self.setSceneRect(self.itemsBoundingRect())
        
    def clear2(self):
        """Removes all the items from the scene."""
        QGraphicsScene.clear(self)
        for item in self.items():
            if not isinstance(item, QGraphicsTextItem):
                self.removeItem(item)
                del item
        self.setSceneRect(self.itemsBoundingRect())
        
        """myrect = self.sceneRect()
        r = QGraphicsRectItem(myrect)
        self.addItem(r)
        print self.sceneRect()"""

    """def render(self, imagePainter):
        image= QImage(self.width(), self.height(),  QImage.Format_ARGB32_Premultiplied)
        self.image = image
        #imagePainter.initFrom(self)
        imagePainter.setRenderHint(QPainter.Antialiasing, True)
        imagePainter.setRenderHint(QPainter.Antialiasing)
        imagePainter.setRenderHint(QPainter.TextAntialiasing)
        imagePainter.setPen(self.palette().color(QPalette.Mid))
        imagePainter.setBrush(self.palette().brush(QPalette.AlternateBase))
        imagePainter.eraseRect(self.sceneRect())

        QGraphicsScene.render(self, imagePainter)

        imagePainter.end()
        painter = QPainter()
        painter.drawImage(0, 0, image)"""
        
    def drawBackground(self, painter, rect):
        if self.gridActive:
            gridSize = 50
            left = int(rect.left()) - (int(rect.left()) % gridSize)
            top = int(rect.top()) - (int(rect.top()) % gridSize)
            lines = []
            right = int(rect.right())
            bottom = int(rect.bottom())
            for x in range(left, right, gridSize):
                lines.append(QLineF(x, rect.top(), x, rect.bottom()))
            for y in range(top, bottom, gridSize):
                lines.append(QLineF(rect.left(), y, rect.right(),y))


            painter.setPen(QPen(Qt.lightGray))
            painter.drawLines(lines)

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

    def replaceEdge(self, edge):
        pass

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


class EdgeItem(QGraphicsItem):
    def __init__(self, start, end, path, edge):
        QGraphicsItem.__init__(self, None, None)
        self.start = start
        self.end   = end
        self.path  = path
        self.edge = edge
        self.arrowHead = QPolygonF()
        self.onlyUpMiddle = True
        
        intersectionPoint = self.startPoint()
        path[0] = intersectionPoint

        intersectionPoint = self.endPoint()
        path[-1] = intersectionPoint      

        self.rect = self.findBoundingRect()
        self.setZValue(-10)

    def replaceEdge(self, path, edge):
        self.path = path
        self.edge = edge

        intersectionPoint = self.startPoint()
        path[0] = intersectionPoint

        intersectionPoint = self.endPoint()
        path[-1] = intersectionPoint

        self.rect = self.findBoundingRect()
        
    def startPoint(self):
        return self.start.intersectionPoint(self.path[1])


    def endPoint(self):
        return self.end.intersectionPoint(self.path[-2])
    
    def updatePosition(self):
        intersectionPoint = self.startPoint()
        self.path[0] = intersectionPoint

        intersectionPoint = self.endPoint()
        self.path[-1] = intersectionPoint

        self.update()
        

        
    def paint(self, painter, option, widget):
        thickPen = QPen()
        #thickPen.setWidth(10*self.edge.graphics.width / self.edge.minWidth)
        #thickPen.setColor(QColor(self.edge.graphics.fill))
        newRange = 225
        if self.edge.maxWidth == self.edge.minWidth:
            newRatio = 255
        else:
            newRatio = ((self.edge.graphics.width - self.edge.minWidth)/(self.edge.maxWidth - self.edge.minWidth)) * newRange + 30
        thickPen.setWidth(newRatio/22.5)
        #thickPen.setColor(QColor(0,0,0, newRatio))

        painter.setPen(thickPen)
        for i in range(len(self.path) - 1):
            s = self.path[i]
            e = self.path[i+1]
            try:
                lastLine = line
            except:
                pass
            line = QLineF(s.x(), s.y(), e.x(), e.y())
            painter.drawLine(line)

        noArrow = False
        
        if self.onlyUpMiddle:
            if line.p2().y() < line.p1().y():
                # If the line is going upwards, then draw the arrows.

                if len(self.path) > 3:
                    # If more than 3 segments, better use 2nd from the end.
                    arrowBase = lastLine
                else:
                    # If less than 3 segments, use the last segment.
                    arrowBase = line

                centerPoint = (arrowBase.p1() + arrowBase.p2()) / 2.0

                line = QLineF(arrowBase.p1(), centerPoint)

            else:
                # Going down, so no arrow.
                noArrow = True

        # Now arrow
        if not noArrow:
            arrowSize = 20.0
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

        # Arrow drawing has finished.
        
        if self.isSelected():
            painter.setPen(QPen(myColor, 1, Qt.DashLine))
            myLine = QLineF(line)
            myLine.translate(0, 4.0)
            painter.drawLine(myLine)
            myLine.translate(0,-8.0)
            painter.drawLine(myLine)


    def findBoundingRect(self):
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
        return self.rect
    
class NodeItem(QGraphicsItem):
    def __init__(self, w, parent=None, scene=None):
        QGraphicsItem.__init__(self, parent, scene)
        self.w = 50 # TODO: REMOVE THIS
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        self.arrows = []

    def centerPos(self):
        """Returns the center coordinate of the item."""
        #return QPointF(self.x() + self.w/2, self.y() + self.w/2)
        return QPointF(self.x() + self.w/2, self.y() + self.h/2)

    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemPositionChange:
            for arrow in self.arrows:
                arrow.updatePosition()

            self.setToolTip("(%.2f,%.2f)" % (self.centerPos().x(), self.centerPos().y()) )

        return QVariant(value)

    """def mouseDoubleClickEvent(self, event):
        #print self.x(), self.y()
        print self.centerPos()"""








