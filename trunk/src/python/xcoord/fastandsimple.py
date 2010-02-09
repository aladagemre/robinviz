# -*- coding: utf-8 -*-
from pygml import Graph
#from parser import GMLParser
from math import ceil, floor
import sys
#import cProfile

class FastAndSimple:
    """Implementation of the paper "Fast and Simple Horizontal Coordinate Assignment" by Ulrik Brandes and Boris Köpf."""
    def __init__(self, input_file, minimum_distance, vstart, hstart, layersorpositions='layers', verboseMode=False):
        self.sink = {}
        self.shift = {}
        self.x = {}
        self.align={}
        self.minimum_distance = int(minimum_distance)
        self.layersorpositions = layersorpositions
        self.verboseMode = verboseMode

        if isinstance(input_file, Graph):
            self.g = input_file
        else:
            #self.g = Graph(input_file)            
            self.g = Graph()
            #gp = GMLParser(input_file, self.g)
            self.g.read_gml(input_file)
            self.g.prepare()

        if not (vstart and hstart):
            return
       
        # Uncomment the two lines below if you want to detect type 2 crossings.
        #if self.detect_type2():
            #print "Because of the type 2 crossings, there might be problems."
           
        if self.verboseMode:
            print "Preprocessing..."
        self.preprocessing()

        self.decide_alignment(vstart, hstart) # DOES ALIGNMENT AND COMPACTION
       
        # Uncomment the line below if you want to perform sorting on each individual candidate.        
        #self.node_sort_heuristic()
       
        # Assign the computed coordinates
        for v in self.x:
            v.graphics.x = float(self.x[v])
       
       
        #print "Weighted y coordinates..."
        #self.weighted_y_coordinates(100)
        #self.adjustments()
        self.debug()
        #self.paint_type1()
       
        # Uncomment the line below if you want to straighten the edges in each individual candidate.
        #self.straighten_bends()
       
    def color_blocks(self):
        """Paints the blocks with different colors."""
        colors = ("#FF0000", "#0363FF", "#FFF826", "#BA0BFF", "#FFA42D", "#16FF54", "#934446", "#FF9595", "#8860FF")
        color_index = 0
        for v in self.g.nodes:
            if self.root[v] == v:
                if self.align[v] == v: continue
                w = v
                start = True
                while v!=w or start:
                    start = False
                    w.graphics.fill = colors[color_index]
                    w = self.align[w]
            color_index = (color_index + 1) % 9
           
    def color_classes(self):
        """Paints the classes with different colors."""
        colors = ("#FF0000", "#0363FF", "#FFF826", "#BA0BFF", "#FFA42D", "#16FF54", "#934446", "#FF9595", "#8860FF")
        color_index = 0
        sinks = set([])
        for v in self.g.nodes:
            u = self.sink[v]
            sinks.add(u)
        sinks = list(sinks)
        for sink in sinks:
            for v in self.g.nodes:
                if self.sink[v] == sink:
                    v.graphics.fill = colors[color_index]
            color_index = (color_index + 1) % 9
       
    def correct_triple_edge(self):
        """Corrects sloping inner segment of 3 segmented edges."""
        if self.verboseMode:
            print "Correcting triple edges..."
        for u in self.g.virtual_nodes:
            upper_real = u.upper_neighbors()[0]
            if upper_real.virtual: continue

            v = u.lower_neighbors()[0]
            if not v.virtual: continue

            lower_real = v.lower_neighbors()[0]
            if lower_real.virtual: continue
            #print "Found a triple edge"
            if u.graphics.x == v.graphics.x: continue
            #print "that needs to be corrected."

            if upper_real.graphics.x + lower_real.graphics.x > u.graphics.x + v.graphics.x:
                # Means that real nodes are on the right of the dummies.
                # So we should shift the dummy on the left to right.
                #print "Reals are on the right."
                if u.graphics.x < v.graphics.x:
                    #print "U is on the left of v"
                    # If u is on the left of v,
                    if self.feasible(v.graphics.x, u.layer):
                        #print "It is feasible to put it right."
                        # If no other node exists there,
                        u.graphics.x = v.graphics.x
    def feasible(self, x, layer):
        """Returns if any node exists on the given x coordinate and layer."""
        get_x = lambda a: a.graphics.x
        if x in ( get_x(v) for v in self.g.layers[layer] ):
            return False
        return True

    def paint_type1(self):
        """Paints the edges that are marked by preprocessing step with red"""
        # Mark type 1 crossing edges with red.
        for edge in self.g.edges:
            if edge.marked:
                edge.graphics.fill = "#FF0000"
       
    def decide_alignment(self, vstart, hstart):
        """Decides which methods shall be used according to
        the direction parameters."""
        if vstart == "up":
            if hstart == "left":
                self.vertical_alignment_up_left()
                self.horizontal_compaction_up_left()
            else:
                self.vertical_alignment_up_right()
                self.horizontal_compaction_up_right()
        else:
            if hstart == "left":
                self.vertical_alignment_down_left()
                self.horizontal_compaction_down_left()
            else:
                self.vertical_alignment_down_right()
                self.horizontal_compaction_down_right()
       
    def detect_type2(self):
        """Detects type 2 crossings."""
        if self.verboseMode:
            print "Searching for type 2 crossings..."""
        detected = False
        for layer in self.g.layers:
            for u in self.g.layers[layer]:
                if u.virtual:
                    uoe = u.outgoing_edges
                    for out_edge in uoe:
                        if out_edge.v.virtual:
                            for i in range(u.position + 1, len(self.g.layers[layer])):
                                v = self.g.layers[layer][i]
                                for edge in v.outgoing_edges:
                                    if edge.u.virtual and edge.v.virtual and edge.v.position < out_edge.v.position:
                                        if self.verboseMode:
                                            print "(%d, %d) and (%d, %d) are type 2 crossing." % (out_edge.u.id, out_edge.v.id, edge.u.id, edge.v.id)
                                        detected = True
        return detected
           
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
           
    def post_adjustments(self):
        """Some post adjustments after balancing."""
        # Shifts the overlapping nodes by half of their widths.
        if self.verboseMode:
            print "Post adjustments..."
       
        layer_overlaps = {0: 1}
        tour = 0
        while sum(layer_overlaps.values()) != 0:
            if self.verboseMode:
                print "New tour"
            tour += 1
           
            for layer in self.g.layers:
                layer_overlaps[layer] = 0
                for u in self.g.layers[layer]:
                    for v in self.g.layers[layer]:
                        if u!=v and abs(u.graphics.x - v.graphics.x) < (u.graphics.w + v.graphics.w)/2 :
                            if tour > 15:
                                if self.verboseMode:
                                    print u.id,":",u.graphics.x,"---",v.id,":", v.graphics.x
                                    print "-------------------------------------------------"*10
                                return
                           
                           
                            if self.verboseMode:
                                print "Adjusting %d and %d" % (u.id, v.id)
                            if u.position < v.position:
                                if u.pred:
                                    prev_x = u.pred.graphics.x
                                else:
                                    prev_x = u.graphics.x - self.minimum_distance
                                if prev_x + self.minimum_distance not in [node.graphics.x for node in self.g.layers[layer]]:
                                    u.graphics.x = prev_x + self.minimum_distance
                                else:
                                    u.graphics.x = prev_x + self.minimum_distance/2
                            else:
                                if v.pred:
                                    prev_x = v.pred.graphics.x
                                else:
                                    prev_x = v.graphics.x - self.minimum_distance
                                   
                                if prev_x + self.minimum_distance not in [node.graphics.x for node in self.g.layers[layer]]:
                                    v.graphics.x = prev_x + self.minimum_distance
                                else:
                                    v.graphics.x = prev_x + self.minimum_distance/2
                            layer_overlaps[layer] = 1
    def order_changes(self, node, target_x):
        """Checks if the order changed when the node is moved to the target_x."""
        if target_x > node.graphics.x:
            # If we are moving the node to the right,
            if not node.succ: return False
            if node.succ.graphics.x <= target_x:
                # If we passed the successor node, the order has changed.
                return True
            else:
                return False
        elif target_x < node.graphics.x:
            # If we are moving the node to the left,
            if not node.pred: return False
            if node.pred.graphics.x >= target_x:
                # If we passed the predecessor node, the order has changed.
                return True
            else:
                return False
        else:
            # If same coordinate given, order doesn't change.
            return False
           
    def center_nodes(self):
        "Centering nodes..."
        n = len(self.g.layers.keys())
        first_line = self.g.layers[0]
        last_line = self.g.layers[n-1]
       
        if len(first_line) < 3:
            for node in first_line:
                oe = node.outgoing_edges
                if oe > 0:
                    xs = ( e.v.graphics.x for e in oe )
                    new_coordinate = sum(xs)/len(oe)
                    node.graphics.x = new_coordinate
               
        if len(last_line) < 3:
            for node in last_line:
                oe = node.incoming_edges
                if oe > 0:
                    xs = ( e.u.graphics.x for e in oe )
                    new_coordinate = sum(xs)/len(oe)
                    node.graphics.x = new_coordinate
               
                       
               
    def straighten_bends(self):
        """Straightens the edge bends."""
        if self.verboseMode:
            print "Straightening bends."
       
        # Start from the middle.
       
        l = len(self.g.layers)
        if l%2 == 0:
            b = int(l/2) - 1
        else:
            b = int(floor(l/2))
           
        a = 0
        d = l-1
       
        self.g.find_virtual_vertices('#FF8C00')
       
        # ####################################################################################
        # Middle to Top
        for layer in xrange(b,a-1,-1):
            v_nodes = [v for v in self.g.layers[layer] if v.virtual ]          
            length_of_layer = len(v_nodes)
            middle = int(floor(length_of_layer / 2))
            # -----------------------------------------------------------------------------------------
            # From right to left.
            our_list = v_nodes[middle:]
            our_list.reverse()
            for u in our_list:
                v = u.incoming_edges[0].u
                t = u.outgoing_edges[0].v
                if u.graphics.x == v.graphics.x:
                    continue
                if v.virtual:
                    if self.feasible(u.graphics.x, v.layer):
                        # Move v to the left.
                        v.graphics.x = u.graphics.x
                    elif self.feasible(v.graphics.x, u.layer):
                        # Move u to the right.
                        u.graphics.x = v.graphics.x
                # Only one bend => align vertically to the closest neighbor.
                elif not t.virtual:
                    if abs(v.graphics.x - u.graphics.x) > abs(t.graphics.x - u.graphics.x):
                        boss = t
                    else:
                        boss = v
                   
                    if self.feasible(boss.graphics.x, u.layer) and not self.order_changes(u, boss.graphics.x):
                        u.graphics.x = boss.graphics.x
                    elif self.feasible(u.graphics.x, boss.layer) and not self.order_changes(boss, u.graphics.x):
                        boss.graphics.x = u.graphics.x
                       
                else:
                    # First dummy node after real.
                    # Align u to t.
                    #print "First dummy after real! Align u to t:  v=%d, u=%d, t=%d" % (v.id, u.id, t.id)
                    boss = t
                    if self.feasible(boss.graphics.x, u.layer) and not self.order_changes(u, boss.graphics.x):
                        u.graphics.x = boss.graphics.x
                        #print "Doing what we want"
                    # MAYBE WE SHOULD REMOVE THIS ELSE!
                    elif self.feasible(u.graphics.x, boss.layer) and not self.order_changes(boss, u.graphics.x):
                        boss.graphics.x = u.graphics.x
                        #print "We dont want this."
                   
                    else:
                        #print "It seems it's not feasible."
                        pass
                        #print "Feasible to align lower neighbor? ", self.feasible(boss.graphics.x, u.layer)
                        #print "Order changes?", self.order_changes(u, boss.graphics.x)
                        #print "Boss: ", boss.id, boss.graphics.x
                        #print "Node: ", u.id, u.graphics.x
            # ---------------------------------------------------------------------------------------
            # From left to right
            for u in v_nodes[0:middle+1]:
                v = u.incoming_edges[0].u
                t = u.outgoing_edges[0].v
                if u.graphics.x == v.graphics.x:
                    continue
           
                if v.virtual:
                    if self.feasible(u.graphics.x, v.layer) and not self.order_changes(v, u.graphics.x):
                        # Align v to u (move v to right)
                        v.graphics.x = u.graphics.x
                    elif self.feasible(v.graphics.x, u.layer) and not self.order_changes(u, v.graphics.x):
                        # Align u to v (move u to left)
                        u.graphics.x = v.graphics.x
               
                # Only one bend => align vertically to the closest neighbor.
                elif not t.virtual:
                    #print "Only one bend! v=%d, u=%d, t=%d" % (v.id, u.id, t.id)
                    if abs(v.graphics.x - u.graphics.x) > abs(t.graphics.x - u.graphics.x):
                        boss = t
                    else:
                        boss = v
                       
                    if self.feasible(boss.graphics.x, u.layer) and not self.order_changes(u, boss.graphics.x):
                        u.graphics.x = boss.graphics.x
                    elif self.feasible(u.graphics.x, boss.layer) and not self.order_changes(boss, u.graphics.x):
                        boss.graphics.x = u.graphics.x
                     
                       
                else:
                    # First dummy node after real.
                    # Align u to t.
                    boss = t
                    if self.feasible(boss.graphics.x, u.layer) and not self.order_changes(u, boss.graphics.x):
                        u.graphics.x = boss.graphics.x
                    # MAYBE WE SHOULD REMOVE THIS ELSE!
                    elif self.feasible(u.graphics.x, boss.layer) and not self.order_changes(boss, u.graphics.x):
                        boss.graphics.x = u.graphics.x
        # ###################################################################################            
        # Middle to Bottom
        for layer in xrange(b,d+1):
            v_nodes = [v for v in self.g.layers[layer] if v.virtual ]
           
            length_of_layer = len(v_nodes)
            middle = int(floor(length_of_layer / 2))
            our_list = v_nodes[middle:]
            our_list.reverse()
            for u in our_list:
                v = u.outgoing_edges[0].v
                if u.graphics.x == v.graphics.x:
                    continue
           
                if v.virtual:
                    if self.feasible(v.graphics.x, u.layer) and not self.order_changes(u, v.graphics.x):
                        # Move u to left
                        u.graphics.x = v.graphics.x
                       
                    elif self.feasible(u.graphics.x, v.layer) and not self.order_changes(v, u.graphics.x):
                        # Move v to right

                        v.graphics.x = u.graphics.x
               
            # From left to right
           
            for u in v_nodes[0:middle+1]:
                v = u.outgoing_edges[0].v
                t = u.incoming_edges[0].u
                if u.graphics.x == v.graphics.x:
                    continue
           
                if v.virtual:
                    if self.feasible(v.graphics.x, u.layer) and not self.order_changes(u, v.graphics.x):
                        u.graphics.x = v.graphics.x    
                    elif self.feasible(u.graphics.x, v.layer) and not self.order_changes(v, u.graphics.x):
                        v.graphics.x = u.graphics.x
                       
                # Only one bend => align vertically to the closest neighbor.
                elif not t.virtual:
                    #print "Only one bend! v=%d, u=%d, t=%d" % (v.id, u.id, t.id)
                    if abs(v.graphics.x - u.graphics.x) > abs(t.graphics.x - u.graphics.x):
                        boss = t
                    else:
                        boss = v
                       
                    if self.feasible(boss.graphics.x, u.layer) and not self.order_changes(u, boss.graphics.x):
                        u.graphics.x = boss.graphics.x
                        #print "Aligning dummy."
                    #elif self.feasible(u.graphics.x, boss.layer) and not self.order_changes(boss, u.graphics.x):
                    #    boss.graphics.x = u.graphics.x
                    #    print "Aligning boss."
       
                """else:
                    # First dummy node after real.
                    # Align u to t.
                    boss = t
                    if self.feasible(boss.graphics.x, u.layer) and not self.order_changes(u, boss.graphics.x):
                        u.graphics.x = boss.graphics.x
                    # MAYBE WE SHOULD REMOVE THIS ELSE!
                    elif self.feasible(u.graphics.x, boss.layer) and not self.order_changes(boss, u.graphics.x):
                        boss.graphics.x = u.graphics.x"""
                       
    def hide_dummy_nodes(self):
        """Hides dummy nodes in the graph"""
        if self.verboseMode:
            print "Hiding dummy nodes..."
        #print self.g.virtual_nodes
        for node in self.g.virtual_nodes:
            node.graphics.w = 0.01
            node.graphics.h = 0.01
            #node.graphics.fill = "#FFFFFF"
            for edge in self.g.edges:
                if edge.v.virtual:
                    edge.graphics.arrow = "none"
                   
    def adjustments(self):
        """Some adjustments for the bug of the algorithm. Seperates overlapping nodes."""
       
        if self.verboseMode:
            print "Adjusting..."
        # Aligns the false-ordered nodes to their root.        
        for layer in self.g.layers:
            for i in range(len(self.g.layers[layer])):
                current_layer = self.g.layers[layer]
                if i>0 and i<len(current_layer) and current_layer[i].graphics.x <= current_layer[i-1].graphics.x:
                    current_layer[i].graphics.x = self.root[current_layer[i]].graphics.x
                    #current_layer[i].graphics.x = (current_layer[i-1].graphics.x + current_layer[i+1].graphics.x)/2
       
        """
        # Shifts the overlapping nodes by half of their widths.
        for u in self.g.nodes:
            for v in self.g.nodes:
                if u != v and u.graphics.x == v.graphics.x and u.graphics.y == v.graphics.y:
                    if self.align[u].graphics.x > self.align[v].graphics.x:
                        u.graphics.x += u.graphics.w/2+3
                        v.graphics.x -= v.graphics.w/2+3
                    else:
                        u.graphics.x -= u.graphics.w/2+3
                        v.graphics.x += v.graphics.w/2+3"""
       
        layer_overlaps = {0: 1}
        while sum(layer_overlaps.values()) != 0:
            for layer in self.g.layers:
                layer_overlaps[layer] = 0
                for u in self.g.layers[layer]:
                    v = u.succ
                    if v!=None and abs(u.graphics.x - v.graphics.x) < (u.graphics.w + v.graphics.w)/2:
                        u.graphics.x -= u.graphics.w/2
                        v.graphics.x += v.graphics.w/2
                        layer_overlaps[layer] = 1
       
   
    def debug(self):
        """
        Some debugging stuff
        """
        if self.verboseMode:
            print "Checking for overlaps..."
        # Print the nodes that are overlapping
        count = 0
        for layer in self.g.layers:
            for u in self.g.layers[layer]:    
                for v in self.g.layers[layer]:
                    if u!=v and u.graphics.x == v.graphics.x and u.graphics.y == v.graphics.y:
                        if self.verboseMode:
                            print "%s (%0.0f, %0.0f) , %s (%0.0f, %0.0f)" % (u.id, u.graphics.x, u.graphics.y, v.id, v.graphics.x, v.graphics.y)
                        count+=1
        if self.verboseMode and count:
            print "%d overlaps exist." % count
       
    def preprocessing(self):
        """Marks the edges with type 1 crossing."""
        h = len(self.g.layers)
        for i in xrange(1, h-2): #i=2 to h-2
            k0 = 0
            l = 0
            for l1 in range(0, self.g.layer_num_elements(i+1)):
                if l1 == self.g.layer_num_elements(i+1) or self.g.is_end_incident(self.g.get_node(i+1, l1)): #incident means target v(i+1, l1) should be real and source is virtual?
                    k1 = self.g.layer_num_elements(i)
                    if self.g.is_end_incident(self.g.get_node(i+1, l1)):
                        k1 = self.g.get_node(i+1, l1).upper_neighbors()[0].position
                    while l < l1:
                       
                        for un in self.g.get_node(i+1, l).upper_neighbors():
                            if un.position < k0 or un.position > k1:
                                self.g.mark_segment(un, self.g.get_node(i+1, l))
                        l += 1
                    k0 = k1
   
    def vertical_alignment_up_left(self):
        """Aligns the nodes to their roots (constructing blocks) - ALG2 for upper left"""
        if self.verboseMode:
            print "Vertical Alignment Up left..."
        self.root = {}
        self.align = {}
        for node in self.g.nodes:
            self.root[node] = node
            self.align[node] = node
       
        for layer in range(len(self.g.layers)):
            r = -1
            for v in self.g.layers[layer]:
                neighbors = v.upper_neighbors()
                d = float(len(neighbors))
               
                if d > 0:
                    for m in ( floor((d+1)/2), ceil((d+1)/2) ):
                        m = int(m) - 1
                        if (self.align[v] == v):
                            if not self.g.get_edge(neighbors[m], v).marked and r < neighbors[m].position:
                                self.align[neighbors[m]] = v
                                self.root[v] = self.root[neighbors[m]]    
                                self.align[v] = self.root[v]
                                r = neighbors[m].position
               
    def vertical_alignment_up_right(self):
        """Aligns the nodes to their roots (constructing blocks) - ALG2 for upper right"""
        if self.verboseMode:
            print "Vertical Alignment Up Right..."
        self.root = {}
        self.align = {}
        for node in self.g.nodes:
            self.root[node] = node
            self.align[node] = node
       
        for layer in range(len(self.g.layers)):
            r = len(self.g.layers[layer]) # Changed 0 to last index for right
            for v in self.g.layers[layer][::-1]: # Changed order for right
                neighbors = v.upper_neighbors()
                d = float(len(neighbors))
                if d > 0:
                    for m in ( ceil((d+1)/2), floor((d+1)/2) ): #changed order for right
                        m = int(m) - 1
                       
                        if (self.align[v] == v):
                           
                            if not self.g.get_edge(neighbors[m], v).marked and r > neighbors[m].position: # Changed < to > for right
                                self.align[neighbors[m]] = v
                                self.root[v] = self.root[neighbors[m]]
                                self.align[v] = self.root[v]
                                r = neighbors[m].position
                               
    def vertical_alignment_down_right(self):
        """Aligns the nodes to their roots (constructing blocks) - ALG2 for lower right"""
        if self.verboseMode:
            print "Vertical Alignment Down Right..."
        self.root = {}
        self.align = {}
        for node in self.g.nodes:
            self.root[node] = node
            self.align[node] = node
       
        for layer in range(len(self.g.layers))[::-1]:
            r = len(self.g.layers[layer]) # Changed 0 to last index for right
            for v in self.g.layers[layer][::-1]: # Changed order for right
                neighbors = v.lower_neighbors()
                neighbors.reverse()
                d = float(len(neighbors))
                if d > 0:
                    for m in ( floor((d+1)/2), ceil((d+1)/2) ): #changed order for right
                        m = int(m) - 1
                        if (self.align[v] == v):
                            if not self.g.get_edge(v, neighbors[m]).marked and r > neighbors[m].position: # Changed < to > for right
               
                                self.align[neighbors[m]] = v
                                self.root[v] = self.root[neighbors[m]]
                                self.align[v] = self.root[v]
                                r = neighbors[m].position
                               
                               
    def vertical_alignment_down_left(self):
        """Aligns the nodes to their roots (constructing blocks) - ALG2 for lower left"""
        if self.verboseMode:
			print "Vertical Alignment Down left..."
        self.root = {}
        self.align = {}
        for node in self.g.nodes:
            self.root[node] = node
            self.align[node] = node
       
        for layer in range(len(self.g.layers))[::-1]:
            r = -1
            for v in self.g.layers[layer]:
                neighbors = v.lower_neighbors()
                d = float(len(neighbors))
   
                if d > 0:
                    for m in ( floor((d+1)/2), ceil((d+1)/2) ):
                        m = int(m) - 1
                       
                        if (self.align[v] == v):
                            if not self.g.get_edge(v, neighbors[m]).marked and r < neighbors[m].position: # Changed (n,v) to (v, neighbors[m])
                                self.align[neighbors[m]] = v
                                self.root[v] = self.root[neighbors[m]]
                                self.align[v] = self.root[v]
                                r = neighbors[m].position


    def place_block_left(self, v):
        """Places the block of the given node into classes. (ALG 3) for left."""
        if not self.x.get(v):
            self.x[v] = 0
            w = v
            start = True
            while w!=v or start:
                start = False
                if w.position > 0:
                    u = self.root[w.pred]
                   
                    self.place_block_left(u)
                   
                    if self.sink[v] == v:
                        self.sink[v] = self.sink[u]
                    if self.sink[v] != self.sink[u]:      
                        self.shift[self.sink[u]] = min(self.shift[self.sink[u]], self.x[v] - self.x[u] - self.minimum_distance)
                    else:
                        self.x[v]  = max(self.x[v], self.x[u] + self.minimum_distance)
                w = self.align[w]
               
    def place_block_right(self, v):
        """Places the block of the given node into classes. (ALG 3) for right."""
        if not self.x.get(v):
            self.x[v] = 0
            w = v
            start = True
            while w!=v or start:
                start = False
                if w.position < len(self.g.layers[w.layer])-1:
                    u = self.root[w.succ]
                    self.place_block_right(u)
                    if self.sink[v] == v:
                        self.sink[v] = self.sink[u]
                    if self.sink[v] != self.sink[u]:                                              
                        self.shift[self.sink[u]] = min(self.shift[self.sink[u]], self.x[u] - self.x[v] - self.minimum_distance)
                    else:
                        self.x[v]  = min(self.x[v], self.x[u] - self.minimum_distance)
               
                w = self.align[w]
   
   
   
    def horizontal_compaction_up_left(self):
        """Final X-coordinate assignment phase (ALG3) for upper left."""
        if self.verboseMode:
            print "Horizontal Compaction up left..."
        for v in self.g.nodes:
            self.sink[v] = v
            self.shift[v] = float("infinity")
   
        # root coordinates relative to self.sink
        for layer in self.g.layers:
            for v in self.g.layers[layer]:
                if self.root[v] == v:
                    self.place_block_left(v)
       
        # absolute coordinates
        #for v in self.g.nodes:
       
        for layer in self.g.layers:
            for v in self.g.layers[layer]:
                self.x[v] = self.x[self.root[v]]        
               
                if v==self.root[v] and self.shift[self.sink[v]] < float("infinity"):
                    self.x[v] = self.x[v] + self.shift[self.sink[v]]
               

    def horizontal_compaction_down_left(self):
        """Final X-coordinate assignment phase (ALG3) for lower left."""
        if self.verboseMode:
            print "Horizontal Compaction down left..."
        for v in self.g.nodes:
            self.sink[v] = v
            self.shift[v] = float("infinity")
       
        num_layers = len(self.g.layers)
        # root coordinates relative to self.sink        
        for layer in xrange(num_layers-1, -1, -1):
            for v in self.g.layers[layer]:
                if self.root[v] == v:
                    self.place_block_left(v)
               
        # absolute coordinates
        #for v in self.g.nodes:
        for layer in xrange(num_layers-1, -1, -1):
            for v in self.g.layers[layer]:
                self.x[v] = self.x[self.root[v]]
                if v==self.root[v] and self.shift[self.sink[v]] < float("infinity"):
                    self.x[v] = self.x[v] + self.shift[self.sink[v]]    
           
    def horizontal_compaction_up_right(self):
        """Final X-coordinate assignment phase (ALG3) for upper right."""
        if self.verboseMode:
            print "Horizontal Compaction up right..."
        for v in self.g.nodes:
            self.sink[v] = v
            self.shift[v] = float("infinity")
   
        # root coordinates relative to self.sink
       
        for layer in self.g.layers:
            for v in self.g.layers[layer][::-1]:
                if self.root[v] == v:
                    self.place_block_right(v)
                   
        # absolute coordinates
        for layer in self.g.layers:
            for v in self.g.layers[layer][::-1]:
                self.x[v] = self.x[self.root[v]]
                if v==self.root[v] and self.shift[self.sink[v]] < float("infinity"):
                    self.x[v] = self.x[v] - self.shift[self.sink[v]]
                   
    def horizontal_compaction_down_right(self):
        """Final X-coordinate assignment phase (ALG3) for lower right."""
        if self.verboseMode:
            print "Horizontal Compaction down right..."
        for v in self.g.nodes:
            self.sink[v] = v
            self.shift[v] = float("infinity")
   
        num_layers = len(self.g.layers)
        # root coordinates relative to self.sink
        for layer in xrange(num_layers-1, -1, -1):
            for v in self.g.layers[layer][::-1]:
                if self.root[v] == v:
                    self.place_block_right(v)
                   
        # absolute coordinates
        for layer in xrange(num_layers-1, -1, -1):
            for v in self.g.layers[layer][::-1]:
                self.x[v] = self.x[self.root[v]]
                if v==self.root[v] and self.shift[self.sink[v]] < float("infinity"):
                    self.x[v] = self.x[v] - self.shift[self.sink[v]]            
   
    def node_sort_heuristic(self):
        """Sorts the resulting one-way layout nodes for the case any overlap occurs."""
        for layer in self.g.layers:
            #xcoords = [self.x[node] for node in self.g.layers[layer]]
            this_layer = self.g.layers[layer]
            n = this_layer[0]
            finished = False
            while not finished:
                if n.succ and self.x[n] >= self.x[n.succ]:
                    self.x[n] = self.x[n.succ] - self.minimum_distance
                    n = this_layer[0]
                    continue
                if n.succ:
                    n = n.succ
                else:
                    finished = True
                   
    def node_sort_combined_heuristic(self):
        """Sorts the resulting combined layout nodes for the case any overlap occurs."""
        for layer in self.g.layers:
            this_layer = self.g.layers[layer]
            n = this_layer[0]
            finished = False
            while not finished:
                if n.succ and n.graphics.x >= n.succ.graphics.x:
                    n.graphics.x = n.graphics.x - self.minimum_distance
                    n = this_layer[0]
                    continue
                if n.succ:
                    n = n.succ
                else:
                    finished = True
                   
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
            if self.verboseMode:
		print "Unknown txt type: %s. It should be either layers or positions" % self.layersorpositions

