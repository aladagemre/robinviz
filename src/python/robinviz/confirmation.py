from PyQt4.QtCore import *
from PyQt4.QtGui import *
from extensions import MainView, PeripheralView
from PyQt4 import QtWebKit
from os.path import normcase
import os
from bicluster import *

class CoRegulationMainView(MainView):
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
