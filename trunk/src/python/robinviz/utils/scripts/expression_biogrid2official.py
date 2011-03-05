"""Converts existing GEO files in BIOGRID annotation to OFFICIAL_SYMBOL."""

from genequery import *
import os

def convert(filename):
	print "Converting", filename
	db = GeneDB()
	f = open(filename)
	lines = f.readlines()
	f.close()
	o = open(filename, "w")
	for line in lines[:2]:
		o.write(line)
	for line in lines[2:]:
		cols = line.split("\t")
		gene_bid = cols[0]
		try:			
			gene_bid = int ( gene_bid )
			rest = "\t".join ( cols[1:] )
			gene_os = db.biogrid2value(gene_bid, "OFFICIAL_SYMBOL")
			o.write("%s\t%s" % ( gene_os, rest ) )
		except:
			pass
			#o.write(line)
			
	o.close()


if __name__ == "__main__":
	files = os.listdir("/home/emre/robinviz/data2/expressions/gse/")
	files = filter( lambda filename: filename[0] != "." , files )
	for filename in files:
		convert( "/home/emre/robinviz/data2/expressions/gse/%s" % filename )