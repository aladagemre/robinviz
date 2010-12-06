/*****************************************************************************************/
/*	Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>   	 */
/*											 */
/*    This program is free software: you can redistribute it and/or modify		 */
/*    it under the terms of the GNU General Public License as published by		 */
/*    the Free Software Foundation, either version 3 of the License, or			 */
/*    (at your option) any later version.						 */
/*											 */
/*    This program is distributed in the hope that it will be useful,			 */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of			 */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			 */
/*    GNU General Public License for more details.					 */
/*											 */
/*    You should have received a copy of the GNU General Public License			 */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.		 */
/*											 */
/*    Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.         */
/*    This is free software, and you are welcome to redistribute it                      */
/*    under certain conditions; type `show c' for details.			         */
/*****************************************************************************************/

/*************************************************************************************

runFunctions.h is for general coding. Specifically, functions in this header files
implements
      Obtaining Final Layered Layouts For
	  Main Centered Graphs(RUN_SELF2 and RUN_AGAIN2 to draw the graph )
	  Peripheral Graphs(RUN_SELF and RUN_AGAIN to draw the peripheral graphs )
      Obtaining Force Directed Layout(RUN_FFD)
      

Created

01 January 2009

*************************************************************************************/

#include "springLayout.h"
#include "algorithmwolf.h"
#include "algorithmwolf_m.h"

struct Strings{
	char name[ 128 ];
	int nodeNumber;
};


GRAPH<int,int> RUN_SELFGD(  GRAPH<int,int> &G,
                          array<list<node> > &layers,
                          int W,
                          node_array<double> &Xpos,
                          node_array<double> &Ypos,
                          int graphNo,
                          node_array<point> &posx,
                          edge_array<list<point> > &bendsx,
                          int algorithmFlag,
                          int space,
                          bool xCoordFlag,
                          int increment,
                          bool ledaPostFlag
){

	cout << " We are in now\n";
        GRAPH<int,int> H;

        edge e;
        node n;
        //random_graph( G, noden, edgen );
        int random_walk;

//*********************//
// Give rank assignmen //
//*********************//
//        forall_edges( e, G )
//        {
//                        if( G.source( e ) == G.target( e ) )
//                                        G.del_edge( e );
//        }
//        forall_edges( e, G )
//        {
//                        if( G.source( e ) == G.target( e ) )
//                                        G.del_edge( e );
//        }

        list<edge> edgelist;
        list_item it,it2;
        node_array<bool> marked(G,false);

/*
        forall_edges( e1 ,G ){
                forall_edges( e2, G ){
                        if( G.target( e1 ) == G.target( e2 ) && G.source( e1 ) == G.source( e2 ) && e1 != e2 ){
                                        cout << " " << G[ e1 ] << " - " << G[ e2 ] << endl;
                                        G.del_edge( e2 );
                        }
                }
        }
*/

        list<edge> paralells;
        // Leda function to arrange Graph like removing parallel edges
//        paralells = Make_Simple( G );
//        G.del_edges( paralells );
        list<node> A;
        forall_nodes( n , G ){
                        A.append( n );
        }

        node root;
        forall_nodes( n , G ){
                        root = n;
                        break;
        }

        list<edge> feedback;
        node_array<bool> visited( G, false );
        list<edge> dfsEList;
        list<edge> suspis;
        list<node> Atemp;

        dfs( root, G, visited, A, dfsEList );

/****************************************************************/
/*********************** ACYCLIC PHASE I ************************/
/****************************************************************/
	cout << " We are in now acyclic\n";
        node_array<int> ord(G);
        bool acyclicBool = TOPSORTG(G,ord);
        list<edge> cycleFinder;
        list<edge> reversedEdges;
        H = G;

        edge_array<int> prevEdgeList( G, 0 );
        forall_edges( e, G ){
                        prevEdgeList[ e ] = G[ e ];
        }

        // Catch the cycle and if you can you get the min weighted edge
        // in cylce that removes the cycle when it is deleted.
        while( !acyclicBool ){
                        list_item it;
                        Is_Acyclic( G, cycleEdges );

                        forall_items( it, cycleEdges ){
                                        e = cycleEdges[ it ];
                                        node_array<bool> nvisit( G, false );
                                        edge_array<bool> eonPath( G, false );
                                        cycleFinder = cycleCatcher( G, e, G.source( e ), G.target( e ), eonPath, nvisit );

                                        #ifdef DEBUG
                                                        cycleFinder.print();
                                        #endif

                                                int max = 0;
                                                edge etemp;
                                                forall_items( it, cycleEdges ){
                                                                e = cycleEdges[ it ];
                                                                if( max <= H[ e ] ){
                                                                                max = H[ e ];
                                                                }
                                                }
                                                int min = max;

                                                forall_items( it, cycleFinder ){
                                                                e = cycleFinder[ it ];
                                                                if( min >= H[ e ] ){
                                                                                min = H[ e ];
                                                                                etemp = e;
                                                                }
                                                                #ifdef DEBUG
                                                                                cout << H[ e ] << " - ";
                                                                #endif

                                                }
                                                int c = 0;
                                                forall_items( it, cycleFinder ){
                                                                e = cycleFinder[ it ];
                                                                H[ e ] -= min;
                                                                if( H[ e ] == 0 && inList( suspis , e ) == false ){
                                                                                c = 1;
                                                                                suspis += e;
                                                                                G.hide_edge( e );
                                                                }

                                                                #ifdef DEBUG
                                                                                cout << endl << " suspis - > ";
                                                                                suspis.print();
                                                                                cout << endl;
                                                                #endif

                                                }

                        }
                        #ifdef DEBUG
                                        drawCycleEdges( G, visited, dfsEList, suspis );
                        #endif
                        cycleEdges.clear();
                        acyclicBool = TOPSORTG(G,ord);
        }

/****************************************************************/
/*********************** ACYCLIC PHASE II ***********************/
/****************************************************************/

//-> Restore All edges give the original value to suspicious edges
            G.restore_all_edges();
                forall_edges( e, G ){
                                G[ e ] = prevEdgeList[ e ];
                }
                edge etemp;
//-> Hide again to test that suspis are realy suspicious ones
                G.hide_edges( suspis );


#ifdef DEBUG
                suspis.print();
#endif

                list<edge> popedList;

                END:
                                int max = 0;
                                popedList.clear();
                                int etempChanged = 0;
                                while( !(suspis.empty()) ){
                                                e = suspis.pop();
                                                if( max <= G[ e ] ){
                                                                max = G[ e ];
                                                                etemp = e;
                                                                etempChanged = 1;
                                                }
                                                popedList.append( e );
                                }
                                forall_items(it,popedList ){
                                                e = popedList[ it ];
                                                if( e == etemp )
                                                                ;
                                                else
                                                        suspis.append( e );
                                }
                                if( etempChanged == 1  ){
                                        G.restore_edge( etemp );
                                        etemp = G.rev_edge( etemp );
                                        reversedEdges.append( etemp );
                                }
                                if( Is_Acyclic( G ) != true ){
                                                etemp = G.rev_edge( etemp );
                                                forall_items( it, reversedEdges ){
                                                            e = popedList[ it ];
                                                                if( e == etemp )
                                                                        reversedEdges.del_item( it );
                                                }
                                }
                                if( suspis.empty() == false )
                                                goto END;

#ifdef DEBUG
                cout << endl << "suspis";
                suspis.print();
                cout << endl;
#endif

        G.restore_all_edges();
        cycleEdges.clear();
        array<list<node> > sourceAndTargets( reversedEdges.size() );

        random_walk = 0;
        forall_items( it, reversedEdges ){
                sourceAndTargets[ random_walk ].push_back( G.source( reversedEdges[ it ] ) );
                sourceAndTargets[ random_walk ].push_back( G.target( reversedEdges[ it ] ) );
                random_walk++;
        }

#ifdef DRAW
                drawCycleEdges( G, visited, dfsEList, reversedEdges );
#endif


/*********************** END OF ACYLIC **************************/
/****************************************************************/
#ifdef DEBUG_ROBINVIZ
cout << "\n Acyclic Done \n" ;
#endif
/****************************************************************/
/******************** LAYER ASSIGNMENT **************************/
	cout << " We are in now Layering\n";
        /****** Transitive Edges Removal **************/

                list<edge> transitives;
                //do_call( G, transitives );

#ifdef TRANSITIVES
                transitives.print();
#endif

                //G.hide_edges( transitives );
                //drawCycleEdges( G, visited, dfsEList, suspis );

        /****** End of Transitive Edges Removal *******/
        /****** Coffman Graham Ranking Phase 1 ********/
                node_array<int> ready( G,0 );

                CGwithPromotionMinWidthW( G,ready, W );
                //CGwithPromotionMinWidthW( G, ready, W );

        /****** Coffman Graham Ranking Phase 1 End ****/
        /****** Coffman Graham Ranking Phase 2 ********/
                int count = 0;

                max = 0;
                forall_nodes( n , G ){

#ifdef COUNT
                        cout << endl << "- " << count << " :" << n << "  "<< ready[ n ] << endl;
#endif
                        count++;
                        if( max < ready[ n ] )
                                        max = ready[ n ];
                }

                array<list<node> > Layers( max + 1 );
                array<list<node> > LayersTemp( max + 1 );

                forall_nodes( n , G ){
                                Layers[ max - ready[ n ] ].append( n );
                }
                //draw_new( G, Layers, max );
                G.restore_all_edges();
                //draw_new( G, Layers, max );
                LayersTemp = Layers;

        /****** Coffman Graham Ranking Phase 2 End ****/

/******************** END OF LAYER ASSIGNMENT *******************/
/****************************************************************/

#ifdef DEBUG_ROBINVIZ
cout << "\n Layering Done \n" ;
#endif
/****************************************************************/
/******************** ADD DUMMY VERTICES ************************/
		cout << " We are in now dummy\n";
                node target,source;
                edge_array<bool> eMarked( G, false );
                list<edge> allEdges;
                list<node> dummyNodes;

                allEdges = G.all_edges();
                forall_items( it, allEdges ){
                                e = allEdges[ it ];
                                if( ( max - ready[ G.source( e ) ] ) - 1 > ( max - ready[ G.target( e ) ] ) && eMarked[ e ] == false ){

                                                int sourceLayer = max - ready[ G.source( e ) ];
                                                int targetLayer = max - ready[ G.target( e ) ];
                                                target = G.source( e );
                                                source = G.target( e );

                                                int etempx = G[ e ];
                                                G.del_edge( e );

                                                int x = 0;
                                                while( sourceLayer - 1 != targetLayer ){
                                                                n = G.new_node();
                                                                dummyNodes.append( n );
                                                                e = G.new_edge( target, n );
                                                                if( x % 2 == 0 )
                                                                        Layers[ sourceLayer - 1 ].push_back( n );
                                                                else
                                                                        Layers[ sourceLayer - 1 ].push_front( n );

                                                                sourceLayer--;
                                                                G[ e ] = etempx;
                                                                eMarked.init( G );
                                                                eMarked[ e ] = true;
                                                                target = n;
                                                                x++;
                                                }
                                                e = G.new_edge( n, source );
                                                G[ e ] = etempx;
                                                eMarked.init( G );
                                                eMarked[ e ] = true;
                                }
                                else{
                                                eMarked[ e ] = true;
                                }
                }

/******************** END OF ADDING DUMMY VERTICES **************/
/****************************************************************/

#ifdef DEBUG_ROBINVIZ
cout << "\n Dummy Adding Done \n" ;
#endif
/****************************************************************/
/********************* CROSSING REDUCTION ***********************/
		cout << " We are in now crossing\n";
                array<list<node> > Layers2( max + 1 );
                Layers2 = Layers;

                integer cross_count = 0;
                /*for( int i = max; i > 0; i-- ){
                        cross_count += crossing_numberG( G, Layers[ i ], Layers[ i - 1 ] );
                }*/
                //cout << " Crossings  Before : " << cross_count << endl;
                cross1 = cross_count;
// 		G.rev_all_edges();
// /*		//1st Way
// 		for( int i = max; i > 0 && i - 2 > 0; i-=2 ){
// 			wolf( G, Layers[ i - 1 ], Layers[ i ] );
// 		}
// */
// 		//2nd Way
// 		/*for( int i = max; i > 0; i-- ){
// 			wolfBarycenter( G, Layers[ i - 1 ], Layers[ i ] );
// 		}*/
//
// 		for( int i = max; i > 0; i-- ){
// 			median_graph( G, Layers[ i - 1 ], Layers[ i ] );
// 		}
// 		/*for( int i = max; i > 0; i-- ){
// 			median_graph( G, Layers2[ i - 1 ], Layers2[ i ] );
// 		}*/
//
// /*		//3rd Way
// 		for( int i = max; i > 0; i-- ){
// 			wolf( G, Layers[ i ], Layers[ i - 1 ] );
// 		}
// */
// /*		//4th Way
// 		for( int i = max; i > 0 && i - 2 > 0; i-=2 ){
// 			wolf( G, Layers[ i ], Layers[ i - 1 ] );
// 		}
// */
// 		G.rev_all_edges();

                for( int repeat_i = 0; repeat_i < 40 ; repeat_i++ ){
                      G.rev_all_edges();
                      //1st Way
                      for( int i = Layers.size()-1; i > 0; i-- ){
                          wolf( G, Layers[ i - 1 ], Layers[ i ] );
                      }
                      G.rev_all_edges();
                      //2nd Way
                      for( int i = 0; i < Layers.size()-1; i++ ){
                          wolf( G, Layers[ i + 1 ], Layers[ i ] );
                      }
                      G.rev_all_edges();
                      G.rev_all_edges();
                }

                cross_count = 0;
// 		for( int i = max; i > 0; i-- ){
// 			cross_count += crossing_numberG( G, Layers[ i ], Layers[ i - 1 ] );
// 		}
// // 		cout << " Crossings  After Wolf: " << cross_count << endl;
// 		cross2 = cross_count;
//
// 		cross_count = 0;
// 		for( int i = max; i > 0; i-- ){
// 			cross_count += crossing_numberG( G, Layers2[ i ], Layers2[ i - 1 ] );
// 		}
// // 		cout << " Crossings  After Median: " << cross_count << endl;
// 		cross3 = cross_count;
// 		cout << endl;

/********************* END OF CROSSING REDUCTION ****************/
/****************************************************************/
#ifdef DEBUG_ROBINVIZ
        cout << "\n Crossing Done \n";
#endif
                GraphWin gw(G);

                node_array<double> xpos(G);
                node_array<double> ypos(G);
// 		forall_nodes( n, G ){
// 			cout << G[ n ] << " - " << xpos[ n ] << " - " << ypos[ n ] << endl;
// 		}
                node_array<int> pos_in_layer( G, 0 );
                node_array<int> assigned_rank( G, 0 );
                node_array<int> width( G, 10 );
                node_array<bool> is_dummy( G, false );

                forall_items( it, dummyNodes ){
                    is_dummy[ dummyNodes[ it ] ] = true;
                }
                for( int i = 0; i < Layers.size(); i++ ){
                    count = 0;
                    forall_items( it, Layers[ i ] ){
                        pos_in_layer[ Layers[ i ][ it ] ] = count;
                        assigned_rank[ Layers[ i ][ it ] ] = i;
                        count++;
                    }
                }

		if( xCoordFlag == true ){
                    identify_dummy_positionsOur( G, Layers, max, dummyNodes, xpos, ypos, algorithmFlag, space, increment, ledaPostFlag );
                }
                else{
                    identify_dummy_positionsX( G, Layers, max, dummyNodes, xpos, ypos, algorithmFlag, space, increment, ledaPostFlag );
                }
// 		forall_nodes( n, G ){
// 			cout << G[ n ] << " - " << xpos[ n ] << " - " << ypos[ n ] << endl;
// 		}
                node_array<bool> isDummy( G, false );
                node_array<bool> isUsed( G, false );

                node m,temp;
                forall_nodes( n , G ){
                                forall_items( it, dummyNodes ){
                                                m = dummyNodes[ it ];
                                                if( m == n )
                                                        isDummy[ n ] = true;
                                }
// 				cout << xpos[ n ] << " - " << ypos[ n ] << endl;
                }
// 		cout << endl;

                for( int i = 0; i < sourceAndTargets.size(); i++ ){
                        forall_edges( e, G ){
                                if( G.source( e ) == sourceAndTargets[ i ][ sourceAndTargets[ i ].first_item() ] &&
                                    G.target( e ) == sourceAndTargets[ i ][ sourceAndTargets[ i ].last_item() ]){
                                        e = G.rev_edge( e );
// 					cout << " REVERSED \n";
                                }
                        }
                }

                edge_array<list<double> > edgePositionsX( G );
                edge_array<list<double> > edgePositionsY( G );
                edge_array<list<point> > bends(G);
                list<point> tempPoints;

                list<edge> inEdges;

                forall_nodes( n , G ){
                                node source;
                                node target;
                                int edgeValue;
                                //IS dummy ancak ondan onceki node source olacakti


                                if( isUsed[ n ] == false && isDummy[ n ] == true ){
                                                list<double> eXpos;
                                                list<double> eYpos;

                                                temp = n;

                                                eXpos.append( xpos[ temp ] );
                                                eYpos.append( ypos[ temp ] );
                                                point p( xpos[ temp ], ypos[ temp ] );
                                                tempPoints.append( p );

                                                inEdges = G.in_edges( temp );
                                                forall_items( it, inEdges ){
                                                                e = allEdges[ it ];
                                                                n = G.source( e );
                                                }
                                                isUsed[ n ] = true;
                                                source = n;

                                                NEXTDUMMY:
                                                allEdges = G.out_edges( temp );
                                                edge etemp;
                                                forall_items( it, allEdges ){
                                                                e = allEdges[ it ];
                                                                m = G.target( e );
                                                                eXpos.append( xpos[ m ] );
                                                                eYpos.append( ypos[ m ] );
                                                                point p( xpos[ m ], ypos[ m ] );
                                                                tempPoints.append( p );

                                                                isUsed[ m ] = true;
                                                                edgeValue = G[ e ];
                                                                G.del_edge( e );
                                                }
                                                if( isDummy[ m ] == true ){
                                                                temp = m;
                                                                goto NEXTDUMMY;
                                                }
                                                else{
                                                                target = m;
                                                }
                                                etemp = G.new_edge( source, target );
                                                G[ etemp ] = edgeValue;
                                                bends[ etemp ] = tempPoints;
                                                edgePositionsX.init( G );
                                                edgePositionsY.init( G );
                                                edgePositionsX[ etemp ] = eXpos;
                                                edgePositionsY[ etemp ] = eYpos;
#ifdef POSITIONS
                                                cout << endl << " X : ";
                                                edgePositionsX[ etemp ].print();
                                                cout << endl << " Y : ";
                                                edgePositionsY[ etemp ].print();
#endif
                                                tempPoints.clear();
                                }
                                isUsed[ n ] = true;
                }

                G.del_nodes( dummyNodes );
                node_array<point> pos(G);
                forall_nodes( n, G ){
// 			cout <<  xpos[ n ]<< "-" << ypos[ n ] << endl;
                        point p( xpos[ n ], ypos[ n ] );
                        pos[ n ] = p;
                }

		cout << " We are in now final process\n";
                //runWithMenu( G );
                layers = Layers;
                if( xCoordFlag == true )
                        pos = draw_final3( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, graphNo, xpos, ypos );
                else
                        draw_finalX( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, graphNo, pos );

                posx = pos;
                bendsx = bends;
                return G;
}


