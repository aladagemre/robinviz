# -*- coding: utf-8 -*-

import os
# Remove series of files.
for directory in ("sources/graph_sources", "outputs/heatmap", "outputs/bicgenes", "outputs/graphs", "outputs/psfiles", "outputs/enrich", "outputs/parallel"):
    fileList = os.listdir(directory)
    for filename in fileList:
	if filename and "." in filename and filename[0] != ".":
	    l = filename.split(".")
	    lastdigit = l[-2][-1]
	    # If last digit before the extension is number, delete it. It's an output.
	    try:
		lastdigit = int(lastdigit)
		print "Removing:", filename
		os.remove(directory+"/"+filename)		
	    except:
		pass
	

# Remove individual files.
files = ("src/python/xcoord/gmls/pyt_inp.gml", 
"src/python/xcoord/gmls/pyt_out.gml",
"positionsAfter.txt","positionsAfterPostProcess.txt", 
"positions.txt", 
"layers.txt",
"sources/graph_sources/graph_infos.txt",
"outputs/graphs/maingraph.gml",
"outputs/graphs/graph6circular.gml",
"outputs/psfiles/main.ps",
"outputs/enrich/result.html",
"outputs/enrich/result.txt",)
for filename in files:
    try:
	print "Removing:", filename
	os.remove(filename)
    except:
	print filename, "does not exist."
