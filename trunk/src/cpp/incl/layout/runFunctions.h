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
	char name[ 16 ];
	int nodeNumber;
};



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

		//runWithMenu( G );
		layers = Layers;
		if( xCoordFlag == true )
			pos = draw_final3( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, graphNo, xpos, ypos );
		else
			draw_finalX( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends, graphNo, pos );

		posx = pos;
		bendsx = bends;
		

		//ofstream NodePositions("C://GraphUnion//GraphPositions.txt", ios::in | ios::trunc);
		//ofstream EdgePositions("C://GraphUnion//EdgePositions.txt", ios::in | ios::trunc);
		//ofstream FirstEdgePositions("C://GraphUnion//FirstEdgePositions.txt", ios::in | ios::trunc );

		//forall_nodes( n, G ){
		//		NodePositions << xpos[ n ] << "\n" << ypos[ n ] << "\n";
		//}

		//forall_edges( e, G ){
		//		list<point> tempPoints = bends[ e ];
		//		int counter = 0;
		//		node s = G.source( e );
		//		node t = G.target( e );
		//		EdgePositions << xpos[ s ] << "\n" << ypos[ s ] << "\n";
		//		

		//		int melcount = 0;
		//		forall_items( it, tempPoints ){
		//				if( it == tempPoints.first_item() ){
		//						FirstEdgePositions << xpos[ s ] << "\n" << ypos[ s ] << "\n";
		//						FirstEdgePositions << tempPoints[ it ].xcoord()<< "\n" << tempPoints[ it ].ycoord() << "\n";
		//						melcount = 1;
		//				}	
		//				EdgePositions << tempPoints[ it ].xcoord()<< "\n" << tempPoints[ it ].ycoord() << "\n";
		//				/*if( tempPoints.succ( it ) != NULL ){
		//					list_item it2 = tempPoints.succ( it );
		//					angles = tempPoints[ it ].angle( tempPoints[ it ], tempPoints[ it2 ] );
		//					length = tempPoints[ it ].( tempPoints[ it ], tempPoints[ it2 ] );
		//				}*/
		//				counter = 1;
		//		}
		//		//if( counter != 0 )
		//				//EdgePositions << (double)100000 << endl;
		//		//else{
		//				EdgePositions << xpos[ t ] << "\n" << ypos[ t ] << "\n" << 100000 << endl;
		//		//}
		//				
		//}
		//EdgePositions << (double) 200000 << endl;
		//FirstEdgePositions << 200000 << endl << 200000 << endl << 200000 << endl << 200000;
		//NodePositions.close();
		//FirstEdgePositions.close();
		//EdgePositions.close();
		////draw_planets( G, Layers, max, dummyNodes, edgePositionsX , edgePositionsY, bends );

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
	SPRING_EMBEDDING_( G, xpos, ypos, 0,500,0,500, 500, nodeid, abbv, cat_num, Categories );
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
		
		gw2.set_edge_thickness( 3, true );
		forall_nodes( n, G ){
			for( int i = 0; i < cat_num; i++ ){
				if( Categories[ G[ n ] ] == abbv[ i ] ){
					leda::color x( i /*+ 1*/ );
					gw2.set_border_color( n,x );
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
			it = nodeNames.succ( it );
		}

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

		node n;
		GraphWin gw2(G,500,500);	

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
					gw2.set_border_color( n,x );
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
			gw2.set_thickness( e, 0.1 );
			gw2.set_color( e, Tp.first() );
// 			cout << Tp << endl;
		}

		gw2.set_node_height(5,true);
// 		gw2.set_node_border_color( red, true );
		gw2.set_node_border_thickness( 3, true );
		gw2.set_node_width( 10, true );
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
			it = nodeNames.succ( it );
		}
		gw2.place_into_box( 0, 0, 500, 500 );
		gw2.set_layout( pos, bends );
// 		gw2.set_edge_shape( leda::bezier_edge, true );
		
// 		gw2.set_animation_steps(5);
		gw2.save_gml(filename);
// 		gw2.display();
// 		gw2.save_gml(filename);
// 		gw2.zoom_graph();
		char filename2[32] = "outputs/psfiles/img_";
		sprintf( filename2, "%s%d%s", filename2, graphNo, ".ps" );
		gw2.save_ps( filename2 );
// 		gw2.edit();
}