/* Function is based for Weighted Layered Drawing, 
proposed in ISB'10. Each step considers weight.
Inputs are straightforward, for the ISB paper,
this procedure calculates the peripheral graphs.
Note that for x-coordinate case we are
using python codes embeded into this procedure's
sub-functions */
GRAPH<int,int> RUN_SELF(  GRAPH<int,int> &G, 
			  array<list<node> > &layers, 
			  int W, 
			  node_array<double> &Xpos, 
			  node_array<double> &Ypos, 
			  int graphNo, 
			  node_array<point> &posx,
			  edge_array<list<point> > &bendsx, 
			  int algorithmFlag, 
			  int space, 
			  bool xCoordFlag, 
			  int increment,
			  bool ledaPostFlag 
){

	GRAPH<int,int> H;

	edge e;
	node n;
	//random_graph( G, noden, edgen );
	int random_walk;

//*********************//
// Give rank assignmen //
//*********************//
	forall_edges( e, G )
	{
			if( G.source( e ) == G.target( e ) )
					G.del_edge( e );
	}
	forall_edges( e, G )
	{
			if( G.source( e ) == G.target( e ) )
					G.del_edge( e );
	}

	list<edge> edgelist;
	list_item it,it2;
	node_array<bool> marked(G,false);

	list<edge> paralells;
	// Leda function to arrange Graph like removing parallel edges
	paralells = Make_Simple( G );
	G.del_edges( paralells );
	list<node> A;
	forall_nodes( n , G ){
			A.append( n );
	}

	node root;
	forall_nodes( n , G ){
			root = n;
			break;
	}

	list<edge> feedback;
	node_array<bool> visited( G, false );
	list<edge> dfsEList;
	list<edge> suspis;
	list<node> Atemp;

	dfs( root, G, visited, A, dfsEList );

/****************************************************************/
/*********************** ACYCLIC PHASE I ************************/
/****************************************************************/

	node_array<int> ord(G);
	bool acyclicBool = TOPSORTG(G,ord);
	list<edge> cycleFinder;
	list<edge> reversedEdges;
	H = G;

	edge_array<int> prevEdgeList( G, 0 );
	forall_edges( e, G ){
			prevEdgeList[ e ] = G[ e ];
	}

	// Catch the cycle and if you can you get the min weighted edge 
	// in cylce that removes the cycle when it is deleted.
	while( !acyclicBool ){
			list_item it;
			Is_Acyclic( G, cycleEdges );		
			 				
			forall_items( it, cycleEdges ){
					e = cycleEdges[ it ];
					node_array<bool> nvisit( G, false );
					edge_array<bool> eonPath( G, false );
					cycleFinder = cycleCatcher( G, e, G.source( e ), G.target( e ), eonPath, nvisit );
			
					#ifdef DEBUG	
							cycleFinder.print();
					#endif

						int max = 0;
						edge etemp;
						forall_items( it, cycleEdges ){
								e = cycleEdges[ it ];
								if( max <= H[ e ] ){
										max = H[ e ];
								}
						}
						int min = max;

						forall_items( it, cycleFinder ){
								e = cycleFinder[ it ];
								if( min >= H[ e ] ){
										min = H[ e ];
										etemp = e;
								}
								#ifdef DEBUG
										cout << H[ e ] << " - ";
								#endif

						}
						int c = 0;
						forall_items( it, cycleFinder ){
								e = cycleFinder[ it ];
								H[ e ] -= min; 
								if( H[ e ] == 0 && inList( suspis , e ) == false ){
										c = 1;
										suspis += e;
										G.hide_edge( e );
								}

								#ifdef DEBUG
										cout << endl << " suspis - > ";
										suspis.print();
										cout << endl;
								#endif

						}
						
			}
			#ifdef DEBUG
					drawCycleEdges( G, visited, dfsEList, suspis );
			#endif
			cycleEdges.clear();
			acyclicBool = TOPSORTG(G,ord);
	}

/****************************************************************/
/*********************** ACYCLIC PHASE II ***********************/
/****************************************************************/

//-> Restore All edges give the original value to suspicious edges
	    G.restore_all_edges();
		forall_edges( e, G ){
				G[ e ] = prevEdgeList[ e ];
		}
		edge etemp;
//-> Hide again to test that suspis are realy suspicious ones
		G.hide_edges( suspis );


#ifdef DEBUG
		suspis.print();
#endif

		list<edge> popedList;

		END:
				int max = 0;
				popedList.clear();
				int etempChanged = 0;
				while( !(suspis.empty()) ){
						e = suspis.pop();
						if( max <= G[ e ] ){
								max = G[ e ];
								etemp = e;
								etempChanged = 1;
						}
						popedList.append( e );
				}
				forall_items(it,popedList ){
						e = popedList[ it ];
						if( e == etemp )
								;
						else
							suspis.append( e );
				}
				if( etempChanged == 1  ){
					G.restore_edge( etemp );
					etemp = G.rev_edge( etemp );
					reversedEdges.append( etemp );
				}
				if( Is_Acyclic( G ) != true ){
						etemp = G.rev_edge( etemp );
						forall_items( it, reversedEdges ){
							    e = popedList[ it ];
								if( e == etemp )
									reversedEdges.del_item( it );
						}
				}
				if( suspis.empty() == false )
						goto END;
				
#ifdef DEBUG
		cout << endl << "suspis";
		suspis.print();
		cout << endl;
#endif
	
	G.restore_all_edges();
	cycleEdges.clear();
	array<list<node> > sourceAndTargets( reversedEdges.size() );
	
	random_walk = 0;
	forall_items( it, reversedEdges ){
		sourceAndTargets[ random_walk ].push_back( G.source( reversedEdges[ it ] ) );
		sourceAndTargets[ random_walk ].push_back( G.target( reversedEdges[ it ] ) );
		random_walk++;
	}

#ifdef DRAW
		drawCycleEdges( G, visited, dfsEList, reversedEdges );
#endif


/*********************** END OF ACYLIC **************************/
/****************************************************************/
#ifdef DEBUG_ROBINVIZ		
cout << "\n Acyclic Done \n" ;
#endif
/****************************************************************/
/******************** LAYER ASSIGNMENT **************************/

	/****** Transitive Edges Removal **************/

		list<edge> transitives;
		//do_call( G, transitives );

#ifdef TRANSITIVES
		transitives.print();
#endif

		//G.hide_edges( transitives );
		//drawCycleEdges( G, visited, dfsEList, suspis );
	
	/****** End of Transitive Edges Removal *******/
	/****** Coffman Graham Ranking Phase 1 ********/
		node_array<int> ready( G,0 );

		CGwithPromotionMinWidthW( G,ready, W );
		//CGwithPromotionMinWidthW( G, ready, W );

	/****** Coffman Graham Ranking Phase 1 End ****/
	/****** Coffman Graham Ranking Phase 2 ********/
		int count = 0;

		max = 0;
		forall_nodes( n , G ){

#ifdef COUNT
			cout << endl << "- " << count << " :" << n << "  "<< ready[ n ] << endl;  
#endif
			count++;
			if( max < ready[ n ] )
					max = ready[ n ];
		}

		array<list<node> > Layers( max + 1 );
		array<list<node> > LayersTemp( max + 1 );

		forall_nodes( n , G ){
				Layers[ max - ready[ n ] ].append( n );
		}
		//draw_new( G, Layers, max );
		G.restore_all_edges();
		//draw_new( G, Layers, max );
		LayersTemp = Layers;

	/****** Coffman Graham Ranking Phase 2 End ****/

/******************** END OF LAYER ASSIGNMENT *******************/
/****************************************************************/

#ifdef DEBUG_ROBINVIZ        
cout << "\n Layering Done \n" ;
#endif
/****************************************************************/
/******************** ADD DUMMY VERTICES ************************/
		node target,source;
		edge_array<bool> eMarked( G, false );
		list<edge> allEdges;
		list<node> dummyNodes;

		allEdges = G.all_edges();
		forall_items( it, allEdges ){
				e = allEdges[ it ];
				if( ( max - ready[ G.source( e ) ] ) - 1 > ( max - ready[ G.target( e ) ] ) && eMarked[ e ] == false ){
						
						int sourceLayer = max - ready[ G.source( e ) ];
						int targetLayer = max - ready[ G.target( e ) ];
						target = G.source( e );
						source = G.target( e );

						int etempx = G[ e ];
						G.del_edge( e );
						
						int x = 0;
						while( sourceLayer - 1 != targetLayer ){
								n = G.new_node();
								dummyNodes.append( n );
								e = G.new_edge( target, n );
								if( x % 2 == 0 )
									Layers[ sourceLayer - 1 ].push_back( n );
								else
									Layers[ sourceLayer - 1 ].push_front( n );

								sourceLayer--;
								G[ e ] = etempx;
								eMarked.init( G );
								eMarked[ e ] = true;
								target = n;
								x++;
						}
						e = G.new_edge( n, source );
						G[ e ] = etempx;
						eMarked.init( G );
						eMarked[ e ] = true;
				}
				else{
						eMarked[ e ] = true;
				}
		}

/******************** END OF ADDING DUMMY VERTICES **************/
/****************************************************************/

#ifdef DEBUG_ROBINVIZ
cout << "\n Dummy Adding Done \n" ;
#endif
/****************************************************************/
/********************* CROSSING REDUCTION ***********************/

		array<list<node> > Layers2( max + 1 );
		Layers2 = Layers;

		integer cross_count = 0;
		cross1 = cross_count; 
		
		for( int repeat_i = 0; repeat_i < 20 ; repeat_i++ ){
		      G.rev_all_edges();
		      //1st Way
		      for( int i = Layers.size()-1; i > 0; i-- ){
			  wolf( G, Layers[ i - 1 ], Layers[ i ] );
		      }
		      G.rev_all_edges();
		      //2nd Way
		      for( int i = 0; i < Layers.size()-1; i++ ){
			  wolf( G, Layers[ i + 1 ], Layers[ i ] );
		      }
		      G.rev_all_edges();
		      G.rev_all_edges();
		}

		cross_count = 0;

/********************* END OF CROSSING REDUCTION ****************/
/****************************************************************/
#ifdef DEBUG_ROBINVIZ
        cout << "\n Crossing Done \n";
#endif
		GraphWin gw(G);

		node_array<double> xpos(G);
		node_array<double> ypos(G);
// 		forall_nodes( n, G ){
// 			cout << G[ n ] << " - " << xpos[ n ] << " - " << ypos[ n ] << endl;
// 		}
		node_array<int> pos_in_layer( G, 0 );
		node_array<int> assigned_rank( G, 0 );
		node_array<int> width( G, 10 );
		node_array<bool> is_dummy( G, false );

		forall_items( it, dummyNodes ){
		    is_dummy[ dummyNodes[ it ] ] = true;
		}
		for( int i = 0; i < Layers.size(); i++ ){
		    count = 0;   
		    forall_items( it, Layers[ i ] ){
			pos_in_layer[ Layers[ i ][ it ] ] = count;
			assigned_rank[ Layers[ i ][ it ] ] = i;
			count++;
		    }
		}

        if( xCoordFlag == true ){
		    identify_dummy_positionsOur( G, Layers, max, dummyNodes, xpos, ypos, algorithmFlag, space, increment, ledaPostFlag );
		}
		else{
		    identify_dummy_positionsX( G, Layers, max, dummyNodes, xpos, ypos, algorithmFlag, space, increment, ledaPostFlag );
		}
// 		forall_nodes( n, G ){
// 			cout << G[ n ] << " - " << xpos[ n ] << " - " << ypos[ n ] << endl;
// 		}	
		node_array<bool> isDummy( G, false );
		node_array<bool> isUsed( G, false );

		node m,temp;
		forall_nodes( n , G ){
				forall_items( it, dummyNodes ){
						m = dummyNodes[ it ];
						if( m == n )
							isDummy[ n ] = true;
				}
// 				cout << xpos[ n ] << " - " << ypos[ n ] << endl;
		}
// 		cout << endl;

		for( int i = 0; i < sourceAndTargets.size(); i++ ){
			forall_edges( e, G ){
				if( G.source( e ) == sourceAndTargets[ i ][ sourceAndTargets[ i ].first_item() ] &&
				    G.target( e ) == sourceAndTargets[ i ][ sourceAndTargets[ i ].last_item() ]){
					e = G.rev_edge( e );
// 					cout << " REVERSED \n";
				}
			}
		}

		edge_array<list<double> > edgePositionsX( G );
		edge_array<list<double> > edgePositionsY( G );
		edge_array<list<point> > bends(G);
		list<point> tempPoints;

		list<edge> inEdges;

		forall_nodes( n , G ){
				node source;
				node target;
				int edgeValue;
				//IS dummy ancak ondan onceki node source olacakti


				if( isUsed[ n ] == false && isDummy[ n ] == true ){
						list<double> eXpos;
						list<double> eYpos;
						
						temp = n;
						
						eXpos.append( xpos[ temp ] );
						eYpos.append( ypos[ temp ] );
						point p( xpos[ temp ], ypos[ temp ] );
						tempPoints.append( p );

						inEdges = G.in_edges( temp );
						forall_items( it, inEdges ){
								e = allEdges[ it ];	
								n = G.source( e );
						}
						isUsed[ n ] = true;
						source = n;
						
						NEXTDUMMY:
						allEdges = G.out_edges( temp );
						edge etemp;
						forall_items( it, allEdges ){
								e = allEdges[ it ];	
								m = G.target( e );
								eXpos.append( xpos[ m ] );
								eYpos.append( ypos[ m ] );
								point p( xpos[ m ], ypos[ m ] );
								tempPoints.append( p );

								isUsed[ m ] = true;
								edgeValue = G[ e ];
								G.del_edge( e );
						}
						if( isDummy[ m ] == true ){
								temp = m;
								goto NEXTDUMMY;
						}
						else{
								target = m;
						}
						etemp = G.new_edge( source, target );
						G[ etemp ] = edgeValue;
						bends[ etemp ] = tempPoints;
						edgePositionsX.init( G );
						edgePositionsY.init( G );
						edgePositionsX[ etemp ] = eXpos;
						edgePositionsY[ etemp ] = eYpos;
#ifdef POSITIONS
						cout << endl << " X : ";
						edgePositionsX[ etemp ].print();
						cout << endl << " Y : ";
						edgePositionsY[ etemp ].print();
#endif
						tempPoints.clear();
				}
				isUsed[ n ] = true;
		}

		G.del_nodes( dummyNodes );
		node_array<point> pos(G);
		forall_nodes( n, G ){
// 			cout <<  xpos[ n ]<< "-" << ypos[ n ] << endl;
			point p( xpos[ n ], ypos[ n ] );
			pos[ n ] = p;
		}

		//runWithMenu( G );
		layers = Layers;
		if( xCoordFlag == true )
			pos = draw_final3( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, graphNo, xpos, ypos );
		else
			draw_finalX( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, graphNo, pos );

		posx = pos;
		bendsx = bends;
		
		return G;
}

