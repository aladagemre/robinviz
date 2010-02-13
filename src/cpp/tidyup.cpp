#include "incl/handlers.h"

int main(int argc, char** argv) {
	if( argc > 1 ){
                double pi = 2.0 * 3.147;
                double tmp = pi;
		list_item it;
		leda::string fname = argv[1];
                node n;
                int count = 0,maxLayerIdNow;
                GraphWin gw;
                gw.read_gml(fname);
                graph G = gw.get_graph();
                node_array<double> xpos( G );
                node_array<double> ypos( G );
                node_array<point> pos( G );
		double x0, y0, x1, y1;
                gw.get_bounding_box(x0, y0, x1, y1);
                gw.get_position( pos );
                forall_nodes( n , G ){
                    xpos[ n ] = pos[ n ].xcoord();
                    ypos[ n ] = pos[ n ].ycoord();
                }

                list<int> deleted;
                FILE *fptr;
                if( (fptr = fopen( "outputs/graphs/independent.txt", "w")) == NULL && (fptr = fopen( "outputs//graphs//independent.txt", "w")) == NULL )
                    cout << "FILE could not be opened";

                list<double> x_l,y_l;
                list<node> others,layouts;
                int otherCount = 0;
                forall_nodes( n , G ){
                    if( G.degree(n) == 0 ){
                        //gw.del_node( n );
                        others.append( n );
                        //cout << "deleted\n";
                        otherCount++;
                    }
                    else{
                        x_l.append( xpos[ n ] );
                        y_l.append( ypos[ n ] );
                        layouts.append( n );
                    }
                }
                xpos.init( G );
                ypos.init( G );
                count = 0;
                double xmax = x_l[ x_l.get_item( count ) ],xmin = x_l[ x_l.get_item( count ) ];
                double ymax = y_l[ y_l.get_item( count ) ],ymin = y_l[ y_l.get_item( count ) ];
                forall( n , layouts ){
                       xpos[ n ] = x_l[ x_l.get_item( count ) ];
                       ypos[ n ] = y_l[ y_l.get_item( count ) ];
                       if( xmax < xpos[ n ] )
                           xmax = xpos[ n ];
                       if( xmin > xpos[ n ] )
                           xmin = xpos[ n ];
                       if( ymax < ypos[ n ] )
                           ymax = ypos[ n ];
                       if( ymin > ypos[ n ] )
                           ymin = ypos[ n ];
                       count++;
                }
                double centerx = xmin;
                double centery = ymin - 300.0;

                forall( n, others ){
                        xpos[ n ] = centerx;
                        ypos[ n ] = centery;
                        if( centerx > xmax ){
                            centery -= 200.0;
                            centerx = xmin;
                        }
                        centerx += 200.0;
                }

                gw.set_position( xpos, ypos );
                gw.place_into_box(x0, y0, x1, y1);
                fname = fname.replace( ".gml", "tidyup.gml" );
                gw.save_gml( fname );
                fclose( fptr );
	}
	return 0;
}
