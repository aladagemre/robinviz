from PyQt4.QtCore import *
from PyQt4.QtGui import *
import sys


class View(QGraphicsView):
    def __init__(self, parent=None):
        QGraphicsView.__init__(self, parent)

    def wheelEvent(self, event):
        coords = self.mapToScene(event.pos())

        self.centerOn(coords.x(), coords.y())
        #factor = 1.41 ** (event.delta() / 240.0)
        factor = 1.02 ** (event.delta() / 240.0)
        self.scale(factor, factor)
        
class PiechartNode(QGraphicsItem):
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

class PiechartNode2(QGraphicsItem):
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

def main():
    app = QApplication(sys.argv)
    displayer= ItemDisplayer()
    displayer.addItem(PiechartNode())

    elips = QGraphicsEllipseItem(10,10,20,20)
    elips.setBrush(QBrush(QColor(Qt.black)))
    elips.setFlag(QGraphicsItem.ItemIsMovable, True)
    elips.setFlag(QGraphicsItem.ItemIsSelectable, True)
    #displayer.addItem(elips)
    displayer.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()