/* Function is based for Force Directed Layout 
Inputs are straightforward, this procedure 
calculates the peripheral graphs */
GRAPH<int,int> RUN_FFD(  GRAPH<int,int> &G, 
			  array<list<node> > &layers, 
			  int W, 
			  node_array<double> &Xpos, 
			  node_array<double> &Ypos, 
			  int graphNo, 
			  node_array<point> &posx,
			  edge_array<list<point> > &bendsx, 
			  int algorithmFlag, 
			  int space, 
			  bool xCoordFlag, 
			  int increment,
			  bool ledaPostFlag,
			  array<char> &abbv,
			  int cat_num,
			  array<char> &Categories
){	
	edge e;
	node n;
	int random_walk;
	node_array<double> xpos( G, 0 );
	node_array<double> ypos( G, 0 );
//*********************//
// Give rank assignmen //
//*********************//
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	node_array<int> nodeid( G, 3 );
	if( G.number_of_nodes() < 30 )
		SPRING_EMBEDDING_( G, xpos, ypos, 0, 2000 ,0, 1000, 250, nodeid, abbv, cat_num, Categories );
	else
		SPRING_EMBEDDING_( G, xpos, ypos, 0, 2000 + G.number_of_nodes()*13 ,0, 1000 + G.number_of_nodes()*5, 250, nodeid, abbv, cat_num, Categories );

	Xpos = xpos;
	Ypos = ypos;

	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
	return G;
}

/* Function is based for Force Directed Layout
Inputs are straightforward, this procedure
calculates the peripheral graphs */
GRAPH<int,int> RUN_SPRING_EMBEDDER(  GRAPH<int,int> &G,
                                      node_array<double> &Xpos,
                                      node_array<double> &Ypos,
                                      node_array<point> &posx,
                                      int node_width,
                                      int graphNo )
{
    list_item it;
    edge e;

    double pi = 2.0 * 3.147;
    double tmp = pi;
    int count = 0,maxLayerIdNow;
    double x0, y0, x1, y1;

    node_array<int> neighbors( G );
    node_array<int> indexs( G );
    node_array<point> pos( G );
    node_array<double> xpos( G );
    node_array<double> ypos( G );
    GRAPH<int,int> H = G;
    node_array<double> xpos2( H );
    node_array<double> ypos2( H );
    node_array<int> indexs2( H );
    node_array<int> nodeId( H, 3 );
    array<node> nodes( G.number_of_nodes() + 1 );
    node n,adj,source;
    list<node> fixedNodes;

    // FINDING COMPONENTS using LEDA function
    node_array<int> comp( G, 0 );
    COMPONENTS( G, comp );
    int max = 0;
    forall_nodes( n, G ){
        if( max < comp[ n ] )
            max = comp[ n ];
    }
    array<list<node> > COMPS( max + 1 );

    int cnt = 0;
    forall_nodes( n, G ){
        indexs[ n ] = cnt;
        nodes[ cnt ] = n;
        COMPS[ comp[ n ] ].append( n );
        cnt++;
    }
    list<int> notEnoughComponents;
    int maxComp = 0;
    for(int i = 0; i <= max; i++ ){
        if( COMPS[ i ].size() > maxComp ){
            maxComp = COMPS[ i ].size();
        }
    }
//    cout << " -1 \n";
    int election = 10;
    if( maxComp < election )
        election = maxComp;


    // Smaller component should be hided before spring algorithm in order
    // to obtain good visuals.
    list<int> hidedIndexs;
    for(int i = 0; i <= max; i++ ){
        if( COMPS[ i ].size() < election ){
            notEnoughComponents.append( i );
            forall_items( it, COMPS[ i ] ){
                hidedIndexs.append( indexs[ COMPS[ i ][ it ] ] );
//                cout << indexs[ n ] << "-";
                G.hide_node( COMPS[ i ][ it ] );
            }
        }
    }

//    cout << endl << "___________________" << endl;
//    cout << " 0 \n";


    cnt = 0;
    forall_nodes( n, H ){
        indexs2[ n ] = cnt;
        cnt++;
    }

//    cout << " 0 \n";
    forall_nodes( n, H ){
        bool isHided = false;
        forall_items( it, hidedIndexs ){
            if( indexs2[ n ] == hidedIndexs[ it ] ){
                isHided = true;
            }
        }
        if( isHided == true ){
            H.hide_node( n );
//            cout << indexs2[ n ] << "-";
        }
    }
//    cout << endl;
//    cout << " 0 \n";
    list<node> fixedNodes2;
    if( G.number_of_edges() < 5 )
        SPRING_EMBEDDING_our2( H, fixedNodes2, xpos2, ypos2, 0, 800 ,0, 600, 1000, nodeId );
    else
        SPRING_EMBEDDING_our2( H, fixedNodes2, xpos2, ypos2, 0, 2000 + G.number_of_nodes()*13 ,0, 1000 + G.number_of_nodes()*5, 1000, nodeId );
//    cout << " 0 \n";
    forall_nodes( n, H ){
        xpos[ nodes[ indexs2[ n ] ] ] = xpos2[ n ];
        ypos[ nodes[ indexs2[ n ] ] ] = ypos2[ n ];
//        point p( xpos2[ n ], ypos2[ n ] );
//        cout << p << " - " << H.degree( n ) << endl;
    }

//    cout << "\n_____________________\n";
//    cout << " G " << G.number_of_nodes() << " - " << G.number_of_edges() << endl;
//    cout << "\n_____________________\n";
//    cout << " H " << H.number_of_nodes() << " - " << H.number_of_edges() << endl;
//    cout << "\n_____________________\n";
//    cout << " 0.5 \n";
    double averageDistance = 0;
    forall_edges( e, G ){
        point p( xpos[ G.source( e ) ], ypos[ G.source( e ) ] );
        point q( xpos[ G.target( e ) ], ypos[ G.target( e ) ] );
        averageDistance += p.distance( q );
    }
    averageDistance = averageDistance / (double)(G.number_of_edges());
    if( node_width > averageDistance )
        averageDistance = node_width;
    G.restore_all_nodes();
    G.restore_all_edges();

    list<node> justOnes;
    // and handle self nodes
    forall_nodes( n, G ){
        if( G.degree( n ) == 0 ){
            justOnes.append( n );
            G.hide_node( n );
        }
    }
    // Discover the new layout, find limits
    double xmin=0.0, xmax=200.0, ymin=0.0, ymax=200.0;
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
//    cout << " 2 \n";

    // Handle smaller components that have not been placed yet.
    xmax += node_width;
    double xpos1 = xmin - node_width;
    double ypos1 = ymax + node_width;
    int maxDiam = 0;
    //cout << max << " - " << election << endl;
    for(int i = 0; i <= max; i++ ){
            //cout << COMPS[ i ].size() << "\t";
            int diam = 30;
            if( COMPS[ i ].size() < election && COMPS[ i ].size() > 1 ){
                    if( xpos1 < xmax ){
                        if( COMPS[ i ].size() < 3 ){
                            diam = averageDistance / 3.0;
                        }
                        else{
                            diam = (COMPS[ i ].size() / averageDistance ) * 10.0 + averageDistance / 3.0;
                        }
                        if( maxDiam < diam )
                            maxDiam = diam;
                        circle C( xpos1 + diam, ypos1, diam );
                        double min = pi / (double)COMPS[ i ].size();
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
                            xpos1 = xmin - node_width;
                            ypos1 += maxDiam + node_width;
                            maxDiam = 0;
                            int diam = 30;
                            if( COMPS[ i ].size() < 3 ){
                                diam = averageDistance / 3.0;
                            }
                            else{
                                diam = (COMPS[ i ].size() / averageDistance ) * 10.0 + averageDistance / 3.0;
                            }
                            if( maxDiam < diam )
                                maxDiam = diam;
                            circle C( xpos1 + diam, ypos1, diam );
                            double min = pi / (double)COMPS[ i ].size();
                            tmp = pi;
                            forall_items( it, COMPS[ i ] ){
                                pos[ COMPS[ i ][ it ]  ] = C.point_on_circle( tmp );
                                //cout << pos[ COMPS[ i ][ it ]  ] << "\t";
                                xpos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].xcoord();
                                ypos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].ycoord();
                                tmp -= min;
                            }
                    }
                    xpos1 += diam * 2 + 25.0;
            }
    }
    G.restore_all_nodes();
    G.restore_all_edges();

//            cout << endl << xmin << " | " << xmax << endl;
//                if( xmax - xmin < 400 )
//                    xmax = xmin + 400;
    ypos1 = ymin - node_width;
    if( G.number_of_edges() < 10 ){
        averageDistance = averageDistance;
        xmin -= G.number_of_edges() * 2 * node_width;
        xmax += G.number_of_edges() * 2 * node_width;
    }
    else
        averageDistance = averageDistance / 3;

    xpos1 = xmin;
    forall_items( it, justOnes ){
        if( xpos1 < xmax ){
            xpos[ justOnes[ it ] ] = xpos1;
            ypos[ justOnes[ it ] ] = ypos1;
        }
        else{
            xpos1 = xmin;
            ypos1 -= averageDistance;
            xpos[ justOnes[ it ] ] = xpos1;
            ypos[ justOnes[ it ] ] = ypos1;
        }
        xpos1 += averageDistance;
    }

    forall_nodes( n, G ){
            pos[ n ] = point( xpos[n], ypos[n]);
    }

    Xpos = xpos;
    Ypos = ypos;
    forall_nodes( n, G ){
            Xpos[ n ] = Xpos[ n ] * 3.4;
            Ypos[ n ] = Ypos[ n ] * 1.8;
            point p( Xpos[ n ], Ypos[ n ] );
            posx[ n ] = p;
    }
    if( graphNo != -1 ){
        char filename_[64] = "outputs/graphs/graph_";
        sprintf( filename_, "%s%d%s", filename_, graphNo, ".gml" );
        G.write_gml( filename_ );
    }
    return G;
}


/* Function is based for Force Directed Layout 
Inputs are straightforward, this procedure 
calculates the peripheral graphs */
GRAPH<int,int> RUN_FFDANDCIRCLE(  GRAPH<int,int> &G, 
			  array<list<node> > &layers, 
			  int W, 
			  node_array<double> &Xpos, 
			  node_array<double> &Ypos, 
			  int graphNo, 
			  node_array<point> &posx,
			  edge_array<list<point> > &bendsx, 
			  int algorithmFlag, 
			  int space, 
			  bool xCoordFlag, 
			  int increment,
			  bool ledaPostFlag,
			  array<char> &abbv,
			  int cat_num,
			  int node_width,
			  int minRadius
){	
	edge e;
	node n;
	int random_walk;
	node_array<double> xpos( G, 0 );
	node_array<double> ypos( G, 0 );
//*********************//
// Give rank assignmen //
//*********************//
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	node_array<int> nodeid( G, 3 );
	if( G.number_of_nodes() < 30 )
		SPRING_EMBEDDING_( G, xpos, ypos, 0, 2000 ,0, 1000, 250, nodeid, abbv, cat_num );
	else
		SPRING_EMBEDDING_( G, xpos, ypos, 0, 2000 + G.number_of_nodes()*13 ,0, 1000 + G.number_of_nodes()*5, 250, nodeid, abbv, cat_num );

	Xpos = xpos;
	Ypos = ypos;

	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
	
	double xmean=0,ymean=0;
	double xmax=xpos[G.first_node()],ymax=ypos[G.first_node()],xmin=xpos[G.first_node()],ymin=ypos[G.first_node()];
	forall_nodes( n, G ){
		xmean += xpos[ n ];
		ymean += ypos[ n ];
		if( xmax < xpos[ n ])
			xmax = xpos[ n ];
		if( xmin > xpos[ n ] )
			xmin = xpos[ n ];
		if( ymax < ypos[ n ])
			ymax = ypos[ n ];
		if( ymin > ypos[ n ] )
			ymin = ypos[ n ];
	}
	double maxRadius = ( xmax - xmin ) / 2.0 > ( ymax - ymin ) / 2.0 ? ( xmax - xmin ) / 2.0 : ( ymax - ymin ) / 2.0;
	xmean = xmean / (double)G.number_of_nodes();
	ymean = ymean / (double)G.number_of_nodes();

	list<circle> Circles;
	double radius = minRadius,radBefore=0;
	node_array<bool> marked( G, false );
	int loopCount = 0;
	while( allLabeled( G, marked ) != true ){
		int count = 0;		
		circle C( xmean, ymean, radius );
		list<node> inCircles;
		forall_nodes( n, G ){
			if( marked[ n ] != true && C.inside( posx[ n ] ) ){
				count++;
				inCircles.append( n );
				marked[ n ] = true;
			}
		}
		if( count != 0 ){
			array<bool> circlePosition( count + 1 );
			array<double> radianPositions( count + 1 );
			for( int a = 0; a < inCircles.size(); a++ ){
				circlePosition[ a ] = false;
			}
			double length = node_width * count;		
			double tmp = 2.0 * 3.147;
			double radiusC = length / tmp;
			circle C2;
			if( loopCount == 0 ){
				circle C3( xmean, ymean, radiusC );
				C2 = C3;
			}
			else{
				if( radiusC < radBefore + node_width * 2 ){
					radiusC = radBefore + node_width * 2;
					circle C3( xmean, ymean, radiusC );
					C2 = C3;
				}
				else{
					circle C3( xmean, ymean, radiusC );
					C2 = C3;
				}
			}
			double min = tmp / (double)count;
			for( int a = 0; a < inCircles.size(); a++ ){
				radianPositions[ a ] = tmp;
				tmp -= min; 
			}		
			forall( n, inCircles ){
				double minR = radiusC + 10000;
				int storeA = 0;
				for( int a = 0; a < inCircles.size(); a++ ){
					point target = C2.point_on_circle( radianPositions[ a ] );
					point source( xpos[ n ], ypos[ n ] );				
					if( minR > abs( target.distance( source ) ) && circlePosition[ a ] == false ){
						minR = abs( target.distance( source ) );
						storeA = a;
					}
				}
				circlePosition[ storeA ] = true;
				xpos[ n ] = C2.point_on_circle( radianPositions[ storeA ] ).xcoord();
				ypos[ n ] = C2.point_on_circle( radianPositions[ storeA ] ).ycoord();
			}
			radBefore = radiusC;	
			loopCount++;
		}
		radius += radius;		
	}

	Xpos = xpos;
	Ypos = ypos;
	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
        char filename_[64] = "outputs/graphs/graph_";
        sprintf( filename_, "%s%d%s", filename_, graphNo, ".gml" );
        G.write_gml( filename_ );
	return G;
}

