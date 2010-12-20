# -*- coding: utf-8 -*-
"""This script downloads GO Annotation page and parses it. Creates an index file containing
Organism - URL pairs with some additional information
"""
import sys
sys.path.append("../..")

from utils.info import ap
from utils.BeautifulSoup import BeautifulSoup
import urllib

def generate_index():
    html = urllib.urlopen("http://www.geneontology.org/GO.downloads.annotations.shtml").read()
    soup = BeautifulSoup(html)

    filtered = []
    unfiltered = []

    for tr in soup('tr')[2:]:
        children = tr.findChildren()
        try:
            url = children[10].attrs[0][1]
        except:
            """print "==============="
            for i, child in enumerate(children):
            print i," - ", child
            print"""
            try:
                url = children[9].attrs[0][1]
            except:
                continue
        #if not ".gz" in url: continue
        source_db = children[0].contents[-1]
        source_db = source_db.replace("&nbsp;"," ")
        try:
            if len(children[0].contents) < 4:
                strain = None
            else:
                strain = children[0].contents[1].strip()
        except:
            print children[0]

        if getattr(children[0].contents[0], 'name', None) == "span":
            organism_name = children[1].getText()
        else:
            organism_name = str(children[0].contents[0])
            
        gene_products_annotated = children[3].getText()
        #annotations = children[4].getText()
        #submission_date = children[6].getText()
        
        if not url.startswith("http"):
            url = "http://www.geneontology.org/" + url
        record = (organism_name, source_db, gene_products_annotated, url)
        
        if "?rev=HEAD" in url:
            filtered.append(record)
        else:
            unfiltered.append(record)

        #print "%s (%s) from %s [%s genes]" % (organism_name, strain, source_db, gene_products_annotated)

    output = open(ap("assocdata/index.txt"), "w")
    output.write("Filtered\n")
    for record in filtered:
        output.write("\t%s\n" % "\t".join(record))
    output.write("Unfiltered\n")
    for record in unfiltered:
        output.write("\t%s\n" % "\t".join(record))    
    output.close()
    
if __name__ == "__main__":
    generate_index()