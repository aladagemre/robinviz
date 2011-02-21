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
        if not self.db.db_exists():
            raise Exception("Identifier database file not available")
        
    def set_filename(self, filename):
        self.filename = filename

    def translate(self):
        #print "Translating %s to biogrid ids" % self.filename
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


class BiogridOspreyFormatterOfficial:
    def __init__(self, filename=None):
        if filename:
            self.set_filename(filename)

    def set_filename(self, filename):
        self.filename = filename

    def translate(self):
        #print "Translating %s to biogrid ids" % self.filename
        lines = open(self.filename).readlines()
        output = open("%s-OFFICIAL_SYMBOL" % self.filename, "w")
        for line in lines[1:]:
            cols = line.split("\t")
            p1 = cols[2]
            p2 = cols[3]
            output.write("%s\t%s\n" % (p1, p2) )
        output.close()

class HitpredictToOfficialTranslator:
    def __init__(self, filename=None):
        if filename:
            self.set_filename(filename)

        self.db = GeneDB()
        if not self.db.db_exists():
            raise Exception("Identifier database file not available")
        
    def set_filename(self, filename):
        self.filename = filename

    def translate(self):
        #print "Translating %s to biogrid ids" % self.filename
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
        if filename:
            self.set_filename(filename)
        
        self.db = GeneDB()

        if not self.db.db_exists():
            raise Exception("Identifier database file not available")
        
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
        print "Translating ", filename.split("/")[-1]
        f = open(filename)
        for line in f:
            if line[0] == "!":
                continue
            cols = line.split("\t")

            # Get the information from the line
            go_term = cols[4]
            db_object_id = cols[1].strip()
            db_object_symbol = cols[2].strip()
            #db_object_name = cols[9].strip()
            db_object_synonym = cols[10].strip().split("|")

            # Gather the genes
            genes = []
            for object in [db_object_id, db_object_symbol]: #, db_object_name]:
                if object:
                    genes.append(object)
            if db_object_synonym:
                genes.extend(db_object_synonym)

            # Get the Association list
            l = go_dict.get(go_term)
            if not l:
                l = []

            # Translate and extend association list.
            for gene in genes:
                bids = self.db.value2biogrids(gene, only_ids=True)
                
                if bids:
                    converted += 1
                    l.extend(bids)
                else:
                    not_converted += 1
                    
            #print genes
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
        if not self.db.db_exists():
            return

        if not from_type:
            from_type = self.detect_annotation_type()
        if not from_type:
            print "Can not translate this document as source format is unknown."
            return False

        print "Annotation type:", from_type

        if from_type == to_type:
            print "Source and target types are the same, not converting but just reformatting"
            WILL_CONVERT = False
        else:
            print "Translating ", self.filename.split("/")[-1]
            WILL_CONVERT = True
            
    	o = open("%s-%s" % (self.filename, to_type) ,"w")
	converted = 0
	not_converted = 0
	self.go_dict = go_dict = {}
        self.translation_dict = {} # holds GENE: Translation pairs.

	# ===============================================
	
        f = open(self.filename)
        linenum = 0
        for line in f:
            if linenum % 100 == 0:
                print linenum
            linenum+=1
            if line[0] == "!":
                continue
            cols = line.split("\t")

            # Get the information from the line
            go_term = cols[4]
            db_object_id = cols[1].strip()
            db_object_symbol = cols[2].strip()
            #db_object_name = cols[9].strip()
            db_object_synonym = cols[10].strip().split("|")
            # I don't convert all the synonyms to OFFICIAL SYMBOL because
            # official symbols will be provided if any exists.

            # Gather the genes
            genes = []
            for object in [db_object_id, db_object_symbol]: #, db_object_name]:
                if object:
                    genes.append(object)
            if db_object_synonym:
                genes.extend(db_object_synonym)

            # Get the Association list
            l = go_dict.get(go_term)
            if not l:
                l = []

            # Translate and extend association list.

            for gene in genes:
                #print gene
                if WILL_CONVERT:
                    # if we are going to perform conversion,
                    # try cached translation first. have we translated it before?
                    result = self.translation_dict.get(gene)
                    if result:
                        # if we had translated before, use it.
                        gene = result
                    else:
                        # if we have not, then translate and save it for future use.
                        result = self.db.svalue2svalue(gene, from_type, to_type)
                        self.translation_dict[gene] = result
                        gene = result
                        
                if gene:
                    converted+=1
                    l.append(gene)
                else:
                    not_converted+=1
                

            go_dict[go_term] = list(set(l))
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



if __name__ == "__main__":
    t = AssociationTranslator()
    t.set_filename("assocdata/gene_association.goa_human")
    t.translate(from_type=None, to_type="OFFICIAL_SYMBOL")