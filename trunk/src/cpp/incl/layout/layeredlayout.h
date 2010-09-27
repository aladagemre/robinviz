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

graphUnion.h is for general coding. Specifically, functions in this header files
implements
  Sugiyama Layout Weighted/Unweighted  
    Cycle Removal
    Layering
    X Coordinate Assignment
  Drawing Functions
  

Created

01 January 2009

*************************************************************************************/

#include "misc/declerations.h"
#include "misc/graphToMCLInput.h"
#include "misc/MCLInputToGraph.h"

//#define DRAW_FINAL
#define DRAW_FINAL2
// #define DEBUG_X_OUR  
#define SIZE_sep 50.0

list<edge> cycleEdges;
integer cross1;
integer cross2;
integer cross3;


void callCoffman ( GRAPH<int,int> &G, node_array<int>& rank, int W);
void callModifiedCoffman ( GRAPH<int,int> &G, node_array<int>& rank, int W);
void insert ( GRAPH<int,int> &G,node v, list<node> &ready, const node_array<int> &pi);
void insert ( GRAPH<int,int> &G, node u, list<two_tuple<node,int> > &ready_nodes);

int cycleEdgesCount( list<edge> cycleEdges );
void cycleFinder( node root, GRAPH<int,int> &G ,node_array<bool>& visited ,list<node>& A ,list<edge>& dfsEList );

void draw( GRAPH<int,int> &G, node_array<bool> visited, list<edge> visits );
void draw_bicomponents( GRAPH<int,int> &G );
void drawCycleEdges( GRAPH<int,int> &G, node_array<bool> visited, list<edge> visits, list<edge> cycles );
node_array<point> draw_final ( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy , edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends, int graphNo );
void draw_dummy_centered ( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy, node_array<double> &x , node_array<double> &y );
void draw_dummy ( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy );
void draw_new ( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE );
void draw_planets( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy , edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends );
void drawComponentsWindow(edge_array<int> compnum,GRAPH<int,int> &G );
void draw_network( GRAPH<int,int> &G , node_array<double> &x, node_array<double> &y );



void dfsReduced(node v, node_array<int> &mark, b_stack<node> &visited  ); // Layer Assignment Reduced digraph
void do_call (const GRAPH<int,int>& G, list<edge>& transitives ); // Layer Assignment Reduced digraph

node find_nodes( int source , GRAPH<int,int> &G );
bool inNodeList( list<node> U, node n );
void identify_dummy_positions( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy, node_array<double> &x , node_array<double> &y );
bool inList( list<edge> suspis, edge e );
bool isFound( GRAPH<int,int> &X , int source );

bool TOPSORTG(GRAPH<int,int> &G, node_array<int>& ord);
void processCycleEdges( GRAPH<int,int> &G, list<edge> visits, list<edge> &cycles );
void printWeight( GRAPH<int,int> &G );

node find( node x, GRAPH<d3_rat_point,int> &H, node_array<int> indexH, node_array<int> index );
void redraw( list<d3_rat_point>& ptlist , window &W );
GRAPH<int,int> RUN2( int noden , int edgen, int max_maight, int W );
int RUN( int noden , int edgen, int max_maight, int W );
void runWithMenu( GRAPH<int,int> G );


void determineYPositions( GRAPH<int,int> &G , array<list<node> > &AB, node_array<double> &y, double increment );
void pos_process( GRAPH<int,int> &G , array<list<node> > &AB, node_array<double> &x, double minSepConst, double increment, int iterations, list<node> &dummy );
//Determine the components with coloring


void drawComponentsWindow(edge_array<int> compnum,GRAPH<int,int> &G ){
	
	edge e;
	double range = 50.0;
	node_array<int> x(G);
	node_array<int> y(G);
	
	random_source X( 0,1000 );
	random_source Y( 0,1000 );
	int xx;
	int yy;

	node n;
	forall_nodes( n, G ){
		X >> xx;
		Y >> yy;
		x[n] = xx;
		y[n] = yy;
	}

	window W;
	double max_x = 0;
	forall_nodes( n ,G ){
		if( x[ n ] > max_x )
			max_x = x[ n ];
	}
	double min_x = max_x;
	forall_nodes( n ,G ){
		if( x[ n ] < min_x )
			min_x = x[ n ];
	}
	double max_y = 0;
	forall_nodes( n ,G ){
		if( y[ n ] > max_y )
			max_y = y[ n ];
	}
	double min_y = max_y;
	forall_nodes( n ,G ){
		if( y[ n ] < min_y )
			min_y = y[ n ];
	}

	W.init( min_x-20-range,max_x+20+range,min_y-20-range,max_y+20+range );
	W.open(); W.display();
	int comp = 0;
	for (;;) {
		W.clear();

		list<point> L;
				 
		forall_nodes( n, G ){
			point z( x[ n ], y[ n ] );
			L.append( z );
		}
		W.set_node_width(6);W.set_line_width(1);

		point p;
		forall(p,L) 
			W.draw_filled_node(p.to_point());
		forall_edges(e,G) {
			if( compnum[ e ] < comp ){
				point r1( x[ G.source( e )],y[ G.source( e )] );
				point r2( x[ G.target( e )],y[ G.target( e )] );
				
				if( compnum[ e ] % 4 == 0 )
					W.draw_edge( r1, r2, red );
				if( compnum[ e ] % 4 == 1 )
					W.draw_edge( r1, r2, green );
				if( compnum[ e ] % 4 == 2 )
					W.draw_edge( r1, r2, blue );
				if( compnum[ e ] % 4 == 3 )
					W.draw_edge( r1, r2, pink );
					
				//W.draw_edge( r1, r2, red );
				cout << compnum[ e ];
			}
		}
		comp++;
		//W.zoom_area( r1,r2,2 );
		W.redraw();
		if (W.read_mouse()==MOUSE_BUTTON(3)) break;
	}
}

//Used in main
bool isFound( GRAPH<int,int> &X , int source ){
	node n;
	forall_nodes( n , X ){
		if( X[ n ] == source ){
			return true;
		}
	}
	return false;
}
//Used in main
node find_nodes( int source , GRAPH<int,int> &G ){
	node n;
	forall_nodes( n , G ){
			if( G[ n ] == source )
				return n;
	}
	return n;
}
void draw_bicomponents( GRAPH<int,int> &G ){

	double initialX = 100 , initialY = 100 , incX = 200 , incY = 100 ,i;
	node_array<double> x(G),y(G) ;
	list<node> A;
	color blue( 4 );

	GraphWin gw(G);

	node n ;
	int cou = 0;

	cout << endl;
	i = initialX ;

	forall_nodes( n,G){
			x[n] = i ; 
			i += incX ;
			y[n] = initialY;
	}

	cou+=1;
	initialY += incY;
	
	gw.set_edge_thickness( 0.5, true );

    gw.set_node_height(10,true);
	gw.set_layout();
    gw.set_position(x,y);
    gw.set_animation_steps(3);
    gw.set_edge_label_type(leda::data_label , true);
	gw.remove_bends();
	gw.display();
	gw.message("X");
	gw.zoom_graph();
	gw.message("You can see the graph below, and you can use menu for some tests");
	gw.edit();

}

list<edge> cycleCatcher( GRAPH<int,int> &G , edge e , node source , node target, edge_array<bool> onPath, node_array<bool> visited ){
    list<edge> cycle;
	list<edge> temp;

	cycle.append( e );
	visited[ source ] = true;

	temp = G.in_edges( source );
	list_item it;
	edge etemp;
	node ntemp;
	
	NEXTNODE:

	int count = 0;
	forall_items( it, temp ){
			etemp = temp[ it ];
			ntemp = G.source( etemp );
			if( onPath[ etemp ] == false && visited[ ntemp ] == false ){
					cycle.append( etemp );
					onPath[ etemp ] = true;
					visited[ ntemp ] = true;
					count = 1;
					if( ntemp == target )
						goto END;
			}
	}
	if( count == 1 ){
			temp = G.in_edges( ntemp );
			goto NEXTNODE;
	}
	
	END:
	return cycle;
}
void dfs( node v, GRAPH<int,int> &G, node_array<bool>& visited, list<node>& A, list<edge>& dfsEList ){
    visited[v] = true;
 	edge e;	A.append( v );

	forall_out_edges(e,v) {
		if( G[ e ] != 0 ){
			node u = G.opposite(v,e);
			if (!visited[u])
			{
					dfsEList.append( e );
					A.append( u );
					dfs(u,G,visited,A, dfsEList);		
			}
		}
	}
}

void printWeight( GRAPH<int,int> &G ){
	list_item it;
	edge e;
	cout << endl;
	forall_items( it, cycleEdges ){
			e = cycleEdges[ it ];
			cout << "->" << G[ e ];
	}
	cout << endl;
}

bool inList( list<edge> suspis, edge e ){
		list_item it;
		edge exist;
		forall_items( it , suspis ){
			exist = suspis[ it ];
			if( exist == e ){
					return true;
			}
		}
		return false;
}

int cycleEdgesCount( list<edge> cycleEdges ){
		list_item it;
		int count = 0;
		forall_items( it , cycleEdges ){
				count++;
		}
		return count;
}

void draw( GRAPH<int,int> &G, node_array<bool> visited, list<edge> visits ){

	double initialX = 100 , initialY = 100 , incX = 200 , incY = 100 ,i;
	node_array<double> x(G),y(G) ;
	list<node> A;
	color blue( 4 );

	GraphWin gw(G);

	node n ;
	int cou = 0;

	cout << endl;
	i = initialX ;

	forall_nodes( n,G){
			x[n] = i ; 
			i += incX ;
			y[n] = initialY;
	}

	edge e1,e2;
	forall( e1, visits ){
			forall_edges( e2, G ){
					if( e1 == e2 )
						gw.set_color( e1, blue );
			}
	}

	cou+=1;
	initialY += incY;
	
	gw.set_edge_thickness( 0.5, true );

    gw.set_node_height(10,true);
    gw.set_position(x,y);
    gw.set_animation_steps(3);
    gw.set_edge_label_type(leda::data_label , true);
	gw.remove_bends();
	gw.display();
	gw.message("X");
	gw.zoom_graph();
	gw.message("You can see the graph below, and you can use menu for some tests");
	gw.edit();

}

void drawCycleEdges( GRAPH<int,int> &G, node_array<bool> visited, list<edge> visits, list<edge> cycles ){

	double initialX = 100 , initialY = 100 , incX = 200 , incY = 100 ,i;
	node_array<double> x(G),y(G) ;
	list<node> A;
	color blue( 4 );
	color red( 7 );

	GraphWin gw(G);

	node n ;
	int cou = 0;

	cout << endl;
	i = initialX ;

	forall_nodes( n,G){
			x[n] = i ; 
			i += incX ;
			y[n] = initialY;
	}

	edge e1,e2;
	forall( e1, visits ){
			forall_edges( e2, G ){
					if( e1 == e2 )
						gw.set_color( e1, blue );
			}
	}

	forall( e1, cycles ){
			forall_edges( e2, G ){
					if( e1 == e2 )
						gw.set_color( e1, red );
			}
	}

	cou+=1;
	initialY += incY;
	
	gw.set_edge_thickness( 0.5, true );

    gw.set_node_height(10,true);
    gw.set_position(x,y);
    gw.set_animation_steps(3);
    gw.set_edge_label_type(leda::data_label , true);
	gw.remove_bends();
	gw.display();
	gw.message("X");
	gw.zoom_graph();
	gw.message("You can see the graph below, and you can use menu for some tests");
	gw.edit();

}

void processCycleEdges( GRAPH<int,int> &G, list<edge> visits, list<edge> &cycles ){
	list_item it,ittemp;
	edge e,emax;
	list<edge> sortedCycles;

	REPEAT:
	int max = 0;
	forall_items( it, cycles ){
			e = cycles[ it ];
			if( max < G[ e ] && inList( sortedCycles, e ) == false ){
					max = G[ e ];
					emax = e;
					ittemp = it;
			}
	}

	sortedCycles.push_front( emax );
	if( cycleEdgesCount( sortedCycles ) != cycleEdgesCount( cycles ) )
			goto REPEAT;
	
	cycles = sortedCycles;
	//cycles.print();

}
void cycleFinder( node root, GRAPH<int,int> &G ,node_array<bool>& visited ,list<node>& A ,list<edge>& dfsEList ){
		
		node n;
		list<edge> elist;
		list_item it,it2;
		elist = G.out_edges( root );
		edge e1,e2;
	    
		visited[ root ] = true;
		int i = 1;
		int counter = 0;

		forall( n, A )
			counter++;

		forall_items( it, elist ){
				e1 = elist[ it ];
				forall_items( it2, dfsEList ){
						e2 = dfsEList[ it2 ];
						if( e1 == e2 && ( G[ e1 ] != 0 || G[ e2 ] != 0 ) ){
								n = G.target( e2 );
								i++;
								cycleEdges += e2;
#ifdef CYCLES
								cycleEdges.print();
								cout << endl;
#endif
								visited[ n ] = true;
						}
				}
		}
		BREAK:
		elist = G.out_edges( n );
		forall_items( it, elist ){	
			if( i <= counter ){
					e1 = elist[ it ];
					n = G.target( e1 );
					if( G[ e1 ] != 0 ){
							if( visited[ n ] != true ){
									cycleEdges += e1;
									printWeight( G );
#ifdef CYCLES
									cycleEdges.print();
									cout << endl;
#endif
									visited[ n ] = true;
							}
							else{
									n = G.target( e1 );
									i++;
									goto BREAK;
							}
					}
			}
			else{
				break;
			}
		}	
}


bool TOPSORTG(GRAPH<int,int> &G, node_array<int>& ord) 
{
	node_array<int> INDEG(G);
	queue<node> ZEROINDEG;	
		
	node v;
	forall_nodes(v,G) {
		INDEG[v]=G.indeg(v);
		if (INDEG[v]==0) ZEROINDEG.append(v); 
	}

	int count=0;
	while (!ZEROINDEG.empty()) {
		v=ZEROINDEG.pop();
		ord[v]=++count;
				
		edge e;
		forall_out_edges(e,v) {
			  node w=G.target(e);
			  if (--INDEG[w]==0){
				  if( G[ e ] > 0 )	
						ZEROINDEG.append(w);
			  }
		}
	}
	
  return (count==G.number_of_nodes());
}


/****************************************************************/
/**************** LAYER ASSIGNMENT FUNCTIONS ********************/


/**************** REDUCED DIGRAPH FUNCTIONS  ********************/

void dfsReduced(node v, node_array<int> &mark, b_stack<node> &visited  )
{
	visited.push(v);
	mark[v] |= 1;

	node w;
	forall_adj_nodes(w,v) {
		if (mark[w] & 2) mark[w] |= 4;

		if ((mark[w] & 1) == 0)
			dfsReduced(w,mark,visited);
	}
}

void do_call (const GRAPH<int,int>& G, list<edge>& transitives )
{
	node v, w;
	edge e;

	node_array<int> mark(G,0);
	b_stack<node> visited(G.number_of_nodes());

	forall_nodes(v,G) {
		forall_adj_nodes(w,v)
			mark[w] = 2;
		forall_adj_nodes(w,v)
			if ((mark[w] & 1) == 0) dfsReduced( w, mark, visited );

		forall_out_edges(e,v)
			if (mark[target(e)] & 4)
				transitives.append(e);

		while (!visited.empty())
			mark[visited.pop()] = 0;
	}

	mark.init();
	//delete visited;
}

