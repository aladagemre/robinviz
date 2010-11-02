
void mainGraph( GRAPH<leda::string,int> &PROJECT,
		list<GRAPH<int,int> > &GraphList,
		list<GRAPH<leda::string,int> > &GraphList_S, 
		list<list<Strings> > &namesForEachGraph,
		array<GENENAMES> &GenesNode,
		list<double> &H_values,
		node_array<double> &Xpos,
		node_array<double> &Ypos,		
		node_array<point> &pos,
		edge_array<list<point> > &bends,
		int algorithmFlag,
		bool brandFlag,
		bool brandFlag2,
		bool ourMethodFlag,
		double space,
		int increment,
		bool ledaPostFlag,
		double nodeSize,
		double edgeBendImp,
		double colorScale,
		double edgThicknessTher,
		GRAPH<leda::string,int> &G1,
		GRAPH<leda::string,int> &G2,
		GRAPH<int,int> &TEMPINT,
		GRAPH<int,int> &G,
		int simplify,
		double removeRat,
		bool sharedGenes,
		double Hmax,
		bool edgesBetween,
		list<node_array<point> > &POS,
		list<edge_array<list<point> > > &BENDS,
		list<array<list<node> > > &LAYERS,
		array<char> &abbv,
		int cat_num,
		array<char> &Categories,
		int width,
		bool hvalueWeighting,
		bool enrichmentWeighting_o,
		bool enrichmentWeighting_f,
		bool ppihitratioWeighting,
		list<int> &categ)
{
		int max_weight=10;
		bool autoFlag = true;
		node_array<double> HValues(PROJECT, 0 );
		edge_array<int> edgeNumbersForStr( PROJECT );
		node_array<int> nodesOfProjectStr( PROJECT );
		array<node> nodesOfProjectInt( PROJECT.number_of_nodes() );
		array<edge> edgeNumbersForInt;
		edge e;
		list_item it,it2;
		GRAPH<int,int> PROJECT2 = mainGraphHandling(  PROJECT,
							      nodesOfProjectStr,
							      nodesOfProjectInt,
							      GraphList,
							      GraphList_S,
							      namesForEachGraph,
							      GenesNode,
							      HValues,
							      H_values,
							      abbv,
							      cat_num,
							      Xpos,
							      Ypos,
							      pos,
							      bends,
							      edgeNumbersForStr,
							      edgeNumbersForInt,
							      algorithmFlag,
							      brandFlag,
							      brandFlag2,
							      ourMethodFlag,
							      space,
							      increment,
							      ledaPostFlag,
							      nodeSize,
							      edgeBendImp,
							      colorScale,
							      edgThicknessTher,
							      G1,
							      G2,
							      TEMPINT,
							      G,
							      simplify,
							      removeRat,
							      sharedGenes,
							      Hmax,
							      max_weight,
							      edgesBetween,
							      hvalueWeighting,
							      enrichmentWeighting_o,
							      enrichmentWeighting_f,
							      ppihitratioWeighting,
							      categ,
                                                              width ); 
	cout << "/**************************************************/" << endl;
	cout << "\t" << " End of Building High Level Graph " << endl;
	cout << "/**************************************************/" << endl;
	

	int count = 1;


	cout << "/**************************************************/" << endl;
	cout << "\t" << " Forming High Level Layout " << endl;
	cout << "/**************************************************/" << endl;

	count = 1;
      	char filename[ 64 ];
	if( autoFlag == true ){
		for( count = 0; count < GraphList.size(); count++ ){

			sprintf( filename, "outputs/graphs/graph%d.gml", count );

			array<list<node> > layers_ = LAYERS[ LAYERS.get_item( count )];
			node_array<point> pos_ = POS[ POS.get_item( count )];
			edge_array<list<point> > bends_ = BENDS[ BENDS.get_item( count )];

			G = GraphList[ GraphList.get_item( count ) ];
			if( brandFlag == true || ourMethodFlag == true || brandFlag2 == true ){
				RUN_AGAIN2( G, width, Xpos, Ypos, filename, count, namesForEachGraph, pos_, bends_, abbv, cat_num,  Categories, edgeBendImp, colorScale, edgThicknessTher  ); 
			}
		}
	}
}

