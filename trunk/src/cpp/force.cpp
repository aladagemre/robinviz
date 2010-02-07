#include "incl/handlers.h"
#include <LEDA/graph/graph_draw.h>

int main(int argc, char** argv) {
	GraphWin gw;
	if( argc > 1 ){
		list_item it;
		leda::string fname = argv[1];
		double pi = 2.0 * 3.147;
		double tmp = pi;
		int count = 0,maxLayerIdNow;
		gw.read_gml( fname );
		graph G = gw.get_graph();
		double x0, y0, x1, y1;
		gw.get_bounding_box(x0, y0, x1, y1);
		node_array<point> pos( G );
		node_array<double> xpos( G );
		node_array<double> ypos( G );
		node_array<int> neighbors( G );
		node n,adj,source;
		gw.get_position( pos );
		list<node> fixedNodes;
		int max_degree = 0;
		node maxDegNode;
		forall_nodes( n, G ){
			neighbors[ n ] = G.degree( n );
			if( max_degree < neighbors[ n ] ){
				max_degree = neighbors[ n ];
				maxDegNode = n;
			}
		}
		xpos[ maxDegNode ] = gw.get_xmin() + ( abs( gw.get_xmin() ) + abs( gw.get_xmax() ) ) / 2.0;
		ypos[ maxDegNode ] = gw.get_ymin() + ( abs( gw.get_ymin() ) + abs( gw.get_ymax() ) ) / 2.0;
		fixedNodes.append( maxDegNode );
		list<node> N = G.adj_nodes( maxDegNode );
                circle C( 0, 0, abs( abs( gw.get_ymax() ) - gw.get_ymin() ) / 3.0 );
		double min = pi / (double)count;
		forall( n , N ){
		      pos[ n ] = C.point_on_circle( tmp );
		      xpos[ n ] = pos[ n ].xcoord();
		      ypos[ n ] = pos[ n ].ycoord();
		      fixedNodes.append( n );
		      tmp -= min;   
		}



                int lower_bound=static_cast<int>(gw.get_xmin()+20.0);
                int upper_bound=static_cast<int>(gw.get_xmax()-20.0);
                random_source S2(lower_bound,upper_bound);
                int x1_;
                node v;
                forall_nodes(v,G) {
                    S2 >> x1_;
                    xpos[v]=x1_;
                }
                lower_bound=static_cast<int>(gw.get_ymin()+20.0);
                upper_bound=static_cast<int>(gw.get_ymax()-20.0);
                S2.set_range(lower_bound,upper_bound);
                int y1_;
                forall_nodes(v,G) {
                    S2 >> y1_;
                    ypos[v]=y1_;
                }

// 		cout << gw.get_xmin() << "\t" << gw.get_xmax() << "\t" << gw.get_ymin() << "\t" << gw.get_ymax() << endl;
                SPRING_EMBEDDING( G, fixedNodes, xpos, ypos, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 500 );
		gw.remove_bends();
		forall_nodes( n, G ){
			xpos[ n ] = xpos[ n ]*5.0;
			ypos[ n ] = ypos[ n ]*5.0;
		}
		gw.set_position( xpos, ypos );
		gw.place_into_box(x0, y0, x1, y1);
		fname = fname.replace( ".gml", "force.gml" );
		gw.save_gml( fname );
	}
	return 0;
}
