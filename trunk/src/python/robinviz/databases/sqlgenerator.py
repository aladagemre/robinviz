# -*- coding: utf-8 -*-
"""
Generates a sqlite database from the BIOGRID-IDENTIFIERS.tab.txt file.
http://thebiogrid.org/downloads/archives/Release%20Archive/BIOGRID-3.0.67/BIOGRID-IDENTIFIERS-3.0.67.tab.zip
"""
import sqlite3
import urllib
import os
import zipfile


# ==================================
# ============ METHODS =============
def download_file(url):
    webFile = urllib.urlopen(url)
    localFile = open(url.split('/')[-1], 'w')
    localFile.write(webFile.read())
    webFile.close()
    localFile.close()

# ===================================
# ============ PARAMETERS ===========
version = "3.0.67"
zipname = "BIOGRID-IDENTIFIERS-%s.tab.zip" % version
filename = "BIOGRID-IDENTIFIERS-%s.tab.txt" % version
fileurl = "http://thebiogrid.org/downloads/archives/Release Archive/BIOGRID-%s/%s" % (version, zipname)


# =======================================
# ============ INITIALIZATION ===========
if not os.path.exists(filename):
    print "txt file not found, zip file downloading..."
    download_file(fileurl)
    print "zip file downloaded. Now extracting..."
    zf = zipfile.ZipFile(zipname)
    zf.extractall()
    print "Extracted."

print "SQL conversion starts..."
conn = sqlite3.connect("identifier.db")
curs = conn.cursor()
curs.execute("DROP TABLE IF EXISTS translation;")
curs.execute("""
CREATE TABLE "translation" (
    "biogrid_id" INTEGER NOT NULL,
    "identifier_value" TEXT NOT NULL,
    "identifier_type" TEXT NOT NULL
)
""")

curs.execute('CREATE INDEX "biogrid_index" on translation (biogrid_id ASC);')
curs.execute('CREATE UNIQUE INDEX "identifier_index" on translation (identifier_value ASC)')

input_stream = open(filename)
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
    