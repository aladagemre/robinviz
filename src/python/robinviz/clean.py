# -*- coding: utf-8 -*-

import os
import sys

def clean(param=None):
    print "Performing cleaning of the leftover files."
    # Revert some files
    if param == "rv":
	files = ("sources/usr_sources/visualization_data/genenames.txt",
		 "sources/usr_sources/visualization_data/geneNameConversion.txt",
		 "settings.ini",
		 "src/cpp/incl/bicalg/bimax/matrix_robin.txt",
		 "outputs/colors_func.txt", )
	for filename in files:
	    os.system("svn revert %s" % filename)
    
    
    # Remove series of files.
    for directory in ( "outputs/heatmap", "outputs/bicgenes", "outputs/graphs", "outputs/psfiles", "outputs/enrich", "outputs/parallel", "outputs/go"):
	fileList = os.listdir(directory)
	for filename in fileList:
	    if filename and "." in filename and filename[0] != ".":
		l = filename.split(".")
		lastdigit = l[-2][-1]
		# If last digit before the extension is number, delete it. It's an output.
		try:
		    lastdigit = int(lastdigit)
		    #print "Removing:", filename
		    os.remove(directory+"/"+filename)		
		except:
		    pass

    # Remove completely
    directories = ("outputs/graphs",)
    for directory in directories:
	for filename in os.listdir(directory):
	    try:
		#print "Removing", filename
		os.remove(directory+'/'+filename)
	    except:
		#print "Could not remove", filename
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
    "outputs/enrich/result.txt",
    "src/cpp/incl/bicalg/cc/matrix_robin.txt",
    "outputs/biclusters/CCResult.txt",
    "outputs/biclusters/RLEBResult.txt",
    "outputs/biclusters/BIMAXResult.txt",
    "outputs/biclusters/scoring.txt",
    "outputs/localization/localized_input.txt",
    "outputs/gene_index.shelve",
    )
    for filename in files:
	try:
	    #print "Removing:", filename
	    os.remove(filename)
	except:
	    #print filename, "does not exist."
	    pass

if __name__ == "__main__":
    if len(sys.argv) > 1:
	clean(sys.argv[1])
    else:
	clean()