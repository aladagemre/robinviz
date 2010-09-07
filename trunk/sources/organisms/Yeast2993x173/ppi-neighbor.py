# -*- coding: utf-8 -*-
 
class NeighborFinder:
    def __init__(self):
	f = open("ppi.txt")
	self.gene_dict = {}

	for line in f:
	    gene1, gene2, confidence = line.split("\t\t")
	    l = self.gene_dict.get(gene1)
	    if not l:
		l = []
	    l.append( (gene1, gene2, confidence.strip()) )
	    self.gene_dict[gene1] = l

    def get_target(self, interaction):
	return interaction[1]
	
    def get_2_neighbors(self, gene):
	"""Returns 2-level neighbors of the given gene."""
	root_interactions = self.gene_dict.get(gene)
	interactions =  root_interactions 
    
	targets = map(self.get_target, root_interactions)
	target_interactions = map(self.gene_dict.get, targets)
	for target_interaction in target_interactions:
	    interactions.extend( target_interaction )
	
	return interactions    
    def write_interactions(self, interactions, filename):
	"""Writes the given interactions to the file with given filename."""
	f = open(filename,"w")
	
	for i in interactions:
	    f.write("\t\t".join(i)+"\n")
	f.close()
    
#print gene_dict
nf = NeighborFinder()
n2 = nf.get_2_neighbors("YLR192C")
nf.write_interactions(n2, "neighbor-ppi.txt")