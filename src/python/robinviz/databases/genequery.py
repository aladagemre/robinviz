# -*- coding: utf-8 -*-
"""
A gene name conversion utility. Can do conversions

Biogrid ID -> Other Identifier IDs
Biogrid ID -> Specific Identifier ID
Specific Identifier ID -> Biogrid ID
"""
import sqlite3

class GeneDB:
    def __init__(self):
	self.conn = sqlite3.connect("identifier.db")
	self.cursor = self.conn.cursor()
	
    def value2biogrids(self, value):
	"""Converts a given value to corresponding biogrid ids."""
	self.cursor.execute("SELECT biogrid_id FROM translation WHERE identifier_value='%s';" % value )
	result = self.cursor.fetchall()
	
	biogrid_ids = list(set([str(row[0]) for row in result]))	    
	return biogrid_ids
    
    
    def value2biogrid(self, identifier_value, identifier_type):
	"""Converts a given value to corresponding biogrid ids."""
	self.cursor.execute("SELECT biogrid_id FROM translation WHERE identifier_value='%s' AND identifier_type='%s';" % (identifier_value , identifier_type))
	result = self.cursor.fetchone()
	
	biogrid_id = result[0][0]
	return biogrid_id
	
    def biogrid2values(self, biogrid_id):
	"""Converts a given biogrid_id to the other identifier values.
	Result is a list in the format:
	[  (value1, type1), (value2, type2), ... ]
	"""
	statement = "SELECT identifier_value, identifier_type FROM translation WHERE biogrid_id='%s';" % biogrid_id 
	#print statement
	self.cursor.execute(statement)
	result = self.cursor.fetchall()
	return result
	
    def biogrid2value(self, biogrid_id, identifier_type):
	"""Converts a given biogrid_id to the specified identifier type's value.
	Result is a single string like: ETG814974
	"""
	self.cursor.execute("SELECT identifier_value FROM translation WHERE biogrid_id='%s' and identifier_type='%s';" % (biogrid_id, identifier_type)  )
	result = self.cursor.fetchone()
	if not result:
	    return None
	    
	return str(result[0])
	
    def value2value(self, value, identifier_type):
	"""Converts a given value to a specific type of value."""
	# Find corresponding biogrid IDs. There might be more than one.
	bids = db.value2biogrid(value)
	    
	# Convert biogrid ids in the list to specified identifier type and return the resulting list.
	return [db.biogrid2value(bid, identifier_type) for bid in bids]
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
print db.value2biogrid("YCR011C")
# ['30990']

print db.biogrid2values("30990")
#[(u'850369', u'ENTREZ_GENE'), (u'ETG850369', u'ENTREZ_GENE_ETG'), (u'YCR011C', u'SYSTEMATIC_NAME'), (u'ADP1', u'OFFICIAL_SYMBOL'), (u'51013563', u'GENBANK_PROTEIN_GI'), (u'51013562', u'GENBANK_GENOMIC_DNA_GI'), (u'14588926', u'GENBANK_PROTEIN_GI'), (u'85666111', u'GENBANK_GENOMIC_DNA_GI'), (u'X59720', u'GENBANK_GENOMIC_DNA_ACCESSION'), (u'AAT93075', u'GENBANK_PROTEIN_ACCESSION'), (u'P25371', u'GENBANK_PROTEIN_ACCESSION'), (u'14588895', u'GENBANK_GENOMIC_DNA_GI'), (u'AY693056', u'GENBANK_GENOMIC_DNA_ACCESSION'), (u'10383780', u'GENBANK_PROTEIN_GI'), (u'45644964', u'GENBANK_PROTEIN_GI'), (u'CAA42328', u'GENBANK_PROTEIN_ACCESSION'), (u'NP_009937', u'REFSEQ_PROTEIN_ACCESSION'), (u'NC_001135', u'REFSEQ_GENOMIC_DNA_ACCESSION'), (u'P25371', u'SWISSPROT'), (u'S000000604', u'SGD'), (u'649', u'GRID_LEGACY'), (u'30990', u'BIOGRID')]

print db.biogrid2value("30990", "SYSTEMATIC_NAME")
# YCR011C
"""

db = GeneDB()
print db.checkUniqueValues()