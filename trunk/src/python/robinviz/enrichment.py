#encoding:utf8
"""Calculates enrichment values for a given gene."""

from utils.info import ap,rp
import re

def C(n, k):
    """
    A fast way to calculate binomial coefficients by Andrew Dalke (contrib).
    """
    if 0 <= k <= n:
        ntok = 1
        ktok = 1
        for t in xrange(1, min(k, n - k) + 1):
            ntok *= n
            ktok *= t
            n -= 1
        return ntok // ktok
    else:
        return 0


def analyse( category_code, graph_code ):
    """Performs enrichment calculations for a given category."""
    genes_in_category = get_genes_in_category(code_to_graphnum(category_code) )
    highlevel_categories = get_highlevel_codes()
    genes_associated_with_ahighlevel, highlevel_dict = get_genes_in_highlevel_cats()
    genes_in_whole_ppi =  get_genes_in_thewhole_ppi()
    genes_associated_with_ahighlevel_in_ppi =genes_in_whole_ppi.intersection(genes_associated_with_ahighlevel)


    I = len(genes_in_category)
    m =  len(genes_associated_with_ahighlevel_in_ppi) # toplam PPIDa herhangi bir high kategori ile eşleşmiş protein sayısı (gri olmayan)
    k = len( get_highlevel_codes() ) # highlevel kategori sayısı
    
    for highlevel_i in highlevel_categories:
        associated_genes = highlevel_dict.get(highlevel_i)
        if not associated_genes:
            continue
        intersection = genes_in_category.intersection(associated_genes)
        y = len(intersection)
        n = len ( genes_in_whole_ppi.intersection(highlevel_dict[highlevel_i]) ) # toplam PPIda highlevel_i ile eşleşmiş gene sayısı

        p_value =( C(I, y) * C(m-I, n-y) ) / float( C(m,n) )
        #for variable in [I, y, m, n, k]:
        print "I=", I, "\tGenes in this category"
        print "y=", y, "\tGenes in this category associated with", highlevel_i
        print "m=", m, "\ttoplam PPIDa herhangi bir high kategori ile eşleşmiş protein sayısı (gri olmayan)"
        print "n=", n, "\ttoplam PPIda %s ile eşleşmiş gene sayısı" % highlevel_i
        print "k=", k, "\thighlevel kategori sayısı"
        print "P Value for %s is %f" % ( highlevel_i, p_value)
        print "="*50
    
def get_highlevel_codes():
    content = open( rp("outputs/resultparams.txt") ).read()
    coloring_type = content.strip().split()[-1][1:]

    highlevel_file = ap("godata/toplevel_%s.txt" % coloring_type)
    print highlevel_file
    codes = open(highlevel_file).read().split("\n")
    return codes

def get_genes_in_category(graph_code):
    pattern = re.compile('label "[\w-]+_?[A-Z0-9:]*"')
    content = open(rp( 'outputs/graphs/graph%s.gml' % graph_code )).read() # read the gml file
    labels = pattern.findall(content) # find all label lines.
    # Clear label " and " characters and get the gene name
    labels = map(lambda line: line[7:-1].split("_")[0], labels)
    return set(labels)

def get_genes_in_thewhole_ppi():
    genes = set()
    for line in open ( ap("ppidata/ppi.txt") ):
        cols = line.split("\t")
        genes.add(cols[0])
        genes.add(cols[1])
    genes.remove('')
    return genes

def get_genes_in_highlevel_cats():
    genes = set()
    highlevel_dict = {}
    
    highlevel_categories = get_highlevel_codes()
    f = open ( ap("assocdata/go.txt") )
    for line in f:
        code = line[:10]
        if code in highlevel_categories:
            gene_list = line.split("\t")[2:]
            genes.update ( gene_list )
            highlevel_dict[code] = gene_list
    return genes, highlevel_dict

def code_to_graphnum( code ):
    codes = open ( ap("godata/selected_terms.txt") ).read().split("\n")
    return codes.index(code)

print analyse("GO:0009055", 2)
    