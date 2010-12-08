# -*- coding: utf-8 -*-
import sys
import urllib
import os

if not "utils" in sys.path:
    sys.path.append("../..")
    
from utils.info import ap
from utils.compression import download_file_to, ungz
from databases.genequery import GeneDB

class GOAParser:
    def __init__(self):
	self.file_urls = { \
	"Arabidopsis_thaliana": "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.tair.gz?rev=HEAD",   
	"Caenorhabditis_elegans": "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.wb.gz?rev=HEAD",
	"Drosophila_melanogaster": "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.fb.gz?rev=HEAD",
	"Escherichia_coli_K12_MG1655": "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.ecocyc.gz?rev=HEAD",
	"Homo_sapiens": "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.goa_human.gz?rev=HEAD",
	"Mus_musculus": "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.mgi.gz?rev=HEAD",
	"Rattus_norvegicus": "http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.rgd.gz?rev=HEAD", 
	"Saccharomyces_cerevisiae":"http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.sgd.gz?rev=HEAD",
	"Schizosaccharomyces_pombe":"http://cvsweb.geneontology.org/cgi-bin/cvsweb.cgi/go/gene-associations/gene_association.GeneDB_Spombe.gz?rev=HEAD"
	}
	
	self.ungz_names = { \
	"Arabidopsis_thaliana":'gene_association.tair',
	"Caenorhabditis_elegans":'gene_association.wb',
	"Drosophila_melanogaster":'gene_association.fb',
	"Escherichia_coli_K12_MG1655":'gene_association.ecocyc',
	"Homo_sapiens": 'gene_association.goa_human',
	"Homo_sapiens":'gene_association.mgi',
	"Rattus_norvegicus":'gene_association.rgd',
	"Saccharomyces_cerevisiae":'gene_association.sgd',
	"Schizosaccharomyces_pombe":'gene_association.GeneDB_Spombe'
	}
	
	self.db = GeneDB()

    def check_organism(self, organism_name):
	if os.path.exists(ap("godata/%s.txt" % organism_name)):
	    return True
	else:
	    self.download_organism(organism_name)
	    
    
    def download_organism(self, organism_name):
	filename = download_file_to(self.file_urls[organism_name], ap("godata"), binary=True)
	ap("godata/%s" % filename)
	ungz(ap("godata/%s" % filename))

    def parse_organism(self, organism_name):
	filename = self.ungz_names[organism_name]
	print "Parsing", filename
	converted = 0
	not_converted = 0
	f = open(ap("godata/%s" % filename) )
	o = open(ap("godata/%s.txt" % organism_name),"w")
	go_dict = {}
	
	for line in f:
	    if line[0] == "!":
		continue
	    cols = line.split("\t")
	    protein_name1 = cols[2]
	    protein_name2 = cols[3]
	    go_term = cols[4]
	    
	    l = go_dict.get(go_term)
	    if not l:
		l = []
	    if protein_name2:
		protein_name2 = self.db.value2value(protein_name2,"SYSTEMATIC_NAME")
		if protein_name2:
		    converted+=1
		    l.extend(protein_name2)
		else:
		    not_converted+=1
		
	    else:
		protein_name1 = self.db.value2value(protein_name1,"SYSTEMATIC_NAME")
		if protein_name1:
		    converted+=1
		    l.extend(protein_name1)
		else:
		    not_converted+=1
		
	    go_dict[go_term] = l
	
	for key in sorted(go_dict.keys()):
	    try:
		o.write("%s\t%s\n" % (key, "\t".join(go_dict[key])) )
	    except:
		raise Exception, "problem here. key: %s; values = %s" % (key, go_dict[key])
	o.close()
    
	print "Converted:",converted
	print "Could not convert:",not_converted
	
    def _convert_path_to_organism_name(self, path):
	if os.name == "posix":
	    seperator = "/"
	else:
	    seperator = "\\"
	# TODO: could be better
	return path.split(seperator)[-2]

    def findSelectedOrganisms(self):
	f = open(ap("ppidata/selected_ppis.txt"))
	checkedPaths = map(lambda line: line.strip(), f.readlines())
	
	organisms = set()
	for path in checkedPaths:
	    organismName = self._convert_path_to_organism_name(path)
	    organisms.add(organismName)
	    
	return list(organisms)
	
    def start(self):
	self.organisms = self.findSelectedOrganisms()
	for organism in self.organisms:
	    self.check_organism(organism)
	    self.parse_organism(organism)
	    
if __name__ == "__main__":
    g = GOAParser()
    g.start()