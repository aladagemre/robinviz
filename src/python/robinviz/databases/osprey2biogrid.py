# -*- coding: utf-8 -*-

import sys
sys.path.append("..")
from translator import BiogridOspreyFormatterOfficial
from inputtrees.ppi_downloader import get_organism_code
from inputtrees.hp_to_tabbedppi import tabify
import os
from utils.info import ap, latest_osprey_dir
#from multiprocessing import Pool
#import time
from PyQt4.QtCore import QThread, pyqtSignal, QString

# TODO: this static value might cause bugs!
OSPREY_BASE = ap("ppidata/%s" % latest_osprey_dir()) + "/"
hitpredict_original_datadir = ap("ppidata/hitpredict/original/")

OSPREY_ORGANISMS = ["Arabidopsis_thaliana",
        "Caenorhabditis_elegans",
        "Drosophila_melanogaster",
        "Escherichia_coli_K12_MG1655",
        "Homo_sapiens",
        "Mus_musculus",
        "Rattus_norvegicus",
        "Saccharomyces_cerevisiae",
        "Schizosaccharomyces_pombe"]



def get_hitpredict_files(organism_name):
    """Returns experiment files for given organism"""
    code = get_organism_code(organism_name)
    return [ "%s/%d_hc_interactions" % (hitpredict_original_datadir, code),
            "%s/%d_spurious_interactions" % (hitpredict_original_datadir, code),
            "%s/%d_ss_interactions" % (hitpredict_original_datadir, code) ]

def convert_organism(organism):
    print "Converting", organism

    t = BiogridOspreyFormatterOfficial()
    tabify(get_hitpredict_files(organism), organism)
    
    print "Formatting Biogrid data for", organism, "in OFFICIAL_SYMBOL format."
    for filename in os.listdir(os.path.join(OSPREY_BASE,organism)):
        osprey_file = os.path.join(OSPREY_BASE, organism, filename)
        biogrid_file = osprey_file + "-OFFICIAL_SYMBOL"
        if not filename[0]=="." and not filename.endswith("-OFFICIAL_SYMBOL") and not os.path.exists(biogrid_file):
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