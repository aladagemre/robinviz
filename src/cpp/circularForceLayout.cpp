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

        GraphWin gw2;
        gw2.read_gml( fname );
        graph T = gw2.get_graph();
        double x0, y0, x1, y1;
        gw2.get_bounding_box(x0, y0, x1, y1);

        node_array<int> comp( G, 0 );
        node_array<double> Ypos( G );
        node_array<double> Xpos( G );
        node_array<point> pos( G );
        edge_array<list<point> > bends( G );
        array<list<node> > layers( 2 );

        cout << " MAIN0\n";
        int lower_bound=static_cast<int>(gw2.get_xmin()+20.0);
        int upper_bound=static_cast<int>(gw2.get_xmax()-20.0);
        random_source S2(lower_bound,upper_bound);
        int x1_;
        node v;
        forall_nodes(n,G) {
            S2 >> x1_;
            Xpos[n]=x1_;
        }
        lower_bound=static_cast<int>(gw2.get_ymin()+20.0);
        upper_bound=static_cast<int>(gw2.get_ymax()-20.0);
        S2.set_range(lower_bound,upper_bound);
        int y1_;
        forall_nodes(n,G) {
            S2 >> y1_;
            Ypos[n]=y1_;
        }

//        GRAPH<int,int> RUN_FFDANDCIRCLE(  GRAPH<int,int> &G,
//                                  node_array<double> &Xpos,
//                                  node_array<double> &Ypos,
//                                  node_array<point> &posx,
//                                  edge_array<list<point> > &bendsx,
//                                  GraphWin &gw,
//                                  int node_width,
//                                  int minRadius
//        )
//        cout << " MAIN1\n";
        H = RUN_FFDANDCIRCLE( G, Xpos, Ypos, pos, bends, gw2, 100, 40.0 );

        gw.set_layout( pos, bends );
        forall_edges( e, G ){
                gw.set_thickness( e, G[ e ] );
        }
        gw.place_into_box(x0, y0, x1, y1);
        fname = fname.replace( ".gml", "circularForceLayout.gml" );
        gw.save_gml( fname );
    }
    return 0;
}
