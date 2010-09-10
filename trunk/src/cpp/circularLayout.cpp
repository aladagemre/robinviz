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
		node n;
		gw.get_position( pos );
		circle C( 0, 0, G.number_of_nodes() * 30.0 );
		forall_nodes( n, G ){
		      count++;
		}
		double min = pi / (double)count;
		forall_nodes( n, G ){
		      pos[ n ] = C.point_on_circle( tmp );
		      tmp -= min;   
		}      
		forall_nodes( n, G ){
			xpos[ n ] = pos[ n ].xcoord();
			ypos[ n ] = pos[ n ].ycoord();
		}
		gw.remove_bends();
		gw.set_position( xpos, ypos );
		gw.place_into_box(x0, y0, x1, y1);
                fname = fname.replace( ".gml", "circularLayout.gml" );
		gw.save_gml( fname );
	}
	return 0;
}
