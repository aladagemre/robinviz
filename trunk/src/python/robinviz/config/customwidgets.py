from PyQt4.QtCore import *
from PyQt4.QtGui import *
from os.path import normcase, abspath, dirname, exists, join
import os
from string import digits

def correctPath(path):
        return abspath(normcase(path))
    

class Organism:
    """Holds directory and file information for an organism."""
    def __init__(self, name=None):
        self.setName(name)
        directory = correctPath("sources/organisms/%s" % name)
        if name and exists(directory):
            self.directory = directory
            self.setExpressionMatrix(join(directory, "expression_matrix.txt"))
            self.setPPI(join(directory, "ppi.txt"))
            self.setGO(join(directory, "go.txt"))
            self.setCategory(join(directory, "category.txt"))
    
    def setName(self, name):
        """Sets its name (directory name)."""
        self.name = name
    def setExpressionMatrix(self, path):
        """Sets expression matrix file. File must exist."""
        if not exists(path):
            QMessageBox.information(None, 'File does not exist', 'File %s does not exist. Please check it.' % path)
        else:
            self.expressionMatrix = path
    def setPPI(self, path):
        """Sets PPI file. File must exist."""
        if not exists(path):
            QMessageBox.information(None, 'File does not exist', 'File %s does not exist. Please check it.' % path)
        else:
            self.PPI = path
    def setGO(self, path):
        """As this file is optional, only a warning is issued if file does not exist."""
        if not exists(path):
            print "GO File %s does not exist" % path
        self.go = path
    def setCategory(self, path):
        """Sets category file. This must exist."""
        if not exists(path):
            QMessageBox.information(None, 'File does not exist', 'File %s does not exist. Please check it.' % path)
        else:
            self.category = path
        
    def title(self):
        """Adds space between the name and the dimensions and returns it."""
        for i in range(len(self.name)):
            letter = self.name[i]
            if letter in digits:
                return self.name[:i] + " " + self.name[i:]
            
