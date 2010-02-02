"""
 swig -python -llibc.i -lgml_scanner.i gml_parser.i 
 gcc -c gml_scanner.c gml_parser.c gml_parser_wrap.c -I/usr/include/python2.6
 ld -shared gml_scanner.o gml_parser.o gml_parser_wrap.o -o _gml_parser.so

"""
from gml_parser import *
from pygml import BaseClass
from pygml import Edge
from pygml import EdgeAnchor
from pygml import Graph
from pygml import Graphics
from pygml import LabelGraphics
from pygml import Line
from pygml import Node
from pygml import Point


class GMLParser:
    def __init__(self, filename, graph):
        stat = GML_stat()
        stat.key_list = None
        import os
        if not os.path.exists(filename):
            print "File %s does not exist." % filename
            raise IOError

        
        file = fopen(filename, "r")
        GML_init()

        list = GML_parser(file, stat, 0)
        #self.prlist(list, 0)
        
        #self.g = Graph()
        self.g = graph
        self.obj = self.g
        
        self.parse(list, 0)
        #self.g = self.obj

    def parse(self, list, level):
        tmp = list
        
        class_dict = {'graph': Graph, 'graphics': Graphics, 'LabelGraphics': LabelGraphics, 'Line': Line, 'point': Point, 'edgeAnchor': EdgeAnchor}
        def get_value(o):
            if o.kind == GML_INT:
                return o.value.integer
            elif o.kind == GML_DOUBLE:
                return o.value.floating
            elif o.kind == GML_STRING:
                return o.value.string
            else:
                return None

        while tmp:
            if level == 0:
                value = get_value(tmp)
                if value != None:
                    self.__dict__[tmp.key] = value
                else:
                    self.parse(tmp.value.list, 1)
            if level == 1:
                if tmp.key == "node":
                    old_obj = self.obj
                    self.obj = Node()
                    self.g.nodes.append(self.obj)
                    self.parse(tmp.value.list, 2)
                    self.obj = old_obj
                elif tmp.key == "edge":
                    old_obj = self.obj
                    self.obj = Edge()
                    self.g.edges.append(self.obj)
                    self.parse(tmp.value.list, 2)
                    self.obj = old_obj
                else:
                    value = get_value(tmp)
                    if value != None:
                        self.g.__dict__[tmp.key] = value
                    else:
                        print tmp.key, value
                        print "Unknown type! Not edge or node."
                
            elif level >= 2:
                value = get_value(tmp)
                if value != None:
                    # e.g. source 10
                    self.obj.__dict__[tmp.key] = value
                else:               
                    # e.g. graphics [
                    old_obj = self.obj
                    if tmp.key == "Line":
                        self.obj = []
                    else:
                        self.obj = class_dict[tmp.key]()
						
                    self.parse(tmp.value.list, level + 1)
                    if tmp.key == "point":
                        old_obj.append(self.obj)
                    else:
                        old_obj.__dict__[tmp.key] = self.obj
                    self.obj = old_obj
            

            tmp = tmp.next

    def prlist(self, list, level):
        tmp = list

        while tmp:
            for i in range(0, level):
                print "|",

            print "*KEY*: %s" % tmp.key

            if tmp.kind == GML_INT:
                print "  *VALUE* (long) : %ld" % tmp.value.integer
            elif tmp.kind == GML_DOUBLE:
                print "  *VALUE* (double) : %f" % tmp.value.floating
            elif tmp.kind == GML_STRING:
                print "  *VALUE* (string) : %s" % tmp.value.string
            elif tmp.kind == GML_LIST:
                print "  *VALUE* (list) :"
                self.prlist(tmp.value.list, level+1)

            tmp = tmp.next

#parser = GMLParser("graph.gml")

