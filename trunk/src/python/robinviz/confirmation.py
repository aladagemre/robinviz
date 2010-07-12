from PyQt4.QtCore import *
from PyQt4.QtGui import *
from extensions import MainView, PeripheralView
from drawing import MainScene, PeripheralScene, CircleNode
from PyQt4 import QtWebKit
from os.path import normcase
import os
from bicluster import *


CATEGORY_COLORS = {}
colorFile = open("outputs/colors_func.txt")
lineNum = 0
for line in colorFile:
    name, r, g, b = line.strip().split()
    CATEGORY_COLORS[str(lineNum)] = name.replace("_", " ")
    lineNum+=1



class CoRegulationMainScene(MainScene):
    def __init__(self, parent=None):
        MainScene.__init__(self, parent)
        self.readSettings()

        # By default, Scene is NOT directed.
        # But main scene can be directed or undirected.
        if self.parameters["edgesBetween"]:
            self.directed = True
            self.onlyUp = True
        else:
            self.directed = False

    def loadGraph(self, filename):
        MainScene.loadGraph(self, filename)
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

class CoRegulationMainView(MainView):
    def __init__(self, parent=None):
        MainView.__init__(self, parent)
        self.mainSceneClass = CoRegulationMainScene

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
        self.enrichmentTable.showMaximized()

     # ========= Additional Methods =============
     # ========= ================== =============
class CoRegulationPeripheralView(PeripheralView):

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
        if self.params["Algorithms"]["edgesBetween"]:
            self.directed = True
            self.onlyUp = True
        else:
            self.directed = False

    def loadGraph(self, filename):
        MainScene.loadGraph(self, filename)
        #self.determineScoring()

    def addNode(self, node):
        item = CircleNode(node)
        self.addItem(item)
        self.nodeDict[node] = item
        self.nodeDict[node.id] = item

    def loadYAMLSettings(self):
        import yaml
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

        enrichmentTable = menu.addAction("Enrichment Table")
        actionToFunction[enrichmentTable] = self.showEnrichmentTable


    # ========= Event Handlers =============

    def showEnrichmentTable(self):
        self.enrichmentTable= QtWebKit.QWebView()
        self.enrichmentTable.setUrl(QUrl(normcase("outputs/enrich/result.html")))
        self.enrichmentTable.showMaximized()

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
