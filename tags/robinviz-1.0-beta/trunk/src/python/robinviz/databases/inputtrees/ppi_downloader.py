# -*- coding: utf-8 -*-
"""
Downloads HitPredict data for a given organism. Uses hitpredict module to combine Small Scale, High Confidence, Low Confidence datas.
"""
import os
from hp_to_tabbedppi import *
sys.path.append("../..")
from utils.compression import download_targz
from utils.info import ap

organism_codes = {
    "S. cerevisiae": 4932,
    "H. sapiens": 9606,
    "D. melanogaster": 7227,
    "E. coli": 83333,
    "C. elegans": 6239,
    "A. thaliana": 3702,
    "M. musculus": 10090,
    "R. norvegicus": 10116,
    "S. pombe": 4896,
    }

def shorten_organism(long_name):
    """Converts Arabidopsis_thaliana to A. thaliana"""
    if "_" in long_name:
        names = long_name.split("_")
        return names[0][0] + ". " + names[1]

def get_ss_url(organism_name):
    """Returns small scale experiment result url"""
    code = organism_codes[ shorten_organism(organism_name) ]
    return "http://hintdb.hgc.jp/htp/download/%d_ss_interactions.tar.gz" % code

def get_hc_url(organism_name):
    code = organism_codes[ shorten_organism(organism_name) ]
    return "http://hintdb.hgc.jp/htp/download/%d_hc_interactions.tar.gz" % code
    
def get_lc_url(organism_name):
    code = organism_codes[ shorten_organism(organism_name) ]
    return "http://hintdb.hgc.jp/htp/download/%d_spurious_interactions.tar.gz" % code

def download_organism(organism_name):
    hitpredict_combined_ppi = ap("ppidata/hitpredict/%s.txt" % organism_name)
    if os.path.exists(hitpredict_combined_ppi):
        print "Hitpredict PPI file already exists, not creating again:", hitpredict_combined_ppi
	return
	
    files = []
    targzs = []
    for function in (get_hc_url, get_lc_url, get_ss_url):
	# Small scale should be last! hc will define max confidence. ss will take that value instead of t
	
	url = function(organism_name)
	targzs.append(url.split("/")[-1])
	filename = download_targz(url)[0]
	files.append(filename)	
	
    tabify(files, organism_name)
    map(os.remove, files)
    map(os.remove, targzs)

if __name__ == "__main__":
    organism = "Saccharomyces_cerevisiae"
    download_organism(organism)