class Aligner:
    """Utilities for aligning the 4 candidate graphs."""
    def __init__(self, graphs):
        # Store x values
        x_ul = {}
        x_dl = {}
        x_ur = {}
        x_dr = {}
       
        names = ["ul","ur","dl","dr"]
        # Find the reference graph.
        min_width = float("infinity")
        for graph in graphs:
            w = graph.get_width()
            if  w < min_width:
                min_width = w
                reference_graph = graph
        self.reference_direction = names[graphs.index(reference_graph)]
        (ul, ur, dl, dr) = graphs
       
        min_ref = reference_graph.min_x()        
        max_ref = reference_graph.max_x()



        # Shift left
        min_ul = ul.min_x()
        min_dl = dl.min_x()
   
        difference = min_ul - min_ref
       
        for node in ul.nodes:
            if reference_graph != ul:
                node.graphics.x -= difference
            x_ul[node.id] = node.graphics.x
           
        difference = min_dl - min_ref
       
        for node in dl.nodes:
            if reference_graph != dl:
                node.graphics.x -= difference
            x_dl[node.id] = node.graphics.x
        # Shift right
       
        max_ur = ur.max_x()
        max_dr = dr.max_x()
       
        difference = max_ur - max_ref
        for node in ur.nodes:
            if reference_graph != ur:        
                node.graphics.x -= difference
            x_ur[node.id] = node.graphics.x
   
        difference = max_dr - max_ref
        for node in dr.nodes:
            if reference_graph != dr:
                node.graphics.x -= difference
            x_dr[node.id] = node.graphics.x
   
       
        # Average of median
        for node_id in x_ul.keys(): # Not particularly x_ul. As ids are all the same.
            x_list = [ x_ul[node_id], x_ur[node_id], x_dl[node_id], x_dr[node_id] ]
            x_list.sort()
           
            average = (x_list[1] + x_list[2])/2
            reference_graph.get_node_by_id(node_id).graphics.x = average
       
        self.result = reference_graph
       
    def get_result(self):
        """Returns the reference graph"""
        return self.result            

