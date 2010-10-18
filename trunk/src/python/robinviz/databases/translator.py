# -*- coding: utf-8 -*-
"""Performs translation operations on files."""
import sys

if not "utils" in sys.path:
    sys.path.append("..")

from utils.info import ap
from genequery import GeneDB

class BiogridOspreyTranslator:
    def __init__(self, filename=None):
        if filename:
            self.set_filename(filename)
        self.db = GeneDB()
        
    def set_filename(self, filename):
        self.filename = filename

    def translate(self):
        print "Translating %s to biogrid ids" % self.filename
        lines = open(self.filename).readlines()
        output = open("%s-BIOGRID" % self.filename, "w")
        for line in lines[1:]:
            cols = line.split("\t")
            p1 = cols[0]
            p2 = cols[1]
            t1 = self.db.value2biogrids(p1, only_ids=True)
            t2 = self.db.value2biogrids(p2, only_ids=True)

            for t1x in t1:
                for t2x in t2:
                    output.write("%s\t%s\n" % (t1x, t2x) )
        output.close()
                    
            
class AssociationTranslator:
    def __init__(self, filename=None):
        self.db = GeneDB()
        if filename:
            self.set_filename(filename)
        
    def set_filename(self, filename):
        self.filename = ap(filename)

    def detect_annotation_type(self):
	"""Detects and returns the annotation type for the document.
	Uses a quick workaround. If first record seems to be only one type, then
	just return it. If more than one, take first 10 records and returns their
	intersection as the annotation type."""

	f = open( self.filename )
	sets = []
	counter = 0
	for line in f:
	    if line[0] == "!":
		continue

	    cols = line.split("\t")
	    if cols[2]:
		name = cols[2]
	    else:
		name = cols[1]


	    bids = self.db.value2biogrids(name)
	    if len(bids) == 1:
		# if only one type, just return it.
		# as intersection will be this one.
		return bids[0][1]

	    if bids:
                #print cols[2]
		# for each convertable increase the counter
                #print name
		counter += 1
		sets.append(set([bid[1] for bid in bids]))
	    if counter == 10:
		break

	#print sets
	if not sets:
	    return None

	# Now find the intersection. Start with the first one.
	# Iterate over the set cumulatively.
        #print sets
	s = sets[0]
	for i in range(len(sets)-1):
	    s = s.intersection(sets[i+1])

	# Intersection should be one item!
        if len(s) == 1:
            return s.pop()
        else:
            print "No consensus. # of Candidates: %d" % len(s)
            return None
	#assert len(s)==1, "Length of annotation set is not 1 but %d" %len(s)

    def translate_biogrids(self):
        o = open("%s-BIOGRID" % self.filename ,"w")
	converted = 0
	not_converted = 0
	self.go_dict = go_dict = {}

	# ===============================================
	filename = self.filename
        print filename.split("/")[-1]
        f = open(filename)
        for line in f:
            if line[0] == "!":
                continue
            cols = line.split("\t")
            protein_name1 = cols[1]
            protein_name2 = cols[2]
            go_term = cols[4]

            l = go_dict.get(go_term)
            if not l:
                l = []

            if protein_name2:
                candidate_name = protein_name2
            else:
                candidate_name = protein_name1

            if candidate_name:
                bids = self.db.value2biogrids(candidate_name, only_ids=True)
                if bids:
                    converted+=1
                    l.extend(bids)
                else:
                    not_converted+=1

            else:
                not_converted += 1



            go_dict[go_term] = l
        # ===============================================

        for key in sorted(go_dict.keys()):
            try:
                genes = go_dict[key]
                if genes:
                    genes_str = "\t".join( map(str, genes) )
                else:
                    genes_str = "NULL"


                o.write("%s\t%s\n" % ( key,  genes_str) )
            except:
                raise Exception, "problem here. key: %s; values = %s" % (key, go_dict[key])
        o.close()

        print "Converted:",converted
        print "Could not convert:",not_converted
        return True

    def translate(self, from_type, to_type):
        if not from_type:
            from_type = self.detect_annotation_type()
        if not from_type:
            print "Can not translate this document as source format is unknown."
            return False

        print "Annotation type:", from_type
        
    	o = open("%s-%s" % (self.filename, to_type) ,"w")
	converted = 0
	not_converted = 0
	self.go_dict = go_dict = {}

	# ===============================================
	filename = self.filename
        print filename.split("/")[-1]
        f = open(filename)
        for line in f:
            if line[0] == "!":
                continue
            cols = line.split("\t")
            protein_name1 = cols[1]
            protein_name2 = cols[2]
            go_term = cols[4]

            l = go_dict.get(go_term)
            if not l:
                l = []

            if protein_name2:
                candidate_name = protein_name2
            else:
                candidate_name = protein_name1
                
            if candidate_name:
                if to_type == "BIOGRID":
                    candidate_name = self.db.value2biogrid(candidate_name, from_type)
                else:
                    candidate_name = self.db.svalue2svalue(candidate_name, from_type, to_type)
                    
                if candidate_name:
                    converted+=1
                    l.append(candidate_name)
                else:
                    not_converted+=1

            else:
                not_converted += 1

                

            go_dict[go_term] = l
        # ===============================================

        for key in sorted(go_dict.keys()):
            try:
                genes = go_dict[key]
                if genes:
                    genes_str = "\t".join( map(str, genes) )
                else:
                    genes_str = "NULL"


                o.write("%s\t%s\n" % ( key,  genes_str) )
            except:
                raise Exception, "problem here. key: %s; values = %s" % (key, go_dict[key])
        o.close()

        print "Converted:",converted
        print "Could not convert:",not_converted
        return True