GRAPH<int,int> RUN_FFDANDCIRCLE(  GRAPH<int,int> &G, 
			  node_array<double> &Xpos, 
			  node_array<double> &Ypos, 
			  node_array<point> &posx,
			  edge_array<list<point> > &bendsx, 
			  GraphWin &gw,
			  int node_width,
			  int minRadius
){	
	edge e;
	node n;
	int random_walk;
	node_array<double> xpos( G, 0 );
	node_array<double> ypos( G, 0 );
//*********************//
// Give rank assignmen //
//*********************//
// 	forall_edges( e, G )
// 	{
// 		if( G.source( e ) == G.target( e ) )
// 			G.del_edge( e );
// 	}
// 	forall_edges( e, G )
// 	{
// 		if( G.source( e ) == G.target( e ) )
// 			G.del_edge( e );
// 	}
	node_array<int> nodeid( G, 3 );
	list<node> fixedNodes2;
// 	cout << " BEFORE FORCE\n";
// 	cout << gw.get_xmin() << "\t" << gw.get_xmax() << "\t" << gw.get_ymin() << "\t" << gw.get_ymax() << endl;
	SPRING_EMBEDDING_our2( G, fixedNodes2, xpos, ypos, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 500, nodeid );
// 	cout << " AFTER FORCE\n";
	Xpos = xpos;
	Ypos = ypos;

	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
	
	double xmean=0,ymean=0;
	double xmax=xpos[G.first_node()],ymax=ypos[G.first_node()],xmin=xpos[G.first_node()],ymin=ypos[G.first_node()];
	forall_nodes( n, G ){
		xmean += xpos[ n ];
		ymean += ypos[ n ];
		if( xmax < xpos[ n ])
			xmax = xpos[ n ];
		if( xmin > xpos[ n ] )
			xmin = xpos[ n ];
		if( ymax < ypos[ n ])
			ymax = ypos[ n ];
		if( ymin > ypos[ n ] )
			ymin = ypos[ n ];
	}
	double maxRadius = ( xmax - xmin ) / 2.0 > ( ymax - ymin ) / 2.0 ? ( xmax - xmin ) / 2.0 : ( ymax - ymin ) / 2.0;
	xmean = xmean / (double)G.number_of_nodes();
	ymean = ymean / (double)G.number_of_nodes();

	list<circle> Circles;
	double radius = minRadius,radBefore=0;
	node_array<bool> marked( G, false );
	int loopCount = 0;
	while( allLabeled( G, marked ) != true ){
		int count = 0;		
		circle C( xmean, ymean, radius );
		list<node> inCircles;
		forall_nodes( n, G ){
			if( marked[ n ] != true && C.inside( posx[ n ] ) ){
				count++;
				inCircles.append( n );
				marked[ n ] = true;
			}
		}
		if( count != 0 ){
			array<bool> circlePosition( count + 1 );
			array<double> radianPositions( count + 1 );
			for( int a = 0; a < inCircles.size(); a++ ){
				circlePosition[ a ] = false;
			}
			double length = node_width * count;		
			double tmp = 2.0 * 3.147;
			double radiusC = length / tmp;
			circle C2;
			if( loopCount == 0 ){
				circle C3( xmean, ymean, radiusC );
				C2 = C3;
			}
			else{
				if( radiusC < radBefore + node_width * 2 ){
					radiusC = radBefore + node_width * 2;
					circle C3( xmean, ymean, radiusC );
					C2 = C3;
				}
				else{
					circle C3( xmean, ymean, radiusC );
					C2 = C3;
				}
			}
			double min = tmp / (double)count;
			for( int a = 0; a < inCircles.size(); a++ ){
				radianPositions[ a ] = tmp;
				tmp -= min; 
			}		
			forall( n, inCircles ){
				double minR = radiusC + 10000;
				int storeA = 0;
				for( int a = 0; a < inCircles.size(); a++ ){
					point target = C2.point_on_circle( radianPositions[ a ] );
					point source( xpos[ n ], ypos[ n ] );				
					if( minR > abs( target.distance( source ) ) && circlePosition[ a ] == false ){
						minR = abs( target.distance( source ) );
						storeA = a;
					}
				}
				circlePosition[ storeA ] = true;
				xpos[ n ] = C2.point_on_circle( radianPositions[ storeA ] ).xcoord();
				ypos[ n ] = C2.point_on_circle( radianPositions[ storeA ] ).ycoord();
			}
			radBefore = radiusC;	
			loopCount++;
		}
		radius += radius;		
	}
// 	cout << loopCount << endl;
	Xpos = xpos;
	Ypos = ypos;
	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
// 	cout << " BEFORE RETURN\n";
	return G;
}


/* Function is based for Force Directed Layout 
Inputs are straightforward, this procedure 
calculates the peripheral graphs */
GRAPH<int,int> RUN_CIRCLEALONE(  GRAPH<int,int> &G,
			  node_array<double> &Xpos, 
                          node_array<double> &Ypos,
                          node_array<point> &posx,
                          int graphNo,
			  int nodeWidth
){	
	edge e;
	node n;
	int random_walk;
	node_array<double> xpos( G, 0 );
	node_array<double> ypos( G, 0 );
//*********************//
// Give rank assignmen //
//*********************//
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	node_array<int> nodeid( G, 3 );
	node_array<point> pos( G );
	double pi = 2.0 * 3.147;
	double tmp = pi;
	int count = 0;
	double length = nodeWidth * G.number_of_nodes();
	double radius = length / pi;
	circle C( 0, 0, radius );
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

	Xpos = xpos;
	Ypos = ypos;

	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
	return G;
}


/* Function is based for Heuristic Circular Layout
that has two circles inside formed by lower degree
nodes and hub nodes and their neigbors also form
third outer circle around the two circle
Inputs are straightforward, this procedure 
calculates the peripheral graphs */
GRAPH<int,int> RUN_CIRCULAR2C(  GRAPH<int,int> &G, 
							  array<list<node> > &layers, 
							  int W, 
							  node_array<double> &Xpos, 
							  node_array<double> &Ypos, 
							  int graphNo, 
							  node_array<point> &posx,
							  edge_array<list<point> > &bendsx, 
							  int algorithmFlag, 
							  int space, 
							  bool xCoordFlag, 
							  int increment,
							  bool ledaPostFlag,
							  array<char> &abbv,
							  int cat_num,
							  array<char> &Categories,
							  int nodeWidth
){	
	double threshold = 0.10;
	int drawType = 1;
	edge e;
	node n,m,k;
	int random_walk;
	list_item it,it2;
	node_array<double> xpos( G, 0 );
	node_array<double> ypos( G, 0 );

	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	
	node_array<bool> marking( G, false );
	node_array<int> degrees( G, 0 );
	node_array<point> pos( G );
	int max_degree = 0;
	list<node> hubs,hubNeighbors;
	list<two_tuple<node,list<node> > > HUBS;
	forall_nodes( n, G ){
		degrees[ n ] = G.degree( n );
		if( max_degree < degrees[ n ] ){
			max_degree = degrees[ n ];
			m = n;
		}
	}
	marking[ m ] = true;
	forall_adj_nodes( n, m ){
		hubNeighbors.append( n );
		marking[ n ] = true;
	}
	two_tuple<node,list<node> > tup( m, hubNeighbors );
	HUBS.append( tup );

	forall_nodes( n, G ){
		int count = 0;
		forall_adj_nodes( k, n ){
			if( marking[ k ] == false )
				count++;
		}
		hubNeighbors.clear();
		if( count >= max_degree * threshold ){
			forall_adj_nodes( k, n ){
				if( marking[ k ] == false ){
					hubNeighbors.append( k );
					marking[ k ] = true;
				}
			}
			two_tuple<node,list<node> > tup( n, hubNeighbors );
			HUBS.append( tup );
		}
	}


	/* INNER CIRCLE Positioning */
	list<node> alones;
	forall_nodes( n, G ){
		if( marking[ n ] == false )
			alones.append( n );
	}
	int node_width = nodeWidth, node_height = 50;
	int gaps = 20;
	double radius = 2.0 * 3.14159;
	double length = (double)alones.size() * (double)( node_width );
	radius = length / radius;
	circle C( 0, 0, radius );
	double tmp = 2.0 * 3.14159;
	double min = tmp / (double)alones.size();
	if( drawType == 0 ){
		forall_items( it, alones ){
			  n = alones[ it ];
			  pos[ n ] = C.point_on_circle( tmp );
			  tmp -= min;   
		}      
	}
	else{
		if( drawType == 1 ){
			list<node> lowDegree;
			list<node> highDegree;
			forall_items( it, alones ){
				  n = alones[ it ];
				  if( G.degree( n ) < 3 )
					  lowDegree.append( n );
				  else
					  highDegree.append( n );
			}    	

			radius = 2.0 * 3.14159;
			length = (double)lowDegree.size() * (double)( node_width );
			radius = length / radius;
			circle C2( 0, 0, radius );
			tmp = 2.0 * 3.14159;
			min = tmp / (double)lowDegree.size();
			forall_items( it, lowDegree ){
				  n = lowDegree[ it ];
				  pos[ n ] = C2.point_on_circle( tmp );
				  tmp -= min;   
			}

			radius = 2.0 * 3.14159;
			length = (double)( (double)(highDegree.size() + lowDegree.size()) / 2.0 ) * (double)( node_width );
			radius = length / radius;
			if( radius < C2.radius() )
				radius = C2.radius() * 1.5;
			circle C1( 0, 0, radius );
			tmp = 2.0 * 3.14159;
			min = tmp / (double)highDegree.size();
			forall_items( it, highDegree ){
				  n = highDegree[ it ];
				  pos[ n ] = C1.point_on_circle( tmp );
				  tmp -= min;   
			} 
			C = C1;
		}
	}
	forall_items( it, alones ){
		n = alones[ it ];
		xpos[ n ] = pos[ n ].xcoord();
		ypos[ n ] = pos[ n ].ycoord();
	}

	/* OUTER CIRCLE Positioning */
	list<double> radiuses;
	double radiusesMax = 0;
	double sumOfRadiuses = 0;
	double totalLengthOfOuterCircle = 0;
	forall_items( it, HUBS ){
		two_tuple<node,list<node> > tup = HUBS[ it ];
		radius = 2.0 * 3.14159;
		length = (double)tup.second().size() * (double)( node_width );
		radius = length / radius;
		radiuses.append( radius );
		sumOfRadiuses+=radius;
		totalLengthOfOuterCircle += (radius);
		if( radiusesMax < radius )
			radiusesMax = radius;
	}

	circle OUTER;
	if( radiusesMax + C.radius() > totalLengthOfOuterCircle ){
		 circle T( 0, 0, C.radius() + radiusesMax * 3.0 );
		 OUTER = T;
	}
	else{
		circle T( 0, 0, totalLengthOfOuterCircle + radiusesMax * 3.0 );
		OUTER = T;
	}
	int get_r = 0;
	tmp = 2.0 * 3.14159;
	forall_items( it, HUBS ){
		radius = radiuses[ radiuses.get_item( get_r ) ];
		point hubCenter = OUTER.point_on_circle( tmp );
		xpos[ HUBS[ it ].first() ] = hubCenter.xcoord();
		ypos[ HUBS[ it ].first() ] = hubCenter.ycoord();
		/* Now Place Inner circle elemens */
		circle INNER( hubCenter.xcoord(), hubCenter.ycoord(), radius );
		double tmp2 = 2.0 * 3.14159;
		double min2 =  tmp2 / (double)HUBS[ it ].second().size();
		hubNeighbors = HUBS[ it ].second();
		forall_items( it2, hubNeighbors ){
		  n = hubNeighbors[ it2 ];
	      pos[ n ] = INNER.point_on_circle( tmp2 );
	      tmp2 -= min2;   
		}      
		forall_items( it2, hubNeighbors ){
			n = hubNeighbors[ it2 ];
			xpos[ n ] = pos[ n ].xcoord();
			ypos[ n ] = pos[ n ].ycoord();
		}
		min = 2.0 * 3.14159 * radius / sumOfRadiuses;
		tmp -= min;
		get_r++;
	}

	Xpos = xpos;
	Ypos = ypos;
	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
	return G;
}

