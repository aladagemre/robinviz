# -*- coding: utf-8 -*-
"""
Generates a sqlite database from the BIOGRID-IDENTIFIERS.tab.txt file.
http://thebiogrid.org/downloads/archives/Release%20Archive/BIOGRID-3.0.67/BIOGRID-IDENTIFIERS-3.0.67.tab.zip
"""
import sqlite3

conn = sqlite3.connect("identifier2.db")
curs = conn.cursor()
curs.execute("DROP TABLE IF EXISTS translation;")
curs.execute("""
CREATE TABLE "translation" (
    "biogrid_id" INTEGER NOT NULL,
    "identifier_value" TEXT NOT NULL,
    "identifier_type" TEXT NOT NULL
)


""")
input_stream = open("BIOGRID-IDENTIFIERS-3.0.67.tab.txt")
#output_stream = open("sqlstatements.txt","w")

for line in input_stream: 
    try:
	l = line.strip().split("\t")
	#output_stream.write("INSERT INTO translation VALUES (%d, '%s', '%s');\n" % (int(l[0]), l[1], l[2]) )
	curs.execute("INSERT INTO translation VALUES (%d, '%s', '%s');\n" % (int(l[0]), l[1], l[2]) )
	
    except:
	continue
conn.commit()

#output_stream.close()
    