/********* END OF REDUCED DIGRAPH FUNCTIONS ********************/

/*********     OTHER LAYERING ALG.s         ********************/
int promoteW(  GRAPH<int,int> &G, node_array<int>& rank, node v ){ 
	int Dummydiff = 0;
	edge e;
	forall_in_edges(e,v){
		if( rank[ G.source( e ) ] == rank[ v ] + 1 ){
			Dummydiff = Dummydiff + promoteW( G, rank, G.source( e ) );
		}
	}
	rank[v] = rank[v] +1;
	int gain = 0,loss = 0;
	forall_in_edges( e, v ){
		gain = G[ e ];
	}
	forall_out_edges( e, v ){
		loss = G[ e ];
	}
	Dummydiff = Dummydiff  - gain + loss;
	return Dummydiff;
}
int promote( GRAPH<int,int> &G, node_array<int>& rank, node v ){
	int Dummydiff = 0;
	edge e;
	forall_in_edges(e,v){
		if( rank[ G.source( e ) ] == rank[ v ] + 1 ){
			Dummydiff = Dummydiff + promote( G, rank, G.source( e ) );
		}
	}
	rank[v] = rank[v] +1;
	Dummydiff = Dummydiff  - G.indeg(v) + G.outdeg(v);
	return Dummydiff;
}
bool isEqual( GRAPH<int,int> &G, node_array<int> &rank, node_array<int> &realLayering ){
	bool isTrue = true;
	node n;
	forall_nodes( n, G ){
		if( rank[ n ] != realLayering[ n ] )
			isTrue = false;
	}
	return isTrue;
}
void LPwithPromotionW( GRAPH<int,int> &G, node_array<int>& rank ){
	node n;
	list_item it;
	edge e;
	list<node> TEMP;
	int currLayer = 1;

	/**********************************/
	/* Phase 1: Longest Path algorithm*/
	/**********************************/

	node_array<bool> marked( G, false );
	node_array<int> layerTemp(G, 0 ),realLayering(G, 0 ),layerFirst(G, 0 );
	list<edge> hided;
	forall_nodes(n, G){
		if( G.outdeg(n) == 0 && marked[ n ] == false ){
			rank[ n ] = currLayer;
			marked[ n ] = true;
			TEMP.append(n);
			forall_in_edges( e, n ){
				hided.append( e );
			}
			//G.print();
		}
	}
	forall_items( it, hided )
		G.hide_edge( hided[ it ] );
	hided.clear();
	currLayer++;
	while( TEMP.empty() != true ){
		TEMP.clear();
		forall_nodes(n, G){
			if( G.outdeg(n) == 0 && marked[ n ] == false ){
				TEMP.append(n);
				marked[ n ] = true;
				rank[ n ] = currLayer;
				forall_in_edges( e, n ){
					hided.append( e );
				}
				//G.print();
			}
		}
		forall_items( it, hided )
			G.hide_edge( hided[ it ] );
		hided.clear();
		currLayer++;
	}
	G.restore_all_edges();
	//callModifiedCoffman( G, rank, 3 );
	TEMP.clear();	

	
	/**********************************/
	/*		Phase 2: Promote Phase    */
	/**********************************/

	layerTemp = rank;
	realLayering = rank;
	layerFirst = rank;
	int i = 0;
	/* Promote until number of edges achieved */
	while( i != G.number_of_edges() ){
		forall_nodes( n, G ){
			if( G.indeg( n ) > 0 ){
				if( promoteW( G, layerTemp, n ) >= 0 ){
					layerTemp = rank;
				}
				else{
					rank = layerTemp;
				}
			}
		}
		if( isEqual( G, rank, realLayering ) == true ){
			break;
		}	
		else{
			realLayering = rank;
		}
		/*forall_nodes(n, G){
			cout << rank[ n ] << " - ";
		}
		cout << endl; */
		i++;
	}
	
	node_array<int> rank2( G, 0 );
    forall_nodes( n, G ){
        rank2[ n ] = rank[ n ];
    }
    int max = 0;
    forall_nodes( n, G ){
        if( max < rank2[ n ] )
            max = rank2[ n ];
    }
    
    list<node> obs;
    int realLayering_i = 0;
    while( obs.size() != G.number_of_nodes() ){
        int min = max;
        forall_nodes( n, G ){
            if( min >= rank2[ n ] )
                min = rank2[ n ];
        }
        forall_nodes( n, G ){
            if( min == rank[ n ] ){
                obs.append( n );
                rank[ n ] = realLayering_i;
                rank2[ n ] = max;
            }
        }
        realLayering_i++;
    }
	max = 0;
	forall_nodes(n, G){
		if( max < rank[ n ] )
            max = rank[ n ];
		//cout << rank[ n ] << " - ";
	}
	//cout << endl;
	forall_nodes(n,G)
		rank[n] = max - rank[n];
}
void LPwithPromotionMinWidthW( GRAPH<int,int> &G, node_array<int>& rank, int W ){
	node n;
	list_item it;
	edge e;
	list<node> TEMP;
	int currLayer = 1;

	/**********************************/
	/* Phase 1: Longest Path algorithm*/
	/**********************************/

	node_array<bool> marked( G, false );
	node_array<int> layerTemp(G, 0 ),realLayering(G, 0 ),layerFirst(G, 0 );
	list<edge> hided;
	forall_nodes(n, G){
		if( G.outdeg(n) == 0 && marked[ n ] == false ){
			rank[ n ] = currLayer;
			marked[ n ] = true;
				
			TEMP.append(n);
			forall_in_edges( e, n ){
				hided.append( e );
			}
			//G.print();
		}
	}
	forall_items( it, hided )
		G.hide_edge( hided[ it ] );
	hided.clear();
	currLayer++;
	while( TEMP.empty() != true ){
		TEMP.clear();
		forall_nodes(n, G){
			if( G.outdeg(n) == 0 && marked[ n ] == false ){
				TEMP.append(n);
				marked[ n ] = true;
				rank[ n ] = currLayer;
				
				forall_in_edges( e, n ){
					hided.append( e );
				}
				//G.print();
			}
		}
		forall_items( it, hided )
			G.hide_edge( hided[ it ] );
		hided.clear();
		currLayer++;
	}
	G.restore_all_edges();
	//callModifiedCoffman( G, rank, 3 );
	TEMP.clear();	

	
	/**********************************/
	/*		Phase 2: Promote Phase    */
	/**********************************/

	layerTemp = rank;
	realLayering = rank;
	layerFirst = rank;
	int i = 0;
	/* Promote until number of edges achieved */
	while( i != G.number_of_edges() ){
		bool passedTheSizeofW = false;
		forall_nodes( n, G ){
			if( G.indeg( n ) > 0 ){
				if( promoteW( G, layerTemp, n ) >= 0 ){
					layerTemp = rank;
				}
				else{
					passedTheSizeofW = false;
					int maxRank = 0;
					forall_nodes( n, G ){
						if( layerTemp[ n ] > maxRank )
							maxRank = layerTemp[ n ];
					}
					/* Init this array with empty integers */
					array<list<int> > rankWidth( maxRank + 1 );
					forall_nodes( n, G ){
						rankWidth[ layerTemp[ n ] ].append( 1 );
						if( rankWidth[ layerTemp[ n ] ].size() > W )
							passedTheSizeofW = true;
					}
					if( passedTheSizeofW == false ){
						rank = layerTemp;
					}
					else{
						layerTemp = rank;
					}
				}
			}
		}
		if( isEqual( G, rank, realLayering ) == true && passedTheSizeofW == false ){
			break;
		}	
		else{
			realLayering = rank;
		}
		/*forall_nodes(n, G){
			cout << rank[ n ] << " - ";
		}
		cout << endl;*/ 
		i++;
	}
	
	node_array<int> rank2( G, 0 );
    forall_nodes( n, G ){
        rank2[ n ] = rank[ n ];
    }
    int max = 0;
    forall_nodes( n, G ){
        if( max < rank2[ n ] )
            max = rank2[ n ];
    }
    
    list<node> obs;
    int realLayering_i = 0;
    while( obs.size() != G.number_of_nodes() ){
        int min = max;
        forall_nodes( n, G ){
            if( min >= rank2[ n ] )
                min = rank2[ n ];
        }
        forall_nodes( n, G ){
            if( min == rank[ n ] ){
                obs.append( n );
                rank[ n ] = realLayering_i;
                rank2[ n ] = max;
            }
        }
        realLayering_i++;
    }
	max = 0;
	forall_nodes(n, G){
		if( max < rank[ n ] )
            max = rank[ n ];
		/*cout << rank[ n ] << " - ";*/
	}
	//cout << endl;
	forall_nodes(n,G)
		rank[n] = max - rank[n];
}




void CGwithPromotionMinWidthW( GRAPH<int,int> &G, node_array<int>& rank, int W ){
	node n;
	list_item it;
	edge e;
	list<node> TEMP;
	int currLayer = 1;

	node_array<bool> marked( G, false );
	node_array<int> layerTemp(G, 0 ),realLayering(G, 0 ),layerFirst(G, 0 );
	callModifiedCoffman( G, rank, W );
	TEMP.clear();
	int max_i = 0;
	forall_nodes(n,G)
		if( max_i < rank[n] )
			max_i = rank[n];
		
	forall_nodes(n,G)
		rank[n] = max_i - rank[n];
	
	/**********************************/
	/*		Phase 2: Promote Phase    */
	/**********************************/

	layerTemp = rank;
	realLayering = rank;
	layerFirst = rank;
	int i = 0;
	/* Promote until number of edges achieved */
	while( i != G.number_of_edges() ){
		bool passedTheSizeofW = false;
		forall_nodes( n, G ){
			if( G.indeg( n ) > 0 ){
				if( promoteW( G, layerTemp, n ) >= 0 ){
					layerTemp = rank;
				}
				else{
					passedTheSizeofW = false;
					int maxRank = 0;
					forall_nodes( n, G ){
						if( layerTemp[ n ] > maxRank )
							maxRank = layerTemp[ n ];
					}
					/* Init this array with empty integers */
					array<list<int> > rankWidth( maxRank + 1 );
					forall_nodes( n, G ){
						rankWidth[ layerTemp[ n ] ].append( 1 );
						if( rankWidth[ layerTemp[ n ] ].size() > W )
							passedTheSizeofW = true;
					}
					if( passedTheSizeofW == false ){
						rank = layerTemp;
					}
					else{
						layerTemp = rank;
					}
				}
			}
		}
		if( isEqual( G, rank, realLayering ) == true && passedTheSizeofW == false ){
			break;
		}	
		else{
			realLayering = rank;
		}
		/*forall_nodes(n, G){
			cout << rank[ n ] << " - ";
		}
		cout << endl;*/ 
		i++;
	}
	
	node_array<int> rank2( G, 0 );
    forall_nodes( n, G ){
        rank2[ n ] = rank[ n ];
    }
    int max = 0;
    forall_nodes( n, G ){
        if( max < rank2[ n ] )
            max = rank2[ n ];
    }
    
    list<node> obs;
    int realLayering_i = 0;
    while( obs.size() != G.number_of_nodes() ){
        int min = max;
        forall_nodes( n, G ){
            if( min >= rank2[ n ] )
                min = rank2[ n ];
        }
        forall_nodes( n, G ){
            if( min == rank[ n ] ){
                obs.append( n );
                rank[ n ] = realLayering_i;
                rank2[ n ] = max;
            }
        }
        realLayering_i++;
    }
	max = 0;
	forall_nodes(n, G){
		if( max < rank[ n ] )
            max = rank[ n ];
		//cout << rank[ n ] << " - ";
	}
	//cout << endl;
	forall_nodes(n,G)
		rank[n] = max - rank[n];
}



void LPwithPromotion( GRAPH<int,int> &G, node_array<int>& rank ){
	node n;
	list_item it;
	edge e;
	list<node> TEMP;
	int currLayer = 1;

	/**********************************/
	/* Phase 1: Longest Path algorithm*/
	/**********************************/

	node_array<bool> marked( G, false );
	node_array<int> layerTemp(G, 0 ),realLayering(G, 0 ),layerFirst(G, 0 );
	list<edge> hided;
	forall_nodes(n, G){
		if( G.outdeg(n) == 0 && marked[ n ] == false ){
			rank[ n ] = currLayer;
			marked[ n ] = true;
				
			TEMP.append(n);
			forall_in_edges( e, n ){
				hided.append( e );
			}
			//G.print();
		}
	}
	forall_items( it, hided )
		G.hide_edge( hided[ it ] );
	hided.clear();
	currLayer++;
	while( TEMP.empty() != true ){
		TEMP.clear();
		forall_nodes(n, G){
			if( G.outdeg(n) == 0 && marked[ n ] == false ){
				TEMP.append(n);
				marked[ n ] = true;
				rank[ n ] = currLayer;
				
				forall_in_edges( e, n ){
					hided.append( e );
				}
				//G.print();
			}
		}
		forall_items( it, hided )
			G.hide_edge( hided[ it ] );
		hided.clear();
		currLayer++;
	}
	G.restore_all_edges();
	//callModifiedCoffman( G, rank, 3 );
	TEMP.clear();	

	
	/**********************************/
	/*		Phase 2: Promote Phase    */
	/**********************************/

	layerTemp = rank;
	realLayering = rank;
	layerFirst = rank;
	int i = 0;
	/* Promote until number of edges achieved */
	while( true ){
		forall_nodes( n, G ){
			if( G.indeg( n ) > 0 ){
				if( promote( G, layerTemp, n ) >= 0 ){
					layerTemp = rank;
				}
				else{
					rank = layerTemp;
				}
			}
		}
		if( isEqual( G, rank, realLayering ) == true ){
			break;
		}	
		else{
			realLayering = rank;
		}
		/*forall_nodes(n, G){
			cout << rank[ n ] << " - ";
		}
		cout << endl; */
		i++;
	}
	
	node_array<int> rank2( G, 0 );
    forall_nodes( n, G ){
        rank2[ n ] = rank[ n ];
    }
    int max = 0;
    forall_nodes( n, G ){
        if( max < rank2[ n ] )
            max = rank2[ n ];
    }
    
    list<node> obs;
    int realLayering_i = 0;
    while( obs.size() != G.number_of_nodes() ){
        int min = max;
        forall_nodes( n, G ){
            if( min >= rank2[ n ] )
                min = rank2[ n ];
        }
        forall_nodes( n, G ){
            if( min == rank[ n ] ){
                obs.append( n );
                rank[ n ] = realLayering_i;
                rank2[ n ] = max;
            }
        }
        realLayering_i++;
    }
	max = 0;
	forall_nodes(n, G){
		if( max < rank[ n ] )
            max = rank[ n ];
		//cout << rank[ n ] << " - ";
	}
	//cout << endl;
	forall_nodes(n,G)
		rank[n] = max - rank[n];
}
void longestPath( GRAPH<int,int> &G, node_array<int>& rank ){
	node n;
	list_item it;
	edge e;
	list<node> TEMP;
	int currLayer = 1;

	/**********************************/
	/* Phase 1: Longest Path algorithm*/
	/**********************************/

	node_array<bool> marked( G, false );
	node_array<int> layerTemp(G, 0 ),realLayering(G, 0 ),layerFirst(G, 0 );
	list<edge> hided;
	forall_nodes(n, G){
		if( G.outdeg(n) == 0 && marked[ n ] == false ){
			rank[ n ] = currLayer;
			marked[ n ] = true;
				
			TEMP.append(n);
			forall_in_edges( e, n ){
				hided.append( e );
			}
			//G.print();
		}
	}
	forall_items( it, hided )
		G.hide_edge( hided[ it ] );
	hided.clear();
	currLayer++;
	while( TEMP.empty() != true ){
		TEMP.clear();
		forall_nodes(n, G){
			if( G.outdeg(n) == 0 && marked[ n ] == false ){
				TEMP.append(n);
				marked[ n ] = true;
				rank[ n ] = currLayer;
				
				forall_in_edges( e, n ){
					hided.append( e );
				}
				//G.print();
			}
		}
		forall_items( it, hided )
			G.hide_edge( hided[ it ] );
		hided.clear();
		currLayer++;
	}
	G.restore_all_edges();
	//callModifiedCoffman( G, rank, 3 );
	TEMP.clear();	

	int max = 0;
	forall_nodes(n, G){
		if( max < rank[ n ] )
            max = rank[ n ];
		cout << rank[ n ] << " - ";
	}
	cout << endl;
	forall_nodes(n,G)
		rank[n] = max - rank[n];
}

