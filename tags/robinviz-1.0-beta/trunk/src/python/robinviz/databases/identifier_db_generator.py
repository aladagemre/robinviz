# -*- coding: utf-8 -*-
"""
Generates a sqlite database from the BIOGRID-IDENTIFIERS.tab.txt file.
http://thebiogrid.org/downloads/archives/Release%20Archive/BIOGRID-3.0.67/BIOGRID-IDENTIFIERS-3.0.67.tab.zip
"""
import sqlite3
import urllib
import os
import sys
from PyQt4 import QtGui


sys.path.append("..")
sys.path.append("../..")
from utils.downloader import Downloader
from utils.compression import unzip

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


   

def start():
    if not os.path.exists(filename):
        print "zip file downloaded. Now extracting..."
        unzip(zipname)

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
    curs.execute('CREATE INDEX "identifier_index" on translation (identifier_value ASC)')

    # identifier_value is not UNIQUE!
    # INSERT INTO translation VALUES (1, '26556996', 'GENBANK_GENOMIC_DNA_GI');
    # INSERT INTO translation VALUES (2, '26556996', 'GENBANK_GENOMIC_DNA_GI');

    input_stream = open(filename)
    #output_stream = open("sqlstatements.txt","w")

    for line in input_stream:
        try:
            l = line.strip().split("\t")
            biogrid_id = int(l[0])
        except:
            continue
        else:
            #output_stream.write("INSERT INTO translation VALUES (%d, '%s', '%s');\n" % (int(l[0]), l[1], l[2]) )
            statement = 'INSERT INTO translation VALUES (%d, "%s", "%s");\n' % (biogrid_id, l[1], l[2])
            curs.execute(statement)

    conn.commit()

    #output_stream.close()
    print "SQL operation complete"

if __name__ == "__main__":
    if not os.path.exists(filename):
        print "txt file not found, zip file downloading..."
        app = QtGui.QApplication(sys.argv)
        d = Downloader(fileurl, zipname)
        d.finished.connect(start)
        sys.exit(app.exec_())
    else:
        start()