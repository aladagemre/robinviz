# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os


COLORS18 = {
"A": "#CB2838",
"B": "#FEC9BD",
"C": "#5EF6DA",
"D": "#321B28",
"E": "#CD9BB4",
"F": "#28E4F2",
"G": "#BEF98A",
"H": "#6C9627",
"I": "#F481F0",
"J": "#8B7F82",
"K": "#4003C0",
"L": "#499411",
"M": "#E6C8BE",
"N": "#DB26B2",
"O": "#E4C645",
"P": "#87D29A",
"R": "#BC34F3",
"S": "#5D9154",
"X": "#808080",
}

class ColorPair(QWidget):
    def __init__(self, text, color, parent=None):
        QWidget.__init__(self, parent)
        self.color = color
        self.text = text

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setBrush(QBrush(QColor(self.color)))
        painter.drawRect(0,0,50,20)
        painter.drawText(60, 15, self.text)

class LegendWidget(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()


    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)        

        for letter, color in COLORS18.iteritems():
            self.layout.addWidget(ColorPair(letter, color))

        sizePolicy = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
        self.setSizePolicy(sizePolicy)
        self.resize(200,520)

def main():
    app = QApplication(sys.argv)
    mainWindow = LegendWidget()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()