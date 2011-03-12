# -*- coding: utf-8 -*-
import os
import shutil
import sys
sys.path.append("src/python/robinviz")
from utils.info import increment_version

VERSION = raw_input("Enter the version(leave empty for a new version): ") or increment_version()
USER = os.getenv('USER')
RELEASE_DIR = "/home/%s/Desktop/release" % USER
SOURCE_DIR = "%s/robinviz-%s-source" % (RELEASE_DIR, VERSION)
BINARY_DIR =  "%s/robinviz-%s-linux-binary" % (RELEASE_DIR, VERSION)
TRUNK_DIR = "/home/%s/robinviz/trunk" % USER

print "New version is", VERSION
f = open("version.txt","w")
f.write(VERSION)
f.close()

if not os.path.exists(RELEASE_DIR):
    os.mkdir(RELEASE_DIR)
    
# 1) Export svn dir as source dir and clean it
if not os.path.exists(SOURCE_DIR):
    print "Source dir does not exist, checking it out from svn repo"
    os.system("svn export https://robinviz.googlecode.com/svn/trunk %s" % SOURCE_DIR)

# ========= CLEAN UNNECESSARY FILES============
unnecessary_dirs = """src/python/robinviz/old
src/python/robinviz/utils/analyse
src/python/robinviz/utils/playground
src/python/robinviz/utils/scripts
release.py"""

for path in unnecessary_dirs.split("\n"):
    command ="rm -rf %s" % os.path.join(SOURCE_DIR, path)
    #print command
    try:
	os.system(command)
    except Exception,e:
        print "Did not work:", e
	print command
	
shutil.copy("version.txt", SOURCE_DIR) # copy the new version file to the source dir.
#os.mkdir("%s/preconfigurations" % RELEASE_DIR)
os.system("mv %s/sources/preconfigurations %s/preconfigurations" % (SOURCE_DIR, RELEASE_DIR))
os.mkdir("%s/sources/preconfigurations" % SOURCE_DIR)
# =================================================

# 2) Targz source dir
os.chdir(RELEASE_DIR)
sdir = SOURCE_DIR.split("/")[-1]
os.system("tar czf %s.tar.gz %s" % (sdir, sdir))
os.chdir(TRUNK_DIR)

# 3) Rename source dir as binary    
os.system("mv %s %s" % (SOURCE_DIR, BINARY_DIR))
os.system("mv %s/preconfigurations %s/sources" % ( RELEASE_DIR, BINARY_DIR) )
print "Step 3 done..."
# 4) make
os.chdir(BINARY_DIR)
os.system("make all")
os.chdir(TRUNK_DIR)

print "Step 4 done..."
# 5) Copy the following files:
os.rmdir( os.path.join(BINARY_DIR, "bin"))
os.rmdir(os.path.join(BINARY_DIR, "src/python/robinviz/databases/inputtrees/ppidata/hitpredict"))
copy_paths = """bin
src/python/robinviz/databases/inputtrees/ppidata/hitpredict"""

for path in copy_paths.split("\n"):
    command = "cp -rf %s %s" % ( path, os.path.join(BINARY_DIR, path) ) 
    #print command
    try:
	os.system(command)
    
    except Exception,e:
	print "Did not work:", e
	print command

print "Step 5 done..."
# 6) Delete the following files for binary release
delete_paths = """src/python/robinviz/nbproject
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
	
print "Step 6 done..."
# 7) Bundle the binary directory as installer
os.system("/home/%s/.bin/installjammer/installjammer --build /home/%s/robinviz/InstallJammerProjects/Robinviz/Robinviz.mpi" % (USER , USER) )
print "Step 7 done..."