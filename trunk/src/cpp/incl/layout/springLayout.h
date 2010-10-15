/********************************************************************************************* 
Created : 31 October 2009

Modified : 01 November 2009
	Three different call added to SPRING_EMBEDDING function
	  15 December 2009
	Embeded into this software

*	The code adopted from open source AGD and open source OGDF
*	There are several function prototypes 
		1. Main Call :
		void SPRING_EMBEDDING_our(GRAPH<int,int>& G, list<node>& fixed_nodes,
										  node_array<double>& xpos,
										  node_array<double>& ypos,
										  double xleft, double xright,
										  double ybottom, double ytop,
										  int iterations);
		2. SubCalls :
			i.   without fixed nodes
				 void SPRING_EMBEDDING_(GRAPH<int,int>& G, node_array<double>& xpos,
										  node_array<double>& ypos,
										  double xleft, double xright,
										  double ybottom, double ytop,
										  int iterations)
			ii.  without border points and without fixed nodes
				 void SPRING_EMBEDDING_(GRAPH<int,int>& G, node_array<double>& xpos, 
										  node_array<double>& ypos, int iterations)
		
			iii. without border points and with fixed_nodes
				 void SPRING_EMBEDDING_(GRAPH<int,int>& G, list<node>& fixed_nodes, 
										node_array<double>& xpos, node_array<double>& ypos, 
										int iterations)

*********************************************************************************************/

#include "misc/declerations.h"
#include <LEDA/core/array2.h> 
#include <LEDA/graph/basic_graph_alg.h>

#define SPRING_DRAW
#define SPRING_COLOR

//1. Main Call Prototype:
void SPRING_EMBEDDING_our(GRAPH<int,int>& G, list<node>& fixed_nodes,
                                      node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      double xleft, double xright,
                                      double ybottom, double ytop,
                                      int iterations,node_array<int> &nodeIds);
void SPRING_EMBEDDING_our2(GRAPH<int,int>& G, list<node>& fixed_nodes,
                                      node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      double xleft, double xright,
                                      double ybottom, double ytop,
                                      int iterations,
				      node_array<int> &nodeIds);
#define FREPULSE(d) ((k2 > d) ? kk/d : 0)

static float log_2(int x)
{ float l = 0;
  while (x)
  { l++;
    x >>= 1;
   }
  return l/2;
}

//i.   without fixed nodes
void SPRING_EMBEDDING_(GRAPH<int,int>& G, node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      double xleft, double xright,
                                      double ybottom, double ytop,
                                      int iterations,
				      node_array<int> &nodeIds,
				      array<char> &abbv,
				      int cat_num, 
				      array<char> &Categories )
{ 
	list<node> L;
        node_array<int> comp( G, 0 );
	node n;
	list_item it;
	int count = 0;

	SPRING_EMBEDDING_our2(G,L,xpos,ypos,xleft,xright,ybottom,ytop,iterations,nodeIds); 

	GraphWin gw( G );
#ifdef SPRING_COLOR
	random_source S3(1,13);
	int random_value;

	forall_nodes( n, G ){
		//S3 >> random_value;
		color random( nodeIds[ n ] );
		gw.set_color( n, random);
	}
#endif SPRING_COLOR

// cout << "\n------------------------\n";

	gw.set_edge_thickness( 3, true );
	forall_nodes( n, G ){
		for( int i = 0; i < cat_num; i++ ){
			if( Categories[ G[ n ] ] == abbv[ i ] ){
				leda::color x( i + 1 );
				gw.set_border_color( n,x );
			}
		}
		gw.set_color( n, red );
	}
	edge e;
	array<color> color_l( 5 );

	list<int> weights;
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
// cout << "\n-------------------\n";
	edgeWeights.sort();
	int tm_c = (double)(220.0 / (double)edgeWeights.size() );
	count = 20 + edgeWeights.size() * tm_c;
	for( int j = 0; j < color_l.size(); j++ ){
		color el( count, count, count );
		color_l[ j ] = el;
		count -= tm_c;
	}		
	count = 0;
	for( int x = edgeWeights[edgeWeights.first()]; x <= edgeWeights[edgeWeights.last()]; x++ ){	
		for( int j = 0; j < color_l.size(); j++ ){
			two_tuple<color,int> Tp( color_l[ j ], x );
// 			cout << Tp << endl;
			variations.push_back( Tp );
		}
		count++;
	}
// cout << "\n--------\n";
	int increase = (int) ((double)variations.size() / (double)edgeWeights.size());
// 	cout << " increase : " << increase << endl;
	forall_edges( e, G ){		           
		count = 0;
		forall_items( it, edgeWeights ){
		      if( edgeWeights[ it ] == G[ e ] ){
			    break;
		      }
		      count++;
		}
		two_tuple<color,int> Tp = variations[ variations.get_item( increase / 10.0 * count ) ] ;
		gw.set_thickness( e, Tp.second() / 10.0 * 5.0 );
		gw.set_color( e, Tp.first() );
// 		cout << Tp << endl;
	}

cout << "\n-----\n";
#ifdef SPRING_DRAW
	gw.set_position( xpos, ypos );
	gw.set_edge_thickness( 0.1, true );
	gw.set_node_height(4,true);
	gw.set_node_width(4,true);
	gw.set_layout();
	gw.set_animation_steps(3);
	gw.set_edge_label_type(leda::data_label , true);
// 	gw.display();
// 	gw.zoom_graph();
//  	gw.edit();
#endif SPRING_DRAW
}

