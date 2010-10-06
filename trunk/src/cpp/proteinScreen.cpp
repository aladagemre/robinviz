#include "incl/handlers.h"
#include "incl/handlers/_dataread.h"
#include "incl/handlers/_bicread.h"
#include "incl/handlers/_ppiread.h"
#include "incl/handlers/_maingraph.h"


int main(int argc, char** argv) {

    if( argc > 1 ){
            char fname[256];
            char queryGeneName[256];
            sprintf( queryGeneName, "%s", argv[1] );
//            GRAPH<int,int> G;

            if( strcmp( argv[2], "GML" ) == 0 ){
#ifdef LINUX
                sprintf( fname, "outputs/graphs/ppigraph.gml" );
#else
                sprintf( fname, "outputs//graphs/ppigraph.gml" );
#endif
                node n,found,m;
                edge e;
                list<node> firstDegree,firstDegreeT;
                int k = 2,i=1;
                list_item it;

    //            G.read_gml( fname );
                GraphWin gw;
                gw.read_gml( fname );
                graph& G = gw.get_graph();
                node_array<bool> shouldDelete( G, false );

                forall_nodes( n, G ){
    //                    cout << gw.get_label( n ) << endl;
                        if( strcmp( queryGeneName, gw.get_label( n ) ) == 0 ){
                                found = n;
                                cout << "\n FOUND";
                                shouldDelete[ found ] = true;
                        }
                }
//                cout << "\n DONE";
                // Finding the first degree neighbors
                forall_adj_edges( e, found ){
                        if( found == G.target( e ) ){
                                firstDegree.append( G.source( e ) );
                                shouldDelete[ G.source( e ) ] = true;
                        }
                        else{
                                firstDegree.append( G.target( e ) );
                                shouldDelete[ G.target( e ) ] = true;
                        }
                }
                while( i != k ){
                        forall_items( it, firstDegree ){
                                m = firstDegree[ it ];
                                forall_adj_edges( e, m ){
                                        if( m == G.target( e ) ){
                                                if( shouldDelete[ G.source( e ) ] == false ){
                                                        firstDegreeT.append( G.source( e ) );
                                                        shouldDelete[ G.source( e ) ] = true;
                                                }
                                        }
                                        else{
                                            if( shouldDelete[ G.target( e ) ] == false ){
                                                firstDegreeT.append( G.target( e ) );
                                                shouldDelete[ G.target( e ) ] = true;
                                            }
                                        }
                                }
                        }
                        firstDegree = firstDegreeT;
                        i++;
                }
                forall_nodes( n, G ){
                        if( shouldDelete[ n ] == false )
                                gw.del_node( n );
                }

#ifdef LINUX
                sprintf( fname, "outputs/graphs/%s.gml", queryGeneName );
#else
                sprintf( fname, "outputs//graphs/%s.gml", queryGeneName );
#endif
                gw.save_gml( fname );
        }
        else{
             if( strcmp( argv[2], "TXT" ) == 0 ){
#ifdef LINUX
                    sprintf( fname, "outputs/graphs/ppigraph.txt" );
#else
                    sprintf( fname, "outputs//graphs/ppigraph.txt" );
#endif
                    FILE *fptr;
                    char protName1[256],protName2[256];
                    int edgeNumber, weight;
                    list<edge> elist;
                    color red( 255, 0, 0 );

                    if( (fptr = fopen( fname, "r" ) ) != NULL ){
                        node n,m,query,center;
                        edge e;
                        list_item it,it2;
                        GRAPH<leda::string,int> G;

                        list<node> added, stricts;
                        while( !feof( fptr ) ){
                            fscanf( fptr, "%s", protName1 );
                            bool found = false;
                            if( strcmp( queryGeneName, protName1 ) == 0 ){
                                query = G.new_node();
//                                cout << protName1 << " :-: ";
                                G[ query ] = protName1;
                                center = query;
                                found = true;
    //                             cout << " FOUND " << protName1 << endl;
                            }
                            fscanf( fptr, "%d", &edgeNumber );
                            int count = 0;
                            list<edge> elist;
                            while( count != edgeNumber ){
                                fscanf( fptr, "%s", protName2 );
                                if( found ){
                                    n = G.new_node();
                                    G[ n ] = protName2;
//                                    cout << protName2 << " - ";
                                    e = G.new_edge(query,n);
                                    elist.push_back( e );
                                    added.append( n );
    //                                cout << " FOUND " << protName2;
                                }
                                count++;
                            }
//                            cout << endl;
                            count = 0;
                            while( count != edgeNumber ){
                                fscanf( fptr, "%d", &weight );
                                if( found ){
                                    e = elist.pop();
                                    G[ e ] = weight;;
                                }
                                count++;
                            }
                            if( found )
                                break;
                        }
                        rewind( fptr );
    //                    G.print();
    //                    cout << endl;
                        while( !feof( fptr ) ){
                            fscanf( fptr, "%s", protName1 );
                            bool found = false;
                            forall_items( it, added ){
                                if( G[ added[ it ] ].contains( protName1 ) == true &&  added[ it ] != center ){
//                                    forall_nodes( m, G ){
//                                        if( G[ m ].contains( protName1 ) == true ){
//                                            query = m;
//                                            break;
//                                        }
//                                    }
                                    query = added[ it ];
                                    found = true;
                                    break;
    //                                cout << " Neighbour " <<  G[ added[ it ] ] << "\t" <<  protName1 << " is found \n";
                                }
                            }
                            fscanf( fptr, "%d", &edgeNumber );
                            int count = 0;
                            if( found && edgeNumber > 10 ){
                                cout << edgeNumber << endl;
                                list<edge> elist;
                                list<int> weights;
                                list<int> indexs;
                                while( count != edgeNumber ){
                                    fscanf( fptr, "%s", protName2 );
                                    if( found ){
                                        bool found2 = false;
                                        forall_nodes( m, G ){
                                            if( G[ m ].contains( protName2 ) == true && m != center ){
                                                n = m;
                                                found2 = true;
                                                break;
                                            }
                                        }
                                        if( found2 == false ){
                                            n = G.new_node();
                                            G[ n ] = protName2;
                                        }
                                        e = G.new_edge( query,n );
                                        elist.push_back( e );
                                        indexs.push_back( count );
                                    }
                                    count++;
                                }
                                count = 0;
                                while( count != edgeNumber ){
                                    fscanf( fptr, "%d", &weight );
                                    if( found ){                                        
                                        weights.append( weight );
                                    }
                                    count++;
                                }
                                if( found ){
                                    count = 0;
//                                    cout << " i: " << indexs.size() << " e " << elist.size() << endl;
                                    while( count != indexs.size() ){
                                         e = elist.pop();
                                         G[ e ] = weights[ indexs.get_item( count ) ];
                                         count++;
                                    }
                                }
                                else{
                                    stricts.append( query );
                                }
                            }
                            else{
                                count = 0;
                                while( count != edgeNumber ){
                                    fscanf( fptr, "%s", protName2 );
                                    count++;
                                }
                                count = 0;
                                while( count != edgeNumber ){
                                    fscanf( fptr, "%d", &weight );
                                    count++;
                                }
                            }
                        }
//                        G.print();
//                        cout << endl;

                        // Placement of Layout in a way two circle fashion

                        GraphWin gw( G );
                        node_array<double> xpos( G );
                        node_array<double> ypos( G );
                        node_array<bool> marked( G, false );
                        xpos[ center ] = 0;
                        ypos[ center ] = 0;
                        marked[ center ] = true;
                        list<node> circle1, circle2;
                        forall_adj_edges( e, center ){
                            if( G.source( e ) == center ){
                                circle1.append( G.target( e ) );
                                marked[ G.target( e ) ] = true;
                            }
                            else{
                                circle1.append( G.source( e ) );
                                marked[ G.source( e ) ] = true;
                            }
                        }
                        forall_nodes( n, G ){
                            if( marked[ n ] == false )
                                circle2.append( n );
                        }
                        double node_width = 50.0;

                        double length = node_width * circle1.size();
                        double pi = 2.0 * 3.147;
                        double tmp = pi;
                        double radius1 = length / pi;
                        if( G.number_of_nodes() < 6 )
                            radius1 *= G.number_of_nodes() / 2;
                        if( G.number_of_nodes() <= 3 )
                            radius1 *= G.number_of_nodes();
                        circle C1( 0, 0, radius1 );
                        double min = pi / (double)circle1.size();
                        forall_items( it, circle1 ){
                            point p = C1.point_on_circle( tmp );
                            xpos[ circle1[ it ] ] = p.xcoord();
                            ypos[ circle1[ it ] ] = p.ycoord();
                            tmp -= min;
                        }

                        node_width = 50.0;
                        length = node_width * circle2.size();
                        double radius2 = length / pi;
                        tmp = pi;
                        if( radius1  >= radius2 )
                            radius2 = radius1 + 200;
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
                                if( G.source( e ) == n )
                                    m = G.target( e );
                                else
                                    m = G.source( e );
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

    #ifdef LINUX
                        sprintf( fname, "outputs/graphs/%s.gml", queryGeneName );
    #else
                        sprintf( fname, "outputs//graphs/%s.gml", queryGeneName );
    #endif
                        forall_nodes( n, G ){
                            gw.set_label( n, G[ n ] );
                        }
                        forall_items( it, stricts ){
                            gw.set_border_color( stricts[ it ], red );
                        }

                        gw.set_position( xpos, ypos );
                        gw.save_gml( fname );
//                        char command[ 512 ];
//                        sprintf( command, "%s %s", "forceDirectedLayoutW.exe", queryGeneName );
//                        system( command );
                    }
                    else{
                        FILE *erptr;
#ifdef LINUX
                        erptr = fopen( "outputs/error.txt", "w" );
#else
                        erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "You must run one of three co-....exe's to form ppigraph.txt \n" );
                        fclose( erptr );
                        cout << "\nYou must run one of three co-....exe's to form ppigraph.txt\n";
                        exit(1);
                    }
            }
       }
    }
    return 0;
}
