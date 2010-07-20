from PyQt4.QtCore import *
from PyQt4.QtGui import *
from re import compile, match
from os import listdir
from os.path import exists, normcase
import shelve

class HighlightEllipse(QGraphicsItem):
    def __init__(self, x, y, rx, ry, parent = None, scene=None):
        QGraphicsItem.__init__(self, parent)
        self.x = x
        self.y = y
        self.rx = rx
        self.ry = ry
        self.parent = parent
        #self.scene = scene

    def boundingRect(self):
        return QRectF(self.x-self.rx, self.y-self.ry, self.rx*2, self.ry*2)

    def paint(self, painter, option, widget=None):
        painter.setCompositionMode(QPainter.CompositionMode_SourceAtop)
        pen = QPen()

        pen.setWidth(6)
        yellow = QColor()
        yellow.setRgb(255,255,0)

        pen.setColor(yellow)
        painter.setPen(pen)
        painter.setBrush(yellow)
        
        painter.setOpacity(0.2)
        painter.drawEllipse(QPointF(self.x,self.y),self.rx, self.ry)

class ProteinSearchWidget(QWidget):
    """
    Performs protein search in a specific cluster/category.
    """
    def __init__(self, parent=None, flags=Qt.Widget, singleWindow=None):
        QWidget.__init__(self, parent, flags)
        self.singleWindow = singleWindow
        self.scene = self.singleWindow.scene
        self.index = None
        self.setupGUI()
        self.setAutoCompletion()


    def setupGUI(self):
        # ========= Buttons ===========
        self.proteinButton = QPushButton("Pro")
        self.clearButton = QPushButton("C")

        buttonGroup = QHBoxLayout()
        buttonGroup.addWidget(self.proteinButton)
        buttonGroup.addWidget(self.clearButton)

        self.connect(self.proteinButton, SIGNAL('clicked()'), self.listProteins)
        self.connect(self.clearButton, SIGNAL('clicked()'), self.clearAll)

        # ======== Line Edit ==========
        self.lineEdit = QLineEdit()
        self.connect(self.lineEdit, SIGNAL('returnPressed()'), self.search)

        # ======== List Widget ========
        self.listWidget = QListWidget()
        #self.connect(self.listWidget, SIGNAL('itemClicked  (QListWidgetItem *)'), self.itemClicked)
        self.connect(self.listWidget, SIGNAL('currentItemChanged ( QListWidgetItem *, QListWidgetItem * )'), self.itemChanged)
        #self.connect(self.listWidget, SIGNAL('itemDoubleClicked  (QListWidgetItem *)'), self.itemDoubleClicked)

        # ======== General Layout ====
        layout = QVBoxLayout()
        layout.addLayout(buttonGroup)
        layout.addWidget(self.lineEdit)
        layout.addWidget(self.listWidget)
        self.setLayout(layout)
        self.setMaximumWidth(150)

    def focusInNode(self, nodeItem):
        nodeItem.setSelected(True)
        position = nodeItem.centerPos()
        self.scene.focusEllipse = self.focusEllipse = HighlightEllipse(position.x(), position.y(), 100, 100, scene=self.scene)
        self.scene.addItem(self.focusEllipse)
        self.scene.update()
    def focusOutNode(self, nodeItem):
        nodeItem.setSelected(False)
        if hasattr(self, 'focusEllipse'):
            if self.focusEllipse.scene() == self.scene:
                self.scene.removeItem(self.focusEllipse)
                self.scene.update()
        
    # ========= BUTTON EVENTS ==================
    def listProteins(self):
        self.clearAll()
        for protein in sorted(filter(lambda text: text[0]=="Y", self.index.keys())):
            self.listWidget.addItem(protein)

    def clearAll(self):
        self.lineEdit.clear()
        self.listWidget.clear()

    # ========== LIST WIDGET EVENTS ===============
    def itemChanged(self, current, previous):
        if previous:
            nodeItem = self.index.get(str(previous.text()))
            self.focusOutNode(nodeItem)
            
        if current:
            nodeItem = self.index.get(str(current.text()))
            self.focusInNode(nodeItem)


    # ========= LINE EDIT EVENTS ==================
    def setAutoCompletion(self):
        if not self.index:
            self.generateIndex()

        keyList = self.index.keys()

        completer = QCompleter(keyList, self.lineEdit)
        completer.setCaseSensitivity(Qt.CaseInsensitive)
        self.lineEdit.setCompleter(completer)

    def generateIndex(self):
        """Generates an index dictionary: gene - graphs"""
        # Create index dictionary.
        self.index = {}
        # ProteinName - NodeItem matching.
        for item in self.scene.items():
            if hasattr(item, 'node'):
                self.index[item.node.label] = item


    def search(self):
        keyword = self.lineEdit.text()
        self.listWidget.clear()
        if not self.index:
            #self.index = shelve.open("outputs/gene_index.shelve")
            self.generateIndex()

        
        nodeItem = self.index.get(str(keyword))

        if nodeItem:
            self.scene.clearSelection()
            self.focusInNode(nodeItem)
        else:
            QMessageBox.information(self, "No results", "The protein you look for does not exist in any of the sub-graphs.")


