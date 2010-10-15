#include "incl/handlers.h"
#include <LEDA/graph/graph_draw.h>
#include <LEDA/core/array.h>

int main(int argc, char** argv) {
        GraphWin gw;
        if( argc > 1 ){
                graph G;
		list_item it;
		leda::string fname = argv[1];
                char flag1[16];
//                sprintf( flag1, "%s", argv[2] );
                sprintf( flag1, "LEDA" );
		double pi = 2.0 * 3.147;
		double tmp = pi;
		int count = 0,maxLayerIdNow;
//                if( strcmp( flag1, "LEDA" ) == 0 ){
                    gw.read_gml( fname );
                    G = gw.get_graph();
//                }
		double x0, y0, x1, y1;
		gw.get_bounding_box(x0, y0, x1, y1);
		node_array<point> pos( G );
		node_array<double> xpos( G );
		node_array<double> ypos( G );
		node_array<int> neighbors( G );
                node_array<int> indexs( G );
                array<node> nodes( G.number_of_nodes() + 1 );
		node n,adj,source;
		gw.get_position( pos );
		list<node> fixedNodes;
                int max_degree = 0, cnt = 0;
		node maxDegNode;
		forall_nodes( n, G ){
			neighbors[ n ] = G.degree( n );
			if( max_degree < neighbors[ n ] ){
				max_degree = neighbors[ n ];
				maxDegNode = n;
			}
                        indexs[ n ] = cnt;
                        nodes[ cnt ] = n;
                        cnt++;
//                        cout << "|";
		}
//                cout << endl << "D\n";
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
                //SPRING_EMBEDDING( G, fixedNodes, xpos, ypos, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 500 );
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
                list<int> hidedIndexs;
                for(int i = 0; i <= max; i++ ){
                    if( COMPS[ i ].size() < election ){
                        notEnoughComponents.append( i );
                        forall_items( it, COMPS[ i ] ){                            
                            hidedIndexs.append( indexs[ COMPS[ i ][ it ] ] );
                            G.hide_node( COMPS[ i ][ it ] );
                        }
                    }
                }
                if( strcmp( flag1, "LEDA" ) == 0 ){
                    SPRING_EMBEDDING( G, fixedNodes, xpos, ypos, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 1000 );
                }
                else{
                    if( strcmp( flag1, "OUR" ) == 0 ){
                        GRAPH<int,int> H;
                        H.read_gml( fname );
                        node_array<double> xpos2( H );
                        node_array<double> ypos2( H );
                        node_array<int> indexs2( H );
                        node_array<int> nodeId( H, 3 );
                        int cnt = 0;
                        forall_nodes( n, H ){
                            indexs2[ n ] = cnt;
                            cnt++;
                        }

                        forall_nodes( n, H ){
                            bool isHided = false;
                            forall_items( it, hidedIndexs ){
                                if( indexs2[ n ] == hidedIndexs[ it ] ){
                                    isHided = true;
                                }
                            }
                            if( isHided == true )
                                H.hide_node( n );
                        }
                        list<node> fixedNodes2;
                        SPRING_EMBEDDING_our2( H, fixedNodes2, xpos2, ypos2, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 1000, nodeId );
                        forall_nodes( n, H ){
                            xpos[ nodes[ indexs2[ n ] ] ] = xpos2[ n ];
                            ypos[ nodes[ indexs2[ n ] ] ] = ypos2[ n ];
                        }
                    }
                }
                G.restore_all_nodes();
                G.restore_all_edges();
                //cout << " 1 \n";
                double xmin, xmax, ymin, ymax;
                int ncount = 0;
                forall_nodes( n, G ){
                        if( COMPS[ comp[ n ] ].size() >= election ){
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
                //cout << " 2 \n";
                double xpos1 = xmin;
                double ypos1 = ymax + 35.0;
                list<node> justOnes;
                //cout << max << " - " << election << endl;
                for(int i = 0; i <= max; i++ ){
                        //cout << COMPS[ i ].size() << "\t";
                        if( COMPS[ i ].size() < election && COMPS[ i ].size() != 1 ){
                                if( xpos1 < xmax ){
                                    int diam = 30;
                                    if( COMPS[ i ].size() < 3 ){
                                        diam = 10;
                                    }
                                    else{
                                        diam = 10 + COMPS[ i ].size() * 2;
                                    }
                                    circle C( xpos1 + 16, ypos1, diam );
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
                                        ypos1 += 50.0;
                                        int diam = 30;
                                        if( COMPS[ i ].size() < 3 ){
                                            diam = 10;
                                        }
                                        else{
                                            diam = 10 + COMPS[ i ].size();
                                        }
                                        circle C( xpos1 + 16, ypos1, diam );
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
                                xpos1 += 66;
                        }
                }

                forall_nodes( n, G ){
                    if( G.degree( n ) == 0 ){
                        justOnes.append( n );
                    }
                }
                xpos1 = xmin;
                cout << endl << xmin << " | " << xmax << endl;
//                if( xmax - xmin < 400 )
//                    xmax = xmin + 400;
                ypos1 = ymin - 50.0;
                forall_items( it, justOnes ){
                    if( xpos1 < xmax ){
                        xpos[ justOnes[ it ] ] = xpos1;
                        ypos[ justOnes[ it ] ] = ypos1;
                    }
                    else{
                        xpos1 = xmin;
                        ypos1 -= 30.0;
                        xpos[ justOnes[ it ] ] = xpos1;
                        ypos[ justOnes[ it ] ] = ypos1;
                    }
                    xpos1 += 22;
                }

                //cout << "\n 3 \n";
		gw.remove_bends();
		forall_nodes( n, G ){
                        xpos[ n ] = xpos[ n ] * 5.0;
                        ypos[ n ] = ypos[ n ] * 5.0;
		}
		gw.set_position( xpos, ypos );
		gw.place_into_box(x0, y0, x1, y1);
                fname = fname.replace( ".gml", "forceDirectedLayoutUW.gml" );
		gw.save_gml( fname );
	}
	return 0;
}