void mainGraph( GRAPH<leda::string,int> &PROJECT,
                list<GRAPH<int,int> > &GraphList,
                list<GRAPH<leda::string,int> > &GraphList_S,
                list<list<Strings> > &namesForEachGraph,
                array<GENENAMES> &GenesNode,
                list<double> &H_values,
                node_array<double> &Xpos,
                node_array<double> &Ypos,
                node_array<point> &pos,
                edge_array<list<point> > &bends,
                int algorithmFlag,
                bool brandFlag,
                bool brandFlag2,
                bool ourMethodFlag,
                double space,
                int increment,
                bool ledaPostFlag,
                double nodeSize,
                double edgeBendImp,
                double colorScale,
                double edgThicknessTher,
                GRAPH<leda::string,int> &G1,
                GRAPH<leda::string,int> &G2,
                GRAPH<int,int> &TEMPINT,
                GRAPH<int,int> &G,
                int simplify,
                double removeRat,
                bool sharedGenes,
                double Hmax,
                bool edgesBetween,
                list<node_array<point> > &POS,
                list<edge_array<list<point> > > &BENDS,
                list<array<list<node> > > &LAYERS,
                array<char> &abbv,
                int cat_num,
                array<list<char> > &CategoriesXL,
                int width,
                bool hvalueWeighting,
                bool enrichmentWeighting_o,
                bool enrichmentWeighting_f,
                bool ppihitratioWeighting,
                list<int> &categ,
                bool hasColor )
{
                int max_weight=10;
                bool autoFlag = true;
                node_array<double> HValues(PROJECT, 0 );
                edge_array<int> edgeNumbersForStr( PROJECT );
                node_array<int> nodesOfProjectStr( PROJECT );
                array<node> nodesOfProjectInt( PROJECT.number_of_nodes() );
                array<edge> edgeNumbersForInt;
                edge e;
                list_item it,it2;
                GRAPH<int,int> PROJECT2 = mainGraphHandling(  PROJECT,
                                                              nodesOfProjectStr,
                                                              nodesOfProjectInt,
                                                              GraphList,
                                                              GraphList_S,
                                                              namesForEachGraph,
                                                              GenesNode,
                                                              HValues,
                                                              H_values,
                                                              abbv,
                                                              cat_num,
                                                              Xpos,
                                                              Ypos,
                                                              pos,
                                                              bends,
                                                              edgeNumbersForStr,
                                                              edgeNumbersForInt,
                                                              algorithmFlag,
                                                              brandFlag,
                                                              brandFlag2,
                                                              ourMethodFlag,
                                                              space,
                                                              increment,
                                                              ledaPostFlag,
                                                              nodeSize,
                                                              edgeBendImp,
                                                              colorScale,
                                                              edgThicknessTher,
                                                              G1,
                                                              G2,
                                                              TEMPINT,
                                                              G,
                                                              simplify,
                                                              removeRat,
                                                              sharedGenes,
                                                              Hmax,
                                                              max_weight,
                                                              edgesBetween,
                                                              hvalueWeighting,
                                                              enrichmentWeighting_o,
                                                              enrichmentWeighting_f,
                                                              ppihitratioWeighting,
                                                              categ,
                                                              width );
        cout << "/**************************************************/" << endl;
        cout << "\t" << " End of Building High Level Graph " << endl;
        cout << "/**************************************************/" << endl;


        int count = 1;


        cout << "/**************************************************/" << endl;
        cout << "\t" << " Forming High Level Layout " << endl;
        cout << "/**************************************************/" << endl;

        count = 1;
        char filename[ 64 ];
        if( autoFlag == true ){
                for( count = 0; count < GraphList.size(); count++ ){

                        sprintf( filename, "outputs/graphs/graph%d.gml", count );

                        array<list<node> > layers_ = LAYERS[ LAYERS.get_item( count )];
                        node_array<point> pos_ = POS[ POS.get_item( count )];
                        edge_array<list<point> > bends_ = BENDS[ BENDS.get_item( count )];

                        G = GraphList[ GraphList.get_item( count ) ];
                        if( brandFlag == true || ourMethodFlag == true || brandFlag2 == true ){
                                RUN_AGAIN2_COLOR( G, width, Xpos, Ypos, filename, count, namesForEachGraph, pos_, bends_, abbv, cat_num,  CategoriesXL, edgeBendImp, colorScale, edgThicknessTher  );
                        }
                }
        }
}

