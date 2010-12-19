# -*- coding: utf-8 -*-
import os
import sys
import shutil
import tarfile

def compress_files(tarname, files):
    tar = tarfile.open(str(tarname), "w:gz")
    for filename in files: tar.add(filename)
    tar.close()

def clean():
    shutil.rmtree('temp')
    
def prepare():
    os.system("mkdir -p temp/src/python/robinviz/databases/inputtrees/assocdata")
    os.system("mkdir -p temp/src/python/robinviz/databases/inputtrees/godata")
    os.system("mkdir -p temp/src/python/robinviz/databases/inputtrees/ppidata")
    os.system("mkdir -p temp/src/python/robinviz/databases/inputtrees/geodata")
    os.mkdir('temp/outputs')

    geo_file = open('src/python/robinviz/databases/inputtrees/geodata/selected_geo.txt').read().strip()

    shutil.copy('settings.yaml', 'temp/settings.yaml')
    shutil.copy('outputs/resultparams.txt', 'temp/outputs/resultparams.txt')
    confirmation_type = open("outputs/resultparams.txt").read().strip()

    assocdata_files = ('go_slim.txt', 'selected_assoc.txt', 'input_go.txt', 'category_codes.txt',)
    godata_files = ('selected_terms.txt',)
    ppidata_files = ('ppi.txt', 'selected_ppis.txt',)
    geodata_files = ('selected_geo.txt', geo_file,)

    for filename in assocdata_files:
	source = 'src/python/robinviz/databases/inputtrees/assocdata/%s' % filename
	target = 'temp/%s' % source
	shutil.copy(source, target)
	
    for filename in godata_files:
	source = 'src/python/robinviz/databases/inputtrees/godata/%s' % filename
	target = 'temp/%s' % source
	shutil.copy(source, target)
	
    for filename in ppidata_files:
	source = 'src/python/robinviz/databases/inputtrees/ppidata/%s' % filename
	target = 'temp/%s' % source
	shutil.copy(source, target)

    if confirmation_type.startswith( "co_expression.exe" ):
	for filename in geodata_files:
	    source = 'src/python/robinviz/databases/inputtrees/geodata/%s' % filename
	    target = 'temp/%s' % source
	    shutil.copy(source, target)
    



if __name__ == '__main__':
    if len(sys.argv) > 1:
	prepare()
	os.chdir("temp")
	compress_files( sys.argv[1], ['src', 'outputs','settings.yaml'] )
	shutil.move(sys.argv[1], "../sources/preconfigurations/%s" % sys.argv[1] )
	os.chdir("..")
	clean()
	
    else:
	print "Usage: python preconf-generator.py PreConfigName.tar.gz"