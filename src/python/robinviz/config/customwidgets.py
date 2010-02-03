from PyQt4.QtCore import *
from PyQt4.QtGui import *
from os.path import normcase, abspath, dirname

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
    def value(self):
        return normcase(str(self.inputBox.text()))

    def setValue(self, value):
        self.inputBox.setText(value)

    def browse(self):
        s = QFileDialog.getOpenFileName(None, "Select your microarray data", self.defaultDirectory, "Microarray Data (*.txt)")
        if s:
            self.inputBox.setText(s)


class CustomRadio(QRadioButton):
    def __init__(self, text=None, parent=None):
        QRadioButton.__init__(self, text, parent)
    def value(self):
        if self.isChecked():
            return 1
        else:
            return 0

class CustomCheckBox(QCheckBox):
    def __init__(self, text=None, parent=None):
        QCheckBox.__init__(self, text, parent)
    def value(self):
        if self.isChecked():
            return 1
        else:
            return 0
        
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