//i.   without fixed nodes
void SPRING_EMBEDDING_(GRAPH<int,int>& G, node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      double xleft, double xright,
                                      double ybottom, double ytop,
                                      int iterations,
				      node_array<int> &nodeIds,
				      array<char> &abbv,
				      int cat_num )
{ 
	list<node> L;
	SPRING_EMBEDDING_our2(G,L,xpos,ypos,xleft,xright,ybottom,ytop,iterations,nodeIds); 
}

//i.   without fixed nodes for main graph
void SPRING_EMBEDDING2_(	GRAPH<int,int>& G, 
			    node_array<double>& xpos,
                            node_array<double>& ypos,
                            double xleft, double xright,
                            double ybottom, double ytop,
                            int iterations,
			    node_array<int> &nodeIds,
			    node_array<double> &Hvalues )
{ 
	list<node> L;
	SPRING_EMBEDDING_our2(G,L,xpos,ypos,xleft,xright,ybottom,ytop,iterations,nodeIds); 
}

//ii.  without border points and without fixed nodes
void SPRING_EMBEDDING_(GRAPH<int,int>& G, node_array<double>& xpos, node_array<double>& ypos, int iterations,node_array<int> &nodeIds){
	GraphWin gw( G, 1000, 1000 );
	double xleft   = gw.get_xmin();
	double xright  = gw.get_xmax();
	double ybottom = gw.get_ymin();
	double ytop    = gw.get_ymax();
	list<node> L;
	SPRING_EMBEDDING_our(G,L,xpos,ypos,xleft,xright,ybottom,ytop,iterations,nodeIds); 
}

//iii. without border points and with fixed_nodes
void SPRING_EMBEDDING_(GRAPH<int,int>& G, list<node>& fixed_nodes, node_array<double>& xpos, node_array<double>& ypos, int iterations, node_array<int> &nodeIds){
	GraphWin gw( G, 1000, 1000 );
	double xleft   = gw.get_xmin();
	double xright  = gw.get_xmax();
	double ybottom = gw.get_ymin();
	double ytop    = gw.get_ymax(); 
	SPRING_EMBEDDING_our(G,fixed_nodes,xpos,ypos,xleft,xright,ybottom,ytop,iterations, nodeIds); 
}

