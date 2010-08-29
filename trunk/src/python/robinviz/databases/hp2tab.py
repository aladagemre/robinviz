# -*- coding: utf-8 -*-
"""
Given Hitpredict filenames, extracts the confidence values, normalizes and combines them into a single tab-seperated PPI file.
"""
from genequery import GeneDB

def normalize(data):
    match = {}
    max_val = max(data)
    min_val = min(data)
    for i, value in enumerate(data):
	if max_val == min_val:
	    new_val = max_val
	else:
	    new_val = float(value - min_val) / (max_val - min_val)
            data[i] = new_val
        match[value] = new_val
    return data, match
    
def tabify(files):
    lines = []
    for filename in files:
	lines.extend( open(filename).readlines() )
    
    output = open("%s_tabbed.txt" % filename.split("_")[0], "w")
    confs = set()
    max_confidence = 0
    for line in lines:
	if line:
	    try:
		int(line[0])
		fields = line.split("|")
		try:
		    confidence = float(fields[-4])
		    if confidence > max_confidence:
			max_confidence = confidence
		except ValueError:
		    confidence = max_confidence # FIX THIS! LIKELIHOOD IS NOT PROBABILITY
		confs.add(confidence)
	    except:
		pass
    #print confs
    result, match = normalize(list(confs))
    #print result

    db = GeneDB()

    no_correspondance = set()
    for line in lines:
	if line:
	    try:
		int(line[0])
	    except:
		pass
	    else:
		fields = line.split("|")
		protein1 = fields[2]
		value = db.value2value(protein1, "SYSTEMATIC_NAME")
		if isinstance(value, list):
		    value = "/".join(value)
		if value:
		    protein1 = value
		else:
		    no_correspondance.add(protein1)
		protein2 = fields[3]
		value = db.value2value(protein2, "SYSTEMATIC_NAME")
		if isinstance(value, list):
		    value = "/".join(value)
		if value:
		    protein2 = value
		else:
		    no_correspondance.add(protein2)
		
		try:
		    confidence = float(fields[-4])
		except ValueError:
		    confidence = 1.0 # FIX THIS! LIKELIHOOD IS NOT PROBABILITY
		
		t = (protein1, protein2, str(match[confidence]))
		output.write("\t".join(t) + "\n")

    print "These genes have no corresponding names in BIOGRID_IDENTIFIERS:"
    for gene in sorted(no_correspondance):
	print gene
    #print confs
    output.close()