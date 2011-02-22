from utils.info import rp,ap
from re import compile
from os import listdir
def find_top_highlevel_categories():
    # Take the original graph files (not other layouts)
    pattern = compile("^graph\d+.gml$")
    fileList = listdir(rp('outputs/graphs'))
    graphFiles = filter(pattern.match, fileList)
    
    # fetch the labels in each file.
    pattern = compile('label "[\w-]+_?[A-Z:]*"')
    result = []
    for graphFile in graphFiles: # scan all gml files.
        content = open(rp ('outputs/graphs/'+graphFile)).read() # read the gml file
        labels = pattern.findall(content) # find all label lines.
        # Clear label " and " characters and get the gene name
        tags = map(lambda line: line[7:-1].split("_")[1].split(":")[:-1], labels)
        subresult = reduce(lambda x,y: x+y, tags)
        #print tags
        result += subresult

    keys = list( set(result) )
    frequency_list = sorted([(result.count(key), key) for key in keys])
    frequency_list.reverse()
    top10=",".join( map(lambda pair: pair[1], frequency_list[:10]) )
    f = open(ap("godata/top_highlevel_letters.txt"), "w")
    f.write(top10)
    f.close()
    
if __name__ == "__main__":
    find_top_highlevel_categories()
