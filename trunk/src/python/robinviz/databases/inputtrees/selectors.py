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
            return "Co-Functionality"

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

def main():
    app = QApplication(sys.argv)
    mainWindow = ColorSelector()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()