void mainGraph2( GRAPH<leda::string,int> &PROJECT,
		list<GRAPH<int,int> > &GraphList,
		list<GRAPH<leda::string,int> > &GraphList_S, 
		list<list<Strings> > &namesForEachGraph,
		array<GENENAMES> &GenesNode,
		list<double> &H_values,
		node_array<double> &Xpos,
		node_array<double> &Ypos,		
		node_array<point> &pos,
		edge_array<list<point> > &bends,
		int algorithmFlag,
		bool brandFlag,
		bool brandFlag2,
		bool ourMethodFlag,
		double space,
		int increment,
		bool ledaPostFlag,
		double nodeSize,
		double edgeBendImp,
		double colorScale,
		double edgThicknessTher,
		GRAPH<leda::string,int> &G1,
		GRAPH<leda::string,int> &G2,
		GRAPH<int,int> &TEMPINT,
		GRAPH<int,int> &G,
		int simplify,
		double removeRat,
		bool sharedGenes,
		double Hmax,
		bool edgesBetween,
		list<node_array<point> > &POS,
		list<edge_array<list<point> > > &BENDS,
		list<array<list<node> > > &LAYERS,
		array<char> &abbv,
		int cat_num,
		array<char> &Categories,
		int width,
		bool hvalueWeighting,
		bool enrichmentWeighting_o,
		bool enrichmentWeighting_f,
		bool ppihitratioWeighting,
		list<int> &categ )
{
		int max_weight=10;
		bool autoFlag = true;
		node_array<double> HValues(PROJECT, 0 );
		edge_array<int> edgeNumbersForStr( PROJECT );
		node_array<int> nodesOfProjectStr( PROJECT );
		array<node> nodesOfProjectInt( PROJECT.number_of_nodes() );
		array<edge> edgeNumbersForInt;
		edge e;
		list_item it,it2;
		cout << " BEFORE MAIN LAYOUT FORM\n";
		GRAPH<int,int> PROJECT2 = mainGraphHandling2(  PROJECT,
							      nodesOfProjectStr,
							      nodesOfProjectInt,
							      GraphList,
							      GraphList_S,
							      namesForEachGraph,
							      GenesNode,
							      HValues,
							      H_values,
							      abbv,
							      cat_num,
							      Xpos,
							      Ypos,
							      pos,
							      bends,
							      edgeNumbersForStr,
							      edgeNumbersForInt,
							      algorithmFlag,
							      brandFlag,
							      brandFlag2,
							      ourMethodFlag,
							      space,
							      increment,
							      ledaPostFlag,
							      nodeSize,
							      edgeBendImp,
							      colorScale,
							      edgThicknessTher,
							      G1,
							      G2,
							      TEMPINT,
							      G,
							      simplify,
							      removeRat,
							      sharedGenes,
							      Hmax,
							      max_weight,
							      edgesBetween,
							      hvalueWeighting,
							      enrichmentWeighting_o,
							      enrichmentWeighting_f,
							      ppihitratioWeighting,
							      categ,
							      width ); 

	cout << "/**************************************************/" << endl;
	cout << "\t" << " End of Building High Level Graph " << endl;
	cout << "/**************************************************/" << endl;
	cout << "/**************************************************/" << endl;
	cout << "\t" << " Forming High Level Layout " << endl;
	cout << "/**************************************************/" << endl;

	int count = 1;
      	char filename[ 64 ];
	if( autoFlag == true ){
		for( count = 0; count < GraphList.size(); count++ ){
			
			#ifdef LINUX
				sprintf( filename, "outputs/graphs/graph%d.gml", count );
			#else
				sprintf( filename, "outputs//graphs//graph%d.gml", count );
			#endif

			array<list<node> > layers_ = LAYERS[ LAYERS.get_item( count )];
			node_array<point> pos_ = POS[ POS.get_item( count )];
			edge_array<list<point> > bends_ = BENDS[ BENDS.get_item( count )];
			G = GraphList[ GraphList.get_item( count ) ];
			RUN_FFD_AGAIN2( G, width, Xpos, Ypos, filename, count, namesForEachGraph, pos_, bends_, abbv, cat_num, Categories, edgeBendImp, colorScale, edgThicknessTher  ); 
		}
	}
}

