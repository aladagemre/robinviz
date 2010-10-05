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
urls = map(lambda x: x.findChild('a').attrs[0][1], lis)
urls = filter(lambda text: ".gz?rev=HEAD" in text, urls)

pairs =  zip(organism_names, urls)
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
