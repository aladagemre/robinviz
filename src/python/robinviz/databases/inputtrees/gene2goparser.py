#!/usr/bin/python
# -*- coding: utf-8 -*-
"""Parses gene2go file and outputs the GO: genes mapping for given or all GO Terms."""

import os
import sys
import shutil

sys.path.append("../..")
from databases.genequery import GeneDB
from utils.info import ap
from utils.compression import download_file, ungz
from utils.decorators import injectArguments

class Gene2GOParser:
    @injectArguments
    def __init__(self, input_file, output_file, terms):
	self.check_and_download()
	self.f = open(self.input_file)
	self.lines = self.f.readlines()
	self.go_mapping = {}
	self.names = {}
	
	self.parse()
	self.write()

    def check_and_download(self):
	if not os.path.exists(self.input_file):
	    download_file("ftp://ftp.ncbi.nlm.nih.gov/gene/DATA/gene2go.gz")
	    ungz("gene2go.gz")
	    shutil.move("gene2go", self.input_file)
	    
	    
    def parse(self):	
	db = GeneDB()
	
	for line in self.lines[1:]:
	    cols = line.split("\t")
	    biogrid_id = cols[1]
	    go_id = cols[2]
	    name = cols[5]

	    result = db.biogrid2value(biogrid_id, "SYSTEMATIC_NAME")
	    
	    if result:
		biogrid_id = result
		
	    l = self.go_mapping.get(go_id)
	    if not l:
		l = []
	    
	    l.append(biogrid_id)
	    self.go_mapping[go_id] = l	    
	    
	    n = self.names.get(go_id)
	    if not n:
		self.names[go_id] = name

    def write(self):
	output = open(self.output_file,"w")
	if self.terms:
	    keys = self.terms
	else:
	    keys = self.go_mapping.keys()
	
	for key in sorted(keys):
	    try:
		output.write("%s\t%s\t%s\n" % (key, self.names[key], "\t".join(self.go_mapping[key])) )
	    except:
		print "key = ", key
		print "go_mapping[key] = ",self.go_mapping[key]
	output.close()


if __name__ == "__main__":
    ggp = Gene2GOParser(input_file=ap("/godata/gene2go"),output_file=ap("/godata/go_mapping.txt"), terms=None)
    #ggp = Gene2GOParser(input_file="godata/gene2go",output_file="godata/go_mapping.txt", terms=["GO:2000033","GO:2000037"])