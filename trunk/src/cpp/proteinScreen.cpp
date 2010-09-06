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
            cout << "\n DONE";
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
    return 0;
}