//1. Main Call 
void SPRING_EMBEDDING_our(GRAPH<int,int>& G, list<node>& fixed_nodes,
                                      node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      double xleft, double xright,
                                      double ybottom, double ytop,
                                      int iterations,
				      node_array<int> &nodeIds)
{

	int lower_bound=static_cast<int>(xleft);
	int upper_bound=static_cast<int>(xright);
	GraphWin gw( G, 500, 500 );
	node u,v;

/*        COMPONENTS( G, comp );
        int max = 0;
        forall_nodes( n, G ){
            if( max < comp[ n ] )
                max = comp[ n ];
        }
        array<list<node> > COMPS( max + 1 );
        int count = 1;
        forall_nodes( n, G ){
            COMPS[ comp[ n ] ].append( n );
        }  */   
	int max = 0;


	
	random_source S2(lower_bound,upper_bound);
	int x1;
	forall_nodes(v,G) {
			S2 >> x1;
			xpos[v]=x1;
	}
	lower_bound=static_cast<int>(ybottom);
	upper_bound=static_cast<int>(ytop);
	S2.set_range(lower_bound,upper_bound);
	int y; 
	forall_nodes(v,G) {
			S2 >> y;
			ypos[v]=y;
	}
	if (xleft >= xright || ybottom >= ytop)
		cout << "SPRING_EMBDDING: illegal bounds.";

	if (G.number_of_nodes() < 2) 
		return;

	forall_nodes( u, G ){
		if( G.degree( u ) > max ){
			max = G.degree( u );
			v = u;
		}
	}
	circle C( xpos[ v ], ypos[ v ], max * 20.0 );
	max = 0;
	forall_adj_nodes( u, v ){
	      max++;
	}
	double min = 3.1415 / (double)max, tmp;
	forall_adj_nodes( u, v ){
	      point pos = C.point_on_circle( tmp );
	      xpos[ u ] = pos.xcoord();
	      ypos[ u ] = pos.ycoord();
	      fixed_nodes.append( u );
	      tmp -= min;   
	}  
	max = 0;
	node_array<list_item> lit(G);
	node_array<bool> fixed(G,false);


	edge e;

	forall(v,fixed_nodes) 
		fixed[v] = true;

	int c_f = 1;

	double width  = xright - xleft;
	double height = ytop - ybottom;

	double tx_null = width/50;
	double ty_null = height/50;
	double tx = tx_null;
	double ty = ty_null;

	double k = sqrt(width*height / G.number_of_nodes()) / 2;
	double k2 = 2*k;
	double kk = k*k;

	int ki = int(k);

	if (ki == 0) ki = 1;

	//build  matrix of node lists

	int xA = int(width / ki + 1);
	int yA = int(height / ki + 1);

	array2<list<node> > A(-1,xA,-1,yA);

	forall_nodes(v,G)
	{ 
		int i = int((xpos[v] - xleft)   / ki);
		int j = int((ypos[v] - ybottom) / ki);
		if (i >= xA || i < 0 || j >= yA || j < 0)
			printf("spring: (%f %f) out of range (%f,%f,%f,%f)",xpos[v],ypos[v],xleft,ybottom,xright,ytop);
		lit[v] = A(i,j).push(v);
	}


	while (c_f < iterations)
	{
		node_array<double>  xdisp(G,0);
		node_array<double>  ydisp(G,0);

		// repulsive forces

		forall_nodes(v,G)
		{ 
			int i = int((xpos[v] - xleft)   / ki);
			int j = int((ypos[v] - ybottom) / ki);

			double xv = xpos[v];
			double yv = ypos[v];

			for(int m = -1; m <= 1; m++)
				for(int n = -1; n <= 1; n++)
					forall(u,A(i+m,j+n)){ 
					   if(u == v) continue;
							 double xdist = xv - xpos[u];
							 double ydist = yv - ypos[u];
							 double dist = sqrt(xdist * xdist + ydist * ydist);
							 if (dist < 1e-3) dist = 1e-3;
							 xdisp[v] += FREPULSE(dist) * xdist / dist;
							 ydisp[v] += FREPULSE(dist) * ydist / dist;
					}
			xdisp[v] *=  (double(rand_int(75,125))/1000.0);
			ydisp[v] *=  (double(rand_int(75,125))/1000.0);
		 }



		// attractive forces

		forall_edges(e,G){ 
			node u = G.source(e);
			node v = G.target(e);
			double xdist=xpos[v]-xpos[u];
			double ydist=ypos[v]-ypos[u];
			double dist=sqrt(xdist*xdist+ydist*ydist);

			float f = (G.degree(u)+G.degree(v))/6.0;

			dist /= f;

			xdisp[v]-=xdist*dist/k;
			ydisp[v]-=ydist*dist/k;
			xdisp[u]+=xdist*dist/k;
			ydisp[u]+=ydist*dist/k;
		}


		// preventions

		forall_nodes(v,G){
			if (fixed[v]) continue;

			int i0 = int((xpos[v] - xleft)/ki);
			int j0 = int((ypos[v] - ybottom)/ki);

			double xd= xdisp[v];
			double yd= ydisp[v];
			double dist = sqrt(xd*xd+yd*yd);

			if (dist < 1) 
				dist = 1;

			xd = tx*xd/dist;
			yd = ty*yd/dist;

			double xp = xpos[v] + xd;
			double yp = ypos[v] + yd;

			int i = i0;
			int j = j0;

			if (xp > xleft && xp < xright){ 
				xpos[v] = xp;
				i = int((xp - xleft)/ki);
			}

			if (yp > ybottom && yp < ytop){ 
				ypos[v] = yp;
				j = int((yp - ybottom)/ki);
			}

			if (i != i0 || j != j0){ 
				if (lit[v] == nil) 
					error_handler(1,"delete nil item");
				A(i0,j0).del_item(lit[v]);
				lit[v] = A(i,j).push(v);
			}
		}
		tx = tx_null / log_2(c_f);
		ty = ty_null / log_2(c_f);

		c_f++;
	}

// #ifdef SPRING_COLOR
// 	random_source S3(1,13);
// 	int random_value;
// 
// 	forall_nodes( v, G ){
// 		//S3 >> random_value;
// 		color random( nodeIds[ v ] );
// 		gw.set_color( v, random);
// 	}
// #endif SPRING_COLOR
// 
// #ifdef SPRING_DRAW
// 	gw.set_position( xpos, ypos );
// 
// 
// 	gw.set_edge_thickness( 0.1, true );
// 	gw.set_node_height(5,true);
// 	gw.set_node_width(5,true);
// 	gw.set_layout();
// 	gw.set_animation_steps(3);
// 	gw.set_edge_label_type(leda::data_label , true);
// 	gw.display();
// 	gw.zoom_graph();
//  	gw.edit();
// 
// #endif SPRING_DRAW
}

