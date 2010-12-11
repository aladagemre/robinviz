# -*- coding: utf-8 -*-
import os
import shutil

VERSION = raw_input("Enter the version(i.e: 1.0-beta): ") or "1.0-beta"
USER = os.getenv('USER')
RELEASE_DIR = "/home/%s/Desktop/release" % USER
SOURCE_DIR = "%s/robinviz-%s-linux-source" % (RELEASE_DIR, VERSION)
BINARY_DIR =  "%s/robinviz-%s-linux-binary" % (RELEASE_DIR, VERSION)
TRUNK_DIR = "/home/%s/robinviz/trunk" % USER

if not os.path.exists(RELEASE_DIR):
    os.mkdir(RELEASE_DIR)
    
# 1) Export svn dir as source dir.
if not os.path.exists(SOURCE_DIR):
    print "Source dir does not exist, checking it out from svn repo"
    os.system("svn export https://robinviz.googlecode.com/svn/trunk %s" % SOURCE_DIR)

# 2) Targz source dir
os.chdir(RELEASE_DIR)
sdir = SOURCE_DIR.split("/")[-1]
os.system("tar czf %s.tar.gz %s" % (sdir, sdir))
os.chdir(TRUNK_DIR)

# 3) Rename source dir as binary    
os.system("mv %s %s" % (SOURCE_DIR, BINARY_DIR))

# 4) make
os.chdir(BINARY_DIR)
os.system("make all")
os.chdir(TRUNK_DIR)

# 5) Copy the following files:
os.rmdir( os.path.join(BINARY_DIR, "bin"))
os.rmdir(os.path.join(BINARY_DIR, "src/python/robinviz/databases/inputtrees/ppidata/hitpredict"))
copy_paths = """bin
src/python/robinviz/databases/inputtrees/ppidata/hitpredict
src/python/robinviz/databases/inputtrees/ppidata/BIOGRID-OSPREY_DATASETS-3.0.68.osprey
src/python/robinviz/databases/inputtrees/godata/goinfo.sqlite3
src/python/robinviz/databases/inputtrees/assocdata/gene_association.goa_human
src/python/robinviz/databases/inputtrees/assocdata/gene_association.goa_human-BIOGRID
src/python/robinviz/databases/inputtrees/assocdata/gene_association.sgd
src/python/robinviz/databases/inputtrees/assocdata/gene_association.sgd-BIOGRID
src/python/robinviz/databases/inputtrees/assocdata/gene_association.tair
src/python/robinviz/databases/inputtrees/assocdata/gene_association.tair-BIOGRID
src/python/robinviz/databases/inputtrees/assocdata/gene_association.mgi
src/python/robinviz/databases/inputtrees/assocdata/gene_association.mgi-BIOGRID"""
#src/python/robinviz/databases/identifier.db


for path in copy_paths.split("\n"):
    command = "cp -rf %s %s" % ( path, os.path.join(BINARY_DIR, path) ) 
    #print command
    try:
	os.system(command)
    
    except Exception,e:
	print "Did not work:", e
	print command
    
# 6) Delete the following files:

delete_paths = """Makefile
compile.sh
src/python/robinviz/nbproject
src/python/robinviz/old
src/python/robinviz/utils/analyse
src/python/robinviz/utils/playground
src/python/robinviz/utils/scripts
bin/.svn
src/python/robinviz/databases/inputtrees/ppidata/hitpredict/.svn"""

for path in delete_paths.split("\n"):
    command ="rm -rf %s" % os.path.join(BINARY_DIR, path)
    #print command
    try:
	os.system(command)
    except Exception,e:
	print "Did not work:", e
	print command
	
	
# 7) Bundle the binary directory as installer
os.system("/home/%s/bin/installjammer/installjammer --build /home/%s/InstallJammerProjects/Robinviz/Robinviz.mpi" % (USER , USER) )