# -*- coding: utf-8 -*-
from xml.dom.minidom import parse
import shelve


class Term:
    def __init__(self, accession=None):
	self.accession = accession
	self.name = None
	self.children = []
	
    def add_child(self, term):
	self.children.append(term)
	
    def __repr__(self):
	return "%s (%s) - %s" % (self.accession, self.name, self.children)
	
def generateTermDict():
    dom1 = parse("go_daily-termdb.rdf-xml/go_daily-termdb.rdf-xml")
    #dom1 = parse("go_daily-termdb.rdf-xml/slim.xml")
    terms = dom1.childNodes[1].childNodes[1].childNodes


    term_dict = shelve.open("termdict.db")

    for i in range(len(terms)):
	# For all term block, find the subnodes:
	cn = terms[i].childNodes
	
	if cn:
	    for element in cn:
		# for each subnode tag, look its name
		if "accession" in element.nodeName:
		    # if we see accession, it means it's the name of the term.
		    term = Term( str(element.childNodes[0].nodeValue) )
		    term_dict[term.accession] = term
		    
		elif  "is_a" in element.nodeName:
		    is_a_info =element.getAttribute("rdf:resource")
		    if "GO:" in is_a_info:
			parent_accession = str("GO:"+ is_a_info.split("GO:")[1])
			
			try:
			    parent = term_dict[parent_accession]
			except KeyError:
			    new_term = Term( parent_accession )
			    parent = new_term
			    #raise Exception("No such parent has ever created! Parent: %s, Child: %s" % ( parent_accession, term.accession ) ) # this shouldnt happen!
			#print "adding child %s to %s" % (term, parent)
			#print "Before:", parent
			parent.add_child(term)
			#print "After:", parent
			term_dict[parent_accession] = parent
			    
    #print term_dict
    return term_dict

def getTermDict():
    return shelve.open("termdict.db")
    
if __name__ == "__main__":
    d = generateTermDict()
    d.close()
    """
    t1 = Term("GO:0000001")
    t1.name="Term1"
    t2 = Term("GO:0000002")
    t2.name="Term2"
    t3 = Term("GO:0000003")
    t3.name="Term3"
    t4 = Term("GO:0000004")
    t4.name="Term4"
    
    t1.add_child(t2)
    t1.add_child(t3)
    t3.add_child(t4)
    
    print t1"""



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