//2. Main Call 
void SPRING_EMBEDDING_our2(GRAPH<int,int>& G, list<node>& fixed_nodes,
                                      node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      double xleft, double xright,
                                      double ybottom, double ytop,
                                      int iterations,
				      node_array<int> &nodeIds)
{

	int lower_bound=static_cast<int>(xleft);
	int upper_bound=static_cast<int>(xright);
// 	GraphWin gw( G, 500, 500 );
	random_source S2(lower_bound,upper_bound);
	int x1; 
	node u,v;
	forall_nodes(v,G) {
			S2 >> x1;
			xpos[v]=x1;
	}
	lower_bound=static_cast<int>(ybottom);
	upper_bound=static_cast<int>(ytop);
	S2.set_range(lower_bound,upper_bound);
	int y; 
	forall_nodes(v,G) {
			S2 >> y;
			ypos[v]=y;
	}
	if (xleft >= xright || ybottom >= ytop)
		cout << "SPRING_EMBDDING: illegal bounds.";

	if (G.number_of_nodes() < 2) 
		return;

	node_array<list_item> lit(G);
	node_array<bool> fixed(G,false);

// 	cout << "\n Burda 1\n";
	edge e;

	forall(v,fixed_nodes) 
		fixed[v] = true;

	int c_f = 1;

	double width  = xright - xleft;
	double height = ytop - ybottom;

	double tx_null = width/50;
	double ty_null = height/50;
	double tx = tx_null;
	double ty = ty_null;

	double k = sqrt(width*height / G.number_of_nodes()) / 2;
	double k2 = 2*k;
	double kk = k*k;

	int ki = int(k);

	if (ki == 0) ki = 1;

	//build  matrix of node lists

	int xA = int(width / ki + 1);
	int yA = int(height / ki + 1);

	array2<list<node> > A(-1,xA,-1,yA);

	forall_nodes(v,G)
	{ 
		int i = int((xpos[v] - xleft)   / ki);
		int j = int((ypos[v] - ybottom) / ki);
		if (i >= xA || i < 0 || j >= yA || j < 0)
			printf("spring: (%f %f) out of range (%f,%f,%f,%f)",xpos[v],ypos[v],xleft,ybottom,xright,ytop);
		lit[v] = A(i,j).push(v);
	}
// cout << "\n Burda 2\n";

	while (c_f < iterations)
	{
// 		cout << "\t loop" << c_f;
		node_array<double>  xdisp(G,0);
		node_array<double>  ydisp(G,0);

		// repulsive forces

		forall_nodes(v,G)
		{ 
			int i = int((xpos[v] - xleft)   / ki);
			int j = int((ypos[v] - ybottom) / ki);

			double xv = xpos[v];
			double yv = ypos[v];

			for(int m = -1; m <= 1; m++)
				for(int n = -1; n <= 1; n++)
					forall(u,A(i+m,j+n)){ 
					   if(u == v) continue;
							 double xdist = xv - xpos[u];
							 double ydist = yv - ypos[u];
							 double dist = sqrt(xdist * xdist + ydist * ydist);
							 if (dist < 1e-3) dist = 1e-3;
							 xdisp[v] += FREPULSE(dist) * xdist / dist;
							 ydisp[v] += FREPULSE(dist) * ydist / dist;
					}
			xdisp[v] *=  (double(rand_int(75,125))/1000.0);
			ydisp[v] *=  (double(rand_int(75,125))/1000.0);
		 }


                node n;
                node_array<float> degrees( G, 0.0 );
                forall_nodes( n, G ){
                    list<edge> uedge_l = G.adj_edges( n );
                    list_item it;
                    forall_items( it, uedge_l )
                            degrees[ n ] += G[ uedge_l[ it ]];
                }

		// attractive forces

		forall_edges(e,G){ 
			node u = G.source(e);
			node v = G.target(e);
			double xdist=xpos[v]-xpos[u];
			double ydist=ypos[v]-ypos[u];
			double dist=sqrt(xdist*xdist+ydist*ydist);

                        //float f = G[e] * (G.degree(u)+G.degree(v))/6.0;
                        float f = (degrees[u]+degrees[v])/((float)G[e]*10.0);

			dist /= f;

			xdisp[v]-=xdist*dist/k;
			ydisp[v]-=ydist*dist/k;
			xdisp[u]+=xdist*dist/k;
			ydisp[u]+=ydist*dist/k;
		}


		// preventions

		forall_nodes(v,G){
			if (fixed[v]) continue;

			int i0 = int((xpos[v] - xleft)/ki);
			int j0 = int((ypos[v] - ybottom)/ki);

			double xd= xdisp[v];
			double yd= ydisp[v];
			double dist = sqrt(xd*xd+yd*yd);

			if (dist < 1) 
				dist = 1;

			xd = tx*xd/dist;
			yd = ty*yd/dist;

			double xp = xpos[v] + xd;
			double yp = ypos[v] + yd;

			int i = i0;
			int j = j0;

			if (xp > xleft && xp < xright){ 
				xpos[v] = xp;
				i = int((xp - xleft)/ki);
			}

			if (yp > ybottom && yp < ytop){ 
				ypos[v] = yp;
				j = int((yp - ybottom)/ki);
			}

			if (i != i0 || j != j0){ 
				if (lit[v] == nil) 
					error_handler(1,"delete nil item");
				A(i0,j0).del_item(lit[v]);
				lit[v] = A(i,j).push(v);
			}
		}
		tx = tx_null / log_2(c_f);
		ty = ty_null / log_2(c_f);

		c_f++;
	}
// 	cout << endl;
/*
#ifdef SPRING_COLOR
	random_source S3(1,13);
	int random_value;

	forall_nodes( v, G ){
		//S3 >> random_value;
		color random( nodeIds[ v ] );
		gw.set_color( v, random);
	}
#endif SPRING_COLOR

#ifdef SPRING_DRAW
	gw.set_position( xpos, ypos );


	gw.set_edge_thickness( 0.1, true );
	gw.set_node_height(5,true);
	gw.set_node_width(5,true);
	gw.set_layout();
	gw.set_animation_steps(3);
	gw.set_edge_label_type(leda::data_label , true);
	gw.display();
	gw.zoom_graph();
 	gw.edit();

#endif SPRING_DRAW*/
}


