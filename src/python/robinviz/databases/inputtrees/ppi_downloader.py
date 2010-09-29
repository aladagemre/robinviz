# -*- coding: utf-8 -*-
"""
Downloads HitPredict data for a given organism. Uses hitpredict module to combine Small Scale, High Confidence, Low Confidence datas.
"""
import os
import tarfile
from hp_to_tabbedppi import *
sys.path.append("../..")
from utils.compression import download_targz


def shorten_organism(long_name):
    """Converts Arabidopsis_thaliana to A_thaliana"""
    n1, n2 = long_name.split("_")[0:2]    
    return "%s_%s" % (n1[0], n2)
    
def get_ss_url(organism_name):
    """Returns small scale experiment result url"""
    return "http://hintdb.hgc.jp/htp/download/%s_ss.tar.gz" % shorten_organism(organism_name)

def get_hc_url(organism_name):
    return "http://hintdb.hgc.jp/htp/download/%s_hc.tar.gz" % shorten_organism(organism_name)
    
def get_lc_url(organism_name):
    return "http://hintdb.hgc.jp/htp/download/%s_spurious.tar.gz" % shorten_organism(organism_name)

def download_organism(organism_name):
    hitpredict_combined_ppi = "ppidata/hitpredict/%s.txt" % organism_name
    if os.path.exists(hitpredict_combined_ppi):
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