/* Function is based for Heuristic Circular Layout
that has more than two circles inside formed by lower degree
nodes and hub nodes and their neigbors also form
final outer circle around the two circle
Function is based for Heuristic Circular Layout
Inputs are straightforward, this procedure 
calculates the peripheral graphs */
GRAPH<int,int> RUN_CIRCULARKC(  GRAPH<int,int> &G, 
				array<list<node> > &layers, 
				int W, 
				node_array<double> &Xpos, 
				node_array<double> &Ypos, 
				int graphNo, 
				node_array<point> &posx,
				edge_array<list<point> > &bendsx, 
				int algorithmFlag, 
				int space, 
				bool xCoordFlag, 
				int increment,
				bool ledaPostFlag,
				array<char> &abbv,
				int cat_num,
				int nodeWidth,
				int drawType,
				double threshold
){	
	edge e;
	node n,m,k;
	int random_walk;
	list_item it,it2;
	node_array<double> xpos( G, 0 );
	node_array<double> ypos( G, 0 );

	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	forall_edges( e, G )
	{
		if( G.source( e ) == G.target( e ) )
			G.del_edge( e );
	}
	
	node_array<bool> marking( G, false );
	node_array<int> degrees( G, 0 );
	node_array<point> pos( G );
	int max_degree = 0;
	list<node> hubs,hubNeighbors;
	list<two_tuple<node,list<node> > > HUBS;
	forall_nodes( n, G ){
		degrees[ n ] = G.degree( n );
		if( max_degree < degrees[ n ] ){
			max_degree = degrees[ n ];
			m = n;
		}
	}
	marking[ m ] = true;
	forall_adj_nodes( n, m ){
		hubNeighbors.append( n );
		marking[ n ] = true;
	}
	two_tuple<node,list<node> > tup( m, hubNeighbors );
	HUBS.append( tup );

	forall_nodes( n, G ){
		int count = 0;
		forall_adj_nodes( k, n ){
			if( marking[ k ] == false )
				count++;
		}
		hubNeighbors.clear();
		if( count >= max_degree * threshold ){
			forall_adj_nodes( k, n ){
				if( marking[ k ] == false ){
					hubNeighbors.append( k );
					marking[ k ] = true;
				}
			}
			two_tuple<node,list<node> > tup( n, hubNeighbors );
			HUBS.append( tup );
		}
	}


	/* INNER CIRCLE Positioning */
	list<node> alones;
	forall_nodes( n, G ){
		if( marking[ n ] == false )
			alones.append( n );
	}
	int node_width = nodeWidth, node_height = 50;
	int gaps = 20;
	double radius = 2.0 * 3.14159;
	double length = (double)alones.size() * (double)( node_width );
	radius = length / radius;
	circle C( 0, 0, radius );
	double tmp = 2.0 * 3.14159;
	double min = tmp / (double)alones.size();
	if( drawType == 0 ){
		forall_items( it, alones ){
			  n = alones[ it ];
			  pos[ n ] = C.point_on_circle( tmp );
			  tmp -= min;   
		}      
	}
	else{
		if( drawType == 1 ){
			list<node> lowDegree;
			list<node> highDegree;
			forall_items( it, alones ){
				  n = alones[ it ];
				  if( G.degree( n ) < 3 )
					  lowDegree.append( n );
				  else
					  highDegree.append( n );
			}    	

			radius = 2.0 * 3.14159;
			length = (double)lowDegree.size() * (double)( node_width );
			radius = length / radius;
			circle C2( 0, 0, radius );
			tmp = 2.0 * 3.14159;
			min = tmp / (double)lowDegree.size();
			forall_items( it, lowDegree ){
				  n = lowDegree[ it ];
				  pos[ n ] = C2.point_on_circle( tmp );
				  tmp -= min;   
			}

			radius = 2.0 * 3.14159;
			length = (double)( (double)(highDegree.size() + lowDegree.size()) / 2.0 ) * (double)( node_width );
			radius = length / radius;
			if( radius < C2.radius() )
				radius = C2.radius() * 1.5;
			circle C1( 0, 0, radius );
			tmp = 2.0 * 3.14159;
			min = tmp / (double)highDegree.size();
			forall_items( it, highDegree ){
				  n = highDegree[ it ];
				  pos[ n ] = C1.point_on_circle( tmp );
				  tmp -= min;   
			} 
			C = C1;
		}
		else{
			if( drawType == 2 ){
				/*int circleCount = G.number_of_edges() / 1000;
				array<list<node>> Degree( circleCount );
				list<node> highDegree;
				int maxDeg = 0;
				forall_items( it, alones ){
					n = alones[ it ];
					if( G.degree( n ) > maxDeg )
						maxDeg = G.degree( n );
				}

				forall_items( it, alones ){
					  n = alones[ it ];
					  Degree[floorf( maxDeg / G.degree( n ) )].append( n );
				}    	

				radius = 2.0 * 3.14159;
				length = (double)Degree[ circleCount ].size() * (double)( node_width );
				radius = length / radius;
				circle C2( 0, 0, radius );
				tmp = 2.0 * 3.14159;
				min = tmp / (double)Degree[ circleCount ].size();
				forall_items( it, Degree[ circleCount ] ){
					  n = Degree[ circleCount ][ it ];
					  pos[ n ] = C2.point_on_circle( tmp );
					  tmp -= min;   
				}

				for( int x1 = circleCount-1; x1 >= 0; x1-- ){
					radius = 2.0 * 3.14159;
					double sum = 0;
					for( int x2 = circleCount; x2 >= x1; x2-- ){
						sum += Degree[ x2 ].size();
					}
					length = (double)( (double)(sum) / (double)(circleCount - x1 + 1 )) * (double)( node_width );
					radius = length / radius;
					if( radius < C2.radius() )
						radius = C2.radius() * 1.5;
					circle C1( 0, 0, radius );
					tmp = 2.0 * 3.14159;
					min = tmp / (double)Degree[ x1 ].size();
					forall_items( it, Degree[ x1 ] ){
						  n = Degree[ x1 ][ it ];
						  pos[ n ] = C1.point_on_circle( tmp );
						  tmp -= min;   
					} 
					C = C1;
				}*/

				list<node> lowDegree;
				list<node> medDegree;
				list<node> wellDegree;
				list<node> highDegree;
				list<node> highestDegree;
				forall_items( it, alones ){
					  n = alones[ it ];
					  if( G.degree( n ) < 1 )
						  lowDegree.append( n );
					  else
						  if( G.degree( n ) < 2 )
							  medDegree.append( n );
						  else
							  if( G.degree( n ) < 5 )
								  wellDegree.append( n );
							  else
								  if( G.degree( n ) < 7 )
									  highDegree.append( n );
								  else
									  highestDegree.append( n );
				}    	

				radius = 2.0 * 3.14159;
				length = (double)lowDegree.size() * (double)( node_width );
				radius = length / radius;
				circle C2( 0, 0, radius );
				tmp = 2.0 * 3.14159;
				min = tmp / (double)lowDegree.size();
				forall_items( it, lowDegree ){
					  n = lowDegree[ it ];
					  pos[ n ] = C2.point_on_circle( tmp );
					  tmp -= min;   
				}

				radius = 2.0 * 3.14159;
				length = (double)( (double)(lowDegree.size() + medDegree.size()) / 2.0 ) * (double)( node_width );
				radius = length / radius;
				if( radius < C2.radius() )
					radius = C2.radius() + 500;
				circle C3( 0, 0, radius );
				tmp = 2.0 * 3.14159;
				min = tmp / (double)medDegree.size();
				forall_items( it, medDegree ){
					  n = medDegree[ it ];
					  pos[ n ] = C3.point_on_circle( tmp );
					  tmp -= min;   
				} 
				C2 = C3;

				radius = 2.0 * 3.14159;
				length = (double)( (double)(lowDegree.size() + medDegree.size() + wellDegree.size()) / 2.0 ) * (double)( node_width );
				radius = length / radius;
				if( radius < C2.radius() )
					radius = C2.radius() + 500;
				circle C4( 0, 0, radius );
				tmp = 2.0 * 3.14159;
				min = tmp / (double)wellDegree.size();
				forall_items( it, wellDegree ){
					  n = wellDegree[ it ];
					  pos[ n ] = C4.point_on_circle( tmp );
					  tmp -= min;   
				} 
				C2 = C4;

				radius = 2.0 * 3.14159;
				length = (double)( (double)(highDegree.size() + wellDegree.size() + medDegree.size() + lowDegree.size()) / 2.0 ) * (double)( node_width );
				radius = length / radius;
				if( radius < C2.radius() )
					radius = C2.radius() + 500;
				circle C5( 0, 0, radius );
				tmp = 2.0 * 3.14159;
				min = tmp / (double)highDegree.size();
				forall_items( it, highDegree ){
					  n = highDegree[ it ];
					  pos[ n ] = C5.point_on_circle( tmp );
					  tmp -= min;   
				} 
				C2 = C5;

				radius = 2.0 * 3.14159;
				length = (double)( (double)(highestDegree.size() + highDegree.size() + wellDegree.size() + medDegree.size() + lowDegree.size()) / 2.0 ) * (double)( node_width );
				radius = length / radius;
				if( radius < C2.radius() )
					radius = C2.radius() + 500;
				circle C1( 0, 0, radius );
				tmp = 2.0 * 3.14159;
				min = tmp / (double)highDegree.size();
				forall_items( it, highestDegree ){
					  n = highestDegree[ it ];
					  pos[ n ] = C1.point_on_circle( tmp );
					  tmp -= min;   
				} 
				C = C1;				
			}
		}
	}
	forall_items( it, alones ){
		n = alones[ it ];
		xpos[ n ] = pos[ n ].xcoord();
		ypos[ n ] = pos[ n ].ycoord();
	}

	/* OUTER CIRCLE Positioning */
	list<double> radiuses;
	double radiusesMax = 0;
	double sumOfRadiuses = 0;
	double totalLengthOfOuterCircle = 0;
	forall_items( it, HUBS ){
		two_tuple<node,list<node> > tup = HUBS[ it ];
		radius = 2.0 * 3.14159;
		length = (double)tup.second().size() * (double)( node_width );
		radius = length / radius;
		radiuses.append( radius );
		sumOfRadiuses+=radius;
		totalLengthOfOuterCircle += (radius);
		if( radiusesMax < radius )
			radiusesMax = radius;
	}

	circle OUTER;
	if( radiusesMax + C.radius() > totalLengthOfOuterCircle ){
		 circle T( 0, 0, C.radius() + radiusesMax * 3.0 );
		 OUTER = T;
	}
	else{
		circle T( 0, 0, totalLengthOfOuterCircle + radiusesMax * 3.0 );
		OUTER = T;
	}
	int get_r = 0;
	tmp = 2.0 * 3.14159;
	forall_items( it, HUBS ){
		radius = radiuses[ radiuses.get_item( get_r ) ];
		point hubCenter = OUTER.point_on_circle( tmp );
		xpos[ HUBS[ it ].first() ] = hubCenter.xcoord();
		ypos[ HUBS[ it ].first() ] = hubCenter.ycoord();
		/* Now Place Inner circle elemens */
		circle INNER( hubCenter.xcoord(), hubCenter.ycoord(), radius );
		double tmp2 = 2.0 * 3.14159;
		double min2 =  tmp2 / (double)HUBS[ it ].second().size();
		hubNeighbors = HUBS[ it ].second();
		forall_items( it2, hubNeighbors ){
		  n = hubNeighbors[ it2 ];
	      pos[ n ] = INNER.point_on_circle( tmp2 );
	      tmp2 -= min2;   
		}      
		forall_items( it2, hubNeighbors ){
			n = hubNeighbors[ it2 ];
			xpos[ n ] = pos[ n ].xcoord();
			ypos[ n ] = pos[ n ].ycoord();
		}
		min = 2.0 * 3.14159 * radius / sumOfRadiuses;
		tmp -= min;
		get_r++;
	}

	Xpos = xpos;
	Ypos = ypos;
	forall_nodes( n, G ){
		point p( Xpos[ n ], Ypos[ n ] );
		posx[ n ] = p;
	}
	return G;
}


/* Function is specialized to form main layout,
it performs again weighted layout over the 
collected graph edges according to criteria
shown in journal paper */
GRAPH<int,int> RUN_FFD_SELF( GRAPH<int,int> &G, 
			  node_array<int> &PARS,
			  int max_height, 
			  int W, 
			  node_array<double> &Xpos,
			  node_array<double> &Ypos, 
			  int graphNo, 
			  node_array<point> &posx,
			  edge_array<list<point> > &bendsx, 
			  node_array<double> &Hvalues,
			  list<node> &hided, 
			  int algorithmFlag,
			  bool brandFlag,
			  bool brandFlag2,
			  bool ourMethodFlag,
			  double space,
			  int increment,
			  bool ledaPostFlag,
			  double nodeSize,
			  double edgeBendImp,
			  double colorScale,
			  double edgThicknessTher,
			  list<int> categ
 ){

	GRAPH<int,int> H;

	edge e;
	node n;
	random_source S(1,max_height);
	int random_walk;
	color blue( 4 );
	color orange( 7 );
	node_array<point> pos(G);
// 	cout << " 2 " << G.number_of_edges() << " - " << G.number_of_nodes() <<endl;
	SPRING_EMBEDDING2_( G, Xpos, Ypos, 0, 2000 + G.number_of_nodes()*20 ,0, 700 + G.number_of_nodes()*7, 500, PARS, Hvalues );

	double xmin, xmax, ymin, ymax;
	int ncount = 0;
	forall_nodes( n, G ){
                Xpos[ n ] = Xpos[ n ] * 2.5;
                Ypos[ n ] = Ypos[ n ] * 2.5;
		if( G.degree( n ) != 0 ){
			if( ncount == 0 ){
				xmin = Xpos[ n ];
				xmax = Xpos[ n ];
				ymin = Ypos[ n ];
				ymax = Ypos[ n ];
			}
			else{
			    if( Xpos[ n ] < xmin )
				  xmin = Xpos[ n ];
			    if( Xpos[ n ] > xmax )
				  xmax = Xpos[ n ];
			    if( Ypos[ n ] < ymin )
				  ymin = Ypos[ n ];
			    if( Ypos[ n ] > ymax )
				  ymax = Ypos[ n ];
			}
			ncount++;
		}
	}
        double xpos1 = xmin - 100.0;
	double ypos1 = ymax + 100.0;
	forall_nodes( n, G ){
		if( G.degree( n ) == 0 ){
                        if( xpos1 < xmax + 100.0 ){
				Xpos[ n ] = xpos1;
				Ypos[ n ] = ypos1;
			}
			else{
                                xpos1 = xmin - 100.0;
				ypos1 += 100.0;
				Xpos[ n ] = xpos1;
				Ypos[ n ] = ypos1;
			}
			xpos1 += 100.0;
		}
	}
//	cout << " 3 " << endl;
//	array<bool> colorChoose( 13 );
//        for( int c = 0; c < categ.size(); c++ ){
//		colorChoose[ c ] = false;
//	}
//	colorChoose[ 5 ]  = true;
	list_item it;

	GraphWin gw(G);	
//	S(1,8);
//	int random_value;
//	for( int c = 0; c < 13; c++ ){
//		if(colorChoose[ c ] == true ){
//		      random_value = c;
//		      break;
//		}
//	}

	// Make all of the node as orange
        int colorCount = 0;
	forall_nodes( n, G ){
			G[ n ] = PARS[ n ];
                        //G[ n ] = 11;
                        color chosen( categ[ categ.get_item( colorCount ) ] );
                        gw.set_color( n, chosen);
                        gw.set_border_color( n, chosen);
//                        cout << categ[ categ.get_item( colorCount ) ] << endl;
                        colorCount++;
	}

	forall_nodes( n, G ){
			point p( Xpos[ n ], Ypos[ n ] );
			pos[ n ] = p;
	}

	double Hmax = 0;
	forall_nodes( n, G ){
		if( Hmax <  Hvalues[n] )
			Hmax = Hvalues[n];
	}
	// Arrange node sizes according to H-values for each nodes
	forall_nodes( n, G ){
                // We use nodeSize / 2.0 since we do not want to see large nodes.
                if( Hvalues[n] > 0 ){
                    gw.set_width( n, nodeSize / 1.5 + Hvalues[n] / Hmax * 70 );
                    gw.set_height( n, nodeSize / 1.5 + Hvalues[n] / Hmax * 70 );
		}
		else{
                    gw.set_width( n, nodeSize / 1.5 );
                    gw.set_height( n, nodeSize / 1.5 );
		}
	}
//	if( G.number_of_edges() != 0 ){
//		array<color> color_l( 20 );
//		int count = colorScale;
//		for( int j = 0; j < color_l.size(); j++ ){
//			color el( count, count, count );
//			color_l[ j ] = el;
//			count -= 7;
//		}
//		list<int> weights;
//		forall_edges( e, G ){
//
//		}
//		list<two_tuple<color,int> > variations;
//		list<int> edgeWeights;
//// 		cout << " 4 " << endl;
//		// Store Edge weights
//		forall_edges( e, G ){
//			int flag = 0;
//// 			cout << " 2.2 " << endl;
//			forall_items( it, edgeWeights ){
//				if( edgeWeights[ it ] == G[ e ] ){
//					flag = 1;
//					break;
//				}
//			}
//			if( flag == 0 ){
//				edgeWeights.push_back( G[ e ] );
//			}
//// 			cout << " 4.1 " << endl;
//		}
//		edgeWeights.sort();
//		count = 0;
//		// Found the variation of edge weights in the graph
//		for( int x = edgeWeights[edgeWeights.first()]; x <= edgeWeights[edgeWeights.last()]; x++ ){
//			for( int j = 0; j < color_l.size(); j++ ){
//				two_tuple<color,int> Tp( color_l[ j ], x );
//	// 			cout << Tp << endl;
//				variations.push_back( Tp );
//			}
//			count++;
//// 			cout << " 4.2 " << endl;
//		}
//// 		cout << " 5 " << endl;
//	// 	int increase = (int) ((double)variations.size() / (double)edgeWeights.size() );
//		int increase = (int) ((double)variations.size() / (double)color_l.size() );
//
//	// 	cout << " increase : " << increase << endl;
//		// Form first coloring scheme
//		forall_edges( e, G ){
//			count = 0;
//			forall_items( it, edgeWeights ){
//			      if( edgeWeights[ it ] == G[ e ] ){
//				    break;
//			      }
//			      count++;
//			}
//			two_tuple<color,int> Tp = variations[ variations.get_item( increase / edgThicknessTher * count ) ] ;
//			gw.set_thickness( e, Tp.second() / edgThicknessTher );
//			gw.set_color( e, Tp.first() );
//	// 		cout << Tp << endl;
//	// 		cout << " increase : " << increase << endl;
//		}
//	}

	//gw.set_edge_thickness( 3, true );
	//gw.set_node_height(50,true);
        //gw.set_node_border_color( black, true );
	gw.set_node_border_thickness( 4, true );
	//gw.set_node_width( 100, true );
	gw.set_node_shape( leda::circle_node, true );
// 	gw.set_edge_label_type( leda::data_label , true);
//	gw.set_edge_label_font( leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
	gw.set_layout( pos );
	char filename4[128];
	
#ifdef LINUX
	sprintf( filename4, "outputs/graphs/maingraph.gml" );
#else
	sprintf( filename4, "outputs//graphs//maingraph.gml" );
#endif
	gw.save_gml( filename4 );
	//gw.save_gml( "outputs/graphs/maingraph2.gml" );
	return G;
}	






