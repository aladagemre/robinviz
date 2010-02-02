from PyQt4.QtCore import Qt, QPointF
from PyQt4.QtGui import QColor, QRadialGradient

def redRadialGradient(item):
    radialGradient = QRadialGradient(50, 50, 50, 70, 70);
    radialGradient.setColorAt(0.0, QColor(255, 164, 28, 127));
    radialGradient.setColorAt(0.1, Qt.white);
    radialGradient.setColorAt(1.0, Qt.red);
    radialGradient.setFocalPoint(item.pos()+QPointF(item.w/2,item.w/2))

    return radialGradient

def blueRadialGradient(item):
    radialGradient = QRadialGradient(50, 50, 50, 70, 70);
    radialGradient.setColorAt(0.0, QColor(255, 164, 28, 127));
    radialGradient.setColorAt(0.1, Qt.white);
    radialGradient.setColorAt(1.0, Qt.blue);
    radialGradient.setFocalPoint(item.pos()+QPointF(item.w/2,item.w/2))

    return radialGradient