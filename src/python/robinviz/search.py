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
        proteins = keys #set(filter(lambda text: text[0].isdigit(), keys))
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
        self.setupVariables()
        self.setupGUI()
        self.setAutoCompletion()
        self.listCategories()

    def setupVariables(self):
        self.proteinNamePattern = compile('^\d+$') # not used anymore (for biogrid id)
        self.groupNamePattern = compile('^[_]{2}[\w\s-]*$') #__binding

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
        #return self.proteinNamePattern.match(text)
        return not self.isCategory(text.strip())        

    def isCategory(self, text):
        result = self.groupNamePattern.match(text)
        return result
    
    def splitProteinsCategories(self):
        # get the proteins
        keys = self.index.keys()
        # filter the categories
        categories = set (filter ( self.isCategory, keys) )
        # get the proteins
        proteins = list(set(keys) - categories)
        
        # sort proteins
        proteins.sort()

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
            category_id = self.index["__%s" % category][0]

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
        text = str(item.text())
        if text[0].isupper() and not text.startswith("Bicluster"):
            # Protein
            #id = self.index.get(text)[0]
            pass
        else:
            # Bicluster or Category name
            id = self.index.get(self.ec(text))[0]
            self.emit(SIGNAL("graphClicked"), int(id))

    def ec(self, name):
        """Adds preceding __ to the given name."""
        return "__%s" % name
    def dc(self, name):
        """Removes preceding __"""
        return name[2:]
    
    def itemDoubleClicked(self, item):
        text = str(item.text())
        if text[0].isupper() and not text.startswith("Bicluster"):
            # Protein
            self.lineEdit.setText(text)
            self.search()
            
        else:
            # Bicluster or Category name
            id = self.index.get(self.ec(text))[0]
            self.emit(SIGNAL("graphDoubleClicked"), int(id))

                
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
        if not filter( lambda fn: fn.startswith("gene_index.shelve"), listdir("outputs")):
            self.index = shelve.open(normcase("outputs/gene_index.shelve"))
        
            # Now fetch the labels in each file.
            pattern = compile('label "[\w-]+_?[A-Z:]*"')
            for graphFile in self.graphFiles: # scan all gml files.
                content = open('outputs/graphs/'+graphFile).read() # read the gml file
                labels = pattern.findall(content) # find all label lines.
                # Clear label " and " characters and get the gene name
                labels = map(lambda line: line[7:-1].split("_")[0], labels)
                graphNum = int(graphFile[5:-4]) # get the graph number parsing "graphxx.gml" > xx

                # for each label, add this graph file's number to its record in dictionary.
                for label in labels:
                    # try to get existing record label->graph1,graph2,graph3...
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

            # encode category/bicluster name with preceding $ sign.
            for index, groupName in enumerate(self.multiView.keyList):
                self.index["__%s" % groupName] = [ index ]

            self.index.close()
            self.index = shelve.open(normcase("outputs/gene_index.shelve"))
        else:
            # open the existing file.
            self.index = shelve.open(normcase("outputs/gene_index.shelve"))

            
        
    def search(self):
        keyword = self.lineEdit.text()
        self.listWidget.clear()
        if not self.index:
            self.generateIndex()

        self.index = shelve.open(normcase("outputs/gene_index.shelve"))
        graphs = self.index.get(str(keyword))

        if not hasattr(self, 'category_dict'):
            self.splitProteinsCategories()
            
        if graphs:
            for graph in sorted(graphs):
                category_name = self.category_dict[graph]
                self.listWidget.addItem(self.dc(category_name))
        else:
            QMessageBox.information(self, "No results", "The protein you look for does not exist in any of the sub-graphs.")
        