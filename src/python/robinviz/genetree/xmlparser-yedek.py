# -*- coding: utf-8 -*-
from xml.dom.minidom import parse
import shelve

def generateTermDict():
    dom1 = parse("go_daily-termdb.rdf-xml/go_daily-termdb.rdf-xml")
    #dom1 = parse("go_daily-termdb.rdf-xml/slim.xml")
    terms = dom1.childNodes[1].childNodes[1].childNodes


    term_dict = shelve.open("termdict.db")

    for i in range(len(terms)):
	cn = terms[i].childNodes
	if cn:
	    for element in cn:
		if "accession" in element.nodeName:
		    term = str(element.childNodes[0].nodeValue)
		    last_term = term
		    #print "Accession:", term
		    if not term in term_dict:
			term_dict[term] = []
		if  "is_a" in element.nodeName:
		    is_a_info =element.getAttribute("rdf:resource")
		    if "GO:" in is_a_info:
			term = str("GO:"+ is_a_info.split("GO:")[1])
			#print "is_a= ", term
			l =  term_dict.get(term)
			if not l:
			    l = []
			l.append(last_term)
			term_dict[term] = l
    return term_dict

def getTermDict():
    return shelve.open("termdict.db")
    
if __name__ == "__main__":
    d = generateTermDict()
    d.close()



"""
def traverse(node, level):
    print "\t" * level + node
    children = term_dict[node]
    for child in children:
	traverse(child, level+1)
    
for parent in ("GO:0008150", "GO:0005575", "GO:0003674"):
    traverse(parent, 0)
output = ""
"""
		    
"""
for i in range(1, len(terms), 2):
    #print terms[i].childNodes[1].childNodes[0].nodeValue
    for x in terms[i].childNodes:
	if x.childNodes:
	    
	    value=  x.childNodes[0].nodeValue.strip()
	    if value:
		print "'%s'" % value
    
""" 
    
#terms[5].childNodes[1].childNodes[0].nodeValue
