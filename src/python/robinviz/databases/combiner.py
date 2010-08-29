# -*- coding: utf-8 -*-
"""
Downloads HitPredict data for a given organism. Uses hitpredict module to combine Small Scale, High Confidence, Low Confidence datas.
"""
import urllib
import os
import zipfile
import tarfile
from hp2tab import *


organism_codes = {
    "S. cerevisiae": 4932,
    "H. sapiens": 9606,
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

def download_file(url):
    filename = url.split('/')[-1]
    print "Downloading", url
    webFile = urllib.urlopen(url)
    
    localFile = open(filename, 'w')
    localFile.write(webFile.read())
    webFile.close()
    localFile.close()
    return filename

def download_targz(url):
    filename = url.split('/')[-1]
    extracted_filename = without_targz(filename) 
    
    if not os.path.exists(extracted_filename):
	# File does not exist, so download it.
	filename = download_file(url)
	if filename:
	    untar(filename)
    else:
	print "File already exists, skipping."
    
    # Filename without targz extension.
    return extracted_filename
    
def without_targz(filename):
    return ".".join(filename.split(".")[:-2])
    
def untar(filename):
    if not filename: return
    try:
	tar = tarfile.open(filename, 'r:gz')
	for item in tar:
	    tar.extract(item)
	print 'Extracted.'
	os.remove(filename)
    except:
	name = os.path.basename(filename)
	print name[:name.rfind('.')], '<filename>'


def download_organism(organism_name):
    files = []
    
    for function in (get_hc_url, get_lc_url, get_ss_url):
	# Small scale should be last! hc will define max confidence. ss will take that value instead of t
	
	url = function(organism_name)
	filename = download_targz(url)
	files.append(filename)	
	
    tabify(files)

organism = "Saccharomyces_cerevisiae"
download_organism(organism)