void mainGraph2( GRAPH<leda::string,int> &PROJECT,
		list<GRAPH<int,int> > &GraphList,
		list<GRAPH<leda::string,int> > &GraphList_S, 
		list<list<Strings> > &namesForEachGraph,
		array<GENENAMES> &GenesNode,
		list<double> &H_values,
		node_array<double> &Xpos,
		node_array<double> &Ypos,		
		node_array<point> &pos,
		edge_array<list<point> > &bends,
		int algorithmFlag,
		bool brandFlag,
		bool brandFlag2,
		bool ourMethodFlag,
		double space,
		int increment,
		bool ledaPostFlag,
		double nodeSize,
		double edgeBendImp,
		double colorScale,
		double edgThicknessTher,
		GRAPH<leda::string,int> &G1,
		GRAPH<leda::string,int> &G2,
		GRAPH<int,int> &TEMPINT,
		GRAPH<int,int> &G,
		int simplify,
		double removeRat,
		bool sharedGenes,
		double Hmax,
		bool edgesBetween,
		list<node_array<point> > &POS,
		list<edge_array<list<point> > > &BENDS,
		list<array<list<node> > > &LAYERS,
		array<char> &abbv,
		int cat_num,
		array<list<char> > &CategoriesXL,
		int width,
		bool hvalueWeighting,
		bool enrichmentWeighting_o,
		bool enrichmentWeighting_f,
		bool ppihitratioWeighting,
		list<int> &categ )
{
		int max_weight=10;
		bool autoFlag = true;
		node_array<double> HValues(PROJECT, 0 );
		edge_array<int> edgeNumbersForStr( PROJECT );
		node_array<int> nodesOfProjectStr( PROJECT );
		array<node> nodesOfProjectInt( PROJECT.number_of_nodes() );
		array<edge> edgeNumbersForInt;
		edge e;
		list_item it,it2;
		cout << " BEFORE MAIN LAYOUT FORM\n";
		GRAPH<int,int> PROJECT2 = mainGraphHandling2(  PROJECT,
							      nodesOfProjectStr,
							      nodesOfProjectInt,
							      GraphList,
							      GraphList_S,
							      namesForEachGraph,
							      GenesNode,
							      HValues,
							      H_values,
							      abbv,
							      cat_num,
							      Xpos,
							      Ypos,
							      pos,
							      bends,
							      edgeNumbersForStr,
							      edgeNumbersForInt,
							      algorithmFlag,
							      brandFlag,
							      brandFlag2,
							      ourMethodFlag,
							      space,
							      increment,
							      ledaPostFlag,
							      nodeSize,
							      edgeBendImp,
							      colorScale,
							      edgThicknessTher,
							      G1,
							      G2,
							      TEMPINT,
							      G,
							      simplify,
							      removeRat,
							      sharedGenes,
							      Hmax,
							      max_weight,
							      edgesBetween,
							      hvalueWeighting,
							      enrichmentWeighting_o,
							      enrichmentWeighting_f,
							      ppihitratioWeighting,
							      categ,
							      width ); 

	cout << "/**************************************************/" << endl;
	cout << "\t" << " End of Building High Level Graph " << endl;
	cout << "/**************************************************/" << endl;
	cout << "/**************************************************/" << endl;
	cout << "\t" << " Forming High Level Layout " << endl;
	cout << "/**************************************************/" << endl;

	int count = 1;
      	char filename[ 64 ];
	if( autoFlag == true ){
		for( count = 0; count < GraphList.size(); count++ ){
			#ifdef LINUX
				sprintf( filename, "outputs/graphs/graph%d.gml", count );
			#else
				sprintf( filename, "outputs//graphs//graph%d.gml", count );
			#endif
                        array<list<node> > layers_  = LAYERS[ LAYERS.get_item( count )];
                        node_array<point> pos_ = POS[ POS.get_item( count )];                        
                        edge_array<list<point> > bends_ = BENDS[ BENDS.get_item( count )];
                        G = GraphList[ GraphList.get_item( count ) ];
                        cout << endl << count << " - " << G.number_of_nodes() << " - " << G.number_of_edges() << endl;
                        if( G.number_of_edges() > 0 ){
                            RUN_FFD_AGAIN2_COLOR( G, filename, count, namesForEachGraph, pos_, bends_, abbv, cat_num, CategoriesXL, edgeBendImp, colorScale, edgThicknessTher  );
                        }
		}
	}
}

