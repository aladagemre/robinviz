# -*- coding: utf-8 -*-

import sys
sys.path.append("..")
from translator import *
import os
from utils.info import ap, latest_osprey_dir

t = BiogridOspreyTranslator()
base = ap("ppidata/%s" % latest_osprey_dir()) + "/"
print base
organisms = filter(lambda name: not name[0] == ".", os.listdir(base) )
print organisms

for organism in organisms:
    for filename in os.listdir(base+organism):
	if not filename[0]=="." and not filename.endswith("-BIOGRID") and not os.path.exists(base+organism+"/"+filename + "-BIOGRID") :
	    t.set_filename( base+organism+"/"+filename )
	    t.translate()
        


"""for organism in organisms:
    for filename in os.listdir(base+organism):
        fn = base+organism+"/"+filename
        if fn.endswith("-BIOGRID-BIOGRID") or fn.endswith(".txt"): os.remove(fn)"""
