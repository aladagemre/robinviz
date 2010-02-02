from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtWebKit
import sys

from heatmap import HeatMapWidget
from datatable import DataTableWidget

from os.path import normcase

class BiclusterWindow(QMainWindow):
    def __init__(self, id):
        QMainWindow.__init__(self)

        self.id = id
        
        self.layout = QVBoxLayout()
        self.tabWidget = QTabWidget()

        self.setupInformationTab()
        self.setupVisualTab()

        self.layout.addWidget(self.tabWidget)
        self.widget = QWidget()
        self.widget.setLayout(self.layout)
        self.setCentralWidget(self.widget)
        self.setWindowTitle("RobinViz - Bicluster Properties")


    def setupInformationTab(self):
        self.informationTab = QWidget()
        # Do here information stuff
        self.infoGridLayout = QGridLayout(self.informationTab)
        
        self.infoBrowser= QtWebKit.QWebView()
        self.infoBrowser.setUrl(QUrl("outputs/enrich/resultbic%d.html" % self.id))

        self.infoGridLayout.addWidget(self.infoBrowser,0,0)
        self.tabWidget.addTab(self.informationTab, "Information")

        # INFORMATION STUFF ENDS
    def setupVisualTab(self):

        self.visualTab = QWidget()
        # Do here visualization stuff
        visualLayout = QHBoxLayout(self.visualTab)

        #self.heatmap = QtWebKit.QWebView(self.visualTab)
        #self.heatmap.setUrl(QUrl("outputs/heatmap/out%d.html" % self.id))

        self.heatmapWidget = HeatMapWidget(normcase("outputs/heatmap/out%d.txt" % self.id))

        self.heatmap = QScrollArea()
        self.heatmap.setWidgetResizable(True)
        #self.heatmap.setBackgroundRole(QPalette.Dark)
        self.heatmap.setWidgetResizable(True)
        self.heatmap.setWidget(self.heatmapWidget)
        self.heatmap.ensureWidgetVisible(self.heatmapWidget)
        #self.heatmapWidget.repaint()
        #self.heatmapWidget.show()

        self.dataTableWidget = DataTableWidget(normcase("outputs/heatmap/out%d.txt" % self.id))
        self.dataTable = QScrollArea()
        #self.heatmap.setBackgroundRole(QPalette.Dark)
        self.dataTable.setWidgetResizable(True)
        self.dataTable.setWidget(self.dataTableWidget)
        self.dataTable.ensureWidgetVisible(self.dataTableWidget)



        visualLayout.addWidget(self.heatmap)
        visualLayout.addWidget(self.dataTable)

        self.tabWidget.addTab(self.visualTab, "Visualization")

        # VISUALIZATION STUFF ENDS



def main():
    app = QApplication(sys.argv)

    mainWindow = BiclusterWindow(0)
    mainWindow.showMaximized()
    #mainWindow.show()

    sys.exit(app.exec_())


if __name__ == "__main__":
    import cProfile
    cProfile.run('main()')


