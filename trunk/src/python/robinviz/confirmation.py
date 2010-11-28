# -*- coding: utf-8 -*-
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from extensions import MainView, PeripheralView
from drawing import MainScene, PeripheralScene, CircleNode
from PyQt4 import QtWebKit
from os.path import normcase
import os
from bicluster import *
import yaml
from utils.info import rp # root path function
CATEGORY_COLORS = {}
colorFile = open(rp("outputs/colors_func.txt"))
lineNum = 0
for line in colorFile:
    name, r, g, b = line.strip().split()
    CATEGORY_COLORS[str(lineNum)] = name.replace("_", " ")
    lineNum+=1



class CoExpressionMainScene(MainScene):
    def __init__(self, parent=None):
        MainScene.__init__(self, parent)
        self.loadYAMLSettings()

        # By default, Scene is NOT directed.
        """# But main scene can be directed or undirected.
        if self.params["Algorithms"]["edgesBetween"]:
            self.directed = True
            self.onlyUp = True
        else:
            self.directed = False"""
        self.directed = False
        
    def loadGraph(self, filename):
        MainScene.loadGraph(self, filename)
        self.determineScoring()

    def addNode(self, node):
        item = CircleNode(node, parent=None, scene=self)
        #self.addItem(item)
        self.nodeDict[node] = item
        self.nodeDict[node.id] = item
        
    def loadYAMLSettings(self):
        stream = file('settings.yaml', 'r')
        self.params = yaml.load(stream)["Confirmation"]["CoExpression"]
        for section in self.params:
            for key,value in self.params[section].iteritems():
                try:
                    value = int(value)
                except:
                    try:
                        value = float(value)
                    except:
                        pass
                self.params[section][key] = value

    def determineScoring(self):
        """Assigns scoring name and value to the tooltips of the nodes."""
        # Set scoring name

        if self.params["Algorithms"]["hvalueWeighting"]:
            self.scoringName = "H-Value"
        elif self.params["Algorithms"]["enrichmentWeighting_o"]:
            self.scoringName = "Enrichment Ratio"
        elif self.params["Algorithms"]["enrichmentWeighting_f"]:
            self.scoringName = "Enrichment Ratio"
        elif self.params["Algorithms"]["ppihitratioWeighting"]:
            self.scoringName = "PPI Hit Ratio"

        f = open("outputs/biclusters/scoring.txt")
        f.readline() # for the first line (scoring scheme : blabla)
        for line in f:
            ( biclusterstr, id, value ) = line.strip().split()
            item = self.nodeDict[int(id)]
            #tip = "%s: %s\nCategory: %s" % (self.scoringName, value, CATEGORY_COLORS[item.node.parameter])
            tip = "%s: %s" % (self.scoringName, value )
            item.setToolTip(tip)

class CoExpressionMainView(MainView):
    def __init__(self, parent=None):
        MainView.__init__(self, parent)
        self.mainSceneClass = CoExpressionMainScene

    # ========= Abstract Methods =============
    def addCustomMenuItems(self):
        menu = self.menu
        actionToFunction = self._actionToFunction
        
        enrichmentTable = menu.addAction("Enrichment Table")
        actionToFunction[enrichmentTable] = self.showEnrichmentTable


    # ========= Event Handlers =============

    def showEnrichmentTable(self):
        self.enrichmentTable= QtWebKit.QWebView()
        self.enrichmentTable.setUrl(QUrl(normcase("outputs/enrich/result.html")))
        self.enrichmentTable.setWindowTitle("Enrichment Table")
        self.enrichmentTable.showMaximized()

     # ========= Additional Methods =============
     # ========= ================== =============
class CoExpressionPeripheralView(PeripheralView):

    # ========= Abstract Methods =============
    def addCustomMenuItems(self):
        menu = self.menu
        actionToFunction = self._actionToFunction

        if self.preview:
            clearAction = menu.addAction("Clear")
            actionToFunction[clearAction] = self.clearView

        menu.addSeparator()

        goTable = menu.addAction("GO Table")
        actionToFunction[goTable] = self.showGOTable

        propertiesAction = menu.addAction("Properties")
        actionToFunction[propertiesAction] = self.peripheralProperties
        

    # ========= Event Handlers =============
    def clearView(self):
        self.setScene(None)
        self.specialWindow = None

    def showGOTable(self):
        path = normcase("outputs/go/gobicluster%d.html" % self.scene().id)
        if os.path.exists(path):
            self.GOTable= QtWebKit.QWebView()
            self.GOTable.setUrl(QUrl(path))
            self.GOTable.setWindowTitle("GO Table for Bicluster %d" % self.scene().id)
            self.GOTable.show()
        else:
            QMessageBox.information(self, 'GO Table not found.',
     "You need to run the program with the Gene Ontology File option in Biological Settings Tab checked and provide the GO file.")

    def peripheralProperties(self):
        if not hasattr(self, 'biclusterWindow'):
            self.biclusterWindow = BiclusterWindow(self.scene().id)

        self.biclusterWindow.showMaximized()

    # ========= Additional Methods =============
    # ========= ================== =============



