#include "incl/handlers.h"
#include <LEDA/graph/graph_draw.h>

int main(int argc, char** argv) {
	GraphWin gw;
	if( argc > 1 ){
		leda::string fname = argv[1];
		double pi = 2.0 * 3.147;
		double tmp = pi;
		int count = 0;
		gw.read_gml( fname );
		graph G = gw.get_graph();
		node_array<point> pos( G );
		double x0, y0, x1, y1;
		gw.get_bounding_box(x0, y0, x1, y1);
		node_array<double> xpos( G );
		node_array<double> ypos( G );
		node_array<int> neighbors( G );
		node n;
		gw.get_position( pos );
		int max_degree = 0;
		node maxDegNode;
		forall_nodes( n, G ){
			neighbors[ n ] = G.degree( n );
			if( max_degree < neighbors[ n ] ){
				max_degree = neighbors[ n ];
				maxDegNode = n;
			}
		}
		circle C( 0, 0, G.number_of_nodes() * 30.0 );
		forall_nodes( n, G ){
		      count++;
		}
		count = count - 1;
		double min = pi / (double)count;
		forall_nodes( n, G ){
			if( n != maxDegNode ){
				pos[ n ] = C.point_on_circle( tmp );
				tmp -= min;   
			}
			else{
				point P( 0, 0 );
				pos[ n ] = P;
			}
		}
		forall_nodes( n, G ){
			xpos[ n ] = pos[ n ].xcoord();
			ypos[ n ] = pos[ n ].ycoord();
		}
		gw.remove_bends();
		gw.set_position( xpos, ypos );
		gw.place_into_box(x0, y0, x1, y1);
		fname = fname.replace( ".gml", "star.gml" );
		gw.save_gml( fname );
	}
	return 0;
}
