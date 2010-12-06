# -*- coding: utf-8 -*-

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os
from utils.info import ap,rp


CATEGORY_COLORS = None
CATEGORY_NAMES = None
CHARS = None
CHAR_COLOR_ZIP = None

COLOR_DICT = None

def read_cat_info():
    global CHARS, CATEGORY_COLORS, CATEGORY_NAMES, COLOR_DICT, CHAR_COLOR_ZIP
    
    CHARS = map(chr, range(65, 88)) + map(chr, range(89, 91)) + map(chr, range(48, 56))
    CATEGORY_COLORS = open(ap("godata/highlevel_colors.txt")).read().split("\n")
    CATEGORY_NAMES = [ line.strip().split()[0].replace("_"," ") for line in open(rp("outputs/colors_func.txt")).readlines()[:-1] ]

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
        painter.drawText(60, 10, self.text)
        painter.setOpacity(0.5)
        painter.setBrush(QBrush(QColor(self.color)))
        painter.drawRect(0,0,50,12)
        

class LegendWidget(QWidget):
    def __init__(self, parent=None):
        QWidget.__init__(self, parent)
        read_cat_info()
        self.setupGUI()
        self.setWindowTitle("Color Legend for Highlevel Categories")

    def setupGUI(self):
        p = self.palette()
        p.setColor(QPalette.Background, Qt.white)
        self.setPalette(p)

        self.layout = QVBoxLayout()
        self.setLayout(self.layout)        
        # metabolic : (A, "#FFAABB")

        self.layout.addWidget(ColorPair ( ( QColor(None), "Unknown",)  ))
        for name in sorted(COLOR_DICT.keys()):
            letter_n_color = COLOR_DICT[name]
            self.layout.addWidget(ColorPair( ( letter_n_color[1], name, ) ))

        sizePolicy = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
        self.setSizePolicy(sizePolicy)
        self.resize(380,len(COLOR_DICT.keys()) * 20)

def main():
    app = QApplication(sys.argv)
    mainWindow = LegendWidget()
    mainWindow.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()