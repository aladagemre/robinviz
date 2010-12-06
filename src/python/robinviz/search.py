from PyQt4.QtCore import *
from PyQt4.QtGui import *
from re import compile, match
from os import listdir
from os.path import exists, normcase
import shelve

class HighlightEllipse(QGraphicsItem):
    def __init__(self, x, y, rx, ry, parent = None):
        QGraphicsItem.__init__(self, parent)
        self.x = x
        self.y = y
        self.rx = rx
        self.ry = ry
        self.setZValue(-10)
        self.visible = False

    def boundingRect(self):
        return QRectF(self.x-self.rx, self.y-self.ry, self.rx*2, self.ry*2)

    def setPos(self, position):
        self.x = position.x()
        self.y = position.y()
        self.update()

    def show(self):
        self.visible = True
        self.update()
        
    def hide(self):
        self.visible = False
        self.setPos(QPointF(0,0))
        
    def paint(self, painter, option, widget=None):
        if not self.visible:
            return

        painter.setCompositionMode(QPainter.CompositionMode_SourceAtop)
        pen = QPen()

        pen.setWidth(6)
        yellow = QColor()
        yellow.setRgb(255,255,0)

        pen.setColor(yellow)
        painter.setPen(pen)
        painter.setBrush(yellow)
        
        #painter.setOpacity(0.2)
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
        topleft = self.scene.itemsBoundingRect().topLeft()
        self.focusEllipse = HighlightEllipse(topleft.x(), topleft.y(), 100, 100)
        self.scene.addItem(self.focusEllipse)
        self.scene.selectionChanged.connect(self.focusEllipse.hide)
        
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
        self.focusEllipse.setPos(position)
        self.focusEllipse.show()
        self.scene.update()

    def focusOutNode(self, nodeItem):
        nodeItem.setSelected(False)    
        self.listWidget.setCurrentItem(None)
        self.focusEllipse.hide()
        self.scene.update()
        
    # ========= BUTTON EVENTS ==================
    def listProteins(self):
        self.clearAll()
        keys = self.index.keys()
        proteins = set(filter(lambda text: text[0].isdigit(), keys))
        for protein in sorted(proteins):
            self.listWidget.addItem(protein)

    def clearAll(self):
        self.lineEdit.clear()
        self.listWidget.clear()

    # ========== LIST WIDGET EVENTS ===============
    def itemChanged(self, current, previous):
        for item in self.scene.selectedItems():
            item.setSelected(False)
            
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
                self.index[ item.node.label.split("_")[0] ] = item


    def search(self):
        keyword = self.lineEdit.text()
        self.listWidget.clear()
        if not self.index:
            #self.index = shelve.open("outputs/gene_index.txt")
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
        self.setupVariables()
        self.setupGUI()
        self.setAutoCompletion()
        self.listCategories()

    def setupVariables(self):
        self.proteinNamePattern = compile('^\d+$')

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

    def isProtein(self, text):
        return self.proteinNamePattern.match(text)

    def splitProteinsCategories(self):
        # get the proteins
        keys = self.index.keys()
        # filter the proteins
        proteins = set(filter(lambda text: text[0].isdigit(), keys))
        # filter the category names
        categories = set(keys) - proteins

        # sort proteins by int
        proteins_int = map(int, proteins )
        proteins_int.sort()
        proteins = map(str, proteins_int)

        if not hasattr(self, 'category_dict'):
            # now index categories        
            self.category_dict = {}
            for category in categories:
                # for each category, match
                # category # - category name pair.
                i = self.index[category][0]
                self.category_dict[i] = category

        return proteins

    # ========= BUTTON EVENTS ==================
    def listProteins(self):
        self.clearAll()
        proteins = self.splitProteinsCategories()
        
        for protein in proteins:
            self.listWidget.addItem(protein)

    def listCategories(self):
        self.clearAll()
        #print "Keylist:", self.multiView.keyList
        if self.multiView.keyList[0].startswith("Bicluster"):
            keyList = self.multiView.keyList
        else:
            keyList = sorted(self.multiView.keyList)
            
        for category in keyList:
            category_id = self.index[category][0]

            item = QListWidgetItem(category)

            if not exists(normcase("outputs/graphs/graph%d.gml" % category_id)):
                strike_font = QFont()
                strike_font.setStrikeOut(True)
                item.setFont(strike_font)

            self.listWidget.addItem(item)

    def clearAll(self):
        self.lineEdit.clear()
        self.listWidget.clear()

    # ========== LIST WIDGET EVENTS ===============
    def itemClicked(self, item):
        try:
            # if clicked on a protein,
            id = int(item.text())
            # do nothing
        except:
            # if it's not a protein but a category/bicluster
            # get its id and emit a signal
            id = self.index.get(str(item.text()))[0]
            self.emit(SIGNAL("graphClicked"), int(id))

    def itemDoubleClicked(self, item):
        itemText = item.text()
        try:
            # If double clicked on a protein, just search it.
            id = int(itemText)
            self.lineEdit.setText(itemText)
            self.search()
            
        except:
            # If it's not a number, it means it's category/bicluster                
            id = self.index.get(str(itemText))[0]
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
        if not exists(normcase("outputs/gene_index.txt")):
            self.index = shelve.open(normcase("outputs/gene_index.txt"))
        
            # Now fetch the labels in each file.
            pattern = compile('label "\d+_?[A-Z:]*"')
            for graphFile in self.graphFiles:
                content = open('outputs/graphs/'+graphFile).read()
                labels = pattern.findall(content)
                # Clear label " and " characters and get the gene name
                labels = map(lambda line: line[7:-1].split("_")[0], labels)
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
            self.index = shelve.open(normcase("outputs/gene_index.txt"))
        else:
            # open the existing file.
            self.index = shelve.open(normcase("outputs/gene_index.txt"))

            
        
    def search(self):
        keyword = self.lineEdit.text()
        self.listWidget.clear()
        if not self.index:
            self.generateIndex()

        self.index = shelve.open(normcase("outputs/gene_index.txt"))
        graphs = self.index.get(str(keyword))

        if not hasattr(self, 'category_dict'):
            self.splitProteinsCategories()
            
        if graphs:
            for graph in sorted(graphs):
                category_name = self.category_dict[graph]
                self.listWidget.addItem(category_name)
        else:
            QMessageBox.information(self, "No results", "The protein you look for does not exist in any of the sub-graphs.")
        