/********* END OTHER LAYERING ALG.s         ********************/
/**************** COFFMAN GRAHAM FUNCTIONS  ********************/

bool allLabeled( GRAPH<int,int>& G , node_array<bool> labeled ){
		node n;
		forall_nodes( n, G ){
			if( labeled[ n ] == true ){
				;
			}
			else{
				return false;
			}
		}
		return true;
}

bool inNodeList( list<node> U, node n ){
	node x;
	forall( x, U ){
		if( x == n )
			return true;
	}
	return false;
}

void draw_network( GRAPH<int,int> &G , node_array<double> &x, node_array<double> &y ){
	GraphWin gw(G);
	list<node> A;
	//GraphWin gw(G);

	node n ;
	
	random_source X( 0,800 );
	random_source Y( 0,800 );
	int xx;
	int yy;

	forall_nodes( n, G ){
		X >> xx;
		Y >> yy;
		x[n] = xx;
		y[n] = yy;
	}
	gw.set_node_height(30,true);
	gw.set_position(x,y);
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 24);
	gw.set_animation_steps(3);
	
	gw.display();
	gw.zoom_graph();
	gw.edit();
}

void draw_new ( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE ){
	
	double initialX = 100 * KEYWSIZE , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	node_array<double> x(G),y(G) ;
	list<node> A;
	GraphWin gw(G);

	node n ;
	int cou = KEYWSIZE;
	
	while( cou >= 0  )
	{
		//cout << endl;
		i = initialX ;
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX  ;
			y[n] = initialY;

		}
		cou-=1;
		initialY -= incY;
	} 

	gw.set_node_height(30,true);
	gw.set_position(x,y);
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 24);
	gw.set_animation_steps(3);
	
	gw.display();
	gw.zoom_graph();
	gw.edit();
}

void draw_dummy ( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy ){
	
	double initialX = 100 * KEYWSIZE , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	node_array<double> x(G),y(G) ;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	
	GraphWin gw(G);

	node n ;
	int cou = KEYWSIZE;
	
	while( cou >= 0  )
	{
		cout << endl;
		i = initialX ;
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX  ;
			y[n] = initialY;

		}
		cou-=1;
		initialY -= incY;
	} 

	list_item it;
	forall_items( it, dummy ){
			n = dummy[ it ];
			gw.set_color( n, red );
	}

	gw.set_node_height(30,true);
	gw.set_position(x,y);
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 24);
	gw.set_animation_steps(3);
	
	gw.display();
	gw.zoom_graph();
	gw.edit();
}

void draw_dummy_centered ( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy, node_array<double> &x , node_array<double> &y ){
	
	double initialX = 0 , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	

	GraphWin gw(G);

	node n ;
	int cou = KEYWSIZE;

	i = initialX;
	while( cou >= 0  )
	{
		//cout << " - " << AB[ cou ].length()<< endl; 
		//cout << endl;
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX;
			y[n] = initialY;

		}
		cou-=1;
		if( cou >= 0 )
			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
		initialY -= incY;
		
	} 

	list_item it;
	forall_items( it, dummy ){
			n = dummy[ it ];
			gw.set_color( n, red );
	}

// 	gw.set_node_height(20,true);
// 	gw.set_node_shape( leda::ovalrect_node, true );
// 	gw.set_edge_label_type(leda::data_label , true);
// 	gw.set_edge_label_font(leda::roman_font, 24);
// 	gw.set_edge_shape( leda::circle_edge, true );
// 
// 	gw.set_animation_steps(5);
// 
// 	gw.set_position(x,y);
// 	gw.display();
// 	gw.zoom_graph();
// 	gw.edit();

}

void identify_dummy_positions( GRAPH<int,int> &G , array< list<node> > &AB, int KEYWSIZE, list<node> dummy, node_array<double> &x , node_array<double> &y ){
	double initialX = 0 , initialY = 0 , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	

	GraphWin gw(G);

	node n ;
	int cou = KEYWSIZE;

	i = initialX;
	while( cou >= 0  )
	{

#ifdef LENGTH
		cout << " - " << AB[ cou ].length()<< endl; 
		cout << endl;
#endif
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX;
			y[n] = initialY;

		}
		cou-=1;
		if( cou >= 0 )
			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
		initialY -= incY;
		
	}
}
void writeToFileTheReadGraphPositions( GRAPH<int,int> &G , array<list<node> > &AB, node_array<double> &x, node_array<double> &y, char *filename ){
	node n;
	list_item it;
	FILE *fptr;
	if( ( fptr = fopen( filename, "w" ) ) != NULL ){
		fprintf( fptr, "%d\n", G.number_of_nodes() );
		for( int i = AB.size() - 1; i>= 0; i-- ){
			list<node> L = AB[ i ];
			fprintf( fptr, "%d", AB[ i ].size() );
			forall_items( it, L ){
				fprintf( fptr, "%lf %lf\t", x[ L[ it ] ], y[ L[ it ] ] );
			}
			fprintf( fptr, "\n" );
		}
		fclose( fptr );
	}else{
	      cout << "\n File positions.txt could not be opened\n";
	}
}


void identify_dummy_positionsY( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> &dummy, node_array<double> &x , node_array<double> &y, int &algorithmFlag , int space, int increment, bool ledaPostFlag ){
	bool ces = false;
#ifdef DEBUG_ROBINVIZ
	cout << "\nXcoordinate Begins  \n";
#endif
	double initialX = 0 , initialY = space * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	

	GraphWin gw(G);

	node n ;
	int cou = KEYWSIZE;

	i = initialX;
	while( cou >= 0  )
	{
		//cout << " - " << AB[ cou ].length()<< endl; 
		//cout << endl;
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX;
			y[n] = initialY;

		}
		cou-=1;
		if( cou >= 0 )
			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
		initialY -= incY;
		
	} 

	list_item it;
	forall_items( it, dummy ){
			n = dummy[ it ];
			gw.set_color( n, red );
	}
	
	cou = 0;
	forall_nodes( n, G ){
		G[ n ] = cou;
		cou++;
	}
	gw.set_node_height(20,true);
	gw.set_node_shape( leda::ovalrect_node, true );
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 24);
	gw.set_edge_shape( leda::circle_edge, true );
// for( int loop = 0; loop <= 5; loop++ ){
// 
// 	if( ledaPostFlag == true )
// 		pos_process( G, AB, x, space, space, 20 );
	gw.set_position(x,y);
#ifdef DEBUG_ROBINVIZ
	cout << "\nPython Call Begins \n";
#endif
	char command[256];
#ifdef LINUX
	sprintf( command, "src/python/xcoord/gmls/pyt_inp.gml" );
#else
	sprintf( command, "src//python//xcoord//gmls//pyt_inp.gml" );
#endif
	gw.save_gml( command );
// 	cout << increment << " - BEFORE CALL\n";
	if( algorithmFlag == 1 ){
#ifdef LINUX
// 		  cout << " Comm 1 \n";
		  sprintf( command, "python src/python/xcoord/fastandsimple.py src/python/xcoord/gmls/pyt_inp.gml src/python/xcoord/gmls/pyt_out.gml %d %s ", increment, "combined node positions" );
#else
// 		  cout << " Comm 2 \n";
		  sprintf( command, "src\\python\\xcoord\\fastandsimple.py src\\python\\xcoord\\gmls\\pyt_inp.gml src\\python\\xcoord\\gmls\\pyt_out.gml %d %s ", increment, "combined node positions" );
#endif
		  system( command );
	}
// 	cout << " - END CALL\n";
	if( algorithmFlag == 3 ){
#ifdef LINUX
		  sprintf( command, "python src/python/xcoord/fastklevel.py src/python/xcoord/gmls/pyt_inp.gml src/python/xcoord/gmls/pyt_out.gml %d positions", increment );
#else
		  sprintf( command, "src\\python\\xcoord\\fastklevel.py src\\python\\xcoord\\gmls\\pyt_inp.gml src\\python\\xcoord\\gmls\\pyt_out.gml %d positions", increment );
#endif
		  system( command );
	}

	node_array<point> POS( G );
// 	cout << "\nPython Call Done \n";
	x.init(G);
	y.init(G);
	FILE *fptr;
// 	cout << "\nPython Call Done \n";

// 	for( i = AB.size()-1; i >= 0; i-- ){
// 		cout <<endl<< " L "<< i << "with size " << AB[i].size() << endl;
// 	}
// 	cout << "\n" << AB.size() <<"\nPython Call Done \n";
	if( ( fptr = fopen( "positions.txt", "r" ) ) != NULL ){
		int numberOfNodes,layerNodes;
		fscanf( fptr, "%d", &numberOfNodes );
		int count = 0;
		for( i = AB.size()-1; i >= 0; i-- ){
		      fscanf( fptr, "%d", &layerNodes );
// 		      cout << layerNodes << "\t";
		      forall_items( it, AB[ i ] ){
			     fscanf( fptr, "%lf", &x[ AB[ i ][ it ] ] );
			     fscanf( fptr, "%lf", &y[ AB[ i ][ it ] ] );	
// 			     cout << x[ AB[ i ][ it ] ] << " - " << y[ AB[ i ][ it ] ] << " | ";
		      }
// 		      cout << endl;
		      count++;
		}
		fclose( fptr );
	}else{
	      cout << "\n File positions.txt could not be opened\n";
	}
	ces = false;
	if( ces == false ){
 		determineYPositions( G , AB, y, increment );
	}
	else{
		i = initialX;
		int cou = KEYWSIZE;
		while( cou >= 0  )
		{
			A = AB[cou];	
			forall(n,A){
				y[n] = initialY;
			}
			cou-=1;
			initialY -= space;
		} 
	}
        writeToFileTheReadGraphPositions( G, AB, x, y, "positionsAfter.txt" );
	if( ledaPostFlag == true )
		pos_process( G, AB, x, space, space, 20, dummy );
	writeToFileTheReadGraphPositions( G, AB, x, y, "positionsAfterPostProcess.txt" );
	gw.set_position(x,y);
// }
//  #ifdef DRAW_FINAL
// 	gw.set_animation_steps(5);
/*	gw.display();
	gw.zoom_graph();
	gw.edit();*/	
//  #endif
#ifdef DEBUG_ROBINVIZ
	cout << "\nXcoordinate Done \n";
#endif
}

void identify_dummy_positionsX( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> &dummy, node_array<double> &x , node_array<double> &y, int &algorithmFlag, int space, int increment, bool ledaPostFlag ){
#ifdef DEBUG_ROBINVIZ
	cout << "\nXcoordinate Begins  \n";
#endif
	double initialX = 0 , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	

	GraphWin gw(G);

	node n ;
	int cou = KEYWSIZE;

	i = initialX;
	while( cou >= 0  )
	{
		//cout << " - " << AB[ cou ].length()<< endl; 
		//cout << endl;
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX;
			y[n] = initialY;

		}
		cou-=1;
		if( cou >= 0 )
			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
		initialY -= incY;
		
	} 

	list_item it;
	forall_items( it, dummy ){
			n = dummy[ it ];
			gw.set_color( n, red );
	}

	gw.set_node_height(20,true);
	gw.set_node_shape( leda::ovalrect_node, true );
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 24);
	gw.set_edge_shape( leda::circle_edge, true );
	gw.set_position(x,y);
#ifdef DEBUG_ROBINVIZ
	cout << "\nPython Call Begins \n";
#endif
	char command[256];
#ifdef LINUX
	sprintf( command, "src/python/xcoord/gmls/pyt_inp.gml" );
#else
	sprintf( command, "src//python//xcoord//gmls//pyt_inp.gml" );
#endif
	gw.save_gml( command );

	if( algorithmFlag == 1 ){
#ifdef LINUX
// 		  cout << " Comm 1 \n";
		  sprintf( command, "python src/python/xcoord/fastandsimple.py src/python/xcoord/gmls/pyt_inp.gml src/python/xcoord/gmls/pyt_out.gml %d combined node layers", space );
#else
// 		  cout << " Comm 2 \n";
		  sprintf( command, "src\\python\\xcoord\\fastandsimple.py src\\python\\xcoord\\gmls\\pyt_inp.gml src\\python\\xcoord\\gmls\\pyt_out.gml %d combined node layers", space );
#endif
		  system( command );
	}

	if( algorithmFlag == 3 ){
#ifdef LINUX
		  sprintf( command, "python src/python/xcoord/fastklevel.py src/python/xcoord/gmls/pyt_inp.gml src/python/xcoord/gmls/pyt_out.gml %d layers", space );
#else
		  sprintf( command, "src\\python\\xcoord\\fastklevel.py src\\python\\xcoord\\gmls\\pyt_inp.gml src\\python\\xcoord\\gmls\\pyt_out.gml %d layers", space );
#endif
		  system( command );
	}
#ifdef LINUX
	sprintf( command, "src/python/xcoord/gmls/pyt_out.gml" );
#else
	sprintf( command, "src//python//xcoord//gmls//pyt_out.gml" );
#endif

	node_array<point> POS( G );
	gw.read_gml( command );
// 	cout << "\nPython Call Done \n";
	gw.get_position( POS );
// 	gw.set_animation_steps(5);
// 	gw.display();
// 	gw.zoom_graph();
// 	gw.edit();
	x.init(G);
	y.init(G);
	forall_nodes( n, G ){
		x[ n ] = POS[ n ].xcoord();
		y[ n ] = POS[ n ].ycoord();	
	}
	FILE *fptr;
// 	cout << "\nPython Call Done \n";

	dummy.clear();
