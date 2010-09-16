from pygml import Graph
from os.path import exists

graphs_dir = "outputs/graphs/"
maingraph = graphs_dir + "maingraph.gml"
g = Graph()
g.read_gml(maingraph)
g.prepare()

for node in g.nodes:
    if not exists(graphs_dir + "graph%d.gml" % node.id):
        print "%d does not exist, deleting..." % node.id
        g.delete_node(node)


g.write_gml(maingraph)