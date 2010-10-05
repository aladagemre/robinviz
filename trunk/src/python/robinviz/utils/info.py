# -*- coding: utf-8 -*-
import os
# This is a disasterous hack!
# Should find a consistent solution like Environment variables
result = os.path.abspath(__file__).split("src")
root = result[0][:-1]
database_root = os.path.normpath( os.path.join(root, "src/python/robinviz/databases") )
tree_root = os.path.join(database_root, "inputtrees")
ap = lambda x: os.path.normpath( os.path.join(tree_root, x) )

#import ConfigParser
"""config = ConfigParser.ConfigParser()
config.readfp(open(root+'/config.txt'))"""
#print config.get("Paths","databases")