/* Function is specialized to form main layout,
it performs again weighted layout over the 
collected graph edges according to criteria
shown in ISB paper */
GRAPH<int,int> RUN_SELF2( GRAPH<int,int> &G, 
			  node_array<int> &PARS,
			  int max_height, 
			  int W, 
			  node_array<double> &Xpos,
			  node_array<double> &Ypos, 
			  int graphNo, 
			  node_array<point> &posx,
			  edge_array<list<point> > &bendsx, 
			  node_array<double> &Hvalues,
			  list<node> &hided, 
			  int algorithmFlag,
			  bool brandFlag,
			  bool brandFlag2,
			  bool ourMethodFlag,
			  double space,
			  int increment,
			  bool ledaPostFlag,
			  double nodeSize,
			  double edgeBendImp,
			  double colorScale,
			  double edgThicknessTher,
			  list<int> categ
 ){

	GRAPH<int,int> H;

	edge e;
	node n;
	//random_graph( G, noden, edgen );
	random_source S(1,max_height);
	int random_walk;

//*********************//
// Give rank assignmen //
//*********************//
	forall_edges( e, G )
	{
			if( G.source( e ) == G.target( e ) )
					G.del_edge( e );
	}
	forall_edges( e, G )
	{
			if( G.source( e ) == G.target( e ) )
					G.del_edge( e );
	}

	list<edge> edgelist;
	list_item it,it2;
	node_array<bool> marked(G,false);

/*
	forall_edges( e1 ,G ){
		forall_edges( e2, G ){
			if( G.target( e1 ) == G.target( e2 ) && G.source( e1 ) == G.source( e2 ) && e1 != e2 ){
					cout << " " << G[ e1 ] << " - " << G[ e2 ] << endl;
					G.del_edge( e2 );
			}
		}
	}
*/
	
	list<edge> paralells;
	// Leda function to arrange Graph like removing parallel edges
	paralells = Make_Simple( G );
	G.del_edges( paralells );
	list<node> A;
	forall_nodes( n , G ){
			A.append( n );
	}

	node root;
	forall_nodes( n , G ){
			root = n;
			break;
	}

	list<edge> feedback;
	node_array<bool> visited( G, false );
	list<edge> dfsEList;
	list<edge> suspis;
	list<node> Atemp;

	dfs( root, G, visited, A, dfsEList );

/****************************************************************/
/*********************** ACYCLIC PHASE I ************************/
/****************************************************************/

	node_array<int> ord(G);
	bool acyclicBool = TOPSORTG(G,ord);
	list<edge> cycleFinder;
	list<edge> reversedEdges;
	H = G;

	edge_array<int> prevEdgeList( G, 0 );
	forall_edges( e, G ){
			prevEdgeList[ e ] = G[ e ];
	}

	// Catch the cycle and if you can you get the min weighted edge 
	// in cylce that removes the cycle when it is deleted.
	while( !acyclicBool ){
			list_item it;
			Is_Acyclic( G, cycleEdges );		
			 				
			forall_items( it, cycleEdges ){
					e = cycleEdges[ it ];
					node_array<bool> nvisit( G, false );
					edge_array<bool> eonPath( G, false );
					cycleFinder = cycleCatcher( G, e, G.source( e ), G.target( e ), eonPath, nvisit );
			
					#ifdef DEBUG	
							cycleFinder.print();
					#endif

						int max = 0;
						edge etemp;
						forall_items( it, cycleEdges ){
								e = cycleEdges[ it ];
								if( max <= H[ e ] ){
										max = H[ e ];
								}
						}
						int min = max;

						forall_items( it, cycleFinder ){
								e = cycleFinder[ it ];
								if( min >= H[ e ] ){
										min = H[ e ];
										etemp = e;
								}
								#ifdef DEBUG
										cout << H[ e ] << " - ";
								#endif

						}
						int c = 0;
						forall_items( it, cycleFinder ){
								e = cycleFinder[ it ];
								H[ e ] -= min; 
								if( H[ e ] == 0 && inList( suspis , e ) == false ){
										c = 1;
										suspis += e;
										G.hide_edge( e );
								}

								#ifdef DEBUG
										cout << endl << " suspis - > ";
										suspis.print();
										cout << endl;
								#endif

						}
						
			}
			#ifdef DEBUG
					drawCycleEdges( G, visited, dfsEList, suspis );
			#endif
			cycleEdges.clear();
			acyclicBool = TOPSORTG(G,ord);
	}

/****************************************************************/
/*********************** ACYCLIC PHASE II ***********************/
/****************************************************************/

//-> Restore All edges give the original value to suspicious edges
	    G.restore_all_edges();
		forall_edges( e, G ){
				G[ e ] = prevEdgeList[ e ];
		}
		edge etemp;
//-> Hide again to test that suspis are realy suspicious ones
		G.hide_edges( suspis );


#ifdef DEBUG
		suspis.print();
#endif

		list<edge> popedList;

		END:
				int max = 0;
				popedList.clear();
				int etempChanged = 0;
				while( !(suspis.empty()) ){
						e = suspis.pop();
						if( max <= G[ e ] ){
								max = G[ e ];
								etemp = e;
								etempChanged = 1;
						}
						popedList.append( e );
				}
				forall_items(it,popedList ){
						e = popedList[ it ];
						if( e == etemp )
								;
						else
							suspis.append( e );
				}
				if( etempChanged == 1  ){
					G.restore_edge( etemp );
					etemp = G.rev_edge( etemp );
					reversedEdges.append( etemp );
				}
				if( Is_Acyclic( G ) != true ){
						etemp = G.rev_edge( etemp );
						forall_items( it, reversedEdges ){
							    e = popedList[ it ];
								if( e == etemp )
									reversedEdges.del_item( it );
						}
				}
				if( suspis.empty() == false )
						goto END;
				
#ifdef DEBUG
		cout << endl << "suspis";
		suspis.print();
		cout << endl;
#endif
	
	G.restore_all_edges();
	cycleEdges.clear();
	array<list<node> > sourceAndTargets( reversedEdges.size() );
	
	random_walk = 0;
	forall_items( it, reversedEdges ){
		sourceAndTargets[ random_walk ].push_back( G.source( reversedEdges[ it ] ) );
		sourceAndTargets[ random_walk ].push_back( G.target( reversedEdges[ it ] ) );
		random_walk++;
	}

#ifdef DRAW
		drawCycleEdges( G, visited, dfsEList, reversedEdges );
#endif


/*********************** END OF ACYLIC **************************/
/****************************************************************/
#ifdef DEBUG_ROBINVIZ
cout << "\n Acyclic Done \n" ;
#endif
/****************************************************************/
/******************** LAYER ASSIGNMENT **************************/

	/****** Transitive Edges Removal **************/

		list<edge> transitives;
		do_call( G, transitives );

#ifdef TRANSITIVES
		transitives.print();
#endif

		G.hide_edges( transitives );
		//drawCycleEdges( G, visited, dfsEList, suspis );
	
	/****** End of Transitive Edges Removal *******/
	/****** Coffman Graham Ranking Phase 1 ********/
		node_array<int> ready( G,0 );

		CGwithPromotionMinWidthW( G, ready, W );
//                 LPwithPromotionW( G, ready );
	/****** Coffman Graham Ranking Phase 1 End ****/
	/****** Coffman Graham Ranking Phase 2 ********/
		int count = 0;

		max = 0;
		forall_nodes( n , G ){

#ifdef COUNT
			cout << endl << "- " << count << " :" << n << "  "<< ready[ n ] << endl;  
#endif
			count++;
			if( max < ready[ n ] )
					max = ready[ n ];
		}

		array<list<node> > Layers( max + 1 );
		array<list<node> > LayersTemp( max + 1 );

		forall_nodes( n , G ){
				Layers[ max - ready[ n ] ].append( n );
		}
		//draw_new( G, Layers, max );
		G.restore_all_edges();
		//draw_new( G, Layers, max );
		LayersTemp = Layers;

	/****** Coffman Graham Ranking Phase 2 End ****/

/******************** END OF LAYER ASSIGNMENT *******************/
/****************************************************************/

#ifdef DEBUG_ROBINVIZ        
cout << "\n Layering Done \n" ;
#endif
/****************************************************************/
/******************** ADD DUMMY VERTICES ************************/
		node target,source;
		edge_array<bool> eMarked( G, false );
		list<edge> allEdges;
		list<node> dummyNodes;

		allEdges = G.all_edges();
		forall_items( it, allEdges ){
				e = allEdges[ it ];
				if( ( max - ready[ G.source( e ) ] ) - 1 > ( max - ready[ G.target( e ) ] ) && eMarked[ e ] == false ){
						
						int sourceLayer = max - ready[ G.source( e ) ];
						int targetLayer = max - ready[ G.target( e ) ];
						target = G.source( e );
						source = G.target( e );

						int etempx = G[ e ];
						G.del_edge( e );
						
						int x = 0;
						while( sourceLayer - 1 != targetLayer ){
								n = G.new_node();
								dummyNodes.append( n );
								e = G.new_edge( target, n );
								if( x % 2 == 0 )
									Layers[ sourceLayer - 1 ].push_back( n );
								else
									Layers[ sourceLayer - 1 ].push_front( n );

								sourceLayer--;
								G[ e ] = etempx;
								eMarked.init( G );
								eMarked[ e ] = true;
								target = n;
								x++;
						}
						e = G.new_edge( n, source );
						G[ e ] = etempx;
						eMarked.init( G );
						eMarked[ e ] = true;
				}
				else{
						eMarked[ e ] = true;
				}
		}

/******************** END OF ADDING DUMMY VERTICES **************/
/****************************************************************/

#ifdef DEBUG_ROBINVIZ
cout << "\n Dummy Adding Done \n" ;
#endif
/****************************************************************/
/********************* CROSSING REDUCTION ***********************/

		array<list<node> > Layers2( max + 1 );
		Layers2 = Layers;

		integer cross_count = 0;
		/*for( int i = max; i > 0; i-- ){
			cross_count += crossing_numberG( G, Layers[ i ], Layers[ i - 1 ] );
		}*/
		//cout << " Crossings  Before : " << cross_count << endl;
		cross1 = cross_count; 
// 		G.rev_all_edges();
// 		//1st Way
// 		for( int i = max; i > 0; i-- ){
// 			wolf( G, Layers[ i - 1 ], Layers[ i ] );
// 		}
// 		G.rev_all_edges();
		for( int repeat_i = 0; repeat_i < 20 ; repeat_i++ ){
		      G.rev_all_edges();
		      //1st Way
		      for( int i = Layers.size()-1; i > 0; i-- ){
			  wolf( G, Layers[ i - 1 ], Layers[ i ] );
		      }
		      G.rev_all_edges();
		      //2nd Way
		      for( int i = 0; i < Layers.size()-1; i++ ){
			  wolf( G, Layers[ i + 1 ], Layers[ i ] );
		      }
		      G.rev_all_edges();
		      G.rev_all_edges();
		}

		//2nd Way
		/*for( int i = max; i > 0; i-- ){
			wolfBarycenter( G, Layers[ i - 1 ], Layers[ i ] );
		}*/

// 		for( int i = max; i > 0; i-- ){
// 			median_graph( G, Layers[ i - 1 ], Layers[ i ] );
// 		}
		/*for( int i = max; i > 0; i-- ){
			median_graph( G, Layers2[ i - 1 ], Layers2[ i ] );
		}*/

/*		//3rd Way
		for( int i = max; i > 0; i-- ){
			wolf( G, Layers[ i ], Layers[ i - 1 ] );
		}
*/
/*		//4th Way
		for( int i = max; i > 0 && i - 2 > 0; i-=2 ){
			wolf( G, Layers[ i ], Layers[ i - 1 ] );
		}
*/


/********************* END OF CROSSING REDUCTION ****************/
/****************************************************************/
#ifdef DEBUG_ROBINVIZ
        cout << "\n Crossing Done \n";
#endif
		GraphWin gw(G);
// 		list<double> hvalues;
// 		forall_nodes( n, G ){
// cout << Hvalues[ n ]  << endl;
// 			hvalues.append( Hvalues[ n ] );
// 
// }
#ifdef DEBUG_ROBINVIZ
        cout << "\n Crossing Done \n";
#endif
		node_array<double> xpos(G);
		node_array<double> ypos(G);

		node_array<double> xpos2(G);
		node_array<double> ypos2(G);
		draw_dummy_centered(  G, Layers, max, dummyNodes, xpos2, ypos2 );

// 		forall_nodes( n, G ){
// 			cout << G[ n ] << " - " << xpos[ n ] << " - " << ypos[ n ] << endl;
// 		}
		node_array<int> pos_in_layer( G, 0 );
		node_array<int> assigned_rank( G, 0 );
		node_array<int> width( G, 10 );
		node_array<bool> is_dummy( G, false );

		forall_items( it, dummyNodes ){
		    is_dummy[ dummyNodes[ it ] ] = true;
		}
		for( int i = 0; i < Layers.size(); i++ ){
		    count = 0;   
		    forall_items( it, Layers[ i ] ){
			pos_in_layer[ Layers[ i ][ it ] ] = count;
			assigned_rank[ Layers[ i ][ it ] ] = i;
			count++;
		    }
		}
		
		if( ourMethodFlag == true ){
		    identify_dummy_positionsOur( G, Layers, max, dummyNodes, xpos, ypos, algorithmFlag, space, increment, ledaPostFlag );
		}
		else{
		    if( brandFlag == true ){
			  algorithmFlag = 1;
		    }
		    else{
			  if(  brandFlag2 == true ){
				algorithmFlag = 3;
			  }
		    }
		    identify_dummy_positionsY( G, Layers, max, dummyNodes, xpos, ypos, algorithmFlag, space, increment, ledaPostFlag );
		}
		
		node_array<bool> isDummy( G, false );
		node_array<bool> isUsed( G, false );

		node m,temp;
		forall_nodes( n , G ){
				forall_items( it, dummyNodes ){
						m = dummyNodes[ it ];
						if( m == n )
							isDummy[ n ] = true;
				}
		}

		for( int i = 0; i < sourceAndTargets.size(); i++ ){
			forall_edges( e, G ){
				if( G.source( e ) == sourceAndTargets[ i ][ sourceAndTargets[ i ].first_item() ] &&
				    G.target( e ) == sourceAndTargets[ i ][ sourceAndTargets[ i ].last_item() ]){
					e = G.rev_edge( e );
//  					cout << " REVERSED \n";
				}
			}
		}

		edge_array<three_tuple<node,node,int> > tuples( G );
		forall_edges( e, G ){
		      three_tuple<node,node,int> t_e( G.source( e ), G.target( e ), 0 );
		      tuples[ e ] = t_e;
		}

		forall_edges( e, G ){
		      edge e_t;
		      three_tuple<node,node,int> t_e( G.source( e ), G.target( e ), 1 );
		      tuples[ e ] = t_e;
		      forall_edges( e_t, G ){
			      if( tuples[ e_t ].third() != 1 && e != e_t && tuples[ e ].first() == tuples[ e_t ].first() && tuples[ e ].second() == tuples[ e_t ].second() ){
				      three_tuple<node,node,int> t_e2( G.source( e_t ), G.target( e_t ), 1 );
				      tuples[ e_t ] = t_e2;
				      e_t = G.rev_edge( e_t );
// 				      cout << " REVERSED \n";
			      }
		      }
		}

		edge_array<list<double> > edgePositionsX( G );
		edge_array<list<double> > edgePositionsY( G );
		edge_array<list<point> > bends(G);
		list<point> tempPoints;

		list<edge> inEdges;

		forall_nodes( n , G ){
				node source;
				node target;
				int edgeValue;
				//IS dummy ancak ondan onceki node source olacakti


				if( isUsed[ n ] == false && isDummy[ n ] == true ){
						list<double> eXpos;
						list<double> eYpos;
						
						temp = n;
						
						eXpos.append( xpos[ temp ] );
						eYpos.append( ypos[ temp ] );
						point p( xpos[ temp ], ypos[ temp ] );
						tempPoints.append( p );

						inEdges = G.in_edges( temp );
						forall_items( it, inEdges ){
								e = allEdges[ it ];	
								n = G.source( e );
						}
						isUsed[ n ] = true;
						source = n;
						
						NEXTDUMMY:
						allEdges = G.out_edges( temp );
						edge etemp;
						forall_items( it, allEdges ){
								e = allEdges[ it ];	
								m = G.target( e );
								eXpos.append( xpos[ m ] );
								eYpos.append( ypos[ m ] );
								point p( xpos[ m ], ypos[ m ] );
								tempPoints.append( p );

								isUsed[ m ] = true;
								edgeValue = G[ e ];
								G.del_edge( e );
						}
						if( isDummy[ m ] == true ){
								temp = m;
								goto NEXTDUMMY;
						}
						else{
								target = m;
						}
						etemp = G.new_edge( source, target );
						G[ etemp ] = edgeValue;
						bends[ etemp ] = tempPoints;
						edgePositionsX.init( G );
						edgePositionsY.init( G );
						edgePositionsX[ etemp ] = eXpos;
						edgePositionsY[ etemp ] = eYpos;
#ifdef POSITIONS
						cout << endl << " X : ";
						edgePositionsX[ etemp ].print();
						cout << endl << " Y : ";
						edgePositionsY[ etemp ].print();
#endif
						tempPoints.clear();
				}
				isUsed[ n ] = true;
		}
		
		node_array<point> pos(G);
// 		//runWithMenu( G );
		int decision;
		if( brandFlag == true ){
			decision = 1;
		}
		else{
			if( ourMethodFlag == true ){
				decision = 2;
			}
			else{
				if( brandFlag2 == true ){
					decision = 3;
				}
			}
		}

		node_array<leda::color> Colors( G, black );
		node_array<leda::string> Names( G );
		int i = 1;
		forall_nodes( n, G ){
// 		    cout << G[ n ] << " ";
		    G[ n ]=i;

		    i++;
		}
        
		GraphWin gw2(G);
// 		gw2.save_gml( "main_graph" );

// 		ledaToGraphviz( G, Layers, isDummy, Layers.size(), "graph_graphviz", Colors, false, Names, false ); 
		G.del_nodes( dummyNodes );
		pos = draw_final2 ( G, PARS, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, 1, Hvalues, xpos, ypos, decision, nodeSize,edgeBendImp,colorScale,edgThicknessTher, categ );
// 

// 		node_array<point> pos(G);
// 		forall_nodes( n, G ){
// // 			cout <<  xpos[ n ]<< "-" << ypos[ n ] << endl;
// 			point p( xpos[ n ], ypos[ n ] );
// 			pos[ n ] = p;
// 		}
		
		
// 		draw_finalY( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, graphNo, pos, Hvalues );
		
		posx = pos;
		bendsx = bends;
		forall_nodes( n, G ){
			Xpos[ n ] = pos[ n ].xcoord();
			Ypos[ n ] = pos[ n ].ycoord();
		}
		return G;
}

