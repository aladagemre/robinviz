# -*- coding: utf-8 -*-
#from parser import GMLParser
#from styles import *
#import sys
from pygml import Graph

from PyQt4.QtCore import *
from PyQt4.QtGui import *

from os.path import split
from os.path import normcase

from utils.info import rp # root path function
import os

GRAPH_LAYOUTS = {}
layoutFile = open(rp("layouts.ini"))
for line in layoutFile:
    if line.startswith("//"):
        continue # this is a comment
    name, filename = line.strip().split(":")
    GRAPH_LAYOUTS[name] = filename



class View(QGraphicsView):
    def __init__(self, parent=None):
        QGraphicsView.__init__(self, parent)
        self.main = False
        self.mainSceneClass = None # TO BE DEFINED BY CHILD CLASSES
        self.printer = QPrinter(QPrinter.HighResolution)
        self.printer.setPageSize(QPrinter.A4)
        #self.printer.setFullPage(True)
        self.printer.setPageMargins(10, 10, 10, 10, QPrinter.Millimeter)
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
        if itemRightClicked and not isinstance(itemRightClicked , EdgeItem):
            QGraphicsView.contextMenuEvent(self, event)
            return

        self.menu = menu = QMenu(self)
        newWindow = menu.addAction("Open in &new window")

        menu.addSeparator()

        saveAsImage = menu.addAction("Save as &Image")
        saveAsGML= menu.addAction("Save as &GML")
        printGraph = menu.addAction("&Print")
        
        self._actionToFunction = actionToFunction = {newWindow       : self.newWindow,
                            saveAsImage     : self.saveAsImage,
                            printGraph      : self.printGraph,
                            saveAsGML       : self.saveAsGML,
                            }

        # ==== CUSTOM MENU ITEMS START ====
        self.addCustomMenuItems()

        """if getattr(self, 'main', False):
            enrichmentTable = menu.addAction("Enrichment Table")
            actionToFunction[enrichmentTable] = self.showEnrichmentTable
        else:
            clearAction = menu.addAction("Clear")
            actionToFunction[clearAction] = self.clearView
            
            menu.addSeparator()

            goTable = menu.addAction("GO Table")
            actionToFunction[goTable] = self.showGOTable

            propertiesAction = menu.addAction("Properties")
            actionToFunction[propertiesAction] = self.peripheralProperties"""

        # ==== CUSTOM MENU ITEMS END ====
        
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


            



    def addCustomMenuItems(self):
        """ABSTRACT METHOD: Needs to be implemented by each confirmation view type."""
        pass
    
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
            if isinstance(item, TinyNode):
                item.updateLabel()
            
        self.reselectItemsAfterLayoutChange()
        self.refresh()
        
    def refresh(self):
        if self.scene():
            self.scene().update()
            self.fitInView(self.scene().itemsBoundingRect(), Qt.KeepAspectRatio)
        
    def reselectItemsAfterLayoutChange(self):
        """Re-selects previously selected items after layout changed."""
        for item in self.scene().items():
            if isinstance(item, QGraphicsEllipseItem):
                item.setupAnimation() # Setup individual "selected" animation for new position.
                if getattr(item, 'lastSelected', False):
                    item.setSelected(True)
                    item.lastSelected = False

    def startLayoutAnimation(self):
        """Starts the layout animation"""
        self.timer.start()
    def stopLayoutAnimation(self):
        """Stops the layout animation."""
        if hasattr(self, 'timer'):
            self.timer.stop()

    def switchToLayout(self, layoutName):
        """Switches to the given layout with/without animation."""
        if self.main:
            selectedItems = self.scene().selectedItems()
            for selectedItem in selectedItems:
                selectedItem.stopAnimation()
                selectedItem.setSelected(False)
                selectedItem.lastSelected = True

        if self.setupLayoutSwitch(layoutName):
            self.startLayoutAnimation()          
    
    def setupLayoutSwitch(self, layoutName):
        """Setups how the layout switch shall be."""
        filename = self.scene().filename
        if not hasattr(self, 'originalFileName'):
            self.originalFileName = filename

	if layoutName == "Layered" and open("outputs/resultparams.txt").read().strip() == "Co-Regulation":
        #if layoutName == "Layered":
            self.scene().clear()
            # Switch back to original.
            self.scene().reloadGraph(self.originalFileName)
            self.scene().update()
            #self.reselectItemsLayeredLayout()
            return False
        else:
            # Find the exe
            exename = GRAPH_LAYOUTS[str(layoutName)]
            # Run the exe
            command = "%s %s" % (exename, self.originalFileName)
            if os.name == "posix":
                command = "./" + command
            os.system(normcase(command))

            if not self.useAnimation:
                self.scene().reloadGraph("%s%s.gml" % (self.originalFileName.split(".")[0], exename.split(".")[0]) )
                return False
            
            # Load the graph
            self.newGraph = newGraph = Graph()
            """if str(layoutName) == "Layered":
                newFileName = filename
            else:""" # this part is for animation to layered in the future.
            newFileName = "%s%s.gml" % (self.originalFileName.split(".")[0], exename.split(".")[0])
            self.newGraph.read_gml(newFileName)
            newGraph.prepare()
            self.scene().filename = newFileName
            # Clear the scene
            for item in self.scene().items():
                if isinstance(item, EdgeItem):
                    self.scene().removeItem(item)
                    del item
            self.scene().update()
            
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
        painter.setRenderHint(QPainter.Antialiasing)
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

from drawing import *