# ============================================================================
# ================== CO-FUNCTIONALITY ========================================



class CoFunctionalityMainScene(MainScene):
    def __init__(self, parent=None):
        MainScene.__init__(self, parent)
        self.loadYAMLSettings()

        # By default, Scene is NOT directed.
        # But main scene can be directed or undirected.
        """if self.params["Algorithms"]["edgesBetween"]:
            self.directed = True
            self.onlyUp = True
        else:
            self.directed = False"""
        self.directed = False
	self.category_names = map( lambda name: name.strip(), open(self.params["Input"]["dataName_go"]).readlines() )
	
	
    def loadGraph(self, filename):
        MainScene.loadGraph(self, filename)
        #self.determineScoring()

    def addNode(self, node):
	try:
	    item = CircleNode(node, parent=None, scene=self, label=self.category_names[node.id])
	    item.setNoProperties()
	    #self.addItem(item)
	    self.nodeDict[node] = item
	    self.nodeDict[node.id] = item
	except IndexError:
	    print "Node id (%d) is out of bounds (%d)" % (node.id, len(self.category_names) )

    def loadYAMLSettings(self):
        stream = file('settings.yaml', 'r')    # 'document.yaml' contains a single YAML document.
        self.params = yaml.load(stream)["Confirmation"]["CoFunctionality"]
        for section in self.params:
            for key,value in self.params[section].iteritems():
                try:
                    value = int(value)
                except:
                    try:
                        value = float(value)
                    except:
                        pass
                self.params[section][key] = value


    def determineScoring(self):
        """Assigns scoring name and value to the tooltips of the nodes."""
        # TODO: REWRITE THIS!
        # Set scoring name

        """if self.parameters["hvalueWeighting"]:
            self.scoringName = "H-Value"
        elif self.parameters["enrichmentWeighting_o"]:
            self.scoringName = "Enrichment Ratio"
        elif self.parameters["enrichmentWeighting_f"]:
            self.scoringName = "Enrichment Ratio"
        elif self.parameters["ppihitratioWeighting"]:
            self.scoringName = "PPI Hit Ratio"
            """

        self.scoringName = "Score"
        f = open("outputs/biclusters/scoring.txt")
        f.readline() # for the first line (scoring scheme : blabla)
        for line in f:
            ( biclusterstr, id, value ) = line.strip().split()
            item = self.nodeDict[int(id)]
            #tip = "%s: %s\nCategory: %s" % (self.scoringName, value, CATEGORY_COLORS[item.node.parameter])
            tip = "%s: %s" % (self.scoringName, value )
            item.setToolTip(tip)

# TODO: Rewrite this!

class CoFunctionalityMainView(MainView):
    # TODO: Rewrite this!
    def __init__(self, parent=None):
        MainView.__init__(self, parent)
        self.mainSceneClass = CoFunctionalityMainScene

    # ========= Abstract Methods =============
    def addCustomMenuItems(self):
        menu = self.menu
        actionToFunction = self._actionToFunction

        #enrichmentTable = menu.addAction("Enrichment Table")
        #actionToFunction[enrichmentTable] = self.showEnrichmentTable


    # ========= Event Handlers =============

    """def showEnrichmentTable(self):
        self.enrichmentTable= QtWebKit.QWebView()
        self.enrichmentTable.setUrl(QUrl(normcase("outputs/enrich/result.html")))
        self.enrichmentTable.showMaximized()"""

     # ========= Additional Methods =============
     # ========= ================== =============
class CoFunctionalityPeripheralView(PeripheralView):
    # TODO: Rewrite this!

    # ========= Abstract Methods =============
    def addCustomMenuItems(self):
        menu = self.menu
        actionToFunction = self._actionToFunction

        if self.preview:
            clearAction = menu.addAction("Clear")
            actionToFunction[clearAction] = self.clearView

        menu.addSeparator()

        goTable = menu.addAction("GO Table")
        actionToFunction[goTable] = self.showGOTable

        #propertiesAction = menu.addAction("Properties")
        #actionToFunction[propertiesAction] = self.peripheralProperties


    # ========= Event Handlers =============
    def clearView(self):
        self.setScene(None)
        self.specialWindow = None

    def showGOTable(self):
        scene_id = self.scene().id
        path = normcase("outputs/go/gobicluster%d.html" % scene_id)
        category_name = ap("assocdata/input_go.txt").readlines()[scene_id].strip()
        
        if os.path.exists(path):
            self.GOTable= QtWebKit.QWebView()
            self.GOTable.setUrl(QUrl(path))
            self.GOTable.setWindowTitle("GO Table for %s" % category_name)
            self.GOTable.show()
        else:
            QMessageBox.information(self, 'GO Table not found.',
     "An error encountered and GO table file could not be located: %s" % path)

    """def peripheralProperties(self):
        if not hasattr(self, 'biclusterWindow'):
            self.biclusterWindow = BiclusterWindow(self.scene().id)

        self.biclusterWindow.showMaximized()"""

    # ========= Additional Methods =============
    # ========= ================== =============
