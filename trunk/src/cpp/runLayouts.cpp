#include "incl/handlers.h"

class layoutHandler{
    public:
        node n,m;
        edge e;
        int argc;
        char** argv;
        GRAPH<int,int> H,G;
        leda::string fname;
        layoutHandler( int _argc, char** _argv );
        void runSugiyamaStyle( void );
        void rundoubleCircularTracksInside( void );
        void rundoubleCircularTracksOutside( void );
        void runCircularTracks( void );
        void runSpringEmbedder( void );
        void runStarLayout( void );
        void runSpringEmbedderonCircularTracks( void );
};

layoutHandler::layoutHandler( int _argc, char** _argv ){
    char command[256];
    argc = _argc;
    argv = _argv;
    fname = argv[1];
    sprintf( command, "%s", argv[2] );
    if( argc > 2 ){
        if( strcmp( argv[2], "-se" ) == 0 ){
            runSpringEmbedder();
        }
        else{
            if( strcmp( argv[2], "-ci" ) == 0 ){
                rundoubleCircularTracksInside();
            }
            else{
                if( strcmp( argv[2], "-co" ) == 0 ){
                    rundoubleCircularTracksOutside();
                }
                else{
                    if( strcmp( argv[2], "-ct" ) == 0 ){
                        runCircularTracks();
                    }
                    else{
                        if( strcmp( argv[2], "-sl" ) == 0 ){
                            runStarLayout();
                        }
                        else{
                            if( strcmp( argv[2], "-ss" ) == 0 ){
                                runSugiyamaStyle();
                            }
                            else{
                                if( strcmp( argv[2], "-sec" ) == 0 ){
                                    runSpringEmbedderonCircularTracks();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void layoutHandler::runSugiyamaStyle( void ){
    if( argc > 1 ){
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

            H = RUN_SELFGD( G, layers, 10, Xpos, Ypos, 1, pos, bends, 1, 400, true, 300, 0 );

            gw.set_layout( pos, bends );
            forall_edges( e, G ){
                    gw.set_thickness( e, G[ e ] );
            }
            gw.place_into_box(x0-600.0, y0, x1+550.0, y1);
            fname = fname.replace( ".gml", "ss.gml" );
            gw.save_gml( fname );
    }
}

void layoutHandler::rundoubleCircularTracksInside( void ){
    if( argc > 1 ){
        node mark;
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
                circle2.append( n );
            }
            else{
                circle1.append( n );
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
        fname = fname.replace( ".gml", "ci.gml" );
        gw2.save_gml( fname );
    }
}

void layoutHandler::rundoubleCircularTracksOutside( void ){
    if( argc > 1 ){
        node mark;
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
        fname = fname.replace( ".gml", "co.gml" );
        gw2.save_gml( fname );
    }
}

void layoutHandler::runStarLayout( void ){
    if( argc > 1 ){
        GraphWin gw;
        double pi = 2.0 * 3.147;
        double tmp = pi;
        int count = 0;
        gw.read_gml( fname );
        graph G = gw.get_graph();
        list<node> justOnes;
        node_array<point> pos( G );
        double x0, y0, x1, y1;
        list_item it;
        gw.get_bounding_box(x0, y0, x1, y1);
        node_array<double> xpos( G );
        node_array<double> ypos( G );
        node_array<int> neighbors( G );
        gw.get_position( pos );

        node_array<int> comp( G, 0 );
        COMPONENTS( G, comp );
        int max = 0;
        forall_nodes( n, G ){
            if( max < comp[ n ] )
                max = comp[ n ];
        }
        array<list<node> > COMPS( max + 1 );
        count = 1;
        forall_nodes( n, G ){
            COMPS[ comp[ n ] ].append( n );
        }
        list<int> notEnoughComponents;
        int maxComp = 0;
        for(int i = 0; i <= max; i++ ){
            if( COMPS[ i ].size() > maxComp ){
                maxComp = COMPS[ i ].size();
            }
        }

        int election = 10;
        if( maxComp < election )
            election = maxComp;
        for(int i = 0; i <= max; i++ ){
            if( COMPS[ i ].size() > 1 && COMPS[ i ].size() < election ){
                notEnoughComponents.append( i );
                forall_items( it, COMPS[ i ] ){
                    G.hide_node( COMPS[ i ][ it ] );
                }
            }
        }

        int max_degree = 0;
        node maxDegNode;
        forall_nodes( n, G ){
                neighbors[ n ] = G.degree( n );
                if( max_degree < neighbors[ n ] ){
                        max_degree = neighbors[ n ];
                        maxDegNode = n;
                }
        }

        circle C( 0, 0, G.number_of_nodes() * 30.0 );
        forall_nodes( n, G ){
              count++;
        }
        count = count - 1;
        double min = pi / (double)count;
        forall_nodes( n, G ){
                if( n != maxDegNode ){
                        pos[ n ] = C.point_on_circle( tmp );
                        tmp -= min;
                }
                else{
                        point P( 0, 0 );
                        pos[ n ] = P;
                }
                xpos[ n ] = pos[ n ].xcoord();
                ypos[ n ] = pos[ n ].ycoord();
        }

        //cout << " 1 \n";
        double xmin, xmax, ymin, ymax;
        int ncount = 0;
        forall_nodes( n, G ){
                if( COMPS[ comp[ n ] ].size() > election ){
                        if( ncount == 0 ){
                                xmin = xpos[ n ];
                                xmax = xpos[ n ];
                                ymin = ypos[ n ];
                                ymax = ypos[ n ];
                        }
                        else{
                            if( xpos[ n ] < xmin )
                                  xmin = xpos[ n ];
                            if( xpos[ n ] > xmax )
                                  xmax = xpos[ n ];
                            if( ypos[ n ] < ymin )
                                  ymin = ypos[ n ];
                            if( ypos[ n ] > ymax )
                                  ymax = ypos[ n ];
                        }
                        ncount++;
                }
        }
        G.restore_all_nodes();
        G.restore_all_edges();
        //cout << " 2 \n";
        double xpos1 = xmin;
        double ypos1 = ymax + 6 * G.number_of_nodes();
        //cout << max << " - " << election << endl;
        int maxC = 0;
        for(int i = 0; i <= max; i++ ){
                //cout << COMPS[ i ].size() << "\t";
                if(  COMPS[ i ].size() > 1 && COMPS[ i ].size() < election ){
                        if( COMPS[ i ].size() > maxC )
                        maxC = COMPS[ i ].size();
                        if( xpos1 < xmax ){
                            int diam = 30;
                            if( COMPS[ i ].size() < 3 ){
                                diam = 150;
                            }
                            else{
                                diam = 150 + COMPS[ i ].size() * 20;
                            }
                            circle C( xpos1 + 75 + COMPS[ i ].size() * 10, ypos1, diam );
                            min = pi / (double)COMPS[ i ].size();
                            tmp = pi;
                            forall_items( it, COMPS[ i ] ){
                                pos[ COMPS[ i ][ it ]  ] = C.point_on_circle( tmp );
                                //cout << pos[ COMPS[ i ][ it ]  ] << "\t";
                                xpos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].xcoord();
                                ypos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].ycoord();
                                tmp -= min;
                            }
                        }
                        else{
                                xpos1 = xmin;
                                ypos1 += 300 + maxC * 50;
                                maxC = 0;
                                int diam = 30;
                                if( COMPS[ i ].size() < 3 ){
                                    diam = 150;
                                }
                                else{
                                    diam = 150 + COMPS[ i ].size() * 10;
                                }
                                circle C( xpos1 + 75 + COMPS[ i ].size() * 10, ypos1, diam );
                                min = pi / (double)COMPS[ i ].size();
                                tmp = pi;
                                forall_items( it, COMPS[ i ] ){
                                    pos[ COMPS[ i ][ it ]  ] = C.point_on_circle( tmp );
                                    //cout << pos[ COMPS[ i ][ it ]  ] << "\t";
                                    xpos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].xcoord();
                                    ypos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].ycoord();
                                    tmp -= min;
                                }
                        }
                        if( COMPS[ i ].size() == 1 )
                            xpos1 += 150;
                        else
                            xpos1 += 300 + G.number_of_nodes() * 3;
                }
        }

        forall_nodes( n, G ){
            if( G.degree( n ) == 0 ){
                justOnes.append( n );
            }
        }

        gw.remove_bends();
        gw.set_position( xpos, ypos );
        gw.place_into_box(x0, y0, x1, y1);
        fname = fname.replace( ".gml", "sl.gml" );
        gw.save_gml( fname );
    }
}

void layoutHandler::runSpringEmbedder( void ){
    if( argc > 1 ){
            GraphWin gw;
            graph G;
            list_item it;
            // Reading graph
            char flag1[16];
//                sprintf( flag1, "%s", argv[2] );
            sprintf( flag1, "OUR" );
            double pi = 2.0 * 3.147;
            double tmp = pi;
            int count = 0,maxLayerIdNow;
//                if( strcmp( flag1, "LEDA" ) == 0 ){
                gw.read_gml( fname );
                G = gw.get_graph();
//                }
            double x0, y0, x1, y1;
            // Make sure that bounding box is taken
            gw.get_bounding_box(x0, y0, x1, y1);
            node_array<point> pos( G );
            node_array<double> xpos( G );
            node_array<double> ypos( G );
            node_array<int> neighbors( G );
            node_array<int> indexs( G );
            array<node> nodes( G.number_of_nodes() + 1 );
            node n,adj,source;
            gw.get_position( pos );
            list<node> fixedNodes;
            int max_degree = 0, cnt = 0;
            node maxDegNode;
            forall_nodes( n, G ){
                    neighbors[ n ] = G.degree( n );
                    if( max_degree < neighbors[ n ] ){
                            max_degree = neighbors[ n ];
                            maxDegNode = n;
                    }
                    indexs[ n ] = cnt;
                    nodes[ cnt ] = n;
                    cnt++;
//                        cout << "|";
            }
//                cout << endl << "D\n";
            xpos[ maxDegNode ] = gw.get_xmin() + ( abs( gw.get_xmin() ) + abs( gw.get_xmax() ) ) / 2.0;
            ypos[ maxDegNode ] = gw.get_ymin() + ( abs( gw.get_ymin() ) + abs( gw.get_ymax() ) ) / 2.0;
            fixedNodes.append( maxDegNode );
            list<node> N = G.adj_nodes( maxDegNode );
            circle C( 0, 0, abs( abs( gw.get_ymax() ) - gw.get_ymin() ) / 3.0 );
            double min = pi / (double)count;
            forall( n , N ){
                  pos[ n ] = C.point_on_circle( tmp );
                  xpos[ n ] = pos[ n ].xcoord();
                  ypos[ n ] = pos[ n ].ycoord();
//                      fixedNodes.append( n );
                  tmp -= min;
            }

            int lower_bound=static_cast<int>(gw.get_xmin()+20.0);
            int upper_bound=static_cast<int>(gw.get_xmax()-20.0);
            random_source S2(lower_bound,upper_bound);
            int x1_;
            node v;
            forall_nodes(v,G) {
                S2 >> x1_;
                xpos[v]=x1_;
            }
            lower_bound=static_cast<int>(gw.get_ymin()+20.0);
            upper_bound=static_cast<int>(gw.get_ymax()-20.0);
            S2.set_range(lower_bound,upper_bound);
            int y1_;
            forall_nodes(v,G) {
                S2 >> y1_;
                ypos[v]=y1_;
            }

// 		cout << gw.get_xmin() << "\t" << gw.get_xmax() << "\t" << gw.get_ymin() << "\t" << gw.get_ymax() << endl;
            //SPRING_EMBEDDING( G, fixedNodes, xpos, ypos, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 500 );

            // FINDING COMPONENTS using LEDA function
            node_array<int> comp( G, 0 );
            COMPONENTS( G, comp );
            int max = 0;
            forall_nodes( n, G ){
                if( max < comp[ n ] )
                    max = comp[ n ];
            }
            array<list<node> > COMPS( max + 1 );
            count = 1;
            forall_nodes( n, G ){
                COMPS[ comp[ n ] ].append( n );
            }
            list<int> notEnoughComponents;
            int maxComp = 0;
            for(int i = 0; i <= max; i++ ){
                if( COMPS[ i ].size() > maxComp ){
                    maxComp = COMPS[ i ].size();
                }
            }

            int election = 10;
            if( maxComp < election )
                election = maxComp;

            // Smaller component should be hided before spring algorithm in order
            // to obtain good visuals.
            list<int> hidedIndexs;
            for(int i = 0; i <= max; i++ ){
                if( COMPS[ i ].size() < election ){
                    notEnoughComponents.append( i );
                    forall_items( it, COMPS[ i ] ){
                        hidedIndexs.append( indexs[ COMPS[ i ][ it ] ] );
                        G.hide_node( COMPS[ i ][ it ] );
                    }
                }
            }
            if( strcmp( flag1, "LEDA" ) == 0 ){
                ;//SPRING_EMBEDDING( G, fixedNodes, xpos, ypos, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 1000 );
            }
            else{
                if( strcmp( flag1, "OUR" ) == 0 ){
                    GRAPH<int,int> H;
                    H.read_gml( fname );
                    node_array<double> xpos2( H );
                    node_array<double> ypos2( H );
                    node_array<int> indexs2( H );
                    node_array<int> nodeId( H, 3 );
                    int cnt = 0;
                    forall_nodes( n, H ){
                        indexs2[ n ] = cnt;
                        cnt++;
                    }

                    forall_nodes( n, H ){
                        bool isHided = false;
                        forall_items( it, hidedIndexs ){
                            if( indexs2[ n ] == hidedIndexs[ it ] ){
                                isHided = true;
                            }
                        }
                        if( isHided == true )
                            H.hide_node( n );
                    }
                    list<node> fixedNodes2;
                    SPRING_EMBEDDING_our2( H, fixedNodes2, xpos2, ypos2, gw.get_xmin(), gw.get_xmax(), gw.get_ymin(), gw.get_ymax(), 1000, nodeId );
                    forall_nodes( n, H ){
                        xpos[ nodes[ indexs2[ n ] ] ] = xpos2[ n ];
                        ypos[ nodes[ indexs2[ n ] ] ] = ypos2[ n ];
                    }
                }
            }
            double averageDistance = 0;
            forall_edges( e, G ){
                point p( xpos[ G.source( e ) ], ypos[ G.source( e ) ] );
                point q( xpos[ G.target( e ) ], ypos[ G.target( e ) ] );
                averageDistance += p.distance( q );
            }
            averageDistance = averageDistance / (double)(G.number_of_edges());
            G.restore_all_nodes();
            G.restore_all_edges();

            list<node> justOnes;
            // and handle self nodes
            forall_nodes( n, G ){
                if( G.degree( n ) == 0 ){
                    justOnes.append( n );
                    G.hide_node( n );
                }
            }
            //cout << " 1 \n";
            // Discover the new layout, find limits
            double xmin, xmax, ymin, ymax;
            int ncount = 0;
            forall_nodes( n, G ){
                    if( COMPS[ comp[ n ] ].size() >= election ){
                            if( ncount == 0 ){
                                    xmin = xpos[ n ];
                                    xmax = xpos[ n ];
                                    ymin = ypos[ n ];
                                    ymax = ypos[ n ];
                            }
                            else{
                                if( xpos[ n ] < xmin )
                                      xmin = xpos[ n ];
                                if( xpos[ n ] > xmax )
                                      xmax = xpos[ n ];
                                if( ypos[ n ] < ymin )
                                      ymin = ypos[ n ];
                                if( ypos[ n ] > ymax )
                                      ymax = ypos[ n ];
                            }
                            ncount++;
                    }
            }
            //cout << " 2 \n";

            // Handle smaller components that have not been placed yet.
            double xpos1 = xmin;
            double ypos1 = ymax + 20.0;
            int maxDiam = 0;
            //cout << max << " - " << election << endl;
            for(int i = 0; i <= max; i++ ){
                    //cout << COMPS[ i ].size() << "\t";
                    int diam = 30;
                    if( COMPS[ i ].size() < election && COMPS[ i ].size() > 1 ){
                            if( xpos1 < xmax ){
                                if( COMPS[ i ].size() < 3 ){
                                    diam = averageDistance / 3.0;
                                }
                                else{
                                    diam = (COMPS[ i ].size() / averageDistance ) * 10.0 + averageDistance / 3.0;
                                }
                                if( maxDiam < diam )
                                    maxDiam = diam;
                                circle C( xpos1 + diam, ypos1, diam );
                                min = pi / (double)COMPS[ i ].size();
                                tmp = pi;
                                forall_items( it, COMPS[ i ] ){
                                    pos[ COMPS[ i ][ it ]  ] = C.point_on_circle( tmp );

                                    //cout << pos[ COMPS[ i ][ it ]  ] << "\t";
                                    xpos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].xcoord();
                                    ypos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].ycoord();
                                    tmp -= min;
                                }
                            }
                            else{
                                    xpos1 = xmin;
                                    ypos1 += maxDiam + 10.0;
                                    maxDiam = 0;
                                    int diam = 30;
                                    if( COMPS[ i ].size() < 3 ){
                                        diam = averageDistance / 3.0;
                                    }
                                    else{
                                        diam = (COMPS[ i ].size() / averageDistance ) * 10.0 + averageDistance / 3.0;
                                    }
                                    if( maxDiam < diam )
                                        maxDiam = diam;
                                    circle C( xpos1 + diam, ypos1, diam );
                                    min = pi / (double)COMPS[ i ].size();
                                    tmp = pi;
                                    forall_items( it, COMPS[ i ] ){
                                        pos[ COMPS[ i ][ it ]  ] = C.point_on_circle( tmp );
                                        //cout << pos[ COMPS[ i ][ it ]  ] << "\t";
                                        xpos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].xcoord();
                                        ypos[ COMPS[ i ][ it ] ] = pos[ COMPS[ i ][ it ] ].ycoord();
                                        tmp -= min;
                                    }
                            }
                            xpos1 += diam * 2 + 5.0;
                    }
            }
            G.restore_all_nodes();
            G.restore_all_edges();

            xpos1 = xmin;
//            cout << endl << xmin << " | " << xmax << endl;
//                if( xmax - xmin < 400 )
//                    xmax = xmin + 400;
            ypos1 = ymin - 50.0;
            if( G.number_of_edges() < 5 )
                averageDistance = averageDistance / 5;
            else
                averageDistance = averageDistance / 3;

            forall_items( it, justOnes ){
                if( xpos1 < xmax ){
                    xpos[ justOnes[ it ] ] = xpos1;
                    ypos[ justOnes[ it ] ] = ypos1;
                }
                else{
                    xpos1 = xmin;
                    if( fname.contains( "maingraph" ) == true )
                        ypos1 -= 30.0;
                    else
                        ypos1 -= averageDistance;
                    xpos[ justOnes[ it ] ] = xpos1;
                    ypos[ justOnes[ it ] ] = ypos1;
                }
                if( fname.contains( "maingraph" ) == true )
                    xpos1 += 22;
                else
                    xpos1 += averageDistance;
            }

            //cout << "\n 3 \n";
            gw.remove_bends();
            forall_nodes( n, G ){
                    xpos[ n ] = xpos[ n ] * 5.0;
                    ypos[ n ] = ypos[ n ] * 5.0;
            }
            gw.set_position( xpos, ypos );
            gw.place_into_box(x0, y0, x1, y1);
            // save the file as a rule .cpp name and .gml since then renamed.
            fname = fname.replace( ".gml", "se.gml" );
            gw.save_gml( fname );
    }
}

void layoutHandler::runSpringEmbedderonCircularTracks( void ){
    if( argc > 1 ){
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
        fname = fname.replace( ".gml", "sec.gml" );
        gw.save_gml( fname );
    }
}

void layoutHandler::runCircularTracks( void ){
    if( argc > 1 ){
            GraphWin gw;
            double pi = 2.0 * 3.147;
            double tmp = pi;
            int count = 0;
            gw.read_gml( fname );
            graph G = gw.get_graph();
            double x0, y0, x1, y1;
            gw.get_bounding_box(x0, y0, x1, y1);
            node_array<point> pos( G );
            node_array<double> xpos( G );
            node_array<double> ypos( G );
            gw.get_position( pos );
            circle C( 0, 0, G.number_of_nodes() * 30.0 );
            forall_nodes( n, G ){
                  count++;
            }
            double min = pi / (double)count;
            forall_nodes( n, G ){
                  pos[ n ] = C.point_on_circle( tmp );
                  tmp -= min;
            }
            forall_nodes( n, G ){
                    xpos[ n ] = pos[ n ].xcoord();
                    ypos[ n ] = pos[ n ].ycoord();
            }
            gw.remove_bends();
            gw.set_position( xpos, ypos );
            gw.place_into_box(x0, y0, x1, y1);
            fname = fname.replace( ".gml", "ct.gml" );
            gw.save_gml( fname );
    }
}

int main( int argc, char** argv ){
    layoutHandler handler(argc,argv);
    return 0;
}
