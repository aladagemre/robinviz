from PyQt4.QtCore import *
from PyQt4.QtGui import *
import sys

class PiechartNode(QGraphicsItem):
    def __init__(self, parent=None):
        QGraphicsItem.__init__(self, parent)
        self.brush = QBrush(QColor(Qt.black))
        self.setColors([Qt.green, Qt.red, Qt.blue, Qt.yellow])
        
    def setColors(self, colors):
        self.colors = colors
        self.num_colors = len(colors)
        self.angle_per_color = 16* (360 / self.num_colors)


    def paint(self, painter, option, widget):
        #painter.drawRoundedRect(-10, -10, 20, 20, 5, 5)
        painter.setBrush(self.brush)
        w = 20.0
        rectangle = QRectF(0.0, 0.0, w, w)
        """startAngle = 30 * 16;
        spanAngle = 120 * 16;
        painter.drawPie(rectangle, startAngle, spanAngle)"""

        startAngle = 0
        
        for color in self.colors:
            painter.setBrush(QBrush(QColor(color)))
            painter.drawPie(rectangle, startAngle, self.angle_per_color)
            startAngle += self.angle_per_color


class ItemDisplayer(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()

    def addItem(self, item):
        self.scene.addItem(item)
        self.scene.update()
        
    def setupGUI(self):
	layout = QVBoxLayout()
        view = QGraphicsView()
        self.scene = scene = QGraphicsScene()
        view.setScene(scene)
	layout.addWidget(view)

	#button = QPushButton("Report Selected PPIs")
	#button.clicked.connect(self.getCheckedItems)
	#layout.addWidget(button)

	self.setLayout(layout)
        self.setWindowTitle("Item Displayer")
        self.setMinimumSize(600,600)
    
def main():
    app = QApplication(sys.argv)
    displayer= ItemDisplayer()
    displayer.addItem(PiechartNode())
    displayer.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()