class OrganismSelector(QGroupBox):
    """This is a Groupbox containing file browsers, radio buttons and checkboxes.
    Main purpose is to regulate the interactions (load, select, save organism choice)"""
    def __init__(self, parameters):
        QGroupBox.__init__(self)
        self.setTitle("Organism")
        self.layout = QGridLayout(self)
        self.parameters = parameters

        self.createOrganisms()
        self.fillGUI()
    def createOrganisms(self):
        # Create Organism instances
        self.organisms = []
        self.organismDict = {}

        self.organismRoot = "sources/organisms"
        self.organismsDirs = [f for f in os.listdir(self.organismRoot) if f[0]!="." and os.path.isdir(os.path.join(self.organismRoot, f))]
        for organismDir in self.organismsDirs:
            newOrganism = Organism(organismDir)
            self.organisms.append(newOrganism)
            self.organismDict[newOrganism.title()] = newOrganism
        # Now we have organisms.

        
    def fillGUI(self):
        self.labelOrganism = QLabel("Organism:")
        self.comboOrganism = QComboBox()
        self.connect(self.comboOrganism, SIGNAL("currentIndexChanged(QString)"), self.organismSelected)
        for organismTitle in sorted(self.organismDict.keys()):
            self.comboOrganism.addItem(organismTitle, self.organismDict[organismTitle])
        self.comboOrganism.addItem("Other")

        ######## GENE ONTOLOGY PART #############
        self.checkGo = CustomCheckBox("Use Gene Ontology")
        self.checkGo.setChecked(self.parameters["go_info"])
        self.browseGoInput = FileBrowser(abspath(dirname(self.parameters["gofile"])))
        self.browseGoInput.setTitle("Select Gene Ontology File")
        self.browseGoInput.setFilter("Gene Ontology File (*.txt)")

        ######## MICROARRAY (Expression Matrix) FILE PART ###############

        self.radioExpressionMatrixInput = CustomRadio("Expression Matrix w/o Labels:")
        self.browseExpressionMatrixInput = FileBrowser(abspath(dirname(self.parameters["dataName"])))
        self.browseExpressionMatrixInput.setTitle("Select Expression Matrix File (without label)")
        self.browseExpressionMatrixInput.setFilter("Expression Matrix File (*.txt)")

        self.radioExpressionMatrixInputLabel = CustomRadio("Expression Matrix w/ Labels:")
        self.browseExpressionMatrixInputLabel = FileBrowser(abspath(dirname(self.parameters["dataName2"])))
        self.browseExpressionMatrixInputLabel.setTitle("Select Expression Matrix File (with label)")
        self.browseExpressionMatrixInputLabel.setFilter("Expression Matrix File (*.txt)")
        # Select the radio button
        if self.parameters["readOption"]:
            self.radioExpressionMatrixInputLabel.setChecked(True)
        else:
            self.radioExpressionMatrixInput.setChecked(True)


        ####### PPI FILE PART ######################
        self.labelPPIFile = QLabel("PPI Network File:")
        self.browsePPIInput = FileBrowser(abspath(dirname(self.parameters["ppifilename"])))
        self.browsePPIInput.setTitle("Select PPI File")
        self.browsePPIInput.setFilter("PPI File (*.txt)")

        ###### CATEGORY FILE PART ##################
        self.labelCategoryFile = QLabel("Category File:")
        self.browseCategoryFile = FileBrowser(abspath(dirname(self.parameters["catfile"])))
        self.browseCategoryFile.setTitle("Select Category File")
        self.browseCategoryFile.setFilter("Category File (*.txt)")

        # ADD TO LAYOUT
        self.layout.addWidget(self.labelOrganism, 0, 0, Qt.AlignTop|Qt.AlignLeft)
        self.layout.addWidget(self.comboOrganism, 0, 1, Qt.AlignTop|Qt.AlignLeft)
        self.layout.addWidget(self.checkGo, 1, 0, Qt.AlignTop|Qt.AlignLeft)
        self.layout.addLayout(self.browseGoInput, 1, 1, Qt.AlignTop|Qt.AlignLeft)
        self.layout.addWidget(self.radioExpressionMatrixInput, 2, 0, Qt.AlignBottom|Qt.AlignLeft)
        self.layout.addLayout(self.browseExpressionMatrixInput, 2, 1, Qt.AlignBottom|Qt.AlignLeft)
        self.layout.addWidget(self.radioExpressionMatrixInputLabel, 3, 0,Qt.AlignBottom|Qt.AlignLeft)
        self.layout.addLayout(self.browseExpressionMatrixInputLabel, 3, 1,Qt.AlignBottom|Qt.AlignLeft)
        self.layout.addWidget(self.labelPPIFile, 4, 0, Qt.AlignBottom|Qt.AlignLeft)
        self.layout.addLayout(self.browsePPIInput, 4, 1, Qt.AlignBottom|Qt.AlignLeft)
        self.layout.addWidget(self.labelCategoryFile, 5, 0, Qt.AlignBottom|Qt.AlignLeft)
        self.layout.addLayout(self.browseCategoryFile, 5, 1, Qt.AlignBottom|Qt.AlignLeft)
        


        self.portableWidgets = (self.radioExpressionMatrixInput,
                                self.radioExpressionMatrixInputLabel,
                                self.browseExpressionMatrixInput,
                                self.browseExpressionMatrixInputLabel,
                                self.labelPPIFile,
                                self.browsePPIInput,
                                #self.checkGo,
                                self.browseGoInput,
                                self.labelCategoryFile,
                                self.browseCategoryFile,
                                )
                                
        for widget in self.portableWidgets:
            widget.hide()


            # Select the correct item
        if self.parameters["readOption"]: # If labeled EM selected,
            # Find the path in the parameters
            parameterEMPath = correctPath(self.parameters["dataName2"])
            parameterPPIPath = correctPath(self.parameters["ppifilename"])
            parameterGOPath = correctPath(self.parameters["gofile"])
            parameterCATPath = correctPath(self.parameters["catfile"])
            for org in self.organisms:
                # For each organism, look for EM filename.
                if parameterEMPath == org.expressionMatrix \
                and parameterPPIPath == org.PPI \
                and parameterGOPath == org.go \
                and parameterCATPath == org.category:
                    # If filenames matches, find its index and select it.
                    index = self.comboOrganism.findText(org.title())
                    self.comboOrganism.setCurrentIndex(index)
                    break
            else:
                # Select "Other" as no matching record found
                self.comboOrganism.setCurrentIndex(self.comboOrganism.count() - 1)
        else:
            # Select other because default organisms use labelled EM.
            self.comboOrganism.setCurrentIndex(self.comboOrganism.count() - 1)


    def organismSelected(self, itemText):
        # TODO: (Usability) When Others selected and some custom conf is made,
        # and then switched to a defined organism, all the conf is lost.
        
        if not hasattr(self, 'portableWidgets'):
            return
        itemText = str(itemText)

        if itemText == "Other":
            for widget in self.portableWidgets:
                widget.show()
        else:
            for widget in self.portableWidgets:
                widget.hide()
            sOrganism = self.organismDict[itemText]
            
            self.browseExpressionMatrixInputLabel.setValue(abspath(normcase(sOrganism.expressionMatrix)))
            self.browsePPIInput.setValue(abspath(normcase(sOrganism.PPI)))
            self.browseGoInput.setValue(abspath(normcase(sOrganism.go)))
            self.radioExpressionMatrixInputLabel.setChecked(True)
            self.radioExpressionMatrixInput.setChecked(False)
        