# -----------------------------------------


def main():
    """Main program, handles arguments and starts the FaSCA algorithm accordingly"""
    # COMBINED
    if len(sys.argv) > 7 and sys.argv[7] == "-v":
	verbose = True
    else:
	verbose = False
		
    if sys.argv[4] == "combined":
        if verbose:
            print "Up Left"
        ul = FastAndSimple(sys.argv[1], sys.argv[3], "up", "left", sys.argv[6], verbose)
        if verbose:
            print
            print "Up Right"
        ur = FastAndSimple(sys.argv[1], sys.argv[3], "up", "right", sys.argv[6], verbose)
        if verbose:
            print
            print "Down Left"
        dl = FastAndSimple(sys.argv[1], sys.argv[3], "down", "left", sys.argv[6], verbose)
        if verbose:
            print
            print "Down Right"
        dr = FastAndSimple(sys.argv[1], sys.argv[3], "down", "right", sys.argv[6], verbose)
       
        if verbose:
            print "Alignment..."
        aligner = Aligner((ul.g, ur.g, dl.g, dr.g))
        result = aligner.get_result()
        fs = FastAndSimple(result, sys.argv[3], None, None, sys.argv[6], verbose)
       
        fs.node_sort_combined_heuristic()
        fs.center_nodes()
        fs.straighten_bends()
        #fs.hide_dummy_nodes()
        #fs.correct_triple_edge()     # No need for now
        #fs.weighted_y_coordinates()  # LEDA does this.
        fs.post_adjustments()
        fs.debug()
        fs.g.write_gml(sys.argv[2])
        fs.write_txt()
    else:
        # SINGLE WAY
        fs = FastAndSimple(sys.argv[1], sys.argv[3], sys.argv[4], sys.argv[5], sys.argv[6], verbose)
        #fs.color_blocks()
        #fs.color_classes()
        #fs.node_sort_combined_heuristic()        
        #fs.straighten_bends()        
        fs.hide_dummy_nodes()
        #fs.post_adjustments()
        fs.g.write_gml(sys.argv[2])
        fs.write_txt()
   
    #import os
    #os.system("./graphwin %s &" % sys.argv[2])

   
   
if __name__ == "__main__":
    if len(sys.argv) > 6:
        #cProfile.run('main()')
        main()
    else:
        print "Insufficient parameters"
        print "Usage: python %s <input file> <output file> <minimum distance> <combined/up/down> [left/right] [layers/positions] [-v for verbosemode]" % sys.argv[0]
