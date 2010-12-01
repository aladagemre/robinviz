# -*- coding: utf-8 -*-
from genequery import *
db = GeneDB()
f = open("go_slim.txt")
o = open("go_slim_bio.txt","w")
for line in f:
    cols = line.split("\t")
    category = cols[0]
    name = cols[1]
    rest = cols[2].split(" ")
    genes = map(str, filter( lambda gene: gene, map( lambda gene: db.value2biogrid(gene, "SYSTEMATIC_NAME"), rest) ) )
    o.write("%s\t%s\t%s\n" % (category, name, " ".join(genes) ) )

o.close()