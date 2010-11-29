# -*- coding: utf-8 -*-
import os
# This is a disasterous hack!
# Should find a consistent solution like Environment variables
result = os.path.abspath(__file__).split("src")
root = result[0][:-1]
rp = lambda x: os.path.normpath("%s/%s" % (root, x)) # root path
database_root = os.path.normpath( os.path.join(root, "src/python/robinviz/databases") )
tree_root = os.path.normpath( os.path.join(database_root, "inputtrees") )
ap = lambda x: os.path.normpath( os.path.join(tree_root, x) )
id2cat = lambda id: open( ap("assocdata/input_go.txt") ).readlines()[id].strip()

def runcommand(command):
    if os.name == "posix":
        command = "./" + command

    print "Running", command
    return os.system(command)
#import ConfigParser
"""config = ConfigParser.ConfigParser()
config.readfp(open(root+'/config.txt'))"""
#print config.get("Paths","databases")


if __name__ == "__main__":
    print rp("layouts.ini")