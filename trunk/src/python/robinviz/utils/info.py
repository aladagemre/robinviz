# -*- coding: utf-8 -*-
import os
# This is a disasterous hack!
# Should find a consistent solution like Environment variables
result = os.path.abspath(__file__).split("src")
root = result[0][:-1]
rp = lambda x: os.path.normpath("%s/%s" % (root, x)) # root path
database_root = os.path.normpath( os.path.join(root, "src/python/robinviz/databases") )
pp = lambda x: os.path.normpath( os.path.join(root, "src/python/robinviz/", x) )
dp = lambda x: os.path.normpath( os.path.join(database_root, x) )
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


def latest_osprey_dir():
    dir_prefix = "BIOGRID-OSPREY_DATASETS"
    dirs = filter(lambda filename: filename.startswith(dir_prefix), os.listdir(ap("ppidata")) )
    if not dirs:
        return None
    
    latest = dirs[0]
    for directory in dirs:
        major, mid, minor = map(int, latest.split("-")[-1].split(".")[:-1])
        dmajor, dmid, dminor = map(int, directory.split("-")[-1].split(".")[:-1])

        if dmajor > major:
            latest = directory
        elif dmid > mid:
            latest = directory
        elif dminor > minor:
            latest = directory

    return latest

def get_current_version():
    with open(rp("version.txt")) as f:
        return f.readline().strip()

def calculate_next_version(current=None):
    """doctests
    >>> calculate_next_version("1.0.0")
    '1.0.1'
    >>> calculate_next_version("1.0.9")
    '1.1.0'
    >>> calculate_next_version("1.9.9")
    '2.0.0'
    >>> calculate_next_version()
    '1.0.1'
    """
    if not current:
        current = get_current_version()
    
    major, mid, minor = map(int, current.split("."))
    if minor < 9:
        minor += 1
    elif mid < 9:
        mid += 1
        minor = 0
    else:
        major += 1
        mid = 0
        minor = 0
        
    return ".".join(map(str, (major, mid, minor)))

def increment_version():
    new_version = calculate_next_version()
    with open(rp("version.txt"), "w") as f:
        f.write(new_version)
    return new_version

if __name__ == "__main__":
    print rp("layouts.ini")
    runthread("ls")
    import doctest
    doctest.testmod()