void RUN_AGAIN(   GRAPH<int,int> G, 
		  int W, 
		  node_array<double> &Xpos ,
		  node_array<double> &Ypos, 
		  char filename[ 1024 ], 
		  int graphNo, 
		  list<list<struct Strings> > namesForEachGraph, 
		  node_array<point> pos,
		  edge_array<list<point> > bends, 
		  array<char> &abbv, 
		  int cat_num, 
		  array<char> &Categories 
){

		node n;
		GraphWin gw2(G,300,300);	
// 		array<bool> colorChoose( cat_num );
// 		for( int c = 0; c < cat_num; c++ ){
// 			colorChoose[ c ] = false;
// 		}
// 		//black, white, red, green, blue, yellow, violet, orange, cyan, 
// 		//brown, pink, green2, blue2, grey1, grey2, grey3. 
// 		panel P2( 200,150, "Determine Node Colors" );
// 		for( ; ; ){
// 			P2.text_item( "\\bf\\blue Choose Your Color \\" );
// 			P2.bool_item( "Black  ", colorChoose[ 0 ] );
// 			P2.bool_item( "White  ", colorChoose[ 1 ] );
// 			P2.bool_item( "Red    ", colorChoose[ 2 ] );
// 			P2.bool_item( "Green  ", colorChoose[ 3 ] );
// 			P2.bool_item( "Blue   ", colorChoose[ 4 ] );
// 			P2.bool_item( "Yellow ", colorChoose[ 5 ] );
// 			P2.bool_item( "Violet ", colorChoose[ 6 ] );
// 			P2.bool_item( "Orange ", colorChoose[ 7 ] );
// 			P2.bool_item( "Cyan   ", colorChoose[ 8 ] );
// 			P2.bool_item( "Brown  ", colorChoose[ 9 ] );
// 			P2.bool_item( "Pink   ", colorChoose[ 10 ] );
// 			P2.bool_item( "Green2 ", colorChoose[ 11 ] );
// 			P2.bool_item( "Blue2  ", colorChoose[ 12 ] );
// 			P2.open();
// 			if( P2.button_press_time() > 0 ) break;
// 			P2.display();
// 			P2.clear();
// 		}
// 		P2.close();
// 		P2.clear();
// 		int random_value;
// 		for( int c = 0; c < cat_num; c++ ){
// 			if(colorChoose[ c ] == true ){
// 			      random_value = c;
// 			      break;
// 			}
// 		}
// 		color choose( random_value );
		list<struct Strings> nodeNames;
		//list<list<Strings>> namesForEachGraph
		int counter = 0;
		list_item it;
		forall_items( it, namesForEachGraph ){
			nodeNames = namesForEachGraph[ it ];
			if( counter == graphNo )
				break;
			counter++;
		}
		
		gw2.set_edge_thickness( 3, true );
		forall_nodes( n, G ){
			for( int i = 0; i < cat_num; i++ ){
				if( Categories[ G[ n ] ] == abbv[ i ] ){
					leda::color x( i + 1 );
					gw2.set_color( n, x );
				}
			}
		}
		gw2.set_node_height(50,true);
		gw2.set_node_border_color( red, true );
		gw2.set_node_border_thickness( 3, true );
		gw2.set_node_width( 100, true );
		gw2.set_node_shape( leda::ovalrect_node, true );
		gw2.set_edge_label_type(leda::data_label , true);
		gw2.set_edge_label_font(leda::roman_font, 10);
		gw2.set_edge_shape( leda::poly_edge, true );
		gw2.set_edge_color( leda::blue , true );
		node s;
		it = nodeNames.first_item();
		struct Strings temp_str;
		forall_nodes( s, G ){
			temp_str = nodeNames[ it ];
			gw2.set_label(s,temp_str.name);
			it = nodeNames.succ( it );
		}

		gw2.set_layout( pos, bends );
// 		gw2.set_edge_shape( leda::bezier_edge, true );
		
		gw2.set_animation_steps(5);

//  		gw2.display();
// 		gw2.zoom_graph();
// 		char filename2[32] = "sources/a0poster/img";
// 		sprintf( filename2, "%s%d%s", filename2, graphNo, ".ps" );
// 		gw2.save_ps( filename2 );
// 		gw2.edit();
}

void RUN_AGAIN2(  GRAPH<int,int> G, 
		  int W, 
		  node_array<double> &Xpos,
		  node_array<double> &Ypos, 
		  char filename[ 1024 ], 
		  int graphNo, 
		  list<list<struct Strings> > namesForEachGraph, 
		  node_array<point> pos ,
		  edge_array<list<point> > bends, 
		  array<char> &abbv,
		  int cat_num,
		  array<char> &Categories,
		  double edgeBendImp, 
		  double colorScale, 
		  double edgThicknessTher
 ){
	if( G.number_of_nodes() != 0 ){
		node n;
		GraphWin gw2(G,300,300);	

		list<struct Strings> nodeNames;
		//list<list<Strings>> namesForEachGraph
		int counter = 0;
		list_item it;
		forall_items( it, namesForEachGraph ){
			nodeNames = namesForEachGraph[ it ];
			if( counter == graphNo )
				break;
			counter++;
		}
		node_array<int> catid( G, 0 );
		gw2.set_edge_thickness( 3, true );
		forall_nodes( n, G ){
			for( int i = 0; i < cat_num; i++ ){
				if( Categories[ G[ n ] ] == abbv[ i ] ){
					leda::color x( i /*+ 1*/ );
					gw2.set_border_color( n,x );
					catid[ n ] = i;
				}
			}
			gw2.set_color( n, red );
		}
		edge e;
		array<color> color_l( 5 );

		list<int> weights;
		forall_edges( e, G ){

		}
		list<two_tuple<color,int> > variations;
		list<int> edgeWeights;

		forall_edges( e, G ){
			int flag = 0;
			forall_items( it, edgeWeights ){
				if( edgeWeights[ it ] == G[ e ] ){
					flag = 1;
					break;
				}
			}
			if( flag == 0 ){
				edgeWeights.push_back( G[ e ] );
			}
		}
		edgeWeights.sort();
		int tm_c = (double)(220.0 / (double)edgeWeights.size() );
		int count = 20 + edgeWeights.size() * tm_c;
		for( int j = 0; j < color_l.size(); j++ ){
			color el( count, count, count );
			color_l[ j ] = el;
			count -= tm_c;
		}		
		count = 0;
		for( int x = edgeWeights[edgeWeights.first()]; x <= edgeWeights[edgeWeights.last()]; x++ ){	
			for( int j = 0; j < color_l.size(); j++ ){
				two_tuple<color,int> Tp( color_l[ j ], x );
// 				cout << Tp << endl;
				variations.push_back( Tp );
			}
			count++;
		}
		int increase = (int) ((double)variations.size() / (double)edgeWeights.size());
// 		cout << " increase : " << increase << endl;
		forall_edges( e, G ){              
			count = 0;
			forall_items( it, edgeWeights ){
			      if( edgeWeights[ it ] == G[ e ] ){
				    break;
			      }
			      count++;
			}
			two_tuple<color,int> Tp = variations[ variations.get_item( increase / edgThicknessTher * count ) ] ;
			gw2.set_thickness( e, Tp.second() / edgThicknessTher * 5.0 );
			gw2.set_color( e, Tp.first() );
// 			cout << Tp << endl;
		}

		gw2.set_node_height(50,true);
// 		gw2.set_node_border_color( red, true );
		gw2.set_node_border_thickness( 3, true );
		gw2.set_node_width( 100, true );
		gw2.set_node_shape( leda::ovalrect_node, true );
		gw2.set_edge_label_type(leda::data_label , true);
		gw2.set_edge_label_font(leda::roman_font, 10);
		gw2.set_edge_shape( leda::poly_edge, true );
		node s;
		it = nodeNames.first_item();
		struct Strings temp_str;
		forall_nodes( s, G ){
			temp_str = nodeNames[ it ];
			gw2.set_label(s,temp_str.name);
			G.assign( s, catid[ s ] );
			it = nodeNames.succ( it );
		}
		gw2.update_graph();
		gw2.set_layout( pos, bends );
// 		gw2.set_edge_shape( leda::bezier_edge, true );
		
// 		gw2.set_animation_steps(5);
		gw2.save_gml(filename);
// 		gw2.display();
// 		gw2.save_gml(filename);
// 		gw2.zoom_graph();
		char filename2[64] = "outputs/psfiles/img";
		sprintf( filename2, "%s%d%s", filename2, graphNo, ".ps" );
		gw2.save_ps( filename2 );
// 		gw2.edit();
	}
}

