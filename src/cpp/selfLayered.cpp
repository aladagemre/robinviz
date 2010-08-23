#include "incl/handlers.h"
#include "incl/handlers/_dataread.h"
#include "incl/handlers/_bicread.h"
#include "incl/handlers/_ppiread.h"
#include "incl/handlers/_maingraph.h"


int main(int argc, char** argv) {

    if( argc > 1 ){
        leda::string fname = argv[1];
        GRAPH<int,int> H,G;

        node n;
        edge e;
        G.read_gml( fname );
        GraphWin gw(G);
        double x0, y0, x1, y1;
        gw.get_bounding_box(x0, y0, x1, y1);
    
        node_array<int> comp( G, 0 );
        if( G.number_of_nodes() > 10 && G.number_of_edges() > 0 ){
            node_array<double> Ypos( G );
            node_array<double> Xpos( G );
            node_array<point> pos( G );
            edge_array<list<point> > bends( G );
            array<list<node> > layers( 2 );
            
            H = RUN_SELFGD( G, layers, 40, Xpos, Ypos, 1, pos, bends, 1, 50, 0, 50, 0 );

            gw.set_layout( pos, bends );
            forall_edges( e, G ){
                    gw.set_thickness( e, G[ e ] );
            }
            gw.place_into_box(x0, y0, x1, y1);
            fname = fname.replace( ".gml", "layered.gml" );
            gw.save_gml( fname );
        }
    }
    return 0;
}
