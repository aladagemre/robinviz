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
                list_item it;
		gw.get_bounding_box(x0, y0, x1, y1);
		node_array<double> xpos( G );
		node_array<double> ypos( G );
		node_array<int> neighbors( G );
		node n;
		gw.get_position( pos );

                node_array<int> comp( G, 0 );
                COMPONENTS( G, comp );
                int max = 0;
                forall_nodes( n, G ){
                    if( max < comp[ n ] )
                        max = comp[ n ];
                }
                array<list<node> > COMPS( max + 1 );
                count = 1;
                forall_nodes( n, G ){
                    COMPS[ comp[ n ] ].append( n );
                }
                list<int> notEnoughComponents;
                int maxComp = 0;
                for(int i = 0; i <= max; i++ ){
                    if( COMPS[ i ].size() > maxComp ){
                        maxComp = COMPS[ i ].size();
                    }
                }

                int election = 10;
                if( maxComp < election )
                    election = maxComp;
                for(int i = 0; i <= max; i++ ){
                    if( COMPS[ i ].size() < election ){
                        notEnoughComponents.append( i );
                        forall_items( it, COMPS[ i ] ){
                            G.hide_node( COMPS[ i ][ it ] );
                        }
                    }
                }

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
                        xpos[ n ] = pos[ n ].xcoord();
                        ypos[ n ] = pos[ n ].ycoord();
		}

                //cout << " 1 \n";
                double xmin, xmax, ymin, ymax;
                int ncount = 0;
                forall_nodes( n, G ){
                        if( COMPS[ comp[ n ] ].size() > election ){
                                if( ncount == 0 ){
                                        xmin = xpos[ n ];
                                        xmax = xpos[ n ];
                                        ymin = ypos[ n ];
                                        ymax = ypos[ n ];
                                }
                                else{
                                    if( xpos[ n ] < xmin )
                                          xmin = xpos[ n ];
                                    if( xpos[ n ] > xmax )
                                          xmax = xpos[ n ];
                                    if( ypos[ n ] < ymin )
                                          ymin = ypos[ n ];
                                    if( ypos[ n ] > ymax )
                                          ymax = ypos[ n ];
                                }
                                ncount++;
                        }
                }
                G.restore_all_nodes();
                //cout << " 2 \n";
                double xpos1 = xmin;
                double ypos1 = ymax + 6 * G.number_of_nodes();
                //cout << max << " - " << election << endl;
                int maxC = 0;
                for(int i = 0; i <= max; i++ ){
                        //cout << COMPS[ i ].size() << "\t";
                        if( COMPS[ i ].size() < election ){
                                if( COMPS[ i ].size() > maxC )
                                maxC = COMPS[ i ].size();
                                if( xpos1 < xmax ){
                                    int diam = 30;
                                    if( COMPS[ i ].size() < 3 ){
                                        diam = 150;
                                    }
                                    else{
                                        diam = 150 + COMPS[ i ].size() * 20;
                                    }
                                    circle C( xpos1 + 75 + COMPS[ i ].size() * 10, ypos1, diam );
                                    min = pi / (double)COMPS[ i ].size();
                                    tmp = pi;
                                    forall_items( it, COMPS[ i ] ){
                                        pos[ COMPS[ i ][ it ]  ] = C.point_on_circle( tmp );
                                        //cout << pos[ COMPS[ i ][ it ]  ] << "\t";
                                        xpos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].xcoord();
                                        ypos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].ycoord();
                                        tmp -= min;
                                    }
                                }
                                else{
                                        xpos1 = xmin;
                                        ypos1 += 150 + maxC * 20;
                                        maxC = 0;
                                        int diam = 30;
                                        if( COMPS[ i ].size() < 3 ){
                                            diam = 150;
                                        }
                                        else{
                                            diam = 150 + COMPS[ i ].size() * 10;
                                        }
                                        circle C( xpos1 + 75 + COMPS[ i ].size() * 10, ypos1, diam );
                                        min = pi / (double)COMPS[ i ].size();
                                        tmp = pi;
                                        forall_items( it, COMPS[ i ] ){
                                            pos[ COMPS[ i ][ it ]  ] = C.point_on_circle( tmp );
                                            //cout << pos[ COMPS[ i ][ it ]  ] << "\t";
                                            xpos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].xcoord();
                                            ypos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].ycoord();
                                            tmp -= min;
                                        }
                                }
                                if( COMPS[ i ].size() == 1 )
                                    xpos1 += 150;
                                else
                                    xpos1 += 300 + G.number_of_nodes() * 3;
                        }
                }
                //cout << "\n 3 \n";
                gw.remove_bends();

		forall_nodes( n, G ){
			xpos[ n ] = pos[ n ].xcoord();
			ypos[ n ] = pos[ n ].ycoord();
		}
		gw.remove_bends();
		gw.set_position( xpos, ypos );
		gw.place_into_box(x0, y0, x1, y1);
                fname = fname.replace( ".gml", "starLayout.gml" );
		gw.save_gml( fname );
	}
	return 0;
}
