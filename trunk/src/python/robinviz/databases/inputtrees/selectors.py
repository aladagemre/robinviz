# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os


class ConfirmationSelector(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()

    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.coexpression = QRadioButton("Co-Expression: Use biclustering to analyse PPI Network")
        self.coontology = QRadioButton("Co-Ontology: Use Gene Ontology to analyse PPI Network")

        self.coontology.setChecked(True)

        self.layout.addWidget(self.coexpression)
        self.layout.addWidget(self.coontology)

    def getSelection(self):
        if self.coexpression.isChecked():
            return "Co-Expression"
        else:
            return "Co-Ontology"

class ColorSelector(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()

    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)
        self.heading = QLabel("Use one of the following option to color the nodes")
        self.cofunctionality = QRadioButton("Co-Functionality: Molecular Function categories")
        self.colocalization= QRadioButton("Co-Localization: Cellular Compartment categories")
        self.coprocess = QRadioButton("Co-Process: Biological Process categories")
        self.coontology = QRadioButton("Co-Ontology: All categories")

        self.cofunctionality.setChecked(True)

        self.layout.addWidget(self.heading)
        self.layout.addWidget(self.cofunctionality)
        self.layout.addWidget(self.colocalization)
        self.layout.addWidget(self.coprocess)
        self.layout.addWidget(self.coontology)

    def getSelection(self):
        result = {
        self.cofunctionality : "-f",
        self.colocalization : "-l",
        self.coprocess : "-p",
        self.coontology: "-o",
        }
        for key in result.keys():
            if key.isChecked():
                return result[key]

class EdgeWeightSelector(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()
        
    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.heading = QLabel("Edge weights in Main Graph will be based on")
        self.interactions = QRadioButton("Interactions from PPI Network")
        self.common = QRadioButton("Common Genes in Biclusters")

        self.interactions.setChecked(True)

        # ======= EDGE REMOVAL RATIO ========
        self.label_removal = QLabel("Edge weight removal ratio:")
        self.removal_ratio = QDoubleSpinBox()
        self.removal_ratio.setRange(0.01, 0.99)
        self.removal_ratio.setDecimals(2)
        self.removal_ratio.setSingleStep(0.01)
        self.removal_ratio.setValue(0.15)


        self.ratio_layout = QHBoxLayout()
        self.ratio_layout.addWidget(self.label_removal, 3)
        self.ratio_layout.addWidget(self.removal_ratio, 3)

        # =====================================

        self.layout.addWidget(self.heading, 0)
        self.layout.addWidget(self.interactions, 1)
        self.layout.addWidget(self.common, 2)
        self.layout.addLayout(self.ratio_layout, 3)

        

    def getSelection(self):
        if self.interactions.isChecked():
            return "interactions", self.removal_ratio.value()
        else:
            return "common", self.removal_ratio.value()

class NodeWeightSelector(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()

    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.heading = QLabel("Node weights in Main Graph will be based on")
        self.hvalue = QRadioButton("H-Value")
        self.enrichment = QRadioButton("Functional Enrichment")
        self.hitratio = QRadioButton("PPI Hit Ratio")

        self.hvalue.setChecked(True)

        self.layout.addWidget(self.heading, 0)
        self.layout.addWidget(self.hvalue, 1)
        self.layout.addWidget(self.enrichment, 2)
        self.layout.addWidget(self.hitratio, 3)

    def getSelection(self):
        response = {
            self.hvalue: "hvalue",
            self.enrichment : "enrichment",
            self.hitratio : "hitratio",
        }
        
        for option in response.keys():
            if option.isChecked():
                return response[option]

def main():
    app = QApplication(sys.argv)
    mainWindow = EdgeWeightSelector()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()