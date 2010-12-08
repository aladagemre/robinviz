'''
Fast k level layout
'''

#from parser import GMLParser
from pygml import Graph, Node, print_list
from math import ceil, floor
from heapq import heappush, heappop
import sys
from os.path import exists

class FastKLevel(object):
    '''
    Fast k level layout
    '''

    def __init__(self, input_file, output_file, m, lop):
        '''
        Constructor
        '''
        
        if not exists(input_file):
            print "File does not exist:", input_file
            sys.exit(-1)
        
        self.g = Graph()
        #gp = GMLParser(input_file, self.g)
        self.g.read_gml(input_file)
        self.g.prepare()
        
        self.output_file = output_file
        self.minimum_distance = float(m)
        self.layersorpositions = lop
        self.min_level_distance = 100
        self.xm = {}
        self.xp = {}    
        self.x = {}    
        self.y = {}
        self.c = {}
        self.L = {}
        
        self.mdb = {}
        self.construct_L()
    
    ######## SERVICE METHODS START ###############
    
    def prepare_old_x(self):
        self.old_x = {}
        for v in self.g.nodes:
            self.old_x[v] = v.graphics.x
            self.x[v] = v.graphics.x
    
    def check_same_x(self):
        print "Stayed same:"
        for v in self.g.nodes:
            if abs(self.x[v] - self.old_x[v]) < 10:
                print v.id, self.old_x[v], self.x[v]
            
    def prepare_output(self):
        for v in self.g.nodes:
            v.graphics.x = float(self.x[v])
        #self.g.remove_dummy_nodes()
        #self.g.check_overlap()
        self.g.write_gml(self.output_file)
        self.write_txt()
            
    def construct_L(self):
        """Constructs L dictionary in advance to speed up.
        TODO: Doubt: virtual v is included in the returned list?
        """
        for v in self.g.real_nodes:
            self.L[v] = [v]
            for e in v.outgoing_edges:
                next_virtual = e.v
                v_list = []
                while next_virtual.virtual:
                     self.L[next_virtual] = v_list
                     v_list.append(next_virtual) 
                     next_virtual = next_virtual.outgoing_edges[0].v
                                  
            
    def vertices_of_class(self, i):
        result = []
        for (v, c) in self.c.iteritems():
            if c == i:
                result.append(v)
        return result
    
    def set_m(self, u, v, dist):
        """Sets a distance to minimum distance database""" 
        self.mdb[(u.id, v.id)] = dist
        
    def m(self, u, v):
        """Returns the minimum distance between two nodes u,v s.t. pos(u) < pos(v) in the same layer. """
        
        if self.mdb.has_key((u.id, v.id)):
            return self.mdb[(u.id, v.id)]
        
        if u == v:
            return 0.0
        
        total = 0.0
        if u.layer == v.layer and u.position < v.position:
            while u!=v:
                total += (u.graphics.w + u.succ.graphics.w)/2 + self.minimum_distance
                u = u.succ
            return total
        else:
#            print "(%d,%d) (%d,%d)" % (u.layer, u.position, v.layer, v.position)
            return None
        
    def virtual_in_layer(self, l):
        """Returns the outermost left virtual node in the given layer if exists. Otherwise None"""
        for v in self.g.layers[l]:
            if v.virtual:
                return v
        return None     
    
    
    def weighted_y_coordinates(self, base_height=100.0):
        """Assigns weighted y coordinates to the nodes according to
        their cumulative edge weights."""
        
        total_sum = 0.0
        layer_sums = {}
        for layer in self.g.layers:
            layer_sum = 0.0
            for node in self.g.layers[layer]:
                edges = node.outgoing_edges
                for edge in edges:
                    if not "parameter" in edge.__dict__:
                        edge.parameter = 2.0
                    layer_sum += float(edge.parameter)
                    
                        
            layer_sums[layer] = layer_sum
            total_sum += layer_sum
            
        last_y = 0
        for layer in self.g.layers:
            layer_sums[layer]/=total_sum
            if layer > 0:
                last_y += base_height*1.5 + base_height * len(self.g.layers)* layer_sums[layer-1]
            for node in self.g.layers[layer]:                
                node.graphics.y = last_y
                
                
                
    def write_positions(self):
        """Writes the positions of each node to a txt file for LEDA to process it."""
        f = open("positions.txt", "w")
        f.write("%d\n" % len(self.g.nodes))
        for layer in self.g.layers:
            this_layer = self.g.layers[layer]
            f.write("%d " % len(this_layer) + " ".join(["%f %f" % (node.graphics.x, -1.0*node.graphics.y) for node in this_layer]) + "\n")
        f.close()
                
                
    def write_layers(self):
        """Writes the layer number of each node to a txt file for LEDA to process it."""
        f = open("layers.txt", "w")
        l = len(self.g.nodes)
        f.write("%d\n" % l)
        f.write(" ".join([str(node.layer) for node in self.g.nodes]))
        f.close()

    def write_txt(self):
        """Decides whether to write layers or positions file according to the command line argument."""
        if self.layersorpositions == "layers":
            self.write_layers()
        elif self.layersorpositions == "positions":
            self.write_positions()
        else:
            print "Unknown txt type: %s. It should be either layers or positions" % self.layersorpositions
       
                     
    ######## SERVICE METHODS ENDS ###############
    
    
    def level_layout(self):
        print "Placing virtual nodes."
        self.place_virtual()
        print "Placing original nodes."
        self.place_original()
        print "Placement completed."
        #self.place_levels()
    
    ######## VIRTUAL NODE PLACEMENT STARTS #############    
    def place_virtual(self):
        self.compute_pos_left()
        del self.c
        self.c = {}
        self.compute_pos_right()
        
        for v in self.g.nodes:
            self.x[v] = v.graphics.x = float(self.xm[v] + self.xp[v])/2
    
    def compute_pos_left(self):
        self.compute_left_classes()
        class_ids = list(set(self.c.values()))
        class_ids.sort()
        #print class_ids
        for i in class_ids:            
            for v in self.vertices_of_class(i):
                if not self.xm.get(v):
                    self.place_left(v)
            self.adjust_left_class(i)
