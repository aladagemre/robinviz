# -*- coding: utf-8 -*-
"""
Given Hitpredict filenames, extracts the confidence values, normalizes and combines them into a single tab-seperated PPI file.
"""
import sys
import os
from databases.genequery import GeneDB
from utils.info import ap

def normalize(data):
    match = {}
    max_val = max(data)
    min_val = min(data)
    for i, value in enumerate(data):
	if max_val == min_val:
	    new_val = max_val
	else:
	    new_val = 0.2 + 0.8 * (  float(value - min_val) / (max_val - min_val) )
            data[i] = new_val
        match[value] = new_val
    return data, match
    
def tabify(files, organism_name):
    print "Combining Hitpredict PPI experiments for %s..." % organism_name

    hitpredict_combined_ppi = ap("ppidata/hitpredict/%s.txt" % organism_name)
    if os.path.exists(hitpredict_combined_ppi):
        print hitpredict_combined_ppi, "already exists, no need to process."
        return
        
    lines = []
    for filename in files:
        l = open(filename).readlines()
	lines.extend( l )
    
    output = open(hitpredict_combined_ppi, "w")
    confs = set()
    max_confidence = 0
    # TODO: Here max_confidence assignment is faulty! use 1 or 0.1.
    # What about t/f values?
    
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
                # ======PROTEIN1===============
		protein1 = fields[2]
                #value = db.value2biogrids(protein1, types=["SWISSPROT", "GENBANK_PROTEIN_ACCESSION"], only_ids=True)
                #value = db.value2biogrids(protein1, only_ids=True)
                value = db.svalue2svalue(protein1, "GENBANK_PROTEIN_ACCESSION", "OFFICIAL_SYMBOL")

		
		if isinstance(value, list):
		    #if len(value)>1:
			#print protein1, value
		    value = "/".join(value)
		    
		if value:
		    protein1 = str(value)
		else:
                    #print "could not convert:", protein1
		    no_correspondance.add(protein1)
                    continue

                # ======PROTEIN2===============
		protein2 = fields[3]
		#value = db.value2biogrid(protein2, ["SWISSPROT", "GENBANK_PROTEIN_ACCESSION"])
                #value = db.value2biogrids(protein2, types=["SWISSPROT", "GENBANK_PROTEIN_ACCESSION"], only_ids=True)
                #value = db.value2biogrids(protein2, only_ids=True)
                value = db.svalue2svalue(protein2, "GENBANK_PROTEIN_ACCESSION", "OFFICIAL_SYMBOL")
		if isinstance(value, list):
		    value = "/".join(value)
		if value:
		    protein2 = str(value)
		else:
                    #print "could not convert:", protein2
		    no_correspondance.add(protein2)
                    continue
                    
		try:
		    confidence = float(fields[-4])
		except ValueError:
		    confidence = 1.0 # FIX THIS! LIKELIHOOD IS NOT PROBABILITY
		for proteinx in protein1.split("/"):
		    for proteiny in protein2.split("/"):
			t = (proteinx, proteiny, str(match[confidence]))
			output.write("\t".join(t) + "\n")
                
		"""t = (protein1, protein2, str(match[confidence]))
		output.write("\t".join(t) + "\n")"""

    print "These genes have no corresponding names in BIOGRID_IDENTIFIERS:"
    print sorted(no_correspondance)	
    #print confs
    output.close()