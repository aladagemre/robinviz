from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtCore import qFuzzyCompare
import sys
import itertools

class Wizard(QWizard):
    def __init__(self, parent=None):
	QWizard.__init__(self, parent)
        self.addPage(PreSelectionPage())
        self.addPage(NextPage())

class NextPage(QWizardPage):
    def __init__(self, parent=None):
        QWizardPage.__init__(self, parent)
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.layout.addWidget(QLabel("Test"))
        
class PreSelectionPage(QWizardPage):
    def __init__(self, parent=None):
	QWizardPage.__init__(self, parent)
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.last= QRadioButton("Use the last settings")
        self.manual = QRadioButton("Define your manual settings")

        self.manual.setChecked(True)

        self.layout.addWidget(self.last)
        self.layout.addWidget(self.manual)

    def validatePage(self):
        return True

    def nextId(self):
        if self.manual.isChecked():
            return 1
        elif self.last.isChecked():
            return -1


class View(QGraphicsView):
    def __init__(self, parent=None):
        QGraphicsView.__init__(self, parent)

    def wheelEvent(self, event):
        coords = self.mapToScene(event.pos())

        self.centerOn(coords.x(), coords.y())
        #factor = 1.41 ** (event.delta() / 240.0)
        factor = 1.02 ** (event.delta() / 240.0)
        self.scale(factor, factor)
        
class ProteinNode(QGraphicsItem):
    def __init__(self,parent=None):
        QGraphicsItem.__init__(self, parent)
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        self.setColors( map(QColor, [Qt.green, Qt.red, Qt.blue, Qt.yellow]) )
        self.setPos(100,100)

    def setColors(self, colors):
        self.colors = colors
        self.num_colors = len(colors)
        self.angle_per_color = 16* (360 / self.num_colors)

    def boundingRect(self):
        return QRectF( 0, 0, 20, 20)
    
    def paint(self, painter, option, widget):
        rectangle = QRectF(0, 0, 20, 20)
        
        startAngle = 0
        for color in self.colors:
            painter.setBrush(QBrush(color))
            painter.drawPie(rectangle, startAngle, self.angle_per_color)
            startAngle += self.angle_per_color

class ProteinNode3(QGraphicsItem):
    def __init__(self,parent=None):
        QGraphicsItem.__init__(self, parent)
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        self.setColors( map(QColor, [Qt.green, Qt.red, Qt.blue, Qt.yellow, Qt.black, "#ABCDEF"]) )
        #self.setColors( map(QColor, [Qt.green]) )
        self.setPos(100,100)

    def setColors(self, colors):
        self.colors = colors
        self.num_colors = len(colors)
        self.angles = map(lambda x:x*16, [0.612360, 0.306000,0.360000,0.612360,240.918480, 117.244800])
        #self.angles = [360*16]

    def boundingRect(self):
        return QRectF( 0, 0, 20, 20)

    def paint(self, painter, option, widget):
        rectangle = QRectF(0, 0, 20, 20)

        startAngle = 0
        for i,color in enumerate(self.colors):
            painter.setBrush(QBrush(color))
            print startAngle, self.angles[i]
            painter.drawPie(rectangle, startAngle, self.angles[i])
            startAngle += self.angles[i]



class ProteinNode2(QGraphicsItem):
    def __init__(self,parent=None):
        QGraphicsItem.__init__(self, parent)
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        self.setPos(100,100)

    def boundingRect(self):
        return QRectF( 0, 0, 20, 20)

    def paint(self, painter, option, widget):
        painter.setBrush(QColor(Qt.black))
        painter.drawEllipse(0,0,20,20)
        
        self.qt_graphicsItem_highlightSelected(self, painter, option)
        
    def qMin(self, a1,  a2):
        if a1 >= a2:
            return a2
        else:
            return a1
    def qMax(self, a1,  a2):
        if a1 <= a2:
            return a2
        else:
            return a1


    def qt_graphicsItem_highlightSelected(self, item, painter, option):
        murect = painter.transform().mapRect(QRectF(0, 0, 1, 1))
        """if abs(self.qMax(murect.width(), murect.height()) + 1 - 1) < 0.00001:
            return"""

        if qFuzzyCompare(self.qMax(murect.width(), murect.height()) + 1, 1):
            return
        mbrect = painter.transform().mapRect(item.boundingRect())
        if self.qMin(mbrect.width(), mbrect.height()) < 1.0:
            return
        itemPenWidth = 1.0
        pad = itemPenWidth / 2
        penWidth = 0 # cosmetic pen

        fgcolor = option.palette.windowText().color()
        red = 0 if fgcolor.red() > 127 else 255
        green = 0 if fgcolor.green() > 127 else 255
        blue = 0 if fgcolor.blue() > 127 else 255
        bgcolor = QColor ( # ensure good contrast against fgcolor
        red, green, blue)

        painter.setPen(QPen(bgcolor, penWidth, Qt.SolidLine))
        painter.setBrush(Qt.NoBrush)
        painter.drawRect(item.boundingRect().adjusted(pad, pad, -pad, -pad))
        painter.setPen(QPen(option.palette.windowText(), 0, Qt.DashLine))
        painter.setBrush(Qt.NoBrush)
        painter.drawRect(item.boundingRect().adjusted(pad, pad, -pad, -pad))


        
class ItemDisplayer(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()

    def addItem(self, item):
        self.scene.addItem(item)
        self.scene.update()
        
    def setupGUI(self):
	layout = QVBoxLayout()
        view = View()
        self.scene = scene = QGraphicsScene()
        view.setScene(scene)
	layout.addWidget(view)

	self.setLayout(layout)
        self.setWindowTitle("Item Displayer")
        self.setMinimumSize(600,600)



def simplify_interactions(interactions):
    result = []

    for k,g in itertools.groupby(interactions, lambda x: x[0]+x[1]):

        l = list(g)
        print k,g,len(l)
        filtered_list = filter( lambda i: i[2]!="0.1" , l) # remove interactions with no confidence
        length_l = len(filtered_list)
        if length_l == 0:
            result.append(l[0]) # use it.
            continue
        elif length_l == 1: # if only one element
            result.append(filtered_list[0]) # use it.
            continue
        # if more than one confidence value, take average.
        average = sum(map(lambda i: float(i[2]), filtered_list)) / length_l
        print average
        result.append( ( filtered_list[0][0], filtered_list[0][1], average) )
    return result

            

def main():
    app = QApplication(sys.argv)
    """    displayer= ItemDisplayer()
    displayer.addItem(ProteinNode2())

    elips = QGraphicsEllipseItem(10,10,20,20)
    elips.setBrush(QBrush(QColor(Qt.black)))
    elips.setFlag(QGraphicsItem.ItemIsMovable, True)
    elips.setFlag(QGraphicsItem.ItemIsSelectable, True)
    #displayer.addItem(elips)
    displayer.show()

    ps = Wizard()
    ps.show()"""

    interactions = [ ['TOB1', 'SKP2', '0.1'], ['TOB1', 'SKP2', '0.25'], ['TOB1', 'SKP2', '0.3'] ]
    print simplify_interactions(interactions)


    








    sys.exit(app.exec_())

if __name__ == "__main__":
    main()


