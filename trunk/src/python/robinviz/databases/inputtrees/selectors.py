# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
#from config.biclustering import biclusteringAlgorithms
#from config.biclustering import *
from config.customwidgets import *

import sys
import os
from utils.info import rp,ap
from utils.settingswrite import write_values
from utils.compression import ungz
from utils.downloader import Downloader
import yaml

class ConfirmationSelector(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()

    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.coexpression = QRadioButton("Co-&Expression: Use biclustering to analyse PPI Network")
        self.coontology = QRadioButton("Co-&Ontology: Use Gene Ontology to analyse PPI Network")

        if open(rp("outputs/resultparams.txt")).readline()[3]=="e":
            self.coexpression.setChecked(True)
        else:
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
        self.heading = QLabel("Assign node colors according to high-level categories in:")
        self.cofunctionality = QRadioButton("&Molecular Function")
        self.colocalization= QRadioButton("&Cellular Compartments")
        self.coprocess = QRadioButton("Biological &Process")
        self.coontology = QRadioButton("&All")

        self.cofunctionality.setChecked(True)

        self.layout.addWidget(self.heading)
        self.layout.addWidget(self.coprocess)
        self.layout.addWidget(self.cofunctionality)
        self.layout.addWidget(self.colocalization)        
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
    def __init__(self, confirmation, parent=None):
        QWidget.__init__(self, parent)
        self.confirmation = confirmation

    def initialize(self):
        self.loadSettings()
        self.setupGUI()
        
    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.heading = QLabel("Edge weights in Central View will be based on")
        self.interactions = QRadioButton("&Interactions from PPI Network")
        self.common = QRadioButton("&Common Genes in Biclusters")

        # Check the radio buttons
        if self.params["Algorithms"]["edgesBetween"] == 1:
            self.interactions.setChecked(True)
        elif self.params["Algorithms"]["sharedGenes"] == 1:
            self.common.setChecked(True)
        else:
            print "edgesBetween:", self.params["Algorithms"]["edgesBetween"]
            print "sharedGenes:", self.params["Algorithms"]["sharedGenes"]

        # ======= EDGE REMOVAL RATIO ========
        self.label_removal = QLabel("Edge weight removal ratio:")
        self.removal_ratio = QDoubleSpinBox()
        self.removal_ratio.setRange(0.01, 0.99)
        self.removal_ratio.setDecimals(2)
        self.removal_ratio.setSingleStep(0.01)
        self.removal_ratio.setValue(float(self.params["Drawing"]["removeRat"]))


        self.ratio_layout = QHBoxLayout()
        self.ratio_layout.addWidget(self.label_removal, 3)
        self.ratio_layout.addWidget(self.removal_ratio, 3)

        # =====================================

        self.layout.addWidget(self.heading, 0)
        self.layout.addWidget(self.interactions, 1)
        self.layout.addWidget(self.common, 2)
        self.layout.addLayout(self.ratio_layout, 3)

    def loadSettings(self):
        stream = file(rp('settings.yaml'), 'r')
        self.complete_params = yaml.load(stream)
        self.params = self.complete_params["Confirmation"][self.confirmation]

    def saveSettings(self):
        self.params["Drawing"]["removeRat"] = float(self.removal_ratio.value())
        if self.interactions.isChecked():
            self.params["Algorithms"]["edgesBetween"] = 1
            self.params["Algorithms"]["sharedGenes"] = 0
        else:
            self.params["Algorithms"]["edgesBetween"] = 0
            self.params["Algorithms"]["sharedGenes"] = 1

        self.complete_params["Confirmation"][self.confirmation] = self.params
        f = open(rp('settings.yaml'), "w")
        f.write(write_values(self.complete_params))
        f.close()

    def getSelection(self):
        if self.interactions.isChecked():
            return "interactions", self.removal_ratio.value()
        else:
            return "common", self.removal_ratio.value()

class NodeWeightSelector(QWidget):
    def __init__(self, confirmation, parent=None):
        QWidget.__init__(self, parent)
        self.confirmation = confirmation
        
    def initialize(self):
        self.loadSettings()
        self.setupGUI()
        
    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        # ====== PREPARE ========
        self.hvalue = QRadioButton("&H-Value")
        self.enrichment = QRadioButton("&Functional Enrichment")
        self.hitratio = QRadioButton("&PPI Hit Ratio")

        # ======= CHECK ==========
        if self.params["Algorithms"]["hvalueWeighting"] == 1:
            self.hvalue.setChecked(True)
        elif self.params["Algorithms"]["enrichmentWeighting_f"] == 1:
            self.enrichment.setChecked(True)
        elif self.params["Algorithms"]["ppihitratioWeighting"] == 1:
            self.hitratio.setChecked(True)


        # ======= EDGE REMOVAL RATIO ========
        self.label_removal = QLabel("Ratio of hidden central edges:")
        self.removal_ratio = QDoubleSpinBox()
        self.removal_ratio.setRange(0.01, 0.99)
        self.removal_ratio.setDecimals(2)
        self.removal_ratio.setSingleStep(0.01)
        self.removal_ratio.setValue(float(self.params["Drawing"]["removeRat"]))


        self.ratio_layout = QHBoxLayout()
        self.ratio_layout.addWidget(self.label_removal, 3)
        self.ratio_layout.addWidget(self.removal_ratio, 3)


        # ===== ADD TO LAYOUT ====
        
        self.layout.addWidget(self.hvalue, 0)
        self.layout.addWidget(self.enrichment, 1)
        self.layout.addWidget(self.hitratio, 2)
        self.layout.addLayout(self.ratio_layout, 3)

    def getSelection(self):
        response = {
            self.hvalue: "hvalueWeighting",
            self.enrichment : "enrichmentWeighting_f",
            self.hitratio : "ppihitratioWeighting",
        }
        
        for option in response.keys():
            if option.isChecked():
                checked = response[option]
                uncheckeds = set(response.values()) - set([checked])
                return checked, uncheckeds


    def loadSettings(self):
        stream = file(rp('settings.yaml'), 'r')
        self.complete_params = yaml.load(stream)
        self.params = self.complete_params["Confirmation"][self.confirmation]
        
    def saveSettings(self):
        # ==GET CHECKED INFORMATION===
        checked, uncheckeds = self.getSelection()

        # ====== SET VALUES ========
        # for checked 1
        self.params["Algorithms"][checked] = 1
        # for uncheckeds 0
        for unchecked in uncheckeds:
            self.params["Algorithms"][unchecked] = 0

        self.params["Drawing"]["removeRat"] = float(self.removal_ratio.value())
        # ===== PLACE PARAMETERS ====
        # We put partial dict back to the complete dict.
        self.complete_params["Confirmation"][self.confirmation] = self.params

        # ==== WRITE PARAMETERS =====
        f = open(rp('settings.yaml'), "w")
        f.write(write_values(self.complete_params))
        f.close()

class BiclusteringSelector(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.parameterWidgets = {}

    def initialize(self):
        self.downloadCheckedGEO()
        self.loadSettings()
        self.setupGUI()
        self.setValues()
        
    def ungz(self):
        ungz(self.local_gz)
        filename = open(ap("geodata/selected_geo.txt")).readline().strip()
        self.gene, self.cond=open(ap("geodata/%s" % filename) ).readline().strip().split()
        self.labelInfo.setText("This matrix contains %s genes and %s conditions" % (self.gene, self.cond))
        
    def downloadCheckedGEO(self):
        filename = open(ap("geodata/selected_geo.txt")).readline().strip()
	base_url = "http://robinviz.googlecode.com/svn/data2/expressions/gse"
        local_path = ap("geodata/%s" % filename)
        if not os.path.exists( local_path ):
            remote_gz = "%s/%s.gz" % (base_url, filename)
            self.local_gz = "%s.gz" % local_path
            #download_file_to(remote_gz, local_gz, binary=True)
            self.d = Downloader(remote_gz, self.local_gz)
            self.d.finished.connect(self.ungz)
            qApp.processEvents()
            self.d.show()

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

        """stream = file(rp('settings.yaml'), 'w')
        yaml.dump(self.params, stream, default_flow_style=False, indent=4)    # Write a YAML representation of data to 'document.yaml'.
        stream.close()"""
        f = open(rp('settings.yaml'), "w")
        f.write(write_values(self.params))
        f.close()
    
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

    def setValues(self):
        typeConversion = {QSpinBox       : int,
                          CustomRadio    : int,
                          CustomCheckBox : int,
                          QDoubleSpinBox : float,
                          QLineEdit      : str,
                          FileBrowser    : abspath,}

        for parameter, widget in self.parameterWidgets.items():
            convert = typeConversion[type(widget)]
            widget.setValue( convert( self.params["Confirmation"]["CoExpression"]["Biclustering"][parameter]  ) )

    def setupGUI(self):
    
        self.bicLayout = QGridLayout(self)


        algorithmSelectionLayout = QHBoxLayout()
        self.labelInfo = QLabel("")
        self.bicLayout.addWidget(self.labelInfo)


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