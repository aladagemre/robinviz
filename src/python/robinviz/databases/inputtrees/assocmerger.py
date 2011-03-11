import os
import sys
import sqlite3

if not "utils" in sys.path:
    sys.path.append("../..")

from databases.genequery import GeneDB
from databases.translator import AssociationTranslator
from utils.info import ap, rp
from utils.compression import download_file_to, ungz

class AssociationMerger:
    def __init__(self):
        f = open(ap("assocdata/selected_assoc.txt"))
        self.urls = f.read().split("\n")
        print "init run"

    def downloadCheckedAssociation(self):
        print "Downloading..."
	uncompressed_files = []
	for url in self.urls:
	    # Find the uncompressed filename
	    filename = url.split("/")[-1]
	    filename = filename[:filename.find(".gz")]
	    # get the absolute path of this file.
	    local_path = ap("assocdata/%s" % filename)
	    # add this file to uncompressed_files so that we'll return all of them
	    uncompressed_files.append(local_path)
	    # if this file doesn't exist yet, we have to download it.
	    if not os.path.exists( local_path ):
		print local_path, "does not exist, will download it."
		remote_gz = url
		local_gz = "%s.gz" % local_path
		download_file_to(remote_gz, local_gz, binary=True)
		ungz(local_gz)
		# now we have local_path!
	    else:
		print local_path, "exists, skipping."

	return uncompressed_files

    def merge(self):
        files = self.downloadCheckedAssociation()
        print "Merging association sources..."
	o = open(ap("assocdata/go.txt"),"w")
	self.go_dict = go_dict = {}
	TARGET_ANNOTATION= "OFFICIAL_SYMBOL"
	self.db = GeneDB()
	translator = AssociationTranslator()

	# ===============================================
	for filename in files:
            target_file = ap("%s-%s" % (filename, TARGET_ANNOTATION))
            if os.path.exists( target_file ):
                print target_file,"exists, shall not re-generate it."
                newbie = self._get_dictionary_from_file(target_file)
                self._extend_dictionary( go_dict, newbie )
            else:
                translator.set_filename(filename)
                """if translator.translate(None, TARGET_ANNOTATION):
                    self._extend_dictionary( go_dict, translator.go_dict )"""
                if translator.translate(from_type=None, to_type="OFFICIAL_SYMBOL"):
                    self._extend_dictionary( go_dict, translator.go_dict )
     	# ===============================================

        # ============= REMOVE DUPLICATE GENES  ==============
        for key in go_dict.keys():
            l = go_dict[key]
            go_dict[key] = set(l)
        # =====================================================
        conn = sqlite3.connect(ap("godata/goinfo.sqlite3"))
        cursor = conn.cursor()

        def get_name_of_term(i):
            cursor.execute("SELECT name FROM terms WHERE id=?", (str(i),))
            name = cursor.fetchone()
            if not name:
                #print "Name for term %s could not be found." % i
                return "NULL"
            else:
                return name[0]

	for term in sorted(go_dict.keys()):
#	    try:
            genes = list(set(go_dict[term]))
            if genes:
                genes_str = "\t".join( map(str, genes) )
            else:
                genes_str = "NULL"

            #o.write("%s\t%s\n" % ( term,  genes_str) )
            o.write("%s\t%s\t%s\n" % ( term, get_name_of_term(int(term.split(":")[-1])), genes_str ) )
	    #except:
#		raise Exception, "problem here. term: %s; genes= %s" % (term, go_dict[term])
	o.close()


        self.filterSelected()


    def _extend_dictionary(self, pool, newbie):
        """Extends the pool dictionary with the newbie's contents.
        Both dictionaries have list records mapped to string keys.
        """
        for key in newbie.keys():
            l = pool.get(key)
            if not l:
                l = []
            l.extend(newbie[key])
            pool[key] = l

        #return pool

    def _get_dictionary_from_file(self, filename):
        newbie = {}
        f = open(filename)
        for line in f:
            cols = line.strip().split("\t")
            if cols[1] == "NULL":
                continue
            newbie[cols[0]] = cols[1:]
        return newbie

    def filterSelected(self):
        print "Filtering selected GO Terms on the association file"
        flag = open(rp("outputs/resultparams.txt")).read().strip().split("-")[-1]

        top_level_file = ap("godata/toplevel_%s.txt" % flag)

        topLevels = open(top_level_file).read().split("\n")
        selected_terms = sorted(map(lambda x:x.strip(), open(ap("godata/selected_terms.txt")).readlines()))
        united_terms = list(set(sorted(topLevels + selected_terms)))

        output = open(ap("assocdata/go_slim.txt"),"w")
        catnames = open(ap("assocdata/input_go.txt"),"w")
        catcodes = open(ap("assocdata/category_codes.txt"), "w")
        # TODO: category_codes already exists as selected_terms.txt. Why using this?!
        conn = sqlite3.connect(ap("godata/goinfo.sqlite3"))
        cursor = conn.cursor()

        def get_name_of_term(i):
            cursor.execute("SELECT name FROM terms WHERE id=?", (str(i),))
            name = cursor.fetchone()

            if name:
                return name[0]
            else:
                #print "Name for term %s could not be found." % i
                return "NULL"

        name_dict = {}
        for term in united_terms:
            name_dict[term] = get_name_of_term(int(term.split(":")[-1]))

        catnames.write( "\n".join([ name_dict[term] for term in selected_terms]) )
        catcodes.write( "\n".join([ term for term in selected_terms]) )

        for term in united_terms:
            if term in self.go_dict:
                genes = self.go_dict[term]
                if not genes:
                    genes = ["NULL"]
                output.write("%s\t%s\t%s\n" % ( term, name_dict[term], "\t".join( genes ) ) )
            else:
                #print term, "was not found in go dict"
                pass
        output.close()
        catnames.close()

if __name__ == "__main__":
    #files = sys.argv[1:]
    #files = ["C:\\RobinvizDevel\\src\\python\\robinviz\\databases\\inputtrees\\assocdata\\gene_association.GeneDB_Lmajor", "C:\\RobinvizDevel\\src\\python\\robinviz\\databases\\inputtrees\\assocdata\\gene_association.PAMGO_Ddadantii"]
    #files = map( lambda fn: fn.replace("\\", "/"), files)
    merger = AssociationMerger()
    merger.merge()