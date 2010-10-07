# -*- coding: utf-8 -*-
import sys
from genequery import GeneDB

def main(infile,outfile):
    f = open(infile)
    o = open(outfile,"w")
    db = GeneDB()
    for line in f:
	cols = line.strip().split("///")
	#print cols
	txt = "///".join(map(lambda e: str(db.svalue2svalue(e, "ENTREZ_GENE", "OFFICIAL_SYMBOL")), cols))
	o.write(txt+"\n")
    o.close()
    

if __name__ == "__main__":
    assert len(sys.argv) == 3, "2 parameters required: infile outfile"
    main(sys.argv[1], sys.argv[2])