#        print self.xm

    def compute_pos_right(self):
        self.compute_right_classes()
        class_ids = list(set(self.c.values()))
        class_ids.sort()
        
        for i in class_ids:            
            for v in self.vertices_of_class(i)[::-1]:
                if not self.xp.get(v):
                    self.place_right(v)
            self.adjust_right_class(i)
            
            
    def compute_left_classes(self):
        for l in range(len(self.g.layers)):
            cp = l
            for v in self.g.layers[l]:
                if not self.c.get(v):
                    for vp in self.L[v]:
                        self.c[vp] = cp
                else:
                    cp = self.c[v]
                    
    def compute_right_classes(self):
        for l in range(len(self.g.layers)):
            cp = l
            #print self.g.layers[l][::-1]
            for v in self.g.layers[l][::-1]:
            
                if not self.c.get(v):
                    for vp in self.L[v]:
                        #print "Else: c[vp] = %d" % cp
                        self.c[vp] = cp
                else:
                    #print "Else: cp = %d" % self.c[v]
                    cp = self.c[v]

    def place_left(self, v):
        p = float("-inf")
        for vp in self.L[v]:
            if vp.pred and self.c[vp.pred] == self.c[vp]:
                if not self.xm.get(vp.pred):
                    self.place_left(vp.pred)
                p = max(p, self.xm[vp.pred] + self.m(vp.pred, vp))
        if p == float("-inf"): 
            p = 0
        for vp in self.L[v]:
            self.xm[vp] = p
    
    def place_right(self, v):
        p = float("inf")
        for vp in self.L[v]:
            if vp.succ and self.c[vp.succ] == self.c[vp]:
                if not self.xp.get(vp.succ):
                    self.place_right(vp.succ)
                p = min(p, self.xp[vp.succ] - self.m(vp, vp.succ)) # TODO: ???
        if p == float("inf"): 
            p = 0
        for vp in self.L[v]:
            self.xp[vp] = p
            
            
    def adjust_left_class(self, i):
#        print "Adjusting class %d" % i
        d = float("inf")
        for v in self.vertices_of_class(i):
            if v.succ and self.c[v.succ] != i:
                d = min(d, self.xm[v.succ] - self.xm[v] - self.m(v, v.succ))
        if d == float("inf"):
            D = []
            for v in self.vertices_of_class(i):
                for w in v.lower_neighbors():
                    if w.virtual: #TODO: Not sure if this covers sigma bar
                        if self.c[w] < i:
                            heappush(D, self.x[w] - self.x[v]) #TODO: it's not x_ but x in the paper?!
            s = len(D)
            if s == 0:
                d = 0
            else:
                d = D[ceil(s/2) - 1] #TODO: check boundary
        for v in self.vertices_of_class(i):
            self.xm[v] = self.xm[v] + d
            #print "%d - %f" % (v.id, self.xm[v])
    
    
    def adjust_right_class(self, i):