void RUN_AGAIN2_COLOR(  GRAPH<int,int> G, 
		  int W, 
		  node_array<double> &Xpos,
		  node_array<double> &Ypos, 
		  char filename[ 1024 ], 
		  int graphNo, 
		  list<list<struct Strings> > namesForEachGraph, 
		  node_array<point> pos ,
		  edge_array<list<point> > bends, 
		  array<char> &abbv,
		  int cat_num,
		  array<list<char> > &Categories,
		  double edgeBendImp, 
		  double colorScale, 
		  double edgThicknessTher
 ){
// 	cout << " WE ARE IN : " << graphNo << endl;
	if( G.number_of_nodes() != 0 ){
		node n;
		GraphWin gw2(G,300,300);	

		list<struct Strings> nodeNames;
		//list<list<Strings>> namesForEachGraph
		int counter = 0;
		list_item it,it2;
		forall_items( it, namesForEachGraph ){
			nodeNames = namesForEachGraph[ it ];
			if( counter == graphNo )
				break;
			counter++;
		}


                node_array<list<char> > catid( G );
                char pie[256];
                FILE *pieNode;
#ifdef LINUX
                sprintf( pie, "%s%d.txt", "outputs/enrich/pie", graphNo );
                pieNode = fopen( pie, "w" );
#else
                sprintf( pie, "%s%d.txt", "outputs//enrich//pie", graphNo );
                pieNode = fopen( pie, "w" );
#endif

                array<int> abbv_i( abbv.size() );
                int geneCount = 0;
                for( int i = 0; i < cat_num; i++ ){
                    abbv_i[ i ] = 0;
                }

                forall_nodes( n, G ){
                        for( int i = 0; i < cat_num; i++ ){
                                forall_items( it, Categories[ G[ n ] ] ){
                                        if( Categories[ G[ n ] ][ it ] == abbv[ i ] ){
                                                catid[ n ].append( abbv[ i ] );
                                                abbv_i[ i ]++;
                                        }
                                }
                        }
                        geneCount++;
                        gw2.set_color( n, red );
                }

                double dsum = 0;
                for( int i = 0; i < cat_num; i++ ){
                    if( (double)abbv_i[ i ] / (double)geneCount > 0 ){
                        fprintf( pieNode, "%c %lf\n", abbv[ i ], (double)((double)abbv_i[ i ] / (double)geneCount ) );
                        cout << abbv[ i ] << "\t" << (double)abbv_i[ i ] / (double)geneCount << endl;
                        dsum += (double)abbv_i[ i ] / (double)geneCount;
                    }
                }
                if( dsum < 1.0 )
                    fprintf( pieNode, "%c %lf\n", 'X', 1.0 - dsum );

                fclose( pieNode );

// 		cout << " COLOR SELECTION IS DONE\n";
		edge e;
		array<color> color_l( 5 );

		list<int> weights;
//		forall_edges( e, G ){
//
//		}
		list<two_tuple<color,int> > variations;
		list<int> edgeWeights;
                if( G.number_of_edges() != 0 ){
                    forall_edges( e, G ){
                            int flag = 0;
                            forall_items( it, edgeWeights ){
                                    if( edgeWeights[ it ] == G[ e ] ){
                                            flag = 1;
                                            break;
                                    }
                            }
                            if( flag == 0 ){
                                    edgeWeights.push_back( G[ e ] );
                            }
                    }
                    edgeWeights.sort();
                    int tm_c = (double)(220.0 / (double)edgeWeights.size() );
                    int count = 20 + edgeWeights.size() * tm_c;
                    for( int j = 0; j < color_l.size(); j++ ){
                            color el( count, count, count );
                            color_l[ j ] = el;
                            count -= tm_c;
                    }
                    count = 0;
                    for( int x = edgeWeights[edgeWeights.first()]; x <= edgeWeights[edgeWeights.last()]; x++ ){
                            for( int j = 0; j < color_l.size(); j++ ){
                                    two_tuple<color,int> Tp( color_l[ j ], x );
// 				cout << Tp << endl;
                                    variations.push_back( Tp );
                            }
                            count++;
                    }
                    int increase = (int) ((double)variations.size() / (double)edgeWeights.size());
// 		cout << " increase : " << increase << endl;
                    forall_edges( e, G ){
                            count = 0;
                            forall_items( it, edgeWeights ){
                                  if( edgeWeights[ it ] == G[ e ] ){
                                        break;
                                  }
                                  count++;
                            }
                            two_tuple<color,int> Tp = variations[ variations.get_item( increase / edgThicknessTher * count ) ] ;
                            gw2.set_thickness( e, Tp.second() / edgThicknessTher * 5.0 );
                            gw2.set_color( e, Tp.first() );
// 			cout << Tp << endl;
                    }
                    gw2.set_edge_label_type(leda::data_label , true);
                    gw2.set_edge_label_font(leda::roman_font, 10);
                    gw2.set_edge_shape( leda::poly_edge, true );
                }
		gw2.set_node_height(50,true);
// 		gw2.set_node_border_color( red, true );
		gw2.set_node_border_thickness( 3, true );
		gw2.set_node_width( 100, true );
                gw2.set_node_shape( leda::ovalrect_node, true );
		node s;
		it = nodeNames.first_item();
		struct Strings temp_str;
		forall_nodes( s, G ){
			temp_str = nodeNames[ it ];
			strcat( temp_str.name, "_" );
			forall_items( it2, catid[ s ] ){
				char number[4];
				sprintf( number, "%c:", catid[ s ][ it2 ] );
				strcat( temp_str.name, number );
			}
			gw2.set_label(s,temp_str.name);
// 			G.assign( s, catid[ s ] );
			it = nodeNames.succ( it );
		}
		gw2.update_graph();
		gw2.set_layout( pos, bends );
// 		gw2.set_edge_shape( leda::bezier_edge, true );
		
// 		gw2.set_animation_steps(5);
		gw2.save_gml(filename);
// 		gw2.display();
// 		gw2.save_gml(filename);
// 		gw2.zoom_graph();
		char filename2[64] = "outputs/psfiles/img";
		sprintf( filename2, "%s%d%s", filename2, graphNo, ".ps" );
		gw2.save_ps( filename2 );
// 		gw2.edit();
	}
}

void RUN_FFD_AGAIN2(  GRAPH<int,int> G, 
		  int W, 
		  node_array<double> &Xpos,
		  node_array<double> &Ypos, 
		  char filename[ 1024 ], 
		  int graphNo, 
		  list<list<struct Strings> > namesForEachGraph, 
		  node_array<point> pos ,
		  edge_array<list<point> > bends, 
		  array<char> &abbv,
		  int cat_num,
		  array<char> &Categories,
		  double edgeBendImp, 
		  double colorScale, 
		  double edgThicknessTher
 ){
	if( G.number_of_nodes() != 0 ){
		node n;
		GraphWin gw2(G,300,300);	

		list<struct Strings> nodeNames;
		//list<list<Strings>> namesForEachGraph
		int counter = 0;
		list_item it;
		forall_items( it, namesForEachGraph ){
			nodeNames = namesForEachGraph[ it ];
			if( counter == graphNo )
				break;
			counter++;
		}
		node_array<int> catid( G, 0 );
		gw2.set_edge_thickness( 3, true );
		forall_nodes( n, G ){
			for( int i = 0; i < cat_num; i++ ){
				if( Categories[ G[ n ] ] == abbv[ i ] ){
					leda::color x( i /*+ 1*/ );
					gw2.set_border_color( n,x );
					catid[ n ] = i;
				}
			}
			gw2.set_color( n, red );
		}
		edge e;
		array<color> color_l( 5 );

		list<int> weights;
		forall_edges( e, G ){

		}
		list<two_tuple<color,int> > variations;
		list<int> edgeWeights;

		forall_edges( e, G ){
			int flag = 0;
			forall_items( it, edgeWeights ){
				if( edgeWeights[ it ] == G[ e ] ){
					flag = 1;
					break;
				}
			}
			if( flag == 0 ){
				edgeWeights.push_back( G[ e ] );
			}
		}
		edgeWeights.sort();
		int tm_c = (double)(220.0 / (double)edgeWeights.size() );
		int count = 20 + edgeWeights.size() * tm_c;
		for( int j = 0; j < color_l.size(); j++ ){
			color el( count, count, count );
			color_l[ j ] = el;
			count -= tm_c;
		}		
		count = 0;
		for( int x = edgeWeights[edgeWeights.first()]; x <= edgeWeights[edgeWeights.last()]; x++ ){	
			for( int j = 0; j < color_l.size(); j++ ){
				two_tuple<color,int> Tp( color_l[ j ], x );
// 				cout << Tp << endl;
				variations.push_back( Tp );
			}
			count++;
		}
		int increase = (int) ((double)variations.size() / (double)edgeWeights.size());
// 		cout << " increase : " << increase << endl;
		forall_edges( e, G ){              
			count = 0;
			forall_items( it, edgeWeights ){
			      if( edgeWeights[ it ] == G[ e ] ){
				    break;
			      }
			      count++;
			}
			two_tuple<color,int> Tp = variations[ variations.get_item( increase / edgThicknessTher * count ) ] ;
			gw2.set_thickness( e, Tp.second() / edgThicknessTher * 5.0 );
			gw2.set_color( e, Tp.first() );
// 			cout << Tp << endl;
		}

		gw2.set_node_height(50,true);
// 		gw2.set_node_border_color( red, true );
		gw2.set_node_border_thickness( 3, true );
		gw2.set_node_width( 100, true );
		gw2.set_node_shape( leda::ovalrect_node, true );
		gw2.set_edge_label_type(leda::data_label , true);
		gw2.set_edge_label_font(leda::roman_font, 10);
		gw2.set_edge_shape( leda::poly_edge, true );
		node s;
		it = nodeNames.first_item();
		struct Strings temp_str;
		forall_nodes( s, G ){
			temp_str = nodeNames[ it ];
			gw2.set_label(s,temp_str.name);
			G.assign( s, catid[ s ] );
			it = nodeNames.succ( it );
		}
		gw2.update_graph();
		gw2.set_layout( pos, bends );
// 		gw2.set_edge_shape( leda::bezier_edge, true );
		
// 		gw2.set_animation_steps(5);
		gw2.save_gml(filename);
// 		gw2.display();
// 		gw2.save_gml(filename);
// 		gw2.zoom_graph();
		char filename2[64] = "outputs/psfiles/img";
		sprintf( filename2, "%s%d%s", filename2, graphNo, ".ps" );
		gw2.save_ps( filename2 );
// 		gw2.edit();
	}
}

void RUN_FFD_AGAIN2_COLOR(  GRAPH<int,int> G,
		  char filename[ 1024 ], 
		  int graphNo, 
                  list<list<struct Strings> > &namesForEachGraph,
                  node_array<point> &pos ,
                  edge_array<list<point> > &bends,
		  array<char> &abbv,
		  int cat_num,
		  array<list<char> > &Categories,
		  double edgeBendImp, 
		  double colorScale, 
		  double edgThicknessTher
 ){
	if( G.number_of_nodes() != 0 ){
		node n;
		GraphWin gw2(G,300,300);	

		list<struct Strings> nodeNames;
		//list<list<Strings>> namesForEachGraph
		int counter = 0;
                list_item it,it2;
		forall_items( it, namesForEachGraph ){
			nodeNames = namesForEachGraph[ it ];
			if( counter == graphNo )
				break;
			counter++;
		}
                node_array<list<int> > catid( G );
//                cout << "0:";

                char pie[256];
                FILE *pieNode;
#ifdef LINUX
                sprintf( pie, "%s%d.txt", "outputs/enrich/pie", graphNo );
                pieNode = fopen( pie, "w" );
#else
                sprintf( pie, "%s%d.txt", "outputs//enrich//pie", graphNo );
                pieNode = fopen( pie, "w" );
#endif
                array<int> abbv_i( abbv.size() + 1 );
                int geneCount = 0;
                for( int i = 0; i <= cat_num; i++ ){
                    abbv_i[ i ] = 0;
                }

		forall_nodes( n, G ){
                        int colCount_n = 0;
			for( int i = 0; i < cat_num; i++ ){
				forall_items( it, Categories[ G[ n ] ] ){
					if( Categories[ G[ n ] ][ it ] == abbv[ i ] ){
						catid[ n ].append( abbv[ i ] );
                                                abbv_i[ i ]++;
                                                geneCount++;
                                                colCount_n++;
					}
				}
                        }
                        if( colCount_n == 0 ){
                            abbv_i[ cat_num ]++;
                            geneCount++;
                        }
			gw2.set_color( n, red );
		}

                if( geneCount > 0 ){
                    double dsum = 0;
                    for( int i = 0; i < cat_num; i++ ){
                        if( (double)abbv_i[ i ] / (double)geneCount > 0 ){
                            fprintf( pieNode, "%c %lf\n", abbv[ i ], (double)((double)abbv_i[ i ] / (double)geneCount ) );
                            dsum += (double)abbv_i[ i ] / (double)geneCount;
                        }
                    }
                    if( abbv_i[ cat_num ] > 0 )
                        fprintf( pieNode, "%c %lf\n", 'X', (double)abbv_i[ cat_num ] / (double)geneCount );
                }
                fclose( pieNode );

//                cout << "1:";
		edge e;
		array<color> color_l( 5 );

		list<int> weights;
//		forall_edges( e, G ){
//
//		}
		list<two_tuple<color,int> > variations;
		list<int> edgeWeights;
                if( G.number_of_edges() != 0 ){
                    forall_edges( e, G ){
                            int flag = 0;
                            forall_items( it, edgeWeights ){
                                    if( edgeWeights[ it ] == G[ e ] ){
                                            flag = 1;
                                            break;
                                    }
                            }
                            if( flag == 0 ){
                                    edgeWeights.push_back( G[ e ] );
                            }
                    }
    //                cout << "2:";
                    edgeWeights.sort();
                    int tm_c = (double)(220.0 / (double)edgeWeights.size() );
                    int count = 20 + edgeWeights.size() * tm_c;
                    for( int j = 0; j < color_l.size(); j++ ){
                            color el( count, count, count );
                            color_l[ j ] = el;
                            count -= tm_c;
                    }
                    count = 0;
                    for( int x = edgeWeights[edgeWeights.first()]; x <= edgeWeights[edgeWeights.last()]; x++ ){
                            for( int j = 0; j < color_l.size(); j++ ){
                                    two_tuple<color,int> Tp( color_l[ j ], x );
    // 				cout << Tp << endl;
                                    variations.push_back( Tp );
                            }
                            count++;
                    }
    //                cout << "3:";
                    int increase = (int) ((double)variations.size() / (double)edgeWeights.size());
    // 		cout << " increase : " << increase << endl;
                    forall_edges( e, G ){
                            count = 0;
                            forall_items( it, edgeWeights ){
                                  if( edgeWeights[ it ] == G[ e ] ){
                                        break;
                                  }
                                  count++;
                            }
                            two_tuple<color,int> Tp = variations[ variations.get_item( increase / edgThicknessTher * count ) ] ;
                            gw2.set_thickness( e, Tp.second() / edgThicknessTher * 5.0 );
                            gw2.set_color( e, Tp.first() );
    // 			cout << Tp << endl;
                    }
                    gw2.set_edge_thickness( 3, true );
                    gw2.set_edge_label_type(leda::data_label , true);
                    gw2.set_edge_label_font(leda::roman_font, 10);
                    gw2.set_edge_shape( leda::poly_edge, true );
                }
//                cout << "4:";
		gw2.set_node_height(50,true);
// 		gw2.set_node_border_color( red, true );
		gw2.set_node_border_thickness( 3, true );
		gw2.set_node_width( 100, true );
                gw2.set_node_shape( leda::ovalrect_node, true );
		node s;
		it = nodeNames.first_item();
		struct Strings temp_str;
		forall_nodes( s, G ){
			temp_str = nodeNames[ it ];
			strcat( temp_str.name, "_" );
			forall_items( it2, catid[ s ] ){
				char number[4];
				sprintf( number, "%c:", catid[ s ][ it2 ] );
				strcat( temp_str.name, number );
			}
			gw2.set_label(s,temp_str.name);
// 			G.assign( s, catid[ s ] );
			it = nodeNames.succ( it );
		}
		gw2.update_graph();
		gw2.set_layout( pos, bends );
// 		gw2.set_edge_shape( leda::bezier_edge, true );
// 		gw2.set_animation_steps(5);
		gw2.save_gml(filename);
// 		gw2.display();
// 		gw2.save_gml(filename);
// 		gw2.zoom_graph();
// 		char filename2[64] = "outputs/psfiles/img";
// 		sprintf( filename2, "%s%d%s", filename2, graphNo, ".ps" );
// 		gw2.save_ps( filename2 );
// 		gw2.edit();
	}
}