//1. Main Call 
void SPRING_EMBEDDING_our(graph &G, list<node>& fixed_nodes,
                                      node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      double xleft, double xright,
                                      double ybottom, double ytop,
                                      int iterations )
{

	int lower_bound=static_cast<int>(xleft);
	int upper_bound=static_cast<int>(xright);
	random_source S2(lower_bound,upper_bound);
	int x1; 
	node u,v;
	forall_nodes(v,G) {
			S2 >> x1;
			xpos[v]=x1;
	}
	lower_bound=static_cast<int>(ybottom);
	upper_bound=static_cast<int>(ytop);
	S2.set_range(lower_bound,upper_bound);
	int y; 
	forall_nodes(v,G) {
			S2 >> y;
			ypos[v]=y;
	}
	if (xleft >= xright || ybottom >= ytop)
		cout << "SPRING_EMBDDING: illegal bounds.";

	if (G.number_of_nodes() < 2) 
		return;

	node_array<list_item> lit(G);
	node_array<bool> fixed(G,false);


	edge e;

	forall(v,fixed_nodes) 
		fixed[v] = true;

	int c_f = 1;

	double width  = xright - xleft;
	double height = ytop - ybottom;

	double tx_null = width/50;
	double ty_null = height/50;
	double tx = tx_null;
	double ty = ty_null;

	double k = sqrt(width*height / G.number_of_nodes()) / 2;
	double k2 = 2*k;
	double kk = k*k;

	int ki = int(k);

	if (ki == 0) ki = 1;

	//build  matrix of node lists

	int xA = int(width / ki + 1);
	int yA = int(height / ki + 1);

	array2<list<node> > A(-1,xA,-1,yA);

	forall_nodes(v,G)
	{ 
		int i = int((xpos[v] - xleft)   / ki);
		int j = int((ypos[v] - ybottom) / ki);
		if (i >= xA || i < 0 || j >= yA || j < 0)
			printf("spring: (%f %f) out of range (%f,%f,%f,%f)",xpos[v],ypos[v],xleft,ybottom,xright,ytop);
		lit[v] = A(i,j).push(v);
	}


	while (c_f < iterations)
	{
		node_array<double>  xdisp(G,0);
		node_array<double>  ydisp(G,0);

		// repulsive forces

		forall_nodes(v,G)
		{ 
			int i = int((xpos[v] - xleft)   / ki);
			int j = int((ypos[v] - ybottom) / ki);

			double xv = xpos[v];
			double yv = ypos[v];

			for(int m = -1; m <= 1; m++)
				for(int n = -1; n <= 1; n++)
					forall(u,A(i+m,j+n)){ 
					   if(u == v) continue;
							 double xdist = xv - xpos[u];
							 double ydist = yv - ypos[u];
							 double dist = sqrt(xdist * xdist + ydist * ydist);
							 if (dist < 1e-3) dist = 1e-3;
							 xdisp[v] += FREPULSE(dist) * xdist / dist;
							 ydisp[v] += FREPULSE(dist) * ydist / dist;
					}
			xdisp[v] *=  (double(rand_int(75,125))/1000.0);
			ydisp[v] *=  (double(rand_int(75,125))/1000.0);
		 }



		// attractive forces

		forall_edges(e,G){ 
			node u = G.source(e);
			node v = G.target(e);
			double xdist=xpos[v]-xpos[u];
			double ydist=ypos[v]-ypos[u];
			double dist=sqrt(xdist*xdist+ydist*ydist);

			float f = (G.degree(u)+G.degree(v))/6.0;

			dist /= f;

			xdisp[v]-=xdist*dist/k;
			ydisp[v]-=ydist*dist/k;
			xdisp[u]+=xdist*dist/k;
			ydisp[u]+=ydist*dist/k;
		}


		// preventions

		forall_nodes(v,G){
			if (fixed[v]) continue;

			int i0 = int((xpos[v] - xleft)/ki);
			int j0 = int((ypos[v] - ybottom)/ki);

			double xd= xdisp[v];
			double yd= ydisp[v];
			double dist = sqrt(xd*xd+yd*yd);

			if (dist < 1) 
				dist = 1;

			xd = tx*xd/dist;
			yd = ty*yd/dist;

			double xp = xpos[v] + xd;
			double yp = ypos[v] + yd;

			int i = i0;
			int j = j0;

			if (xp > xleft && xp < xright){ 
				xpos[v] = xp;
				i = int((xp - xleft)/ki);
			}

			if (yp > ybottom && yp < ytop){ 
				ypos[v] = yp;
				j = int((yp - ybottom)/ki);
			}

			if (i != i0 || j != j0){ 
				if (lit[v] == nil) 
					error_handler(1,"delete nil item");
				A(i0,j0).del_item(lit[v]);
				lit[v] = A(i,j).push(v);
			}
		}
		tx = tx_null / log_2(c_f);
		ty = ty_null / log_2(c_f);

		c_f++;
	}
}
