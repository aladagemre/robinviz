# -*- coding: utf-8 -*-

import sys
sys.path.append("..")
from translator import BiogridOspreyTranslator
import os
from utils.info import ap, latest_osprey_dir
#from multiprocessing import Pool
#import time
from PyQt4.QtCore import QThread, pyqtSignal, QString

# TODO: this static value might cause bugs!
OSPREY_BASE = ap("ppidata/%s" % latest_osprey_dir()) + "/"

OSPREY_ORGANISMS = ["Arabidopsis_thaliana",
        "Caenorhabditis_elegans",
        "Drosophila_melanogaster",
        "Escherichia_coli_K12_MG1655",
        "Homo_sapiens",
        "Mus_musculus",
        "Rattus_norvegicus",
        "Saccharomyces_cerevisiae",
        "Schizosaccharomyces_pombe"]

def convert_organism(organism):
    OSPREY_BASE = ap("ppidata/%s" % latest_osprey_dir()) + "/"
    try:
        t = BiogridOspreyTranslator()
    except:
        return
    print "Translating", organism
    for filename in os.listdir(os.path.join(OSPREY_BASE,organism)):
        osprey_file = os.path.join(OSPREY_BASE, organism, filename)
        biogrid_file = osprey_file + "-BIOGRID"
        if not filename[0]=="." and not filename.endswith("-BIOGRID") and not os.path.exists(biogrid_file):
            t.set_filename( osprey_file )
            t.translate()

def convert_all_organisms():
    #start_time = time.time()
    #pool = Pool(processes = 4)
    #pool.map(convert_organism, OSPREY_ORGANISMS)
    for organism in OSPREY_ORGANISMS:
        convert_organism(organism)
    #print "Time elapsed: ", time.time() - start_time, "s"


class ConverterThread(QThread):
    status = pyqtSignal(QString)
    done = pyqtSignal(int)
    def __init__(self, parent=None):
        QThread.__init__(self, parent)

    def __del__(self):
        self.status.emit("Conversion Finished")
        self.done.emit(1)
        #self.wait()
        
    def run(self):
        convert_all_organisms()

    def start(self):
        self.run()

def pr(st):
    print st
    
if __name__ == '__main__':
    #convert_all_organisms()
    thread = ConverterThread()
    thread.status.connect(pr)
    thread.start()