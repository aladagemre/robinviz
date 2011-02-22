import sys
sys.path.append("..")
from utils.info import rp,ap
from re import compile

def merge_graph(ids):
    genes = []
    interactions = []
    
    ppi_file = open( ap("ppidata/ppi.txt") )
    pattern = compile('label "[\w-]+_?[A-Z:]*"')
    
    for gid in ids:
        content = open( rp("outputs/graphs/graph%d.gml" % gid) ).read()
        labels = pattern.findall(content) # find all label lines.
        # Clear label " and " characters and get the gene name
        labels = map(lambda line: line[7:-1].split("_")[0], labels)
        genes.extend(labels)

    genes = set(genes)
    print genes
    
    for line in ppi_file:
        gene1,gene2,confidence = line.split("\t\t")
        if gene1 in genes and gene2 in genes:
            interactions.append((gene1, gene2, confidence))

    f = open("merged_graphs.txt", "w")
    f.write("".join(map(lambda i: "\t".join(i), interactions)))
    f.close()


merge_graph([9,21])