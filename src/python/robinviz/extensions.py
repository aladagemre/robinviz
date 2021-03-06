from core import *
from drawing import *

class MainView(View):
    def __init__(self, parent=None):
        View.__init__(self, parent)
        self.main = True
        self.nodeClass = CentralNode


    def newWindow(self):
        if not hasattr(self, 'specialWindow'):
            from windows import SingleMainViewWindow
            self.specialWindow = SingleMainViewWindow(self.__class__, self.mainSceneClass, self.scene())
            #self.specialWindow.loadGraph(self.scene().filename)
        self.specialWindow.showMaximized()


class PeripheralView(View):
    def __init__(self, parent=None):
        View.__init__(self, parent)
        self.preview = False
        
    def setPreview(self, value=True):
        self.preview = value
        
    def mousePressEvent(self, event):
        QGraphicsView.mousePressEvent(self, event)
        if hasattr(self.scene(), "id"):
            self.emit(SIGNAL("viewSelected"), self.scene().id)

    def newWindow(self):
        if not getattr(self, 'specialWindow', False):
            from windows import SinglePeripheralViewWindow
            self.specialWindow = SinglePeripheralViewWindow(self.__class__, self.scene())
            #self.specialWindow.loadGraph(self.scene().filename)
            #self.specialWindow.scene.setId(self.scene().id)
        if len(self.scene().items())>30:
            self.specialWindow.showMaximized()
        else:
            view = self.specialWindow.view
            view.fitInView(view.sceneRect())
            self.specialWindow.show()

        self.emit(SIGNAL("newWindowOpened"))


    def focusInEvent(self, event):
        """Highlights the view with color yellow when focused."""
        View.focusInEvent(self, event)
        if self.preview:
            if not hasattr(self, 'normalBackground'):
                self.normalBackground = self.backgroundBrush()
            self.setBackgroundBrush(QBrush(QColor(Qt.yellow)))
    def focusOutEvent(self, event):
        """Removes yellow highlight when focused out."""
        View.focusOutEvent(self, event)
        if self.preview:
            self.setBackgroundBrush(self.normalBackground)