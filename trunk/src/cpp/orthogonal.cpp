#include "incl/handlers.h"

int main(int argc, char** argv) {
	GraphWin gw;
	if( argc > 1 ){
		leda::string fname = argv[1];
		double pi = 2.0 * 3.147;
		double tmp = pi;
		int count = 0;
		gw.read_gml( fname );
		graph G = gw.get_graph();
		double x0, y0, x1, y1;
		gw.get_bounding_box(x0, y0, x1, y1);
		node_array<point> pos( G );
		node_array<double> xpos( G );
		node_array<double> ypos( G );
                edge_array<list<point> > bends( G );
                node n,picked;
                edge e;

                gw.get_position( pos );
                forall_nodes( n, G ){
                        xpos[ n ] = pos[ n ].xcoord();
                        ypos[ n ] = pos[ n ].ycoord();
                }
                double dx = 3.0;
                node_array<bool> marked( G, false );
                list<node> marked_l;
                while( marked_l.size() != G.number_of_nodes() ){
                    forall_nodes( n, G ){
                        if( marked[n] != true ){
                            picked = n;
                            marked[n]=true;
                            marked_l.append(n);
                            break;
                        }
                    }
                    forall_nodes( n, G ){
                        if( marked[n] != true ){
                            if( ypos[ n ] == ypos[ picked ] ){
                                ypos[ n ] += dx;
                                dx+=3.0;
                                marked[n]=true;
                                marked_l.append(n);
                            }
                        }
                    }
                    dx = 3.0;
                }
                forall_nodes( n, G ){
                    point p(xpos[ n ],ypos[ n ] );
                        pos[ n ] = p;
                }
                gw.remove_bends();
                forall_edges( e, G ){
                    if( ypos[G.source(e)] < ypos[G.target(e)] ){
                        point p( xpos[G.target(e)], ypos[G.source(e)] );
                        bends[ e ].append( p );
                    }
                    else{
                        point p( xpos[G.source(e)], ypos[G.target(e)] );
                        bends[ e ].append( p );
                    }
                }

                gw.set_layout( pos, bends );
		gw.place_into_box(x0, y0, x1, y1);
                fname = fname.replace( ".gml", "othogonal.gml" );
		gw.save_gml( fname );
	}
	return 0;
}