// 	cout << "\n" << AB.size() <<"\nPython Call Done \n";
	if( ( fptr = fopen( "layers.txt", "r" ) ) != NULL ){
		int numberOfNodes;
		int value;
		fscanf( fptr, "%d", &numberOfNodes );
		int count = 0;
		forall_nodes( n, G ){
		      fscanf( fptr, "%d", &value );
 		      //cout << value << " - ";
		      AB[ value ].push_back( n );
		      if( gw.get_color( n ) == red )
				dummy.push_back(n);
		      count++;
		}
		fclose( fptr );
	}else{
	      cout << "\n File layers.txt could not be opened\n";
	} 
	//if( ledaPostFlag == true )
	//	pos_process( G, AB, x, space, space, 20, dummy );
#ifdef DEBUG_ROBINVIZ
	cout << "\nXcoordinate Done \n";
#endif
	sprintf( command, "" );
}

node_array<point> draw_Graphs( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy , edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends, char filename[ 1024 ], int graphNo ){
	
	double initialX = 0 , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	node_array<point> pos(G);
	node_array<double> x(G);
	node_array<double> y(G);

	list_item it;
	node n ;
	edge e;

	GraphWin gw(G);	

	int cou = KEYWSIZE;

	i = initialX;
	while( cou >= 0  )
	{

#ifdef LENGTH
		cout << " - " << AB[ cou ].length()<< endl; 
		cout << endl;
#endif
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX;
			y[n] = initialY;

		}
		cou-=1;
		if( cou >= 0 )
			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
		initialY -= incY;
		
	} 

	random_source S(1,8);
	int random_value;

	forall_nodes( n, G ){
			S >> random_value;
			color random( random_value );
#ifdef DRAW_FINAL
			gw.set_color( n, random);
#endif
	}
	forall_items( it, dummy ){
			n = dummy[ it ];
#ifdef DRAW_FINAL
			gw.set_color( n, leda::red );
#endif
	}

	forall_nodes( n, G ){
			point p( x[ n ], y[ n ] );
			pos[ n ] = p;
	}
	
	gw.set_edge_thickness( 3, true );
	//gw.set_edge_style( leda::gw_edge_style::dotted_edge, true );
	gw.set_edge_shape( leda::bezier_edge, true );
	gw.set_node_height(50,true);
	gw.set_node_border_color( red, true );
	gw.set_node_border_thickness( 3, true );
	gw.set_node_width( 100, true );
	gw.set_node_shape( leda::ovalrect_node, true );
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
	gw.set_edge_color( leda::blue , true );
	
	gw.set_layout( pos, bends );
	gw.set_animation_steps(5);

	//gw.display();
	//gw.zoom_graph();
	//gw.edit();
	//gw.save_ps( filename );
	return pos;

}
node_array<point> draw_final( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy , edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends, int graphNo ){

	double initialX = 0 , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	node_array<double> x(G);
	node_array<double> y(G);
	node_array<point> pos(G);
	list_item it;
	node n ;
	edge e;
#ifdef DRAW_FINAL	
	GraphWin gw(G);	
#endif
	int cou = KEYWSIZE;

	i = initialX;
	while( cou >= 0  )
	{

#ifdef LENGTH
		cout << " - " << AB[ cou ].length()<< endl; 
		cout << endl;
#endif
		A = AB[cou];
		
		forall(n,A){

			x[n] = i ;   
			i += incX;
			y[n] = initialY;

		}
		cou-=1;
		if( cou >= 0 )
			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
		initialY -= incY;
		
	} 

	random_source S(1,8);
	int random_value;

	forall_nodes( n, G ){
			S >> random_value;
			color random( random_value );
#ifdef DRAW_FINAL
			gw.set_color( n, random);
#endif
	}
	forall_items( it, dummy ){
			n = dummy[ it ];
#ifdef DRAW_FINAL
			gw.set_color( n, red );
#endif
	}

	forall_nodes( n, G ){
			point p( x[ n ] , y[ n ] );
			pos[ n ] = p;
	}
	
	/*	int max = 0;
	forall_edges( e, G ){
			if( G[ e ] > max )
				max = G[ e ];
	}
	forall_edges( e, G ){
		if( G[ e ] == max ){
				
				
		}
	}*/
	char filename[64];
#ifdef LINUX
	sprintf( filename, "sources/graph_sources/graph_gml%d.gml", graphNo );
#else
	sprintf( filename, "sources//graph_sources//graph_gml%d.gml", graphNo );
#endif
#ifdef DRAW_FINAL	
	gw.set_edge_thickness( 3, true );
	gw.set_node_height(50,true);
	gw.set_node_border_color( red, true );
	gw.set_node_border_thickness( 3, true );
	gw.set_node_width( 100, true );
	gw.set_node_shape( leda::ovalrect_node, true );
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
	gw.set_edge_color( leda::blue , true );
	gw.set_layout( pos, bends );
	gw.set_animation_steps(5);

	gw.save_gml( filename );
	gw.display();

	gw.zoom_graph();
	gw.edit();
#endif

	if( graphNo > 0 ){
		GraphWin gw(G);	
		gw.set_edge_thickness( 3, true );
		gw.set_node_height(50,true);
		gw.set_node_border_color( red, true );
		gw.set_node_border_thickness( 3, true );
		gw.set_node_width( 100, true );
		gw.set_node_shape( leda::ovalrect_node, true );
		gw.set_edge_label_type(leda::data_label , true);
		gw.set_edge_label_font(leda::roman_font, 42);
		gw.set_edge_shape( leda::poly_edge, true );
		gw.set_edge_color( leda::blue , true );
		gw.set_layout( pos, bends );
        
		gw.save_gml( filename );
#ifdef DRAW_FINAL
	gw.set_animation_steps(5);
	gw.display();

	gw.zoom_graph();
	gw.edit();
#endif
		char filen[ 1024 ];
#ifdef LINUX
		sprintf( filen , "sources/graph_sources/Layout%d.txt" , graphNo );
#else
		sprintf( filen , "sources//graph_sources//Layout%d.txt" , graphNo );
#endif
		
		bool flag = gw.save_layout( filen );
		if( flag == false )
			cout << " Unavailable to write\n";
	}
	return pos;
}

template <class T>
    void ledaToGraphviz( GRAPH<T,T> &G, array<list<node> > &AB, node_array<bool> isDummy, int KEYWSIZE, char *filename, node_array<leda::color> &Colors, bool colorsFlag , node_array<leda::string> &nodeNames, bool nodeNamesFlag ){
        node n;
        list_item it;
        edge e;
        list<node> n_temp;
        char filename2[ 64 ];
       
        sprintf( filename2, "%s.txt", filename );
        FILE *fptr;
        if( ( fptr = fopen( filename2, "w" ) ) == NULL ){
            cout << "\n File open error, could not find specified files \n";
        }
        else{
            fprintf( fptr, "/*%s%s%s*/", filename, " is produced by ", " LEDA to Graphviz convertor" );
            fprintf( fptr, "%s%s%s\n", "digraph ", "filename", "{" );
            for( int i = 0; i < KEYWSIZE; i++ ){
                fprintf( fptr, "%s" ,"{rank=same; " );
                n_temp = AB[ i ];
//                 n_temp.print();
                forall_items( it, n_temp ){
//                     cout << G[ n_temp[ it ] ];
                    if( G[ n_temp[ it ] ] != 0 && isDummy[ n_temp[ it ] ] != true  )
                        fprintf( fptr, " %d ", G[ n_temp[ it ] ] );
                }
                fprintf( fptr, "%s\n" ,";}" );
            }

            forall_nodes( n, G ){
		if( nodeNamesFlag == false ){
		    if( colorsFlag == false )
			fprintf( fptr, "%d [label = %d ]\n", G[ n ], G[ n ] );
		    else
			fprintf( fptr, "%d [label = %d, color = %s ]\n", G[ n ], G[ n ], Colors[ n ]  );
		}
		else{
		    if( colorsFlag == false )
			fprintf( fptr, "%d [label = %d ]\n", G[ n ], nodeNames[ n ] );
		    else
			fprintf( fptr, "%d [label = %d, color = %s ]\n", G[ n ], nodeNames[ n ], Colors[ n ]  );
		}
            }
            forall_edges( e, G ){
                fprintf( fptr, " %d -> %d [label = %d ]\n", G[ G.source( e ) ], G[ G.target( e ) ], G[ e ] );
            }
            fprintf( fptr, "%s\n", "edge [style=invis]" );
            for( int i = 0; i < KEYWSIZE; i++ ){
                n_temp = AB[ i ];
                int count = 0;
                forall_items( it, n_temp ){
                    if( G[ n_temp[ it ] ] > 0 )
                        count++;
                }
                if( count > 1 ){
                    forall_items( it, n_temp ){
                        if( G[ n_temp[ it ] ] != 0 ){
                            if( it != n_temp.last_item() )
                                fprintf( fptr, "%d -> ", G[ n_temp[ it ] ] );
                            else
                                fprintf( fptr, "%d", G[ n_temp[ it ] ] );
                        }
                    }
                    fprintf( fptr, "\n" );
                }
            }
            fprintf( fptr, "%s", "}" );
            fclose( fptr );
        }
    }


void draw_finalX( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy , edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends, int graphNo, node_array<point> &pos ){

	double initialX = 0 , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	node_array<double> x(G);
	node_array<double> y(G);

	list_item it;
	node n ;
	edge e;
#ifdef DRAW_FINAL	
	GraphWin gw(G);	
#endif
	int cou = KEYWSIZE;

	i = initialX;
// 	while( cou >= 0  )
// 	{
// 
// #ifdef LENGTH
// 		cout << " - " << AB[ cou ].length()<< endl; 
// 		cout << endl;
// #endif
// 		A = AB[cou];
// 		
// 		forall(n,A){
// 
// 			x[n] = i ;   
// 			i += incX;
// 			y[n] = initialY;
// 
// 		}
// 		cou-=1;
// 		if( cou >= 0 )
// 			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
// 		initialY -= incY;
// 		
// 	} 

	random_source S(1,8);
	int random_value;

	forall_nodes( n, G ){
			S >> random_value;
			color random( random_value );
#ifdef DRAW_FINAL
			gw.set_color( n, random);
#endif
	}
	forall_items( it, dummy ){
			n = dummy[ it ];
#ifdef DRAW_FINAL
			gw.set_color( n, red );
#endif
	}

	forall_nodes( n, G ){
			y[ n ] = pos[ n ].ycoord();
			x[ n ] = pos[ n ].xcoord();
	}
	
	/*	int max = 0;
	forall_edges( e, G ){
			if( G[ e ] > max )
				max = G[ e ];
	}
	forall_edges( e, G ){
		if( G[ e ] == max ){
				
				
		}
	}*/
	char filename[64];
#ifdef LINUX
	sprintf( filename, "sources/graph_sources/graph_gml%d.gml", graphNo );
#else
	sprintf( filename, "sources//graph_sources//graph_gml%d.gml", graphNo );
#endif
#ifdef DRAW_FINAL	
	gw.set_edge_thickness( 3, true );
	gw.set_node_height(50,true);
	gw.set_node_border_color( red, true );
	gw.set_node_border_thickness( 3, true );
	gw.set_node_width( 100, true );
	gw.set_node_shape( leda::ovalrect_node, true );
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
	gw.set_edge_color( leda::blue , true );
	gw.set_layout( pos, bends );
	gw.set_animation_steps(5);

// 	gw.save_gml( filename );
	gw.display();

	gw.zoom_graph();
	gw.edit();
#endif

	if( graphNo > 0 ){
		GraphWin gw(G);	
		gw.set_edge_thickness( 3, true );
		gw.set_node_height(50,true);
		gw.set_node_border_color( red, true );
		gw.set_node_border_thickness( 3, true );
		gw.set_node_width( 100, true );
		gw.set_node_shape( leda::ovalrect_node, true );
		gw.set_edge_label_type(leda::data_label , true);
		gw.set_edge_label_font(leda::roman_font, 42);
		gw.set_edge_shape( leda::poly_edge, true );
		gw.set_edge_color( leda::blue , true );
		gw.set_layout( pos, bends );
        
// 		gw.save_gml( filename );
#ifdef DRAW_FINAL
	gw.set_animation_steps(5);
	gw.display();
	gw.zoom_graph();
	gw.edit();
#endif
// 		char filen[ 1024 ];
// #ifdef LINUX
// 		sprintf( filen , "sources/graph_sources/Layout%d.txt" , graphNo );
// #else
// 		sprintf( filen , "sources//graph_sources//Layout%d.txt" , graphNo );
// #endif
// 		
// 		bool flag = gw.save_layout( filen );
// 		if( flag == false )
// 			cout << " Unavailable to write\n";
	}
}
void draw_finalY( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy , edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends, int graphNo, node_array<point> &pos, node_array<double> &Hvalues  ){

	double initialX = 0 , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color red( 7 );
	node_array<double> x(G);
	node_array<double> y(G);

	list_item it;
	node n ;
	edge e;
#ifdef DRAW_FINAL	
	GraphWin gw(G);	
#endif
	int cou = KEYWSIZE;

	i = initialX;
// 	while( cou >= 0  )
// 	{
// 
// #ifdef LENGTH
// 		cout << " - " << AB[ cou ].length()<< endl; 
// 		cout << endl;
// #endif
// 		A = AB[cou];
// 		
// 		forall(n,A){
// 
// 			x[n] = i ;   
// 			i += incX;
// 			y[n] = initialY;
// 
// 		}
// 		cou-=1;
// 		if( cou >= 0 )
// 			i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
// 		initialY -= incY;
// 		
// 	} 

	random_source S(1,8);
	int random_value;

	forall_nodes( n, G ){
			S >> random_value;
			color random( random_value );
#ifdef DRAW_FINAL
			gw.set_color( n, random);
#endif
	}
	forall_items( it, dummy ){
			n = dummy[ it ];
#ifdef DRAW_FINAL
			gw.set_color( n, red );
#endif
	}

	forall_nodes( n, G ){
			y[ n ] = pos[ n ].ycoord();
			x[ n ] = pos[ n ].xcoord();
	}
	
	/*	int max = 0;
	forall_edges( e, G ){
			if( G[ e ] > max )
				max = G[ e ];
	}
	forall_edges( e, G ){
		if( G[ e ] == max ){
				
				
		}
	}*/
	char filename[64];
#ifdef LINUX
	sprintf( filename, "sources/graph_sources/graph_gml%d.gml", graphNo );
#else	
	sprintf( filename, "sources//graph_sources//graph_gml%d.gml", graphNo );
#endif
#ifdef DRAW_FINAL	
	gw.set_edge_thickness( 3, true );
	gw.set_node_height(50,true);
	gw.set_node_border_color( red, true );
	gw.set_node_border_thickness( 3, true );
	gw.set_node_width( 100, true );
	gw.set_node_shape( leda::ovalrect_node, true );
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
	gw.set_edge_color( leda::blue , true );
	gw.set_layout( pos, bends );
	gw.set_animation_steps(5);
	double Hmax = 0;
	forall_nodes( n, G ){
		if( Hmax <  Hvalues[n] )
			Hmax = Hvalues[n];
	}
	forall_nodes( n, G ){
		if(  Hmax / Hvalues[n] > 1 ){
			gw.set_width( n, 40 + Hmax / Hvalues[n] * 10 );
		}
		else{
			gw.set_width( n, 40 );	
		}
	}

	list<int> edgeWeights;
	forall_edges( e, G ){
		gw.set_thickness( e, G[ e ] * 1 / 15  );
	}
	gw.save_gml( filename );
	gw.display();

	gw.zoom_graph();
	gw.edit();
#endif

	if( graphNo > 0 ){
		GraphWin gw(G);	
		gw.set_edge_thickness( 3, true );
		gw.set_node_height(50,true);
		gw.set_node_border_color( red, true );
		gw.set_node_border_thickness( 3, true );
		gw.set_node_width( 100, true );
		gw.set_node_shape( leda::ovalrect_node, true );
		gw.set_edge_label_type(leda::data_label , true);
		gw.set_edge_label_font(leda::roman_font, 42);
		gw.set_edge_shape( leda::poly_edge, true );
		gw.set_edge_color( leda::blue , true );
		gw.set_layout( pos, bends );
        
		gw.save_gml( filename );
#ifdef DRAW_FINAL
	gw.set_animation_steps(5);
	gw.display();
	gw.zoom_graph();
	gw.edit();
#endif
		char filen[ 1024 ];
#ifdef LINUX
	sprintf( filename, "sources/graph_sources/Layout%d.gml", graphNo );
#else	
	sprintf( filename, "sources//graph_sources//Layout%d.gml", graphNo );
#endif
		
		bool flag = gw.save_layout( filen );
		if( flag == false )
			cout << " Unavailable to write\n";
	}
}

