#include "incl/handlers.h"

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
		node n,adj,source;
		gw.get_position( pos );
// 		cout << " Gecti 1 " << endl;
		array<list<node> > LAYERS( G.number_of_nodes() );
		node_array<int> layerId( G, 0 );
		node_array<int> neighbors( G, 0 );
		node_array<bool> marked( G, false );
		int max_degree = 0;
		node maxDegNode;
		forall_nodes( n, G ){
			neighbors[ n ] = G.degree( n );
			if( max_degree < neighbors[ n ] ){
				max_degree = neighbors[ n ];
				maxDegNode = n;
			}
		}
		LAYERS[ 0 ].append( maxDegNode );
		marked[ maxDegNode ] = true;
		layerId[ maxDegNode ] = 0;
		maxLayerIdNow = 0;
		list<node> stack;
		stack.push_back( maxDegNode );
		source = maxDegNode;
// 		cout << " Gecti 2 " << endl;
		while( stack.empty() != true ){
			n = stack.pop_front();
			list<node> N = G.adj_nodes( n );
			forall( adj, N ){
// 				cout << layerId[ adj ] << " - " << layerId[ n ] << endl;
				if( marked[ adj ] == false ){
					layerId[ adj ] = layerId[ n ] + 1;
					LAYERS[ layerId[ adj ] ].append( adj );
					stack.push_back( adj );
					marked[ adj ] = true;
					if( maxLayerIdNow < layerId[ adj ] )
						maxLayerIdNow = layerId[ adj ];
				}
// 				cout << " Gecti 2.1 " <</**/ endl;
			}
			if( stack.empty() == true ){
				forall_nodes( n, G ){
					if( marked[ n ] == false ){
						stack.push_back( n );
						layerId[ n ] = maxLayerIdNow;
						LAYERS[ layerId[ n ] ].append( n );
						marked[ n ] = true;
						break;
					}
				}
			}
// 			cout << " Loop " << endl;
		}
// 		cout << " Gecti 3 " << endl;
		double diameter = 0;
		for( int i = 0; i <= maxLayerIdNow; i++ ){
		        if( i != 0 ){
				count = 0;
				forall_items( it , LAYERS[ i ] ){
					count++;
				}
				tmp = pi;
				circle C( 0, 0, LAYERS[ i ].size() * 30.0 + diameter );
				double min = pi / (double)count;
				forall_items( it , LAYERS[ i ] ){
				      pos[ LAYERS[ i ][ it ] ] = C.point_on_circle( tmp );
				      xpos[ LAYERS[ i ][ it ] ] = pos[ LAYERS[ i ][ it ] ].xcoord();
				      ypos[ LAYERS[ i ][ it ] ] = pos[ LAYERS[ i ][ it ] ].ycoord();
				      tmp -= min;   
				}
			}
			else{
				xpos[ source ] = 0.0;
				ypos[ source ] = 0.0;
			}
			diameter += LAYERS[ i ].size() * 30.0;
// 			cout << " Loop " << endl;
		}
// 		cout << " Gecti 4 " << endl;

		gw.remove_bends();
		gw.set_position( xpos, ypos );
		gw.place_into_box(x0, y0, x1, y1);
		fname = fname.replace( ".gml", "tcircular.gml" );
		gw.save_gml( fname );
	}
	return 0;
}