#        print "Adjusting class %d" % i
        d = float("inf")
        for v in self.vertices_of_class(i)[::-1]:
            if v.pred and self.c[v.pred] != i:
                d = min(d,self.xp[v.pred] -  self.xp[v] - self.m(v.pred, v)) # changed order
        if d == float("inf"):
            D = []
            for v in self.vertices_of_class(i)[::-1]:
                for w in v.lower_neighbors()[::-1]:
                    if w.virtual: #TODO: Not sure if this covers sigma bar
                        #print self.c[w], i
                        if self.c[w] > i:
                            #print "push"
                            heappush(D, self.x[w] - self.x[v]) #TODO: it's not x_ but x in the paper?!
            s = len(D)
            if s == 0:
                d = 0
            elif s==1:
                d = D[0]
            elif s==2:
                d = D[1]
            else:
                print D
                print int(ceil(s/2) + 1)
                d = D[int(ceil(s/2) + 1)] #TODO: check boundary + from right to left?
        for v in self.vertices_of_class(i):
            self.xp[v] = self.xp[v] - d       
    
    
    ######## VIRTUAL NODE PLACEMENT ENDS #############
    
    
    
    ######## ORIGINAL NODE PLACEMENT STARTS #############
    def place_original(self):
        self.D = {}
        self.P = {}
        
        for bm in self.g.virtual_nodes:
            bp = bm.succ
            while bp != None and not bp.virtual:
                bp = bp.succ
            if bp != None:
                self.D[bm] = 0
                if self.x[bp] - self.x[bm] == self.m(bm, bp):
                    self.P[bm] = True
                else:
                    self.P[bm] = False
                    
        for d in (1,-1):
            for l in self.g.layers.keys()[::d]:
                #print "%d of %d" % (l, len(self.g.layers.keys())-1)
                bm = self.virtual_in_layer(l)
                if bm:
                    seq = self.g.layers[l][:bm.position] # v1,...,vr
                else:
                    # bm = None  # No need. it's already None
                    seq = self.g.layers[l]
                
                self.place_sequence(None, bm, d, seq) # omitted x.
                for i in range(len(seq) - 1):
                    vi = seq[i]
                    vip1 = seq[i+1] 
                    self.set_m(vi, vip1, self.x[vip1] - self.x[vi])
                if bm != None:
                    if seq:
                        self.set_m(seq[-1], bm, self.x[bm] - self.x[seq[-1]])
                
                while bm != None:
                    bp = bm.succ
                    while bp != None and not bp.virtual:
                        bp = bp.succ
                    if bp == None:
                        #print bm.id
                        seq = self.g.layers[l][bm.position + 1:]
                        if seq:
                            #TODO: Added the if stat above myself!
                            #print_list(self.g.layers[l])
                            self.place_sequence(bm, None, d, seq) # omitted x
                            for i in range(len(seq) - 1):
                                vi = seq[i]
                                vip1 = seq[i+1] 
                                self.set_m(vi, vip1, self.x[vip1] - self.x[vi])
                            
                            self.set_m(bm, seq[0], self.x[seq[0]] - self.x[bm])
                    elif self.D[bm] == d:
                        seq = self.g.layers[l][bm.position:bp.position+1]
                        self.place_sequence(bm, bp, d, seq)
                        self.P[bm] = True
                    bm = bp
                
                self.adjust_directions(l, d) # omitted D and P
                    
    def adjust_directions(self, l, d):
        if l+d >= len(self.g.layers.keys()) or l+d < 0: return # added myself as a boundary check. stay in the layer levels.
        
        vm = None
        for vp in self.g.layers[l+d]:
            if not vp.virtual:
                continue
            wp = vp.neighbor_in_layer(l)
            if wp.virtual:
                if vm != None:
                    # TODO: wm not defined!
                    p = self.P[wm]
                    for w in self.g.layers[l+d][wm.position:wp.position+1]:
                        p = p and self.P.get(w)
                    if p:
                        self.D[vm] = d
                        for v in self.g.layers[l+d][vm.position:vp.position+1]:
                            self.D[v] = d
                vm = vp
                wm = wp
                        
                        
    def place_sequence(self, bm, bp, d, seq):
        r = len(seq)
        if r == 1:
            self.place_single(bm,bp,d,seq[0])
        elif r > 1:
            t = int(floor(float(r) / 2) - 1)
            self.place_sequence(bm, bp, d, seq[:t+1])
            self.place_sequence(bm, bp, d, seq[t+1:])
            self.combine_sequences(bm, bp, d, seq)
    
    def place_single(self, bm, bp, d, v1):
        assert d in (-1,1)
        if d == 1:
            w = filter(lambda x: x.virtual, v1.upper_neighbors())
        elif d == -1:
            w = filter(lambda x: x.virtual, v1.lower_neighbors())
        # TODO: Check w!
        s = len(w)
        if s != 0:
            self.x[v1] = self.x[w[int(ceil(float(s)/2) - 1 )]]
            
            if bm != None:
                self.x[v1] = max(self.x[v1], self.x[bm] + self.m(bm, v1))
            if bp != None:
                self.x[v1] = min(self.x[v1], self.x[bp] - self.m(v1, bp))
            
    def combine_sequences(self, bm, bp, d, v):
        """Combines sequences. v is a sequence of nodes."""
        RM = []
        RP = []
        
        self.collect_left_changes(RM, v, bm, d)
        self.collect_right_changes(RP, v, bp, d)
        
        rm = rp = 0
        
        t =int(floor( len(v) / 2.0 ) - 1)
        m = self.m(v[t], v[t+1])
        while round(self.x[v[t+1]] - self.x[v[t]], 2) < round(m,2):
            #print self.x[v[t+1]] - self.x[v[t]] < m
            #print "while self.x[v[t+1]] - self.x[v[t]] < m:"
            #print "self.x[v[t+1]] = %s\nself.x[v[t]]=%s\nm=%s\nresults=%s" % (self.x[v[t+1]], self.x[v[t]], m,self.x[v[t+1]] - self.x[v[t]] ) 
            if rm < rp:
                if len(RM)==0:
                    self.x[v[t]] = self.x[v[t+1]] - m
                else:
                    cm, xvt = heappop(RM)
                    self.x[v[t]] = xvt # TODO: Not sure!
                    rm = rm + cm
                    self.x[v[t]] = max(self.x[v[t]], self.x[v[t+1]] - m)
            else:
                if len(RP) == 0:
                    self.x[v[t+1]] = self.x[v[t]] + m
                else:
                    cp, xvt1 = heappop(RP)
                    self.x[v[t+1]] = xvt1 # TODO: not sure
                    rp = rp + cp
                    self.x[v[t+1]] = min(self.x[v[t+1]], self.x[v[t]] + m)
                #sys.exit(-1)
        for i in range(t-1, -1, -1):
            self.x[v[i]] = min(self.x[v[i]], self.x[v[t]] - self.m(v[i], v[t]))
            
        for i in range(t+2, len(v)):
            self.x[v[i]] = max(self.x[v[i]], self.x[v[t+1]] + self.m(v[t+1], v[i]) )
                    
    def collect_left_changes(self, RM, v, bm, d):
        # TODO: not sure if I use the right v and t and bm
        t = int(floor( len(v) / 2.0 ) - 1)
        for i in range(0, t+1):
            c = 0
            assert d in (-1,1)
            if d == 1:
                sigma = filter(lambda x: x.virtual, v[i].upper_neighbors())
            elif d == -1:
                sigma = filter(lambda x: x.virtual, v[i].lower_neighbors())
            
            for _v in sigma:
                if self.x[_v] >= self.x[v[i]]:
                    c += 1
                else:
                    c -= 1
                    heappush(RM, (2, self.x[_v] + self.m(v[i], v[t])) )

            heappush(RM, (c, self.x[v[i]] + self.m(v[i], v[t])))
            
        if bm != None:
            heappush(RM, (float("inf"), self.x[bm] + self.m(bm, v[t]) ) )
            
            
    def collect_right_changes(self, RP, v, bp, d):
        # TODO: not sure if I use the right v and t and bp
        # Contains lots of bugs! 
        t = int(floor( len(v) / 2.0 ) - 1)
        
        for i in range(len(v)-1, t-1, -1):
            c = 0
            assert d in (-1,1)
            if d == 1:
                sigma = filter(lambda x: x.virtual, v[i].upper_neighbors())[::-1]
            elif d == -1:
                sigma = filter(lambda x: x.virtual, v[i].lower_neighbors())[::-1]
            
            for _v in sigma:
                if self.x[_v] <= self.x[v[i]]:
                    c += 1
                else:
                    c -= 1
                    heappush(RP, (2, self.x[_v] - self.m(v[t], v[i])) )
            heappush(RP, (c, self.x[v[i]] - self.m(v[t], v[i])))
            
        if bp != None:
            heappush(RP, (float("inf"), self.x[bp] - self.m(v[t], bp) ) )            
            
            
    
    ######## ORIGINAL NODE PLACEMENT ENDS #############
    def place_levels(self):
        
        c = 0
        for l in self.g.layers:
            for v in self.g.layers[l]:
                self.y[v] = v.graphics.y = float(c)
                c = c + self.min_level_distance 


def main():
    fk = FastKLevel(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
    fk.prepare_old_x()
    fk.level_layout()
    #fk.weighted_y_coordinates(50)
    
    fk.prepare_output()
    #fk.check_same_x()
    print "File written."
         
                        
if __name__ == "__main__":
    if len(sys.argv) > 4:
        #cProfile.run('main()')
        #import cProfile
        #cProfile.run('main()')
        main()
    else:
        print "Insufficient parameters"
        print "Usage: python %s <input file> <output file> <minimum distance> <layers/positions>" % sys.argv[0]