class ComprehensiveSearchWidget(QWidget):
    """
    Performs protein search in all clusters.

    TODO (optional):
    When views changed in mainwindow, update
    selected item in list.
    """
    def __init__(self, parent=None, flags=Qt.Widget, multiView=None):
        QWidget.__init__(self, parent, flags)
        self.multiView = multiView
        self.index = None
        self.setupGUI()
        self.setAutoCompletion()
    def setupGUI(self):
        # ========= Buttons ===========
        self.proteinButton = QPushButton("Pro")
        self.categoryButton = QPushButton("Cat")
        self.clearButton = QPushButton("C")

        buttonGroup = QHBoxLayout()
        buttonGroup.addWidget(self.proteinButton)
        buttonGroup.addWidget(self.categoryButton)
        buttonGroup.addWidget(self.clearButton)

        self.connect(self.proteinButton, SIGNAL('clicked()'), self.listProteins)
        self.connect(self.categoryButton, SIGNAL('clicked()'), self.listCategories)
        self.connect(self.clearButton, SIGNAL('clicked()'), self.clearAll)
        # ======== Line Edit ==========
        self.lineEdit = QLineEdit()
        self.connect(self.lineEdit, SIGNAL('returnPressed()'), self.search)

        # ======== List Widget ========
        self.listWidget = QListWidget()
        self.connect(self.listWidget, SIGNAL('itemClicked  (QListWidgetItem *)'), self.itemClicked)
        self.connect(self.listWidget, SIGNAL('itemDoubleClicked  (QListWidgetItem *)'), self.itemDoubleClicked)

        # ======== General Layout ====
        layout = QVBoxLayout()
        layout.addLayout(buttonGroup)
        layout.addWidget(self.lineEdit)
        layout.addWidget(self.listWidget)
        self.setLayout(layout)
        self.setMaximumWidth(200)
    
    # ========= BUTTON EVENTS ==================
    def listProteins(self):
        self.clearAll()
        for protein in sorted(filter(lambda text: text[0]=="Y", self.index.keys())):
            self.listWidget.addItem(protein)
    def listCategories(self):
        self.clearAll()
        for category in self.multiView.keyList:
            self.listWidget.addItem(category)
    def clearAll(self):
        self.lineEdit.clear()
        self.listWidget.clear()

    # ========== LIST WIDGET EVENTS ===============
    def itemClicked(self, item):
        try:
            id = int(item.text())
        except:
            id = self.index.get(str(item.text()))[0]
        self.emit(SIGNAL("graphClicked"), int(id))

    def itemDoubleClicked(self, item):
        try:
            # If double clicked on a number, emit signal
            id = int(item.text())
            self.emit(SIGNAL("graphDoubleClicked"), id)
        except:
            # If it's not a number, it means it's protein/category/bicluster
            if str(item.text()).startswith("Y"):
                # If double clicked on a protein, just search it.
                self.lineEdit.setText(item.text())
                self.search()
            else:
                # If double clicked on a category/bicluster, just emit id signal
                id = self.index.get(str(item.text()))[0]
                self.emit(SIGNAL("graphDoubleClicked"), id)
                
    # ========= LINE EDIT EVENTS ==================
    def setAutoCompletion(self):
        if not self.index:
            self.generateIndex()

        keyList = self.index.keys()

        completer = QCompleter(keyList, self.lineEdit)
        completer.setCaseSensitivity(Qt.CaseInsensitive)
        self.lineEdit.setCompleter(completer)

    def generateIndex(self):
        """Generates an index dictionary: gene - graphs"""
        # Take the original graph files (not other layouts)
        pattern = compile("^graph\d+.gml$")
        fileList = listdir(normcase('outputs/graphs'))
        self.graphFiles = filter(pattern.match, fileList)

        # Create index file.
        if not exists(normcase("outputs/gene_index.shelve")):
            self.index = shelve.open(normcase("outputs/gene_index.shelve"))
        
            # Now fetch the labels in each file.
            pattern = compile('label "\w+-?\w*"')
            for graphFile in self.graphFiles:
                content = open('outputs/graphs/'+graphFile).read()
                labels = pattern.findall(content)
                # Clear label " and " characters and get the gene name
                labels = map(lambda line: line[7:-1], labels)
                graphNum = int(graphFile[5:-4]) # get the number


                for label in labels:
                    x = self.index.get(label)
                    if not x:
                        # if no list entry (graph) yet
                        self.index[label] = [graphNum]
                    else:
                        # if there are previous entries, just append it.
                        temp = self.index[label]
                        temp.append(graphNum)
                        self.index[label] = temp


            # index     groupName
            #   0       mitochondrion inheritance
            #   1       reproduction
            # ...
            #   0       Bicluster 0
            #   1       Bicluster 1
            # ...
            for index, groupName in enumerate(self.multiView.keyList):
                self.index[groupName] = [ index ]

            self.index.close()
            self.index = shelve.open(normcase("outputs/gene_index.shelve"))
        else:
            # open the existing file.
            self.index = shelve.open(normcase("outputs/gene_index.shelve"))

            
        
    def search(self):
        keyword = self.lineEdit.text()
        self.listWidget.clear()
        if not self.index:
            #self.index = shelve.open("outputs/gene_index.shelve")
            self.generateIndex()

        self.index = shelve.open(normcase("outputs/gene_index.shelve"))
        graphs = self.index.get(str(keyword))
        
        if graphs:
            for graph in sorted(graphs):
                self.listWidget.addItem(str(graph))
        else:
            QMessageBox.information(self, "No results", "The protein you look for does not exist in any of the sub-graphs.")
        