# -*- coding: utf-8 -*-
"""
A gene name conversion utility. Can do conversions

Biogrid ID -> Other Identifier IDs
Biogrid ID -> Specific Identifier ID
Specific Identifier ID -> Biogrid ID

>>> from genequery import *
>>> db = GeneDB()
"""
import sqlite3
import sys
import os
sys.path.append("..")
sys.path.append("../..")
from utils.info import database_root

class GeneDB:
    def __init__(self):
	self.conn = sqlite3.connect(os.path.join(database_root, "identifier.db"))
	self.cursor = self.conn.cursor()

    def value2biogrids(self, value):
	"""Converts a given value of an UNKNOWN TYPE to corresponding biogrid ids.
	>>> db.value2biogrids("30990")
	[('57562', 'ENTREZ_GENE'), ('127342', 'HGNC'), ('30990', 'BIOGRID')]
	"""
	self.cursor.execute("SELECT biogrid_id, identifier_type FROM translation WHERE identifier_value=?;",  (value,) )
	result = self.cursor.fetchall()
	
	biogrid_ids = list(set([(str(row[0]), str(row[1])) for row in result]))	    
	
	return biogrid_ids
    
    
    def value2biogrid(self, identifier_value, identifier_type):
	"""Converts a given value in a SPECIFIC TYPE to corresponding biogrid id.
	>>> db.value2biogrid("30990", "ENTREZ_GENE")
	57562
	"""
	self.cursor.execute("SELECT biogrid_id FROM translation WHERE identifier_value=? AND identifier_type=?;" , (identifier_value , identifier_type) )
	result = self.cursor.fetchall()
	#print result
	if not result:
	    return None
	"""if len(result) > 1:
	    print identifier_value, identifier_type, result
	assert len(result) == 1, "Length is not 1 but %d" % len(result)"""
	biogrid_id = result[0][0]
	return biogrid_id
	
    def biogrid2values(self, biogrid_id):
	"""Converts a given biogrid_id to the other identifier values.
	Result is a list in the format:
	[  (value1, type1), (value2, type2), ... ]
	
	>>> db.biogrid2values("30994")
	[(u'850374', u'ENTREZ_GENE'), (u'ETG850374', u'ENTREZ_GENE_ETG'), (u'SUF16', u'OFFICIAL_SYMBOL'), (u'TG(GCC)C', u'SYSTEMATIC_NAME'), (u'85666111', u'GENBANK_GENOMIC_DNA_GI'), (u'NC_001135', u'REFSEQ_GENOMIC_DNA_ACCESSION'), (u'S000006575', u'SGD'), (u'30994', u'BIOGRID')]

	"""
	self.cursor.execute( "SELECT identifier_value, identifier_type FROM translation WHERE biogrid_id=?;", (biogrid_id,) )
	result = self.cursor.fetchall()
	return result
	
    def biogrid2value(self, biogrid_id, identifier_type):
	"""Converts a given biogrid_id to the specified identifier type's value.
	Result is a single string like: ETG814974
	
	>>> db.biogrid2value(35247, "SYSTEMATIC_NAME")
	'YMR072W'
	"""
	self.cursor.execute("SELECT identifier_value FROM translation WHERE biogrid_id=? and identifier_type=?;" , (biogrid_id, identifier_type)  )
	result = self.cursor.fetchone()
	if not result:
	    return None
	    
	return str(result[0])
	
    def value2value(self, value, identifier_type):
	"""Converts a given value to a specific type of value. This gives possible outputs as
	input protein's type is unknown.
	
	>>> db.value2value("30990", "SYSTEMATIC_NAME")
	['Dmel_CG3176', 'YCR011C']
	"""
	# Find corresponding biogrid IDs. There might be more than one.
	bids = self.value2biogrids(value)
	#print bids
	# Convert biogrid ids in the list to specified identifier type and return the resulting list.
	return list(set(filter(lambda x: x, [self.biogrid2value(bid[0], identifier_type) for bid in bids])))

    def svalue2svalue(self, from_value, from_type, to_type):
	"""Converts a given value of a specific type to the given specific type.
	Example:
	>>> db.svalue2svalue("YMR072W", "SYSTEMATIC_NAME", "OFFICIAL_SYMBOL")
	'ABF2'
	"""

	bid = self.value2biogrid(from_value, from_type)
	result = self.biogrid2value(bid, to_type)
	return result
    
    def checkUniqueValues(self):
	"""Checks if identifier values are unique so that no problem would arise when value2biogrid is called.
	Returns True if all values are unique, else False."""
	
	self.cursor.execute("SELECT identifier_type, identifier_value FROM translation;")
	result = self.cursor.fetchall()
	values = [(str(row[0]), str(row[1])) for row in result]
	x = len(values)
	y = len(set(values))
	
	return x - y
	



# Some example runs and their outputs:

"""
db = GeneDB()
print db.value2biogrids("YCR011C")
# ['30990']

print db.biogrid2values("30990")
#[(u'850369', u'ENTREZ_GENE'), (u'ETG850369', u'ENTREZ_GENE_ETG'), (u'YCR011C', u'SYSTEMATIC_NAME'), (u'ADP1', u'OFFICIAL_SYMBOL'), (u'51013563', u'GENBANK_PROTEIN_GI'), (u'51013562', u'GENBANK_GENOMIC_DNA_GI'), (u'14588926', u'GENBANK_PROTEIN_GI'), (u'85666111', u'GENBANK_GENOMIC_DNA_GI'), (u'X59720', u'GENBANK_GENOMIC_DNA_ACCESSION'), (u'AAT93075', u'GENBANK_PROTEIN_ACCESSION'), (u'P25371', u'GENBANK_PROTEIN_ACCESSION'), (u'14588895', u'GENBANK_GENOMIC_DNA_GI'), (u'AY693056', u'GENBANK_GENOMIC_DNA_ACCESSION'), (u'10383780', u'GENBANK_PROTEIN_GI'), (u'45644964', u'GENBANK_PROTEIN_GI'), (u'CAA42328', u'GENBANK_PROTEIN_ACCESSION'), (u'NP_009937', u'REFSEQ_PROTEIN_ACCESSION'), (u'NC_001135', u'REFSEQ_GENOMIC_DNA_ACCESSION'), (u'P25371', u'SWISSPROT'), (u'S000000604', u'SGD'), (u'649', u'GRID_LEGACY'), (u'30990', u'BIOGRID')]

print db.biogrid2value("30990", "SYSTEMATIC_NAME")
# YCR011C
"""

if __name__ == "__main__":
    db = GeneDB()
    import doctest
    doctest.testmod()