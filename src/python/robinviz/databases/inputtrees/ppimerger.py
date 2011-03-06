# -*- coding: utf-8 -*-
import os.path

from PyQt4.QtGui import *
from PyQt4.QtCore import *
import sys
import os

if not "utils" in sys.path:
    sys.path.append("../..")

from utils.info import ap
from utils.compression import *
from databases.translator import BiogridOspreyTranslator
from ppi_downloader import download_organism



class PPIMerger:
    def __init__(self, files):
        self.files = files

    def merge(self):
        self.mergePPIFiles(self.files)

    def convert_path_to_organism_name(self, path):
	if os.name == "posix":
	    seperator = "/"
	else:
	    seperator = "\\"
	# TODO: could be better

	cols =  path.split(seperator)
        return cols[-2]

    def findSelectedOrganisms(self, checkedPaths):
	organisms = set()
	for path in checkedPaths:
	    organismName = self.convert_path_to_organism_name(path)
	    organisms.add(organismName)

	return list(organisms)

    def mergePPIFiles(self, files):
	"""Merges given PPI files."""
        print "PPI Merging starts..."
	confidence_dicts = {}
	organisms = self.findSelectedOrganisms(files)
	# map(download_organism, organisms) # Download HitPredict data and generate (p1 p2 confidence) file.
        #TODO: Check if hitpredict data in OS Format exists.
        # TODO: find the max and min values of the files.
        interactions = {}

	for organism in organisms:
	    """For each organism, create a dictionary holding (p1,p2)=>confidence
	    and store them in confidence_dicts"""
            print "Organism: %s" % organism
	    d = {} # to store
            #interactions = {} # dunno why I put it here. it causes a logical bug!

	    for line in open(ap("ppidata/hitpredict/%s.txt" % organism )):
		cols = line.strip().split("\t")
                #print cols
                # TODO: make a normalization here. remove old normalization
                interaction = cols[0], cols[1]
                confidence = cols[2]
		d[interaction] = confidence
                # get the confidence list for the interaction
		i = interactions.get( interaction )
		if not i:
                    # if no confidence recorded yet, create an empty list
		    i = []
                i.append( float(confidence) )
                interactions[interaction] = i

	    confidence_dicts[organism] = d


	for filename in files:
            bfilename = "%s-BIOGRID" % filename
            if not os.path.exists(bfilename):
                b = BiogridOspreyTranslator(filename)
                b.translate()
            filename = bfilename

	    organism = self.convert_path_to_organism_name(filename)
	    for line in open(filename):
		cols = line.strip().split("\t")
		if cols[0] == cols[1]:
                    # ignore self-interactions
		    continue

                # Find the interaction and its confidence if exists.
		interaction = cols[0], cols[1]
		confidence = confidence_dicts[organism].get(interaction)
		if not confidence:
                    # if no confidence, assign 0.1 (biogrid interaction without confidence)
                    # and add this confidence to the list as it hasn't been added
                    # in the hitpredict part above.
		    confidence = 0.1

                    # get the confidence list for the interaction
                    i = interactions.get( interaction )
                    if i:
                        # if some confidence value has been assigned,
                        # no need to add confidence 0.1 to the list
                        # as there are already more reliable confidence
                        # scores.
                        continue

                    # ELSE: (if no confidence values recorded in the hitpredict part,
                    # if no confidence recorded yet, create an empty list
                    # put the new confidence and save the new list.
                    i = [ float(confidence) ]
                    interactions[ interaction ] = i

	# Now we have an interactions dict, containing (p1,p2) => [conf1, conf2, ...]
	# I'll take average of them. But I don't know if this is a good idea.
	single_ppi_file = open(ap("ppidata/ppi.txt"), "w")

	for key in interactions.keys():
	    p1, p2 = key
	    confidence_values = interactions[key]
	    confidence = sum(confidence_values) / len(confidence_values)
	    single_ppi_file.write("\t\t".join( (p1, p2, str(confidence)) ) + "\n")
	single_ppi_file.close()
        print "PPI Merging finished."

if __name__ == "__main__":
    files = sys.argv[1:]
    merger = PPIMerger(files)
    merger.merge()