# -*- coding: utf-8 -*-
"""Reads from go_daily-termdb.rdf-xml and converts it to an sql file called goinfo.sqlite3 so that it can be queried fastly (via indices)."""
import sys
sys.path.append("../..")

from xml.dom.minidom import parse
import shelve
import sqlite3
import os
from utils.info import ap
from utils.compression import ungz, download_file
import shutil

def createTable(curs):
    try:
	curs.execute('''
	CREATE TABLE terms (
	    id		INT	PRIMARY KEY,
	    name	TEXT,
	    children	TEXT
	) '''
	)
    except sqlite3.OperationalError:
	print "Table already exists"
	

def generateTermDict():
    # DOWNLOAD DATABASE TXT HERE
    input_file = ap("godata/go_daily-termdb.rdf-xml")
    print "Input: %s" % input_file
    if not os.path.exists(input_file):
	download_file("http://archive.geneontology.org/latest-termdb/go_daily-termdb.rdf-xml.gz", binary=True)
	ungz("go_daily-termdb.rdf-xml.gz")
	shutil.move("go_daily-termdb.rdf-xml", input_file)
    # "go_daily-termdb.rdf-xml/slim.xml"
    output_file = ap("godata/goinfo.sqlite3")
    print "Output: %s" % output_file


    # GO ON
    print "Parsing..."
    dom1 = parse(input_file)
    
    terms = dom1.childNodes[1].childNodes[1].childNodes
    
    try:
	os.remove(output_file)
    except:
	pass
    
    if os.path.exists(output_file):
	os.remove(output_file)
    conn = sqlite3.connect(output_file)
    curs = conn.cursor()
    createTable(curs)

    children_dict = {}
    for i in range(len(terms)):
	# For all term block, find the subnodes:
	cn = terms[i].childNodes
	
	if cn:
	    for element in cn:
		# for each subnode tag, look its name
		if "accession" in element.nodeName:
		    # if we see accession, it means it's the name of the term.
		    term_accession = str(element.childNodes[0].nodeValue)
		    try:
			term_id = int(term_accession.split("GO:")[1])
		    except IndexError:
			if term_accession != "all":
			    print "Index error:", term_accession
			continue
		elif  "is_a" in element.nodeName:
		    is_a_info =element.getAttribute("rdf:resource")
		    if "GO:" in is_a_info:
			parent_id = int(is_a_info.split("GO:")[1])
			#parent_accession = str("GO:"+ term_id)
			
			try:
			    child_list = children_dict[parent_id]
			except:
			    child_list = []
			child_list.append(str(term_id))
			children_dict[parent_id] = child_list
			
		elif "name" in element.nodeName:
		    name = str(element.childNodes[0].nodeValue)
		    #curs.execute("UPDATE terms SET name = '%s' where id = %d" % (name, int(term.accession.split("GO:")[1])) )
		    #curs.execute("UPDATE terms SET children = children || ' %s' where RowID = %d" % (term.accession,term_id) )
	    if name == "all":
		continue
	    fields =( str(term_id), '"%s"' % name, '""' )
	    statement = 'INSERT INTO terms VALUES(%s);' % ",".join( fields )
	    #print "Executing:", statement
	    try:
		curs.execute( statement )
	    except:
		print statement
		print 
    for parent_id, child_list in children_dict.iteritems():
	statement = "UPDATE terms SET children = '%s' where id = %d;" % (",".join(sorted(child_list)), parent_id) 
	
	try:
	    curs.execute( statement )
	except:
	    print statement
    
    conn.commit()
    conn.close()

if __name__ == "__main__":
    generateTermDict()