// This function is for drawing the main layout for the proposed in ISB 2010 Paper as it is shown in Figure 1 and Figure 3
node_array<point> draw_final2( GRAPH<int,int> &G, node_array<int> &PARS, array<list<node> > &AB, int KEYWSIZE, list<node> dummy, edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends, int graphNo, node_array<double> &Hvalues, node_array<double> &x , node_array<double> &y, int decision, double nodeSize, double edgeBendImp, double colorScale, double edgThicknessTher, list<int> &categ ){
// 	cout << " 1 " << endl;
	double initialX = 0 , initialY = 100 * KEYWSIZE , incX = 150 , incY = 100 ,i;
	list<node> A;
	color blue( 4 );
	color orange( 7 );
	node_array<point> pos(G);

	array<bool> colorChoose( 13 );
	for( int c = 0; c < 13; c++ ){
		colorChoose[ c ] = false;
	}
	colorChoose[ 7 ]  = true;
	//black, white, red, green, blue, yellow, violet, orange, cyan, 
	//brown, pink, green2, blue2, grey1, grey2, grey3. 
// 	panel P2( 200,150, "Determine Node Colors" );
// 	for( ; ; ){
// 		P2.text_item( "\\bf\\blue Choose Your Color \\" );
// 		P2.bool_item( "Black  ", colorChoose[ 0 ] );
// 		P2.bool_item( "White  ", colorChoose[ 1 ] );
// 		P2.bool_item( "Red    ", colorChoose[ 2 ] );
// 		P2.bool_item( "Green  ", colorChoose[ 3 ] );
// 		P2.bool_item( "Blue   ", colorChoose[ 4 ] );
// 		P2.bool_item( "Yellow ", colorChoose[ 5 ] );
// 		P2.bool_item( "Violet ", colorChoose[ 6 ] );
// 		P2.bool_item( "Orange ", colorChoose[ 7 ] );
// 		P2.bool_item( "Cyan   ", colorChoose[ 8 ] );
// 		P2.bool_item( "Brown  ", colorChoose[ 9 ] );
// 		P2.bool_item( "Pink   ", colorChoose[ 10 ] );
// 		P2.bool_item( "Green2 ", colorChoose[ 11 ] );
// 		P2.bool_item( "Blue2  ", colorChoose[ 12 ] );
// 		P2.open();
// 		if( P2.button_press_time() > 0 ) break;
// 		P2.display();
// 		P2.clear();
// 	}
// 	P2.close();
// 	P2.clear();

	list_item it;
	node n ;
	edge e;
#ifdef DRAW_FINAL2	
	GraphWin gw(G);	
#endif
	int cou = KEYWSIZE;

	random_source S(1,8);
	int random_value;
	for( int c = 0; c < 13; c++ ){
		if(colorChoose[ c ] == true ){
		      random_value = c;
		      break;
		}
	}

	// Make all of the node as orange
	forall_nodes( n, G ){
			G[ n ] = PARS[ n ];
			color random( random_value );
#ifdef DRAW_FINAL2
			gw.set_color( n, random);
#endif
	}
	forall_items( it, dummy ){
			n = dummy[ it ];
			color random( random_value );
#ifdef DRAW_FINAL2
			gw.set_color( n, random);
#endif
	}

	forall_nodes( n, G ){
			point p( x[ n ], y[ n ] );
			pos[ n ] = p;
	}

	double Hmax = 0;
	forall_nodes( n, G ){
		if( Hmax <  Hvalues[n] )
			Hmax = Hvalues[n];
	}
	// Arrange node sizes according to H-values for each nodes
	forall_nodes( n, G ){
		if(  Hmax / Hvalues[n] > 1 ){
			gw.set_width( n, nodeSize + Hmax / Hvalues[n] * 7 );
		}
		else{
			gw.set_width( n, nodeSize );	
		}
	}
	array<color> color_l( 20 );
	int count = colorScale;
	for( int j = 0; j < color_l.size(); j++ ){
		color el( count, count, count );
		color_l[ j ] = el;
		count -= 7;
	}
	list<int> weights;
	forall_edges( e, G ){

	}
	list<two_tuple<color,int> > variations;
	list<int> edgeWeights;
// 	cout << " 2 " << endl;
	// Store Edge weights
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
	count = 0;
	// Found the variation of edge weights in the graph
	for( int x = edgeWeights[edgeWeights.first()]; x <= edgeWeights[edgeWeights.last()]; x++ ){	
		for( int j = 0; j < color_l.size(); j++ ){
			two_tuple<color,int> Tp( color_l[ j ], x );
// 			cout << Tp << endl;
			variations.push_back( Tp );
		}
		count++;
	}
// 	int increase = (int) ((double)variations.size() / (double)edgeWeights.size() );
	int increase = (int) ((double)variations.size() / (double)color_l.size() );

// 	cout << " increase : " << increase << endl;
	// Form first coloring scheme
	forall_edges( e, G ){              
		count = 0;
		forall_items( it, edgeWeights ){
		      if( edgeWeights[ it ] == G[ e ] ){
			     break;
		      }
		      count++;
		}
		two_tuple<color,int> Tp = variations[ variations.get_item( increase / edgThicknessTher * count ) ] ;
		gw.set_thickness( e, Tp.second() / edgThicknessTher );
		gw.set_color( e, Tp.first() );
// 		cout << Tp << endl;
// 		cout << " increase : " << increase << endl;
	}

// 	cout << " 3 " << endl;
	node_array<int> layerInts( G, 0 );
        count = 0;
	// Found layers and store into this node array
	for( int j = AB.size() - 1; j >= 0 ; j-- ){
		forall_items( it, AB[ j ] ){
			layerInts[ AB[ j ][ it ] ] = count;
		}
		count++;
        }

	list<edge> handled_l;
	// Give no arrow to ordinary edges
	forall_edges( e, G ){
// 		cout << color_l[ G_e ] << endl;
		if( layerInts[ G.source( e ) ] - layerInts[ G.target( e ) ] > 0  ){
			;
		}
		else{
			gw.set_direction( e, leda::undirected_edge );
		}
	}
	edge e2;
	list<edge> processed;
	// Find one layer length twin nodes, make the upward ones middirected edge
	forall_edges( e, G ){
		forall_edges( e2, G ){
			if( (layerInts[ G.source( e ) ] - layerInts[ G.target( e ) ] == 1 && G.source( e ) == G.target( e2 ) ) &&
			    (layerInts[ G.target( e2 ) ] - layerInts[ G.source( e2 ) ] == 1 && G.source( e2 ) == G.target( e ) )&&
			    e != e2
			    )
			{
					point p_x( x[ G.source( e )], y[ G.source( e )] );
					point p_y( x[ G.target( e )], y[ G.target( e )] );
					if( x[ G.source( e )] != x[ G.target( e )] ){
						point p_z( (p_x.xcoord() + p_y.xcoord())/2.0 - edgeBendImp - 50.0, (p_x.ycoord() + p_y.ycoord())/2.0 - edgeBendImp + 50.0 );
						bends[ e ].append( p_z );
						processed.append( e );
						handled_l.append( e );
						handled_l.append( e2 );
						gw.set_direction( e, leda::mid_directed_edge );
					}
					else{
						point p_z( (p_x.xcoord() + p_y.xcoord())/2.0 - edgeBendImp - 15.0, (p_x.ycoord() + p_y.ycoord())/2.0 - edgeBendImp - 15.0 );
						bends[ e ].append( p_z );
						processed.append( e );
						handled_l.append( e );
						handled_l.append( e2 );
						gw.set_direction( e, leda::mid_directed_edge );
					}
// 				cout << "\n***************** ------------------ ***********************\n";
			}
// 			else{
// 				if( (layerInts[ G.target( e ) ] - layerInts[ G.source( e ) ] == 1 && G.target( e ) == G.source( e2 ) ) &&
// 				    (layerInts[ G.source( e2 ) ] - layerInts[ G.target( e2 ) ] == 1 && G.target( e2 ) == G.source( e ) )&&
// 				    e != e2
// 				    )
// 				{
// 					point p_x( x[ G.source( e )], y[ G.source( e )] );
// 					point p_y( x[ G.target( e )], y[ G.target( e )] );
// 					if( x[ G.source( e )] != x[ G.target( e )] ){
// 						point p_z( (p_x.xcoord() + p_y.xcoord())/2.0 - edgeBendImp - 25.0, (p_x.ycoord() + p_y.ycoord())/2.0 - edgeBendImp - 25.0 );
// 						bends[ e ].append( p_z );
// 						processed.append( e );
// 						handled_l.append( e );
// 						handled_l.append( e2 );
// 						gw.set_direction( e, leda::mid_directed_edge );
// 					}
// 					else{
// 						point p_z( (p_x.xcoord() + p_y.xcoord())/2.0 - edgeBendImp - 15.0, (p_x.ycoord() + p_y.ycoord())/2.0 - edgeBendImp - 15.0 );
// 						bends[ e ].append( p_z );
// 						processed.append( e );
// 						handled_l.append( e );
// 						handled_l.append( e2 );
// 						gw.set_direction( e, leda::mid_directed_edge );
// 					}
// 				}
// 			}
		}
	}
	//gw.set_edge_thickness( 3, true );
	//gw.set_node_height(50,true);
	gw.set_node_border_color( black, true );
	gw.set_node_border_thickness( 4, true );
	//gw.set_node_width( 100, true );
	gw.set_node_shape( leda::circle_node, true );
// 	gw.set_edge_label_type( leda::data_label , true);
	gw.set_edge_label_font( leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
//  	gw.set_edge_color( leda::blue , true );
	gw.set_layout( pos, bends );
	char filename4[128];
#ifdef LINUX
	sprintf( filename4, "sources/outputs/graphs/maingraph2.gml" );
#else
	sprintf( filename4, "sources//outputs//graphs//maingraph2.gml" );
#endif
	gw.save_gml( filename4 );
// 	cout << " 4 " << endl;
// 	array<color> color_l2( 11 );
// 	count = 255;
// 	for( int j = 0; j < color_l2.size(); j++ ){
// 		color el( count, 0, 0 );
// 		color_l2[ j ] = el;
// 		count -= 10;
// 	}
// 
// 	forall_items( it, processed ){
// 		int G_e = G[ processed[ it ] ] / 10;
// 		gw.set_color( processed[ it ], color_l2[ G_e ] );
// 	}

	edge_array<bool> handled( G, false );
	forall_items( it, handled_l )
		handled[ handled_l[ it ] ] = true;

	//Find multiple layer edges that are reversed in the first phase of sugiyama
	forall_edges( e, G ){
		forall_edges( e2, G ){
			if( layerInts[ G.source( e ) ] == layerInts[ G.source( e2 ) ] && 
			    layerInts[ G.target( e ) ] == layerInts[ G.target( e2 ) ] &&
			    e != e2 &&
			    handled[ e ] == false &&
			    handled[ e2 ] == false 
			    )
			{
				edge e_tmp;
// 				node s_node,t_node;
				if( layerInts[ G.source( e ) ] - layerInts[ G.target( e ) ] < 0 ){
					e_tmp = e;
// 					s_node = G.source( e );
// 					t_node = G.target( e );
				}
				else{
					e_tmp = e2;
// 					s_node = G.source( e );
// 					t_node = G.target( e );
				}
				processed.append( e_tmp );
				list<point> bs = bends[ e_tmp ];
				int e_w = G[ e_tmp ];

				list<point> bs2;
				while( bs.empty() != true ){
				      bs2.push_front( bs.pop_back() );
				}
				bends[ e_tmp ] = bs2;
				G[ e_tmp ] = e_w;
				gw.set_direction( e_tmp, leda::redirected_edge );
				handled[ e ] = true;
				handled[ e2 ] = true;
			}
		}
		handled[ e ] = true;
	}
	list<node> realNodes;

	forall_nodes( n, G ){
	      list<edge> elist = G.adj_edges(n);
	      if( elist.size() == 1 ){
		     gw.set_direction( elist[ elist.first_item()] , leda::undirected_edge );
	      }
	      realNodes.append( n );
	}
//	Uncomment to see reversed edges in tone of red  
// 	array<color> color_l2( 11 );
// 	count = 255;
// 	for( int j = 0; j < color_l2.size(); j++ ){
// 		color el( count, 0, 0 );
// 		color_l2[ j ] = el;
// 		count -= 10;
// 	}
// 
// 	forall_items( it, processed ){
// 		int G_e = G[ processed[ it ] ] / 10;
// 		gw.set_color( processed[ it ], color_l2[ G_e ] );
// 	}


	//gw.set_edge_thickness( 3, true );
	//gw.set_node_height(50,true);
	count = 0;
	forall_nodes( n, G ){
			color random( categ[ categ.get_item(count)]/*+1*/ );
			G.assign( n, categ[ categ.get_item(count)] );
			gw.set_border_color( n, random );
			count++;
	}
	gw.update_graph();
	gw.set_node_border_thickness( 4, true );
	//gw.set_node_width( 100, true );
	gw.set_node_shape( leda::circle_node, true );
// 	gw.set_edge_label_type( leda::data_label , true);
	gw.set_edge_label_font( leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
//  	gw.set_edge_color( leda::blue , true );
	gw.set_layout( pos, bends );
	
	char filename3[64];
#ifdef LINUX
	sprintf( filename3, "outputs/graphs/maingraph.gml" );
#else
	sprintf( filename3, "outputs//graphs//maingraph.gml" );
#endif
	gw.save_gml( filename3 );
// 	cout << " 5 " << endl;
#ifdef LINUX
	char filename2[32] = "outputs/psfiles/main";
#else
	char filename2[64] = "outputs//psfiles//main";
#endif
	sprintf( filename2, "%s%s", filename2, ".ps" );
	gw.save_ps( filename2 );
	return pos;
}



void insert ( GRAPH<int,int> &G, node u, list<two_tuple<node,int> > &ready_nodes)
{
	list_item it;
	int j = 0;
	int k;

	for (it = ready_nodes.last(); it != nil; it = ready_nodes.pred(it)) {

		node v     = ready_nodes[it].first();
		int  sigma = ready_nodes[it].second();

		if (sigma < j) {
			ready_nodes.insert(two_tuple<node,int>(u,j), it, LEDA::after);
			return;
		}

		if (sigma > j) continue;

		const d_int_set x;
		const d_int_set y;

	//	x.insert( G[ u ]);
	//	y.insert( G[ v ]);

		k = leda_min(x.size(),y.size());

		while (j < k && x.member(j) == y.member(j))
			j++;

		if (j == k) {
			if (x.size() < y.size()) continue;

			ready_nodes[it].second() = k;
			ready_nodes.insert(two_tuple<node,int>(u,sigma), it, LEDA::after);
			return;
		}

		if (x.member(j) < y.member(j)) continue;

		ready_nodes[it].second() = j;
		ready_nodes.insert(two_tuple<node,int>(u,sigma), it, LEDA::after);
		return;
	}

	ready_nodes.push_front(two_tuple<node,int>(u,j));
}


void insert (GRAPH<int,int> &G,node v, list<node> &ready, 	const node_array<int> &pi)
{
	for (list_item it = ready.last(); it != nil; it = ready.pred(it)) {

		if (pi[v] <= pi[ready[it]]) {
			ready.push( v );
			return;
		}
	}

	ready.push_front(v);
}

void callCoffman ( GRAPH<int,int> &G, node_array<int>& rank, int W)
{
	
	list<two_tuple<node,int> > ready_nodes;
	node_array<int> deg(G,0);
	node_array<int> pi(G,0);

	node v;
	forall_nodes(v,G) {
		if ((deg[v] = G.indeg(v)) == 0)
			ready_nodes.append(two_tuple<node,int>(v,0));
	}

	int i = 1;
	while(!ready_nodes.empty()) {
		v = ready_nodes.pop_front().first();
		pi[v] = i++;

		node u;
		forall_adj_nodes(u,v) {
			G[u] = pi[v];
			if (--deg[u] == 0)
				insert(G,u,ready_nodes);
		}
	}


	list<node> ready, waiting;

	forall_nodes(v,G) {
		if ((deg[v] = G.outdeg(v)) == 0)
			ready.append(v);
	}

	int k;
	for (k = 1; !ready.empty(); k++) {

		for (i = 1; i <= W && !ready.empty(); i++) {
			node u = ready.pop_front();
			rank[u] = k;

			edge e;
			forall_in_edges(e,u) {
				if (--deg[source(e)] == 0)
					waiting.append(source(e));
			}
		}

		while (!waiting.empty())
			insert (G,waiting.pop(), ready, pi);
	}

	k--;
	forall_nodes(v,G)
		rank[v] = k - rank[v];

}

bool isAllMarked(  GRAPH<int,int> &G, node_array<bool> &marked ){
	node n;
	forall_nodes( n, G ){
		if( marked[ n ] == false )
			return false;
	}
	return true;
}
void TOPSORTG_w( node n, GRAPH<int,int> &G, node_array<int> &ord,  node_array<int> &sumOfWeights ){
	node n1,n2;
	edge e;
	list_item it1,it2,temp_it;
	int ording = 1;
	ord[ n ] = ording;
	ording++;
	node_array<bool> marked( G, false );
	marked[ n ] = true;
	list<node> ready,temp;
	ready.push_back( n );
	while( isAllMarked( G, marked ) != true ){ 
		n1 = ready.pop_front();
		forall_out_edges( e, n1 ){
			if( marked[ G.target( e ) ] == false ){
				temp.push_back( G.target( e ) );
			}
		}
		if( temp.empty() == true ){
			forall_nodes( n2, G ){
				if( marked[ n2 ] == false ){
					ready.push_back( n2 );
					ord[ n2 ] = ording;
					marked[ n2 ] = true;
					ording++;
					break;
				}
			}
		}
		else{
			for( int i = 0; i < temp.size(); i++ ){
				int max = 0;
				forall_items( it1, temp ){
					if( sumOfWeights[ temp[it1] ] > max && marked[ temp[it1] ] == false ){
						max = sumOfWeights[ temp[it1] ];	
						temp_it = it1;
					}
				}
				ready.push_front( temp[ temp_it ] );
				ord[ temp[ temp_it ] ] = ording;
				ording++;
				marked[ temp[ temp_it ] ] = true;
			}
			temp.clear();
		}
	}
}

void algorithmTopologicalSortingU( GRAPH<int,int> &G, node_array<int>& rank, int W, int run ){
    node_array<int> ord( G, 0 ),ord2(G,0);
    node n,source,target,tmp_n;
    edge e;
    node_array<int> sumOfWeights( G, 0 );
    list_item it,tmp_it;

    TOPSORTG( G, ord );
    int max = 0;
    forall_nodes( n, G ){
        cout << ord[ n ] << " - ";
        if( max < ord[ n ] )
            max = ord[ n ];
    }
    array<list<node> > hasNeighbors( max + 1 );
    array<node> orders( max + 1 );

    cout << endl;
    int i = 1;
    forall_nodes( n, G ){
        list<edge> inEdges;
        inEdges = G.in_edges( n );
        forall_items( it, inEdges ){
            sumOfWeights[ n ] += G[ inEdges[ it ] ];
            hasNeighbors[ ord[ n ] ].append( G.target( inEdges[ it ] ) );
        }
        orders[ ord[ n ] ] = n;
    }

    int layerNumber = 1,index=1;
    for( int i = 0; i < G.number_of_nodes(); i++ ){
        list<node> Ranked,temp;
        node_array<bool> marked( G, false );

        forall_nodes( n, G ){
            rank[ n ] = ord[ n ];
            //cout << rank[ n ] << " - ";
        }
        Ranked.clear();
        marked[ orders[ layerNumber ] ] = true;
        Ranked.append( orders[ layerNumber ]  );

       
        while( Ranked.size() != G.number_of_nodes() ){
            bool check = false;
            max = 0;
            forall_nodes( n, G ){
                if( sumOfWeights[ n ] > max && marked[ n ] != true ){
                    max = sumOfWeights[ n ];
                    tmp_n = n;
                    check = true;
                }
            }
            if( check == false ){
                while( marked[ orders[ layerNumber ]] != true )
                    layerNumber++;
                marked[ orders[ layerNumber ]] = true;
                Ranked.append( orders[ layerNumber ] );
            }
            else{
                max = 0;
                forall_in_edges( e, tmp_n ){
                    if( rank[ G.source( e ) ] > max ){
                        max = rank[ G.source( e ) ];
                    }
                }
                rank[ tmp_n ] = max + 1;
                Ranked.append( tmp_n  );
                marked[ tmp_n ] = true;
            }
        }
        forall_nodes( n, G ){
            //cout << rank[ n ] << " - ";
            ord[ n ] = rank[ n ];
        }
    }

    max = 0;
    forall_nodes( n, G ){
        //cout << rank[ n ] << " - ";
        ord[ n ] = rank[ n ];
        if( rank[ n ] > max )
            max = rank[ n ];
    }
	int maxtemp = max;
	bool var = false;
    forall_nodes( source, G ){
        if( G.in_edges( source ).empty() == true && G.out_edges( source ).empty() == true){
			bool done = false;
			while( done != true ){
				var = false;
				forall_nodes( n, G ){
					if( rank[ n ] == max && ( G.in_edges( n ).empty() == false || G.out_edges( n ).empty() == false ) ){
						var = true;
					}
				}
				if( var == true ){
					rank[ source ] = max;
					done = true;
				}
				max--;
				if( max == 0 )
					max = maxtemp;
			}
        }
    }
   
   
    node_array<int> rank2( G, 0 );
    forall_nodes( n, G ){
        //cout << rank[ n ] << " - ";
        rank2[ n ] = rank[ n ];
        //ord[ n ] = rank[ n ];
    }
    max = 0;
    forall_nodes( n, G ){
        if( max < rank2[ n ] )
            max = rank2[ n ];
    }
   
    list<node> obs;
    int realLayering = 0;
    while( obs.size() != G.number_of_nodes() ){
        int min = max;
        forall_nodes( n, G ){
            if( min >= rank2[ n ] )
                min = rank2[ n ];
        }
        forall_nodes( n, G ){
            if( min == rank[ n ] ){
                obs.append( n );
                rank[ n ] = realLayering;
                rank2[ n ] = max;
            }
        }
        realLayering++;
    }
    TOPSORTG( G, ord2 );
    forall_nodes( n, G ){
        G[ n ] = ord2[ n ];
       cout << rank[ n ] << " - ";
    }
    cout << endl;
}

void algorithmTopologicalSortingW( GRAPH<int,int> &G, node_array<int>& rank, int W, int run ){
    node_array<int> ord( G, 0 ),ord2(G,0);
    node n,source,target,tmp_n;
    edge e;
	list_item it,tmp_it;
    node_array<int> sumOfWeights( G, 0 );
    
    int max = 0;
	int i = 1;
    
	forall_nodes( n, G ){
        list<edge> inEdges;
        inEdges = G.in_edges( n );
        forall_items( it, inEdges ){
			sumOfWeights[ n ] += G[ inEdges[ it ] ];
        }
    }

	if( run == 0 )
		forall_nodes( n, G ){
			if( G.in_edges( n ).empty() == true ){
				TOPSORTG_w( n, G, ord, sumOfWeights );
				break;
			}
		}
    cout << endl;
	max = 0;
	forall_nodes( n, G ){
		if( run == 1 || run == 2 ){
			ord[ n ] = rank[ n ];
		}
		//cout << ord[ n ] << " - ";
		if( max < ord[ n ] )
		max = ord[ n ];
	}
	

	array<list<node> > hasNeighbors( max + 1 );
	array<node> orders( max + 1 );

	i = 1;
    forall_nodes( n, G ){
        list<edge> inEdges;
        inEdges = G.in_edges( n );
        forall_items( it, inEdges ){
            sumOfWeights[ n ] += G[ inEdges[ it ] ];
            hasNeighbors[ ord[ n ] ].append( G.target( inEdges[ it ] ) );
        }
		orders[ ord[ n ] ] = n;

    }

    int layerNumber = 1,index=1;
    for( int i = 0; i < G.number_of_nodes(); i++ ){
        list<node> Ranked,temp;
        node_array<bool> marked( G, false );

        forall_nodes( n, G ){
            rank[ n ] = ord[ n ];
            //cout << rank[ n ] << " - ";
        }
        Ranked.clear();
        marked[ orders[ layerNumber ] ] = true;
        Ranked.append( orders[ layerNumber ]  );

       
        while( Ranked.size() != G.number_of_nodes() ){
            bool check = false;
            max = 0;
            forall_nodes( n, G ){
                if( sumOfWeights[ n ] > max && marked[ n ] != true ){
                    max = sumOfWeights[ n ];
                    tmp_n = n;
                    check = true;
                }
            }
            if( check == false ){
                while( marked[ orders[ layerNumber ]] != true )
                    layerNumber++;
                marked[ orders[ layerNumber ]] = true;
                Ranked.append( orders[ layerNumber ] );
            }
            else{
                max = 0;
                forall_in_edges( e, tmp_n ){
                    if( rank[ G.source( e ) ] > max ){
                        max = rank[ G.source( e ) ];
                    }
                }
                rank[ tmp_n ] = max + 1;
                Ranked.append( tmp_n  );
                marked[ tmp_n ] = true;
            }
        }
        forall_nodes( n, G ){
            //cout << rank[ n ] << " - ";
            ord[ n ] = rank[ n ];
        }
    }

    max = 0;
    forall_nodes( n, G ){
        //cout << rank[ n ] << " - ";
        ord[ n ] = rank[ n ];
        if( rank[ n ] > max )
            max = rank[ n ];
    }
	int maxtemp = max;
	bool var = false;
    forall_nodes( source, G ){
        if( G.in_edges( source ).empty() == true && G.out_edges( source ).empty() == true){
			bool done = false;
			while( done != true ){
				var = false;
				forall_nodes( n, G ){
					if( rank[ n ] == max && ( G.in_edges( n ).empty() == false || G.out_edges( n ).empty() == false ) ){
						var = true;
					}
				}
				if( var == true ){
					rank[ source ] = max;
					done = true;
				}
				max--;
				if( max == 0 )
					max = maxtemp;
			}
        }
    }
   
   
    node_array<int> rank2( G, 0 );
    forall_nodes( n, G ){
        //cout << rank[ n ] << " - ";
        rank2[ n ] = rank[ n ];
        //ord[ n ] = rank[ n ];
    }
    max = 0;
    forall_nodes( n, G ){
        if( max < rank2[ n ] )
            max = rank2[ n ];
    }
   
    list<node> obs;
    int realLayering = 0;
    while( obs.size() != G.number_of_nodes() ){
        int min = max;
        forall_nodes( n, G ){
            if( min >= rank2[ n ] )
                min = rank2[ n ];
        }
        forall_nodes( n, G ){
            if( min == rank[ n ] ){
                obs.append( n );
                rank[ n ] = realLayering;
                rank2[ n ] = max;
            }
        }
        realLayering++;
    }
}

void callModifiedCoffman ( GRAPH<int,int> &G, node_array<int>& rank, int W)
{
	
	list<two_tuple<node,int> > ready_nodes;
	node_array<int> deg(G,0);
	node_array<int> pi(G,0);
	list<node> prev_ready;
	list<node> next_ready;

	/* Modification 1 */

	node v;
	forall_nodes(v,G) {
		if ((deg[v] = G.indeg(v)) == 0){
			//cout <<  " " << G.outdeg( v );
			prev_ready.append( v );
		}
	}

	list_item it,it2,it_temp;
	list<edge> out_edges;
	edge e;
	node max_node;
	int sum_max_edge = 0;
	int max = 0;


	while( prev_ready.empty() != true ){
			forall_items( it, prev_ready ){
					out_edges = G.out_edges( prev_ready[ it ] );
					
					forall_items( it2, out_edges ){
							e = out_edges[ it2 ];
							sum_max_edge += G[ e ];
					}
					if( max <= sum_max_edge ){
							max = sum_max_edge;
							max_node = prev_ready[ it ];
							it_temp = it;
					}
					sum_max_edge = 0;
			}

			next_ready.push_front( max_node );
			prev_ready.del_item( it_temp );

			
			max = 0;
	}
	/*cout << " PREV :"; prev_ready.print(); cout << "\n NEXT : "; next_ready.print(); cout << endl;*/
	/* Modification 1 end here */

	forall_items(it,next_ready) {
		v = next_ready[ it ];
		if ((deg[ next_ready[ it ] ] = G.indeg(next_ready[ it ])) == 0)
			ready_nodes.append(two_tuple<node,int>(v,0));
	}
	

	int i = 1;
	while(!ready_nodes.empty()) {
		v = ready_nodes.pop_front().first();
		pi[v] = i++;

		node u;
		forall_adj_nodes(u,v) {
			G[u] = pi[v];
			if (--deg[u] == 0)
				insert(G,u,ready_nodes);
		}
	}


	list<node> ready, waiting;

	forall_nodes(v,G) {
		if ((deg[v] = G.outdeg(v)) == 0)
			ready.append(v);
	}
	

	int k;
	for (k = 1; !ready.empty(); k++) {

		for (i = 1; i <= W && !ready.empty(); i++) {
			node u = ready.pop_front();
			rank[u] = k;

			edge e;
			forall_in_edges(e,u) {
				if (--deg[source(e)] == 0)
					waiting.append(source(e));
			}
		}

		while (!waiting.empty())
			insert (G,waiting.pop(), ready, pi);
	}

	k--;
	forall_nodes(v,G)
		rank[v] = k - rank[v];

}

/********** END OF COFFMAN GRAHAM FUNCTIONS  ********************/


/*******************************************************************/

node_array<point> draw_final3( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> dummy , edge_array<list<double> > &z, edge_array<list<double> > &w, edge_array<list<point> > bends, int graphNo, node_array<double> &x, node_array<double> y ){
        dummy.print();
        double initialX = 0 , initialY = 0 , incX = 150 , incY = 100 ,i;

        list<node> A;
        color blue( 4 );
        color red( 7 );
        node_array<point> pos(G);
        list_item it;
        node n ;
        edge e;

	GraphWin gw(G);  
        int cou = KEYWSIZE;

    //    i = initialX;
    //    while( cou >= 0  )
    //    {
    //
    //#ifdef LENGTH
    //        cout << " - " << AB[ cou ].length()<< endl;
    //        cout << endl;
    //#endif

    //        A = AB[cou];
    //       
    //        forall(n,A){
    //
    //            x[n] = i ;  
    //            i += incX;

    //            y[n] = initialY;
    //
    //        }
    //        cou-=1;
    //        if( cou >= 0 )
    //            i = ( initialX - ( AB[ cou ].length() / 2 ) ) * 100;
    //        initialY -= incY;
    //       
    //    }


        random_source S(1,8);
        int random_value;

        forall_nodes( n, G ){
                S >> random_value;
                color random( random_value );
    #ifdef DRAW_FINAL
                gw.set_color( n, random);
    #endif

        }
        forall_items( it, dummy ){
                n = dummy[ it ];
                gw.set_color( n, red );
        }

        forall_nodes( n, G ){
                point p( x[ n ], y[ n ] );
                pos[ n ] = p;
        }
       
        char filename[64];
// #ifdef LINUX
// 	sprintf( filename, "sources/graph_sources/graph_gml%d.gml", graphNo );
// #else
// 	sprintf( filename, "sources//graph_sources//graph_gml%d.gml", graphNo );
// #endif
#ifdef DRAW_FINAL	
	gw.set_edge_thickness( 3, true );
	gw.set_node_height(50,true);
	gw.set_node_border_color( red, true );
	gw.set_node_border_thickness( 3, true );
	gw.set_node_width( 100, true );
	gw.set_node_shape( leda::ovalrect_node, true );
	gw.set_edge_label_type(leda::data_label , true);
	gw.set_edge_label_font(leda::roman_font, 42);
	gw.set_edge_shape( leda::poly_edge, true );
	gw.set_edge_color( leda::blue , true );
	gw.set_layout( pos, bends );
	gw.set_animation_steps(5);

// 	gw.save_gml( filename );
	gw.display();

	gw.zoom_graph();
	gw.edit();
#endif

	if( graphNo > 0 ){
		GraphWin gw(G);	
		gw.set_edge_thickness( 3, true );
		gw.set_node_height(50,true);
		gw.set_node_border_color( red, true );
		gw.set_node_border_thickness( 3, true );
		gw.set_node_width( 100, true );
		gw.set_node_shape( leda::ovalrect_node, true );
		gw.set_edge_label_type(leda::data_label , true);
		gw.set_edge_label_font(leda::roman_font, 42);
		gw.set_edge_shape( leda::poly_edge, true );
		gw.set_edge_color( leda::blue , true );
		gw.set_layout( pos, bends );
        
// 		gw.save_gml( filename );
#ifdef DRAW_FINAL
	gw.set_animation_steps(5);
	gw.display();
	gw.zoom_graph();
	gw.edit();
#endif
// 		char filen[ 1024 ];
// #ifdef LINUX
// 		sprintf( filen , "sources/graph_sources/Layout%d.txt" , graphNo );
// #else
// 		sprintf( filen , "sources//graph_sources//Layout%d.txt" , graphNo );
// #endif	
// 		
// 		bool flag = gw.save_layout( filen );
// 		if( flag == false )
// 			cout << " Unavailable to write\n";
	}
        return pos;
    }


    void determineYPositions( GRAPH<int,int> &G , array<list<node> > &AB, node_array<double> &y, double increment ){
        node n;
        edge e;
        list_item n_it,e_it;
        array<long> sumOfWeightedEdges( AB.size() + 1 ); // to store total sum of weigths between two consecutive layer
        long sum = 0;
        forall_edges( e, G ){
            sum += G[ e ];
        }
        double ypos = 0;
        list<node> nodesInLayer = AB[ AB.size() - 1 ];
        forall_items( n_it, nodesInLayer ){
            y[ nodesInLayer[n_it] ] = ypos;
        }
        ypos -= increment/2.0;
        double initialY = 0, incrementY = AB.size()*increment;
        for( int i = AB.size() - 2; i >= 0; i-- ){
            if( i == 0 ){
                ypos -= increment/2.0;
            }
            sumOfWeightedEdges[ i ] = 0;
            nodesInLayer = AB[ i ];
            forall_items( n_it, nodesInLayer ){
                list<edge> inEdges = G.in_edges( nodesInLayer[ n_it ] );
                forall_items( e_it, inEdges ){
                    sumOfWeightedEdges[ i ] += G[ inEdges[ e_it ] ];
                }
            }
            //cout << sumOfWeightedEdges[ i ] << endl;
            double result = (double)( (double)sumOfWeightedEdges[ i ] / sum ) * incrementY ;
            ypos -= (increment/2.0 + result);
            forall_items( n_it, nodesInLayer ){
                y[ nodesInLayer[n_it] ] = ypos;
            }
        }
    }


    matrix increaseColumn( matrix X, int endBegin ){
        if( endBegin == 1 ){   
            // Beginning of matrix columns
            matrix Y( X.dim1(), X.dim2() + 1);
            for( int i = 0; i < Y.dim1(); i++ ){
                for( int j = 0; j < Y.dim2(); j++ ){
                    if( j == 0){
                        Y[ i ][ j ] = 0;
                    }
                    else{
                        Y[ i ][ j ] = X[ i ][ j - 1 ];
                    }
                }
            }
            return Y;
        }
        else{
            // End of matrix columns
            matrix Y( X.dim1(), X.dim2() + 1);
            for( int i = 0; i < Y.dim1(); i++ ){
                for( int j = 0; j < Y.dim2(); j++ ){
                    if( j == Y.dim2() - 1){
                        Y[ i ][ j ] = 0;
                    }
                    else{
                        Y[ i ][ j ] = X[ i ][ j - 1 ];
                    }
                }
            }
            return Y;
        }
    }
    matrix increaseColumnI( matrix X, int index, int numberOfCols ){
        // Beginning of matrix columns
        matrix Y( X.dim1(), X.dim2() + numberOfCols);
        for( int i = 0; i < Y.dim1(); i++ ){
            for( int j = 0; j < Y.dim2(); j++ ){
                if( j < index + numberOfCols && j >= index ){
                    Y[ i ][ j ] = 0;
                }
                else{
                    if( j < index ){
                        Y[ i ][ j ] = X[ i ][ j ];
                    }
                    else{
                        Y[ i ][ j ] = X[ i ][ j - numberOfCols ];
                    }
                }
            }
        }
        return Y;
    }
    matrix increaseRow( matrix X, int endBegin ){
        if( endBegin == 1 ){   
            // Beginning of matrix rows
            matrix Y( X.dim1() + 1 , X.dim2());
            for( int i = 0; i < Y.dim1(); i++ ){
                for( int j = 0; j < Y.dim2(); j++ ){
                    if( i == 0){
                        Y[ i ][ j ] = 0;
                    }
                    else{
                        Y[ i ][ j ] = X[ i - 1 ][ j ];
                    }
                }
            }
            return Y;
        }
        else{
            // End of matrix rows
            matrix Y( X.dim1() + 1 , X.dim2());
            for( int i = 0; i < Y.dim1(); i++ ){
                for( int j = 0; j < Y.dim2(); j++ ){
                    if( i == Y.dim1() - 1){
                        Y[ i ][ j ] = 0;
                    }
                    else{
                        Y[ i ][ j ] = X[ i - 1 ][ j ];
                    }
                }
            }
            return Y;
        }
    }

    void print_matrix( matrix M ){
        int i,j;
        cout << "\n***********************************************\n";
        for( i = 0; i < M.dim1(); i++ ){
            for( j = 0; j < M.dim2(); j++ ){
                cout << M[ i ][ j ] << "-";
            }
            cout << endl;
        }
        cout << "***********************************************\n";
    }
    int countZerosAfter( matrix M, int dummyIndexCount_n , int layerNumber_n ){
        int count = 0;
        if( dummyIndexCount_n == M.dim2() )
            return count;
        for( int i = dummyIndexCount_n+1; i < M.dim2(); i++ ){
            if( M[layerNumber_n][i] == 0 ){
                count++;
            }
            else
                return count;
        }
    }
    int countZerosBefore( matrix M, int dummyIndexCount_n , int layerNumber_n ){
        int count = 0;
        if( dummyIndexCount_n == 0 )
            return count;
        for( int i = dummyIndexCount_n-1; i >= 0; i-- ){
            if( M[layerNumber_n][i] == 0 ){
                count++;
            }
            else
                return count;
        }
    }

    void identify_dummy_positionsOur( GRAPH<int,int> &G , array<list<node> > &AB, int KEYWSIZE, list<node> &dummy, node_array<double> &x , node_array<double> &y, int &algorithmFlag, int space, int increment, bool ledaPostFlag){
       
        double initialX = 0 , initialY = space * KEYWSIZE , incX = space , incY = increment;
        int i,j;
        int count_i = 0;   
       
        list_item it,it2,it3;
//         for( i = AB.size() - 1; i >= 0; i-- ){
//             forall_items( it, AB[ i ] ){
//                 G[ AB[ i ][ it ] ] = count_i;
//             }
//             count_i++;
//         }
        list<node> A;
        color blue( 4 );
        color red( 7 );
        color another( 6 );
        node n,z,source,target;
        edge e;

        array<list<int> > divisionPoints( AB.size()+1 );
        node_array<int> layerNumber( G, 0 );
        node_array<bool> isDummy( G, false );
        node_array<bool> marked( G, false );
        node_array<int> dummyIndexCount( G, -1 );
        node_array<int> howManyNodesAfter( G, 0 );
        node_array<int> howManyNodesBefore( G, 0 );

        forall_items( it, dummy ){
            isDummy[ dummy[ it ] ] = true;
        }
        int max = 0,max_layer;
        for( i = KEYWSIZE; i >= 0; i-- ){
            if( AB[ i ].size() >= max ){
                max = AB[ i ].size();
                max_layer = i;
            }
            list<node> L = AB[ i ];
            forall_items( it, L ){
                layerNumber[ L[it] ] = i;
            }
        }


        matrix matrix_binary(AB.size(),max);

        for( i = 0; i < matrix_binary.dim1(); i++ ){
            for( j = 0; j < matrix_binary.dim2(); j++ ){
                matrix_binary[ i ][ j ] = 0;
            }
        }

        int count = 0,count2 = 0;
        for( i = KEYWSIZE; i >= 0; i-- ){
            list<node> L = AB[ i ];
            count = 0;
            forall_items( it, L ){
                howManyNodesAfter[ L[it] ] = AB[ i ].size() - count;
                howManyNodesBefore[ L[it] ] = count - 1;
                count++;
            }
        }
        count = 0;
        forall_items( it, AB[ max_layer ] ){
            if( isDummy[ AB[ max_layer ][ it ] ] == true ){
                matrix_binary[ max_layer ][ count ] = 1;
                dummyIndexCount[ AB[ max_layer ][ it ] ] = count;
                marked[ AB[ max_layer ][ it ] ] = true;
            } 
            else{
                matrix_binary[ max_layer ][ count ] = 2;
                dummyIndexCount[ AB[ max_layer ][ it ] ] = count;
                marked[ AB[ max_layer ][ it ] ] = true;
            }
            count++;
        }
        if( max_layer > 0 ){
            for( i = max_layer -1; i >= 0; i-- ){
                forall_items( it, AB[ i ] ){
                    if( isDummy[ AB[ i ][ it ] ] == true ){
                        list<edge> inEdges = G.in_edges( AB[ i ][ it ] );
                        node source = G.source( inEdges.front() );
                        if( marked[ source ] == true ){
                            dummyIndexCount[ AB[ i ][ it ] ] = dummyIndexCount[ source ];
                            matrix_binary[ i ][ dummyIndexCount[ AB[ i ][ it ] ] ] = 1;
                            marked[ AB[ i ][ it ] ] = true;
                        }
                    }
                    //print_matrix( matrix_binary );
                }
            }
        }
        if( max_layer < KEYWSIZE ){
            for( i = max_layer +1; i <= KEYWSIZE; i++ ){
                forall_items( it, AB[ i ] ){
                    if( isDummy[ AB[ i ][ it ] ] == true ){
                        list<edge> outEdges = G.out_edges( AB[ i ][ it ] );
                        node target = G.target( outEdges.front() );
                        if( marked[ target ] == true ){
                            dummyIndexCount[ AB[ i ][ it ] ] = dummyIndexCount[ target ];
                            matrix_binary[ i ][ dummyIndexCount[ AB[ i ][ it ] ] ] = 1;
                            marked[ AB[ i ][ it ] ] = true;
                        }
                    }
                    /*print_matrix( matrix_binary );*/
                }
            }
        }
#ifdef DEBUG_X_OUR  
        print_matrix( matrix_binary );
#endif
        for( i = AB.size() - 2; i > 0; i-- ){
            count = 0;
            list<int> markedOnes;
            list<int> unmarkedDummies;
            list<int> unmarkedReals;
            list<int> whatisgoing;
            forall_items( it, AB[ i ] ){
                n = AB[ i ][ it ];
                if( marked[ n ] == true ){
                    if( matrix_binary[ i ][ dummyIndexCount[n] ] > 0 ){
                        markedOnes.push_back( count );
                        whatisgoing.push_back( 2 );
                    }
                }
                if( marked[ n ] == false && isDummy[ n ] == true ){
                    unmarkedDummies.push_back( count );
                    whatisgoing.push_back( 0 );
                }
                if( marked[ n ] == false && isDummy[ n ] == false){
                    unmarkedReals.push_back( count );
                    whatisgoing.push_back( 1 );
                }
                count++;
            }
#ifdef DEBUG_X_OUR  
            cout << endl << AB[ i ].size() << " - " << whatisgoing.size() << endl;
            whatisgoing.print();
            cout << endl;
#endif
            for( j = 0; j < AB[ i ].size(); j++ ){
                if( matrix_binary[ i ][ j ] == 0 ){
#ifdef DEBUG_X_OUR     
                    cout << " w : " << whatisgoing.size() << " - uR :" << unmarkedReals.size() << " - uD :" << unmarkedDummies.size() << endl;
#endif
                    int what = whatisgoing.pop_front();
                    if( what == 2 ){
                        while( matrix_binary[ i ][ j ] != 0 )
                            j++;
                        j++;
                    }
                    if( what == 1 ){
                        int tnn = unmarkedReals.pop_front();
                        //cout << endl << AB[ i ].size();
                        node unmarkedReal = AB[ i ][ AB[ i ].get_item( tnn )];
                        marked[ unmarkedReal ] = true;
                        matrix_binary[ i ][ j ] = 2;
                        dummyIndexCount[ unmarkedReal ] = j;
                    }
                    if( what == 0 ){
                        int tnn = unmarkedDummies.pop_front();
                        //cout << endl << AB[ i ].size();
                        node unmarkedDummy = AB[ i ][ AB[ i ].get_item( tnn )];
                        marked[ unmarkedDummy ] = true;
                        matrix_binary[ i ][ j ] = 1;
                        dummyIndexCount[ unmarkedDummy ] = j;
                    }
                }
                else{
                    int what = whatisgoing.pop_front();
                    if( what == 2 ){
                        ;
                    }
                    if( what == 1 ){
                        matrix_binary = increaseColumnI( matrix_binary, j , 1 );
                        forall_nodes( z, G ){
                            if( dummyIndexCount[ z ] > j - 1 )
                                dummyIndexCount[ z ] = dummyIndexCount[ z ] + 1;
                        }
                        node unmarkedReal = AB[ i ][ AB[ i ].get_item( unmarkedReals.pop_front() )];
                        marked[ unmarkedReal ] = true;
                        matrix_binary[ i ][ j + 1 ] = 2;
                        dummyIndexCount[ unmarkedReal ] = j;
                    }
                    if( what == 0 ){
                        matrix_binary = increaseColumnI( matrix_binary, j , 1 );
                        forall_nodes( z, G ){
                            if( dummyIndexCount[ z ] > j - 1)
                                dummyIndexCount[ z ] = dummyIndexCount[ z ] + 1;
                        }
                        node unmarkedDummy = AB[ i ][ AB[ i ].get_item( unmarkedDummies.pop_front() )];
                        marked[ unmarkedDummy ] = true;
                        matrix_binary[ i ][ j + 1 ] = 1;
                        dummyIndexCount[ unmarkedDummy ] = j;
                    }
                }
            }
        }
#ifdef DEBUG_X_OUR       
        for( i = AB.size()-2; i > 0; i-- ){
            forall_items( it, AB[ i ] ){
                n = AB[ i ][ it ];
                if( marked[ n ] == false ){
                    cout << dummyIndexCount[ n ] << "\t";
                }
                else{
                    cout << dummyIndexCount[ n ] << "\t";
                }
            }
            cout << endl;
        }
#endif
       
        for( i = AB.size()-2; i > 0; i-- ){
            forall_items( it, AB[ i ] ){
                n = AB[ i ][ it ];
                if( marked[ n ] == true ){
#ifdef DEBUG_X_OUR  
                    cout << dummyIndexCount[ n ] << "\t";
#endif
                    x[ n ] = dummyIndexCount[ n ] * incX;
                }
            }
#ifdef DEBUG_X_OUR 
            cout << endl;
#endif
        }
        list<node> L = AB[ AB.size() - 1], independents;

        count = 0;
        double maxPos = 0;
        forall_items( it, L ){
            if( marked[ L[ it ] ] != true ){
                list<edge> outEdges = G.out_edges( L[ it ] );
                double position = 0;
                if( outEdges.size() > 0 ){
                    forall_items( it2, outEdges ){
                        position += x[ G.target( outEdges[ it2 ] )];
                    }
                    x[L[ it ] ] =  position / (double) outEdges.size();
                    if( x[L[ it ] ] > maxPos )
                        maxPos = x[L[ it ] ];
                    marked[ L[ it ] ] = true;
                }
                else{
                    independents.append( L[ it ] );
                }
            }
            count++;
        }
        maxPos += incX;
        forall_items( it, independents ){
            x[independents[ it ] ] = maxPos;
            marked[ independents[ it ] ] = true;
        }   
        independents.clear();
        L = AB[ 0 ];
        list<double> coordinates;
        double coord = 0;
        forall_items( it, L ){
            coordinates.push_back( coord );
            x[L[ it ] ] = coord;
            coord+= incX;
        }
        count = 0;
        maxPos = 0;
        forall_items( it, L ){
            if( marked[ L[ it ] ] != true ){
                list<edge> inEdges = G.in_edges( L[ it ] );
                double position = 0;
                if( inEdges.size() > 0 ){
                    forall_items( it2, inEdges ){
                        position += x[ G.source( inEdges[ it2 ] )];
                    }
                    x[L[ it ] ] =  position / (double) inEdges.size();
                    if( x[L[ it ] ] > maxPos )
                        maxPos = x[L[ it ] ];
                    marked[ L[ it ] ] = true;
                }
                else{
                    independents.append( L[ it ] );
                }
                coord = coordinates[ coordinates.get_item( count ) ];
                double diff = x[ L[ it ] ] - coord;
                it2 = it;
                while( L.succ( it2 )!= L.last() ){
                    if( L.succ( it2 )!= NULL )
                        it2 = L.succ( it2 );
                    else
                        break;
                    x[L[ it2 ] ] += diff;
                }
            }
            count++;
        }

        maxPos += incX;
        forall_items( it, independents ){
            x[independents[ it ] ] = maxPos;
            marked[ independents[ it ] ] = true;
	    maxPos += incX;
        }   

        for( i = AB.size() - 1; i > 0; i-- ){

            list<node> L = AB[ i ];
            forall_items( it, L ){
                n = L[ it ];
                if( G.in_edges(n).size() == 0 ){
                    // It is sink node
                    int flag1 = 0,flag2 = 0;
                    double xpos_s = 0;
                    if( L.succ( it ) ){
                        flag1 = 1;
                        xpos_s = x[ L[L.succ( it )]];
                    }
                    double xpos_t = 0;
                    if( L.pred( it ) ){
                        flag2 = 1;
                        xpos_t = x[ L[L.pred( it )]];
                    }
                    if( flag1 == 1 && flag2 == 1 ){
                        x[ n ] = ( xpos_t + xpos_s ) / 2.0;
                    }
                    else{
                        if( flag1 == 1 && flag2 == 0 ){
                            x[ n ] = xpos_s - incX;
                        }
                        else{
                            if( flag1 == 0 && flag2 == 1 ){
                                x[ n ] = xpos_t + incX;

                            }
                        }
                    }
                }
            }
        }



#ifdef DEBUG_X_OUR  
            print_matrix( matrix_binary );
#endif
        int cou = KEYWSIZE;

        i = initialX;
        determineYPositions( G, AB, y, incY );
        //while( cou >= 0  )
        //{
        //    A = AB[cou];           
        //    forall(n,A){
        //        x[n] = dummyIndexCount[ n ] * incX;  
        //        //y[n] = initialY;
        //    }
        //    cou-=1;
        //    //initialY -= incY;
        //}
        //    
      if( ledaPostFlag == true )
		pos_process( G, AB, x, space, space, 20, dummy );
	   
        GraphWin gw(G);   
        forall_items( it, dummy ){
                n = dummy[ it ];
                gw.set_color( n, red );
        }
        forall_nodes( n, G ){
            if( marked[ n ]== false )
                gw.set_color( n, another );
        }

        gw.set_node_height(20,true);
        gw.set_radius1( dummy, 2 );
        gw.set_radius2( dummy, 2 );
        gw.set_node_shape( leda::ovalrect_node, true );
        gw.set_edge_label_type(leda::data_label , true);
        gw.set_edge_label_font(leda::roman_font, 24);
        gw.set_edge_shape( leda::circle_edge, true );
        gw.set_zoom_labels(true);
        gw.set_animation_steps(5);

        gw.set_position(x,y);
#ifdef DEBUG_X_OUR  
        gw.display();
        gw.zoom_graph();
        gw.edit();
    /*    node_array<point> pos;
        gw.get_position( pos );
        forall_nodes( n, G ){
            x[ n ] = pos[ n ].xcoord();
            y[ n ] = pos[ n ].ycoord();
        }*/
        cout << endl;
        for( i = AB.size() - 1; i >= 0; i-- ){
            forall_items( it, AB[ i ] ){
                cout << G[ AB[ i ][ it ] ] << "\t";
            }
            cout << endl;
        }
#endif
    }


/*************************************************************/
/*    POST PROCESS FUNCTION ADDED 24 October                 */
/* Used for tidy up operations for the resulting layout      */
/*************************************************************/

    void pos_process( GRAPH<int,int> &G , array<list<node> > &AB, node_array<double> &x, double minSepConst, double increment, int iterations, list<node> &dummy ){
        node n1,n2;
        list_item n_it,e_it,it;
        edge e1,e2;
        list<node> L;
        node_array<node> prevs( G );
        node_array<node> succs( G );
        for( int i = AB.size() - 1; i >= 0; i-- ){
            L = AB[ i ];
            forall_items( it, L ){
                if( L.succ( it ) )
                    succs[ L[ it ] ] = L[ L.succ( it )];
                if( L.pred( it ) )
                    prevs[ L[ it ] ] = L[ L.pred( it )];
            }
            L.clear();
        }
        int count = 0;
        while( count != iterations ){
            /************************************************/
            /************************************************/
            cout << endl << " Loop 1" << endl;
            for( int i = AB.size() - 1; i >= 0; i-- ){
                L = AB[ i ];
                forall_items( n_it, L ){
                    if( G.indeg(L[ n_it ]) < G.outdeg(L[ n_it ]) ){
                        list<edge> outEdges = G.out_edges( L[ n_it ] );
                        if( outEdges.size() > 0 ){
                            double realWantedPos = 0;
                            double incr = 0;
                            forall_items( e_it, outEdges ){
                                realWantedPos += x[ G.target( outEdges[ e_it ] ) ];
                                incr += 1.0;
                            }
                           
                            realWantedPos = realWantedPos / incr;
                            if( L.pred( n_it ) ){
                                if( realWantedPos - minSepConst > x[ prevs[ L[ n_it ] ]] ){
                                    if( L.succ( n_it ) ){   
                                        if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                                x[ L[ n_it ] ] = realWantedPos;   
                                        }
                                    }
                                }
                            }
                            else{
                                if( L.succ( n_it ) ){   
                                    if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                            x[ L[ n_it ] ] = realWantedPos;   
                                    }
                                }
                            }
                            //cout << endl << " Kontrol " << endl;
                        }
                    }
                }
                L.clear();
            }
            count++;
        }
        count = 0;
        while( count != iterations ){
            /************************************************/
            /************************************************/
            cout << endl << " Loop 2" << endl;
            for( int i = 0; i < AB.size() - 1; i++ ){
                L = AB[ i ];
                forall_items( n_it, L ){
                    if( G.indeg(L[ n_it ]) > G.outdeg(L[ n_it ]) ){
                        list<edge> inEdges = G.in_edges( L[ n_it ] );
                        if( inEdges.size() > 0 ){
                            double realWantedPos = 0;
                            double incr = 0;
                            forall_items( e_it, inEdges ){
                                realWantedPos += x[ G.source( inEdges[ e_it ] ) ];
                                incr += 1.0;
                            }
                           
                            realWantedPos = realWantedPos / incr;
                            if( L.pred( n_it ) ){
                                if( realWantedPos - minSepConst > x[ prevs[ L[ n_it ] ]] ){
                                    if( L.succ( n_it ) ){   
                                        if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                                x[ L[ n_it ] ] = realWantedPos;   
                                        }
                                    }
                                }
                            }
                            else{
                                if( L.succ( n_it ) ){   
                                    if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                            x[ L[ n_it ] ] = realWantedPos;   
                                    }
                                }
                            }
                            //cout << endl << " Kontrol " << endl;
                        }
                    }
                }
                L.clear();
            }
            count++;

        }
        count = 0;
        while( count != iterations ){
            /************************************************/
            /************************************************/
            cout << endl << " Loop 3" << endl;
            for( int i = AB.size() - 1; i >= 0; i-- ){
                L = AB[ i ];
                forall_items( n_it, L ){
                    if( G.indeg(L[ n_it ]) > G.outdeg(L[ n_it ]) ){
                        list<edge> inEdges = G.in_edges( L[ n_it ] );
                        if( inEdges.size() > 0 ){
                            double realWantedPos = 0;
                            double incr = 0;
                            forall_items( e_it, inEdges ){
                                realWantedPos += x[ G.source( inEdges[ e_it ] ) ];
                                incr += 1.0;
                            }
                           
                            realWantedPos = realWantedPos / incr;
                            if( L.pred( n_it ) ){
                                if( realWantedPos - minSepConst > x[ prevs[ L[ n_it ] ]] ){
                                    if( L.succ( n_it ) ){   
                                        if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                                x[ L[ n_it ] ] = realWantedPos;   
                                        }
                                    }
                                }
                            }
                            else{
                                if( L.succ( n_it ) ){   
                                    if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                            x[ L[ n_it ] ] = realWantedPos;   
                                    }
                                }
                            }
                            //cout << endl << " Kontrol " << endl;
                        }
                    }
                }
                L.clear();
            }
            count++;

        }
        count = 0;
        while( count != iterations ){
            /************************************************/
            /************************************************/
            cout << endl << " Loop 4" << endl;
            for( int i = 0; i < AB.size() - 1; i++ ){
                L = AB[ i ];
                forall_items( n_it, L ){
                    if( G.indeg(L[ n_it ]) < G.outdeg(L[ n_it ]) ){
                        list<edge> outEdges = G.out_edges( L[ n_it ] );
                        if( outEdges.size() > 0 ){
                            double realWantedPos = 0;
                            double incr = 0;
                            forall_items( e_it, outEdges ){
                                realWantedPos += x[ G.target( outEdges[ e_it ] ) ];
                                incr += 1.0;
                            }
                           
                            realWantedPos = realWantedPos / incr;
                            if( L.pred( n_it ) ){
                                if( realWantedPos - minSepConst > x[ prevs[ L[ n_it ] ]] ){
                                    if( L.succ( n_it ) ){   
                                        if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                                x[ L[ n_it ] ] = realWantedPos;   
                                        }
                                    }
                                }
                            }
                            else{
                                if( L.succ( n_it ) ){   
                                    if( realWantedPos + minSepConst < x[ succs[ L[ n_it ] ]] ){
                                            x[ L[ n_it ] ] = realWantedPos;   
                                    }
                                }
                            }
                            //cout << endl << " Kontrol " << endl;
                        }
                    }
                }
                L.clear();
            }
            count++;
        }
    }
