#! /usr/bin/python

from PyQt4.QtCore import *
from PyQt4.QtGui import *
import sys


class AnimatedItem(QGraphicsEllipseItem):
    def __init__(self, parent = None):
        QGraphicsEllipseItem.__init__(self, parent)
        self.setRect(0,0,30,30)
        self.setPos(50,50)
        self.animation = QGraphicsItemAnimation()
        self.animation.setItem(self)
        self.timeline = QTimeLine(1000)
        self.timeline.setCurveShape(QTimeLine.SineCurve)
        self.timeline.setFrameRange(0,100)
        self.timeline.setLoopCount(0)
        self.animation.setTimeLine(self.timeline)
        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemIsSelectable, True)
        self.width= self.boundingRect().width()
        self.originalPos = self.scenePos()
        print self.originalPos, self.width
        for i in range(100):

            newxPos = self.originalPos.x() - (0.01 * i) * (self.width /2)
            newyPos = self.originalPos.y() - (0.01 * i) * (self.width /2)

            self.animation.setPosAt(i/100.0, QPointF(newxPos, newyPos))
            self.animation.setScaleAt(i/100.0, 1 + 0.01 * i, 1 + 0.01 * i)
            
    def show(self):

        self.timeline.start()
    def stop(self):
        print "stop"
        self.timeline.stop()
        self.animation.setStep(0)

    def itemChange(self, change, value):
        if change == QGraphicsItem.ItemSelectedChange:
            if self.isSelected():
                print "stop!"
                self.setBrush(QColor(Qt.yellow))
                self.stop()
                

                
            else:
                print "start!"
                self.setBrush(QColor(Qt.red))
                self.show()

        return QVariant(value)
if __name__ =="__main__":
    app = QApplication(sys.argv)
    h = QWidget()
    
    view = QGraphicsView(h)
    scene = QGraphicsScene()
    scene.setSceneRect(QRectF(0,0, 500,500))
    view.setScene(scene)

    a = AnimatedItem()
    scene.addItem(a)
    #a.show()
    h.resize(250, 150)
    h.setWindowTitle('Animation Test')

    h.show()
    sys.exit(app.exec_())

