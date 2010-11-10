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
        
        self.layout.addWidget(self.coexpression)
        self.layout.addWidget(self.coontology)

    def getSelection(self):
        if self.coexpression.isChecked():
            return "Co-Expression"
        else:
            return "Co-Functionality"

def main():
    app = QApplication(sys.argv)
    mainWindow = ConfirmationSelector()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()