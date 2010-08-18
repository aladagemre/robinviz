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
	
    def value2biogrid(self, value):
	"""Converts a given value to biogrid id."""
	self.cursor.execute("SELECT biogrid_id FROM translation WHERE identifier_value='%s';" % value )
	result = self.cursor.fetchall()
	
	if not result:
	    return None
	    
	assert len(result) == 1, "More than one gene with given value."
	return result[0][0]
    
    def biogrid2values(self, biogrid_id):
	"""Converts a given biogrid_id to the other identifier values.
	Result is a tuple in the format:
	(  (value1, type1), (value2, type2), ... )
	"""
	self.cursor.execute("SELECT identifier_value, identifier_type FROM translation WHERE biogrid_id='%s';" % biogrid_id )
	result = self.cursor.fetchall()
	return str(result)
	
    def biogrid2value(self, biogrid_id, identifier_type):
	"""Converts a given biogrid_id to the specified identifier type's value.
	Result is a single string like: ETG814974
	"""
	self.cursor.execute("SELECT identifier_value FROM translation WHERE biogrid_id='%s' and identifier_type='%s';" % (biogrid_id, identifier_type)  )
	result = self.cursor.fetchone()
	if not result:
	    return None
	    
	return str(result[0])
	
db = GeneDB()

# Some example runs and their outputs:

print db.value2biogrid("E84456")
#375

print db.biogrid2values("375")
"""
[(u'814974', u'ENTREZ_GENE'), (u'ETG814974', u'ENTREZ_GENE_ETG'), (u'T23O15.3', u'SYNONYM'), (u'At2g04350', u'OFFICIAL_SYMBOL'), (u'T23O15_3', u'SYNONYM'), (u'25293398', u'GENBANK_PROTEIN_GI'), (u'145361545', u'GENBANK_DNA_GI'), (u'145359896', u'GENBANK_DNA_GI'), (u'30678112', u'GENBANK_PROTEIN_GI'), (u'AAN72299', u'GENBANK_PROTEIN_ACCESSION'), (u'20198205', u'GENBANK_PROTEIN_GI'), (u'AAM28875', u'GENBANK_PROTEIN_ACCESSION'), (u'BT002288', u'GENBANK_GENOMIC_DNA_ACCESSION'), (u'20805877', u'GENBANK_PROTEIN_GI'), (u'4689469', u'GENBANK_PROTEIN_GI'), (u'AAD25843', u'GENBANK_PROTEIN_ACCESSION'), (u'4587615', u'GENBANK_PROTEIN_GI'), (u'25090426', u'GENBANK_GENOMIC_DNA_GI'), (u'E84456', u'GENBANK_PROTEIN_ACCESSION'), (u'AF503758', u'GENBANK_GENOMIC_DNA_ACCESSION'), (u'20805876', u'GENBANK_GENOMIC_DNA_GI'), (u'AAM15458', u'GENBANK_PROTEIN_ACCESSION'), (u'25090427', u'GENBANK_PROTEIN_GI'), (u'AAD27905', u'GENBANK_PROTEIN_ACCESSION'), (u'15228136', u'GENBANK_PROTEIN_GI'), (u'NP_178516', u'REFSEQ_PROTEIN_ACCESSION'), (u'NM_179603', u'REFSEQ_DNA_ACCESSION'), (u'NM_126468', u'REFSEQ_DNA_ACCESSION'), (u'NP_849934', u'REFSEQ_PROTEIN_ACCESSION'), (u'Q9SJD4', u'TREMBL'), (u'AT2G04350', u'TAIR'), (u'375', u'BIOGRID')]
"""

print db.biogrid2value("375", "ENTREZ_GENE_ETG")
# ETG814974