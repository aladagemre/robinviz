#include "incl/handlers.h"
#include "incl/handlers/_dataread.h"
#include "incl/handlers/_bicread.h"
#include "incl/handlers/_ppiread.h"
#include "incl/handlers/_maingraph.h"

int main(int argc, char** argv) {

    if( argc > 1 ){
        leda::string fname = argv[1];
        GRAPH<int,int> H,G;

        node n,m,mark;
        edge e;
        list_item it,it2;
        int count = 0;
        G.read_gml( fname );
        edge_array<int> index( G, 0 );
        array<edge> edges( G.number_of_edges() + 1 );
        forall_edges( e, G ){
            index[ e ] = count;
            edges[ count ] = e;
            count++;
        }

        GraphWin gw2;
        gw2.read_gml( fname );
        graph T = gw2.get_graph();
        double x0, y0, x1, y1;
        gw2.get_bounding_box(x0, y0, x1, y1);
        node_array<double> xpos( T );
        node_array<double> ypos( T );
        node_array<bool> marked( T, false );
        edge_array<int> weightsE( T, 0 );
        node_array<int> weightsN( T, 0 );
        count = 0;
        forall_edges( e, T){
            weightsE[ e ] = G[ edges[ count ] ];
            weightsN[ G.source( e ) ] += weightsE[ e ];
            weightsN[ G.target( e ) ] += weightsE[ e ];
            count++;
        }

        double sumOfWeights = 0,sum = 0;
        forall_nodes( n, T ){
            sumOfWeights += weightsN[ n ];
        }

        while( sumOfWeights / 2.0 > sum ){
            double min = sumOfWeights;
            forall_nodes( n, T ){
                if( marked[ n ] == false ){
                    if( min > weightsN[ n ] ){
                        min = weightsN[ n ];
                        mark = n;
                    }
                }
            }
            sum += weightsN[ mark ];
            marked[ mark ] = true;
        }

        list<node> circle1, circle2;
        forall_nodes( n, T ){
            if( marked[ n ] == true ){
                circle1.append( n );
            }
            else{
                circle2.append( n );
            }
        }
        forall_nodes( n, T )
                marked[ n ] = false;

        double node_width = 50.0;
        double length = node_width * circle1.size();
        double pi = 2.0 * 3.147;
        double tmp = pi;
        double radius1 = length / pi;
        circle C1( 0, 0, radius1 );
        double min = pi / (double)circle1.size();
        forall_items( it, circle1 ){
            point p = C1.point_on_circle( tmp );
            xpos[ circle1[ it ] ] = p.xcoord();
            ypos[ circle1[ it ] ] = p.ycoord();
            marked[ circle1[ it ] ] = true;
            tmp -= min;
        }

        length = node_width * circle2.size();
        double radius2 = length / pi;
        tmp = pi;
        if( radius1  >= radius2 + 200 )
            radius2 = radius1 + 300;
        circle C2( 0, 0, radius2 );

        array<double> slotsD( circle2.size() + 1 );
        array<bool> isOccupied( circle2.size() + 1 );
        slotsD[ 0 ] = tmp;
        isOccupied[ 0 ] = false;
        min = pi / (double)circle2.size();
        for( int i = 1; i < circle2.size(); i++ ){
            tmp -= min;
            slotsD[ i ] = tmp;
            isOccupied[ i ] = false;
        }

        forall_items( it, circle2 ){
            n = circle2[ it ];
            list<point> pList;
            forall_adj_edges( e, n ){
                if( T.source( e ) == n )
                    m = G.target( e );
                else
                    m = T.source( e );
                if( marked[ m ] == true ){
                     point p( xpos[ m ], ypos[ m ] );
                     pList.append( p );
                }
            }
            point target;
            double minD;
            int indexToPlace = 0;
            int firstTime = 0;
            for( int i = 0; i < circle2.size(); i++ ){
                if( isOccupied[ i ] == false ){
                    target = C2.point_on_circle( slotsD[ i ] );
                    double sumD = 0;
                    forall_items( it2, pList ){
                        sumD += target.distance( pList[ it2 ] );
                    }
                    if( firstTime == 0 ){
                        minD = sumD;
                        indexToPlace = i;

//                                    cout << minD << " - ";
                    }
                    else{
                        if( minD > sumD ){
                            indexToPlace = i;
                            minD = sumD;

//                                        cout << minD << " - ";
                        }
                    }
                    firstTime++;
                }
            }
            isOccupied[ indexToPlace ] = true;
//                        cout << indexToPlace << endl;
            target = C2.point_on_circle( slotsD[ indexToPlace ] );
            xpos[ n ] = target.xcoord();
            ypos[ n ] = target.ycoord();
            marked[ n ] = true;
//                        cout << endl << xpos[ n ] << " - " << ypos[ n ] << endl;
        }

        gw2.set_layout( xpos, ypos );
        gw2.place_into_box(x0, y0, x1, y1);
        fname = fname.replace( ".gml", "circularDualRingOutside.gml" );
        gw2.save_gml( fname );
    }
    return 0;
}
