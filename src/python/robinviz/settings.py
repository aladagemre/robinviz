from PyQt4.QtCore import *
from PyQt4.QtGui import *
import sys
from config.biclustering import *
from config.customwidgets import *
from os.path import normcase, abspath


class SettingsDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.setWindowTitle("RobinViz - Settings")
        #self.setWindowFlags(Qt.Window|Qt.FramelessWindowHint)
        self.layout = QVBoxLayout(self)
        self.tabWidget = QTabWidget()

        self.layout.addWidget(self.tabWidget)
        self.layout.addStretch()
        self.buttonBox = QDialogButtonBox()
        self.buttonBox.setStandardButtons(QDialogButtonBox.Ok|QDialogButtonBox.Cancel)
        self.layout.addWidget(self.buttonBox)
        self.setLayout(self.layout)

        self.loadSettings()
        self.loadYAMLSettings()
        self.saveYAMLSettings()
        
        self.setupBiologicalTab()
        self.setupBiclusteringTab()
        self.setupGraphTab()
        self.setupDrawingTab()
        

        self.setValues()
        
        self.connect(self.buttonBox, SIGNAL("rejected()"), self.close)
        self.connect(self.buttonBox, SIGNAL("accepted()"), self.save)
        #self.comboAlgorithm.currentIndexChanged.connect(self.loadAlgorithmParameters)

        #self.connect(self.comboAlgorithm, SIGNAL("currentIndexChanged(QString)"), self.loadAlgorithmParameters)

        self.setMinimumSize(455,300)

    def loadYAMLSettings(self):
        import yaml
        stream = file('settings.yaml', 'r')    # 'document.yaml' contains a single YAML document.
        self.params = yaml.load(stream)
        #print self.params

    def saveYAMLSettings(self):
        import yaml
        stream = file('settings-out.yaml', 'w')
        yaml.dump(self.params, stream, default_flow_style=False, indent=4)    # Write a YAML representation of data to 'document.yaml'.
        stream.close()

    def loadSettings(self):
        self.parameters = {}
        self.parameterWidgets = {}
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
            #print parameterName, parameterValue

    def setValues(self):
        typeConversion = {QSpinBox       : int,
                          CustomRadio    : int,
                          CustomCheckBox : int,
                          QDoubleSpinBox : float,
                          QLineEdit      : str,
                          FileBrowser    : abspath,}
                          
        for parameter, widget in self.parameterWidgets.items():
            convert = typeConversion[type(widget)]
            widget.setValue( convert( self.parameters[parameter]  ) )
            
        
    def save(self):
        """Saves the settings to settings.ini."""
        # TODO: Check form values, file existance, bicluster limits, etc.
        
        # Update parameter dictionary from the widgets.
        for parameter in self.parameterWidgets.keys():
            value = self.parameterWidgets[parameter].value()
            if type(value) == bool:
                value = int(value)
            self.parameters[parameter] = value
        # Add algorithm flags to the parameter dict.
        for algorithm in self.algorithms:
            if algorithm.name == self.comboAlgorithm.currentText():
                #algorithm.flagValue = True
                self.parameters[algorithm.flagName] = 1
            else:
                #algorithm.flagValue = False
                self.parameters[algorithm.flagName] = 0

        
        # todo: make this automatic

        text = """minBicSize %(minBicSize)d
maxBicSize %(maxBicSize)d
width %(width)d
space %(space)d
increment %(increment)d
removeRat %(removeRat)0.2f
nodeSize 50.0
edgeBendImp 35.0
colorScale 130.0
edgThicknessTher 4.0
dataName %(dataName)s
dataName2 %(dataName2)s
ppifilename %(ppifilename)s
catfile %(catfile)s
biclustering 1
bimax_low_dim1 %(bimax_low_dim1)d
bimax_low_dim2 %(bimax_low_dim2)d
ther_disc %(ther_disc)f
bic_num_bimax %(bic_num_bimax)d
bic_num_cc %(bic_num_cc)d
maxScore_ %(maxScore_)d
minHeight_ %(minHeight_)d
minWidth_ %(minWidth_)d
maxSizeSubMatrix_exp1_g %(maxSizeSubMatrix_exp1_g)d
maxSizeSubMatrix_exp1_c %(maxSizeSubMatrix_exp1_c)d
minSizeSubMatrix_exp1_g %(minSizeSubMatrix_exp1_g)d
minSizeSubMatrix_exp1_c %(minSizeSubMatrix_exp1_c)d
repeat %(repeat)d
hvaluemin %(hvaluemin).2f
increment_exp1_g %(increment_exp1_g)d
increment_exp1_c %(increment_exp1_c)d
ownFlag 0
lebFlag 1
ccFlag %(ccFlag)d
msbeFlag 0
bimaxFlag %(bimaxFlag)d
rlebFlag %(rlebFlag)d
brandFlag 1
brandFlag2 0
ourMethodFlag 0
ledaPostFlag 0
readOption %(readOption)d
ffd_layout 0
layered_layout 1
go_info %(go_info)d
gofile %(gofile)s
edgesBetween %(edgesBetween)d
sharedGenes %(sharedGenes)d
hvalueWeighting %(hvalueWeighting)d
enrichmentWeighting_o %(enrichmentWeighting_o)d
enrichmentWeighting_f %(enrichmentWeighting_f)d
ppihitratioWeighting %(ppihitratioWeighting)d""" %  self.parameters
        #print self.parameters
        #print text
        f = open("settings.ini", "w")
        f.write(text)
        f.close()
        #dataName2 = "sources/usr_sources/microarray_data/inputLabel.txt"

        self.close()
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
    
    def setupBiclusteringTab(self):
        self.biclusteringTab = QWidget()
        self.tabWidget.addTab(self.biclusteringTab, "Biclustering")
        self.bicLayout = QGridLayout(self.biclusteringTab)

        algorithmSelectionLayout = QHBoxLayout()
        self.labelUseAlgorithm = QLabel("    Use:")
        self.comboAlgorithm = QComboBox(self.biclusteringTab)                

        algorithmSelectionLayout.addWidget(self.labelUseAlgorithm)
        algorithmSelectionLayout.addWidget(self.comboAlgorithm)
        self.toolboxAP = QToolBox(self.biclusteringTab)
        
        ####### ALGORITHM SELECTION AND OPTIONS ########
        row = self.bicLayout.rowCount()
        self.bicLayout.addLayout(algorithmSelectionLayout, row, 0, 1, 2)
        self.bicLayout.addWidget(self.toolboxAP, row+1, 0, 1, 2)
        
        self.algorithms = []
        
      
        ######## ALGORITHM TOOLBOX PART ############
        # Create algorithm objects.
        for filename in biclusteringAlgorithms:
            self.algorithms.append(BiclusteringAlgorithm(normcase("src/python/robinviz/config/biclustering/"+filename)))

        # Add Algorithm Widgets
        useCachedResults = True
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
            if self.parameters[algorithm.flagName]:
                self.comboAlgorithm.setCurrentIndex(self.comboAlgorithm.count() - 1)
                useCachedResults = False                    

        self.comboAlgorithm.addItem("Cached Results") # As the last option.
        if useCachedResults:
            self.comboAlgorithm.setCurrentIndex(self.comboAlgorithm.count() - 1)
              
        

    def browseInputFile(self):
        fileName = QFileDialog.getOpenFileName(self, "Open File",
                                                 "/home", "Microarray Data (*.txt)");
        self.inputUrlBox.setText(fileName)
    def setupDrawingTab(self):
        self.drawingTab = QWidget()
        self.tabWidget.addTab(self.drawingTab, "Drawing")
        self.drawingLayout = QGridLayout(self.drawingTab)

        self.labelMaximumWidth = QLabel("Maximum Width:")
        self.spinMaximumWidth = QSpinBox()
        self.spinMaximumWidth.setRange(1,100)
        
        self.labelRemovalRatio= QLabel("Edge Weight Removal Ratio:")
        self.labelRemovalRatio.setWordWrap(True)
        self.spinRemovalRatio = QDoubleSpinBox()
        self.spinRemovalRatio.setRange(0, 1)
        self.spinRemovalRatio.setSingleStep(0.01)
        
        self.labelMinimumSeparationX = QLabel("X Separation:")
        self.spinMinimumSeparationX = QSpinBox()
        self.spinMinimumSeparationX.setRange(1,500)
        
        self.labelMinimumSeparationY = QLabel("Y Separation:")
        self.spinMinimumSeparationY = QSpinBox()
        self.spinMinimumSeparationY.setRange(1,500)
        
        self.drawingLayout.addWidget(self.labelMaximumWidth, 0, 0)
        self.drawingLayout.addWidget(self.spinMaximumWidth, 0, 1)
        self.parameterWidgets["width"] = self.spinMaximumWidth
        
        self.drawingLayout.addWidget(self.labelRemovalRatio, 2, 0)
        self.drawingLayout.addWidget(self.spinRemovalRatio, 2, 1)
        self.parameterWidgets["removeRat"] = self.spinRemovalRatio

        self.drawingLayout.addWidget(self.labelMinimumSeparationX, 3, 0)
        self.drawingLayout.addWidget(self.spinMinimumSeparationX, 3, 1)
        self.parameterWidgets["space"] = self.spinMinimumSeparationX
        
        self.drawingLayout.addWidget(self.labelMinimumSeparationY, 4, 0)
        self.drawingLayout.addWidget(self.spinMinimumSeparationY, 4, 1)
        self.parameterWidgets["increment"] = self.spinMinimumSeparationY


    def setupGraphTab(self):
        self.graphTab = QWidget()
        self.tabWidget.addTab(self.graphTab, "Graph")
        self.graphLayout = QGridLayout(self.graphTab)
        basePath = "src/python/robinviz/config/graph/"
        options = ("edge_weights.txt", "node_weights.txt")
        
        
        for option in options:
            groupBox = RadioGroup(basePath + option)
            self.graphLayout.addWidget(groupBox.widget)

            for flagName, widget in groupBox.parameterDict.items():
                self.parameterWidgets[flagName] = widget
                widget.setChecked(self.parameters[flagName] == 1)

    def setupBiologicalTab(self):
        self.biologicalTab = QWidget()
        self.tabWidget.addTab(self.biologicalTab, "Biological")
        self.biologicalLayout = QGridLayout(self.biologicalTab)

        ########## ORGANISM CHOICE ##############

        self.organismSelector = OrganismSelector(self.parameters)

        # Match parameters with widgets
        self.parameterWidgets["dataName"] = self.organismSelector.browseExpressionMatrixInput
        self.parameterWidgets["dataName2"] = self.organismSelector.browseExpressionMatrixInputLabel
        self.parameterWidgets["readOption"] = self.organismSelector.radioExpressionMatrixInputLabel
        self.parameterWidgets["go_info"] = self.organismSelector.checkGo
        self.parameterWidgets["gofile"] = self.organismSelector.browseGoInput
        self.parameterWidgets["ppifilename"] = self.organismSelector.browsePPIInput
        self.parameterWidgets["catfile"] = self.organismSelector.browseCategoryFile

        # ADD TO LAYOUT
        self.biologicalLayout.addWidget(self.organismSelector, 0, 0, 1, 2)

def main():
    app = QApplication(sys.argv)
    settings = SettingsDialog()
    settings.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    #import cProfile
    #cProfile.run('main()')
    main()


