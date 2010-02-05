#include "incl/handlers.h"

int main(int argc, char** argv) {
	if( argc > 1 ){
		list_item it;
		leda::string fname = argv[1];
                node n,n2;
		int count = 0,maxLayerIdNow;
                GRAPH<int,int> G;

                G.read_gml( fname );
                node_array<int> ids( G );
                forall_nodes( n, G ){
                    ids[ n ] = count;
                    count++;
                }
                count = 0;
                GraphWin gw;
                gw.read_gml(fname);
                graph G2 = gw.get_graph();
		double x0, y0, x1, y1;
                gw.get_bounding_box(x0, y0, x1, y1);
                list<int> deleted;
                FILE *fptr;
                if( (fptr = fopen( "outputs/graphs/independent.txt", "w")) == NULL && (fptr = fopen( "outputs//graphs//independent.txt", "w")) == NULL )
                    cout << "FILE could not be opened";
                forall_nodes( n , G ){
                    if( G.degree(n) == 0 ){
                        count = 0;
                        forall_nodes( n2, G2 ){
                            if( count == ids[n] ){
                                fprintf( fptr, "%d ", G[ n ] );                                
                            }
                            count++;
                       }
                    }
                }
                fclose( fptr );
	}
	return 0;
}
