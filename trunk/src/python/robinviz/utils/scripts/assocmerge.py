"""Translates existing formatted association files in BIOGRID annoatation
to OFFICIAL_SYMBOL."""
from genequery import GeneDB

class AssociationB2OTranslator:
    def __init__(self, filename):
        self.filename = filename
        self.db = GeneDB()
    def translate(self):
        f = open(self.filename)
        o = open(self.filename + "-OS", "w")
        for line in f:
            o.write(self.__convert_line(line))
            
        o.close()
            
    def __convert_line(self, line):
        cols = line.strip().split("\t")
        category = cols[0]
        genes = cols[1:]

        gene_set = set()
        for gene in genes:
            official_gene = self.db.biogrid2value(gene, "OFFICIAL_SYMBOL")
            if official_gene:
                gene_set.add(official_gene)

        gene_list = list(gene_set)
        if not gene_list:
            gene_list = ["NULL"]
        return "%s\t%s\n" % (category, "\t".join(gene_list))

if __name__ == "__main__":
    a = AssociationB2OTranslator("inputtrees/assocdata/gene_association.goa_human-BIOGRID")
    a.translate()