class AlgorithmParameter:
    def __init__(self, line):
        casting = {"int" : int, "float" : float }
        line = line.split()
        self.name = line[0]
        self.defaultValue = line[1]
        self.type = line[2]
        if self.type in ("int","float"):
            limits = line[3].split(",")
            self.min = casting[self.type](limits[0])
            self.max = casting[self.type](limits[1])
            self.step = casting[self.type](limits[2])

            self.description = " ".join(line[4:])
        else:
            self.description = " ".join(line[3:])


class BiclusteringAlgorithm:
    def __init__(self, filename):
        self.filename = filename
        self.parameters = []

        f = open(filename)
        self.name = f.readline().strip()
        secondLine = f.readline().strip().split()
        self.flagName = secondLine[0]
        self.flagValue = int(secondLine[1])
        for line in f:
            if line:
                self.parameters.append( AlgorithmParameter(line.strip()) )


class Parameter:
    def __init__(self):
        pass

class FileBrowser(QHBoxLayout):
    def __init__(self, defaultDirectory, parent=None):
        QHBoxLayout.__init__(self, parent)
        self.defaultDirectory = defaultDirectory or ""

        self.inputBox = QLineEdit()
        self.browseButton = QPushButton("..")
        self.addWidget(self.inputBox)
        self.addWidget(self.browseButton)
        self.browseButton.clicked.connect(self.browse)
        self.title = "Select your file"
        self.filter = "Text File (*.txt);;All files (*.*)"

    def value(self):
        """Returns the filebox text."""
        return normcase(str(self.inputBox.text()))

    def setValue(self, value):
        """Sets filebox text."""
        self.inputBox.setText(value)

    def setTitle(self, title):
        self.title = title
        
    def setFilter(self, filter):
        self.filter = filter + ";;All files (*.*)"

    def browse(self):
        """Opens file dialog for browsing file."""
        s = QFileDialog.getOpenFileName(None, self.title, self.defaultDirectory, self.filter)
        if s:
            self.inputBox.setText(s)

    def show(self):
        """Displays the widgets."""
        self.inputBox.show()
        self.browseButton.show()

    def hide(self):
        """Hides the widgets."""
        self.inputBox.hide()
        self.browseButton.hide()

class CustomRadio(QRadioButton):
    def __init__(self, text=None, parent=None):
        QRadioButton.__init__(self, text, parent)
    def value(self):
        """Returns 1 if checked, 0 otherwise."""
        if self.isChecked():
            return 1
        else:
            return 0
    def setValue(self, value):
        """Checks the radio button if value is 1, unchecks otherwise."""
        if value == 1:
            self.setChecked(True)
        else:
            self.setChecked(False)
            
class CustomCheckBox(QCheckBox):
    def __init__(self, text=None, parent=None):
        QCheckBox.__init__(self, text, parent)
    def value(self):
        if self.isChecked():
            return 1
        else:
            return 0

    def setValue(self, value):
        if value == 1:
            self.setChecked(True)
        else:
            self.setChecked(False)
            
class RadioGroup:
    def __init__(self, filename):
        self.filename = filename
        f = open(filename)
        self.title = f.readline().strip()
        self.commonStart = f.readline().strip()
        self.parameters = []
        self.parameterDict = {} # flagName =>  widget
        for line in f:
            line = line.strip().split()
            p = Parameter()
            p.flagName = line[0]
            p.defaultValue = line[1]
            p.type = line[2]
            p.description = " ".join(line[3:])
            self.parameters.append(p)

        self.createWidget()
    def value(self):
        if self.isChecked():
            return 1
        else:
            return 0
    def createWidget(self):
        self.widget = QGroupBox()
        w = self.widget

        w.setTitle(self.title)
        self.layout = QVBoxLayout(w)
        #self.layout = QGridLayout(w)
        #row = 0

        if self.commonStart:
            self.layout.addWidget(QLabel(self.commonStart))


        for parameter in self.parameters:
            #label = QLabel(parameter.description)
            #label.setWordWrap(True)
            radio = CustomRadio(parameter.description, w)
            radio.setToolTip(parameter.flagName)
            self.layout.addWidget(radio)
            #self.layout.addWidget(radio, row, 0)
            #self.layout.addWidget(label, row, 1)

            #row += 1
            self.parameterDict[parameter.flagName] = radio
