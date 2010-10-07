# -*- coding: utf-8 -*-
from BeautifulSoup import BeautifulSoup
import urllib

def split_organism_name(span):
    line = span.findParent().text
    organism_name = span.getText()
    info = line[len(organism_name):]
    
    return organism_name, info.replace("&nbsp;", " ")
    

html = urllib.urlopen("http://www.geneontology.org/GO.downloads.annotations.shtml").read()
soup = BeautifulSoup(html)
organism_names = map(split_organism_name, soup('span')[:-1])

lis = soup('li')
lis = filter(lambda li: "annotations" in li.getText(), lis)
urls = map(lambda x: x.findChild('a').attrs[0][1], lis)
#urls = filter(lambda text: ".gz" in text, urls)

filtered = []
unfiltered = []

"""for li in lis:
    source_info = li.findParent().findParent().findPrevious().findPrevious().findPrevious()
    source_name = source_info.getText()
    organism_name = source_info.findPrevious().findPrevious().findPrevious().getText()
    url = li.findChild('a').attrs[0][1]
    record = (organism_name, source_name, url)
    
    if "?rev=HEAD" in url:
	filtered.append(record)
    else:
	unfiltered.append(record)
"""

for tr in soup('tr')[2:]:
    children = tr.findChildren()
    organism_name = children[1].getText()
    gene_products_annotated = children[3].getText()
    annotations = children[4].getText()
    submission_date = children[6].getText()
    url = children[10].attrs[0][1]
    
	
"""print "Filtered"
print filtered
print
print "Unfiltered"
print unfiltered"""

output = open("assocdata/index.txt", "w")
output.write("Filtered\n")
for record in filtered:
    output.write("\t%s\n" % "\t".join(record))
output.write("Unfiltered\n")
for record in unfiltered:
    output.write("\t%s\n" % "\t".join(record))    
output.close()
    


"""pairs =  zip(organism_names, urls)
for pair in pairs:
    print pair

output = open("assocdata/index.txt", "w")

organism_dict = {}

for pair in pairs:
    organism_dict[pair[0]] = pair[1]

for organism in sorted(organism_dict.keys()):
    line = "\t".join( (organism[0], organism[1], organism_dict[organism] ) )
    output.write( "%s\n" %  line)
output.close()

"""