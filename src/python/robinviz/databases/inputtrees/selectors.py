# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
#from config.biclustering import biclusteringAlgorithms
#from config.biclustering import *
from config.customwidgets import *

import sys
import os
from utils.info import rp
import yaml

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

class BiclusteringSelector(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.loadSettings()
        self.parameterWidgets = {}
        self.setupGUI()

    def loadSettings(self):
        stream = file(rp('settings.yaml'), 'r')    # 'document.yaml' contains a single YAML document.
        self.params = yaml.load(stream)
        #print self.params

    def saveSettings(self):

        # Update parameter dictionary from the widgets.
        for parameter in self.parameterWidgets.keys():
            value = self.parameterWidgets[parameter].value()
            if type(value) == bool:
                value = int(value)
            self.params["Confirmation"]["CoExpression"]["Biclustering"][parameter] = value
        # Add algorithm flags to the parameter dict.
        for algorithm in self.algorithms:
            if algorithm.name == self.comboAlgorithm.currentText():
                #algorithm.flagValue = True
                self.params["Confirmation"]["CoExpression"]["Algorithms"][algorithm.flagName] = 1
            else:
                #algorithm.flagValue = False
                self.params["Confirmation"]["CoExpression"]["Algorithms"][algorithm.flagName] = 0

        stream = file(rp('settings.yaml'), 'w')
        yaml.dump(self.params, stream, default_flow_style=False, indent=4)    # Write a YAML representation of data to 'document.yaml'.
        stream.close()
        
    def __getParameterWidget(self, parameter):
        """Returns label - value widget pair for the given parameter object."""
        labelWidget = QLabel(parameter.description + ":")
        if parameter.type == "int":
            valueWidget = QSpinBox()
            valueWidget.setRange(parameter.min, parameter.max)
            valueWidget.setSingleStep(parameter.step)
        elif parameter.type == "float":
            valueWidget = QDoubleSpinBox()
            valueWidget.setRange(parameter.min, parameter.max)
            valueWidget.setSingleStep(parameter.step)
            if parameter.name == "ther_disc":
                valueWidget.setDecimals(4)
        elif parameter.type == "file":
            valueWidget = QHBoxLayout()
            lineEdit = QLineEdit()
            lineEdit.value = lineEdit.text # to be able to use value method.
            browseButton = QToolBoxButton()
            browseButton.setText("..")
            valueWidget.addWidget(lineEdit)
            valueWidget.addWidget(browseButton)
        elif parameter.type == "str":
            valueWidget = QLineEdit()
            valueWidget.value = valueWidget.text

        else:
            print "Unknown parameter type"

        return (labelWidget, valueWidget)

    def setupGUI(self):
    
        self.bicLayout = QGridLayout(self)

        algorithmSelectionLayout = QHBoxLayout()
        self.labelUseAlgorithm = QLabel("    Use:")
        self.comboAlgorithm = QComboBox(self)

        algorithmSelectionLayout.addWidget(self.labelUseAlgorithm)
        algorithmSelectionLayout.addWidget(self.comboAlgorithm)
        self.toolboxAP = QToolBox(self)

        ####### ALGORITHM SELECTION AND OPTIONS ########
        row = self.bicLayout.rowCount()
        self.bicLayout.addLayout(algorithmSelectionLayout, row, 0, 1, 2)
        self.bicLayout.addWidget(self.toolboxAP, row+1, 0, 1, 2)

        self.algorithms = []


        ######## ALGORITHM TOOLBOX PART ############
        # Create algorithm objects.

        for filename in filter(lambda x:x.endswith(".txt"), os.listdir(rp("src/python/robinviz/config/biclustering"))):
            self.algorithms.append(BiclusteringAlgorithm(rp("src/python/robinviz/config/biclustering/"+filename)))

        # Add Algorithm Widgets
        for algorithm in self.algorithms:
            # Create a page for this algorithm
            pageWidget = QWidget()
            self.toolboxAP.addItem(pageWidget, algorithm.name)

            # Now place the widgets into this page
            pageLayout = QGridLayout(pageWidget)

            # Now construct parameter widgets.
            row = 0
            for parameter in algorithm.parameters:

                (labelWidget, valueWidget) = self.__getParameterWidget(parameter)

                # Save for further reference
                self.parameterWidgets[parameter.name] = valueWidget

                # Add the widgets to the layout
                pageLayout.addWidget(labelWidget, row, 0)
                if isinstance(valueWidget, QWidget):
                    pageLayout.addWidget(valueWidget, row, 1)
                elif isinstance(valueWidget, QLayout):
                    pageLayout.addLayout(valueWidget, row, 1)
                row += 1

            self.comboAlgorithm.addItem(algorithm.name)
            if self.params["Confirmation"]["CoExpression"]["Algorithms"][algorithm.flagName]:
                self.comboAlgorithm.setCurrentIndex(self.comboAlgorithm.count() - 1)
                useCachedResults = False

def main():
    app = QApplication(sys.argv)
    mainWindow = BiclusteringSelector()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()