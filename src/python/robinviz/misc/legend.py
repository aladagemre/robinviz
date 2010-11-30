# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os
from utils.info import ap

CHARS = map(chr, range(65, 88)) + map(chr, range(89, 91)) + map(chr, range(48, 56))
CATEGORY_COLORS = open(ap("godata/highlevel_colors.txt")).read().split("\n")
CATEGORY_NAMES = open(ap("godata/highlevel_categories.txt")).read().split("\n")

CHAR_COLOR_ZIP = zip(CHARS, CATEGORY_COLORS)
COLOR_DICT = dict ( zip ( CATEGORY_NAMES, CHAR_COLOR_ZIP ) )
# metabolic : (A, "#FFAABB")



class ColorPair(QWidget):
    def __init__(self, name_n_color, parent=None):
        QWidget.__init__(self, parent)
        self.color = name_n_color[0]
        self.text = name_n_color[1]

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setBrush(QBrush(QColor(self.color)))
        painter.drawRect(0,0,50,12)
        painter.drawText(60, 10, self.text)

class LegendWidget(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        self.setupGUI()


    def setupGUI(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)        
# metabolic : (A, "#FFAABB")

        for name in sorted(COLOR_DICT.keys()):
            letter_n_color = COLOR_DICT[name]
            self.layout.addWidget(ColorPair( ( letter_n_color[1], name, ) ))

        sizePolicy = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
        self.setSizePolicy(sizePolicy)
        self.resize(380,620)

def main():
    app = QApplication(sys.argv)
    mainWindow = LegendWidget()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()