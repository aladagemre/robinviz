
void _iniReadCoExpression(  int &minBicSize,
			    int &maxBicSize,
			    int &width,
			    int &space,
			    int &increment,
			    double &removeRat,
			    double &nodeSize,
			    double &edgeBendImp,
			    double &colorScale,
			    double &edgThicknessTher,
			    char *dataName,
			    char *dataName2,
			    char *ppifilename,
			    char *catfile,
			    char *gofile,
			    int &biclustering,
			    int &bimax_low_dim1,
			    int &bimax_low_dim2,
			    double &ther_disc,
			    int &bic_num_bimax, 
			    int &bic_num_cc, 
			    int &maxScore_, 
			    int &minHeight_, 
			    int &minWidth_, 
			    int &maxSizeSubMatrix_exp1_g,
			    int &maxSizeSubMatrix_exp1_c, 
			    int &minSizeSubMatrix_exp1_g, 
			    int &minSizeSubMatrix_exp1_c,
			    int &repeat,
			    double &hvaluemin,
			    int &increment_exp1_g,
			    int &increment_exp1_c,
			    bool &ownFlag, 
			    bool &lebFlag, 
			    bool &ccFlag, 
			    bool &msbeFlag, 
			    bool &bimaxFlag, 
			    bool &rlebFlag, 
			    bool &brandFlag, 
			    bool &brandFlag2,
			    bool &ourMethodFlag, 
			    bool &ledaPostFlag, 
			    bool &readOption, 
			    bool &layered_layout, 
			    bool &go_info, 
			    bool &edgesBetween, 
			    bool &sharedGenes,
			    bool &hvalueWeighting, 
			    bool &enrichmentWeighting_o, 
			    bool &enrichmentWeighting_f, 
			    bool &ppihitratioWeighting,
			    bool &ffd_layout,
			    bool &circular_layout,
			    bool &circular_ffd_layout,
			    bool &is_ppi_directed
){
	char temp[512],empty[512];
	int bool_i;
	FILE *fptr;
        if( (fptr = fopen( "settings.yaml", "r" ) ) == NULL ){
		printf( "\n Ini file can not be found\n" );
                        FILE *erptr;
#ifdef LINUX
                        erptr = fopen( "outputs/error.txt", "w" );
#else
                        erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "Ini file can not be found\n" );
                        fclose( erptr );
                        cout << "\nIni file can not be found\n";
                        exit(1);
	}
	else{
	
	//1 Confirmation:
	      fscanf( fptr, "%s", temp );
	//2     CoExpression:
	      fscanf( fptr, "%s", temp );
	//3         Input:
	      fscanf( fptr, "%s", temp );
	//4             dataName_bic: sources/usr_sources/microarray_data/input_bic.txt
	      fscanf( fptr, "%s%s", temp, dataName );
	//5             dataName2_bic: sources/organisms/Yeast2993x173/expression_matrix.txt
	      fscanf( fptr, "%s%s", temp, dataName2 );
	//6             ppifilename_bic: sources/organisms/Yeast2993x173/ppi2.txt *
	      fscanf( fptr, "%s%s", temp, ppifilename );
	//7             catfile_bic: sources/organisms/Yeast2993x173/category.txt *
	      fscanf( fptr, "%s%s", temp, catfile );
	//8             gofile_bic: sources/organisms/Yeast2993x173/go.txt
	      fscanf( fptr, "%s%s", temp, gofile );
	//9         Drawing:
	      fscanf( fptr, "%s", temp );
	//1             minBicSize: 0
	      fscanf( fptr, "%s%d", temp, &minBicSize );
	//2             maxBicSize: 1000
	      fscanf( fptr, "%s%d", temp, &maxBicSize );
	//3             width: 15
	      fscanf( fptr, "%s%d", temp, &width );
	//4             space: 150
	      fscanf( fptr, "%s%d", temp, &space );
	//5             increment: 300
	      fscanf( fptr, "%s%d", temp, &increment );
// 	      cout << "0______________________\n";
// 	      cout << increment << endl;
// 	      cout << "0______________________\n";
	//6             removeRat: 0.15 *
	      fscanf( fptr, "%s%lf", temp, &removeRat );
	//7             nodeSize: 50.0 *
	      fscanf( fptr, "%s%lf", temp, &nodeSize );
	//8             edgeBendImp: 35.0
	      fscanf( fptr, "%s%lf", temp, &edgeBendImp );
	//9             colorScale: 130.0 *
	      fscanf( fptr, "%s%lf", temp, &colorScale );
	//1             edgThicknessTher: 4.0 *
	      fscanf( fptr, "%s%lf", temp, &edgThicknessTher );
	//2         Biclustering:
	      fscanf( fptr, "%s", temp );
	//3             biclustering: 1
	      fscanf( fptr, "%s%d", temp, &biclustering );
	//4             bimax_low_dim1: 10
	      fscanf( fptr, "%s%d", temp, &bimax_low_dim1 );
	//5             bimax_low_dim2: 10
	      fscanf( fptr, "%s%d", temp, &bimax_low_dim2 );
	//6             ther_disc: 0.481600
	      fscanf( fptr, "%s%lf", temp, &ther_disc );
	//7             bic_num_bimax: 25
	      fscanf( fptr, "%s%d", temp, &bic_num_bimax );
	//8             bic_num_cc: 100
	      fscanf( fptr, "%s%d", temp, &bic_num_cc );
	//9             maxScore_: 2000
	      fscanf( fptr, "%s%d", temp, &maxScore_ );
	//1             minHeight_: 50
	      fscanf( fptr, "%s%d", temp, &minHeight_ );
	//2            minWidth_: 20
	      fscanf( fptr, "%s%d", temp, &minWidth_ );
	//3             maxSizeSubMatrix_exp1_g: 300
	      fscanf( fptr, "%s%d", temp, &maxSizeSubMatrix_exp1_g );
	//4             maxSizeSubMatrix_exp1_c: 50
	      fscanf( fptr, "%s%d", temp, &maxSizeSubMatrix_exp1_c );
	//5             minSizeSubMatrix_exp1_g: 200
	      fscanf( fptr, "%s%d", temp, &minSizeSubMatrix_exp1_g );
	//6             minSizeSubMatrix_exp1_c: 30
	      fscanf( fptr, "%s%d", temp, &minSizeSubMatrix_exp1_c );
	//7             repeat: 50
	      fscanf( fptr, "%s%d", temp, &repeat );
	//8             hvaluemin: 1500.00
              fscanf( fptr, "%s%lf", temp, &hvaluemin );
	//9             increment_exp1_g: 10
	      fscanf( fptr, "%s%d", temp, &increment_exp1_g );
	//1             increment_exp1_c: 10
	      fscanf( fptr, "%s%d", temp, &increment_exp1_c );
	//2         Algorithms:
	      fscanf( fptr, "%s", temp );
	//3             ownFlag: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		ownFlag = ( bool_i == 0 ? 0 : 1 );
	//4             lebFlag: 1
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		lebFlag = ( bool_i == 0 ? 0 : 1 );
// 		cout << lebFlag << " - " << bool_i << endl;
	//5             ccFlag: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		ccFlag = ( bool_i == 0 ? 0 : 1 );
// 		cout << ccFlag << " - " << bool_i << endl;
	//6             msbeFlag: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		msbeFlag = ( bool_i == 0 ? 0 : 1 );
	//7             bimaxFlag: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		bimaxFlag = ( bool_i == 0 ? 0 : 1 );
	//8             rlebFlag: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		rlebFlag = ( bool_i == 0 ? 0 : 1 );
	//9             brandFlag: 1
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		brandFlag = ( bool_i == 0 ? 0 : 1 );
	//1             brandFlag2: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		brandFlag2 = ( bool_i == 0 ? 0 : 1 );
	//2             ourMethodFlag: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		ourMethodFlag = ( bool_i == 0 ? 0 : 1 );
	//3             ledaPostFlag: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		ledaPostFlag = ( bool_i == 0 ? 0 : 1 );
	//4             readOption: 1
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		readOption = ( bool_i == 0 ? 0 : 1 );
	//5             ffd_layout: 1*
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		ffd_layout = ( bool_i == 0 ? 0 : 1 );
	//6             layered_layout: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		layered_layout = ( bool_i == 0 ? 0 : 1 );
	//7             circular_layout: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		circular_layout = ( bool_i == 0 ? 0 : 1 );
	//8             circular_ffd_layout: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		circular_ffd_layout = ( bool_i == 0 ? 0 : 1 );
	//9             go_info 0: *
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		go_info = ( bool_i == 0 ? 0 : 1 );
	//1             edgesBetween: 1 *
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		edgesBetween = ( bool_i == 0 ? 0 : 1 );
	//2             sharedGenes: 0 *
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		sharedGenes = ( bool_i == 0 ? 0 : 1 );
	//3             hvalueWeighting: 1
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		hvalueWeighting = ( bool_i == 0 ? 0 : 1 );
	//4             enrichmentWeighting_o: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		enrichmentWeighting_o = ( bool_i == 0 ? 0 : 1 );
	//5             enrichmentWeighting_f: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		enrichmentWeighting_f = ( bool_i == 0 ? 0 : 1 );
	//6             ppihitratioWeighting: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		ppihitratioWeighting = ( bool_i == 0 ? 0 : 1 );
	//7             is_ppi_directed: 0
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		is_ppi_directed = ( bool_i == 0 ? 0 : 1 );
	// 62 lines with one empty line
		fclose( fptr );
	}
}

void _iniReadCoFunctionality( double &removeRat, 
			 double &nodeSize, 
			 double &edgeBendImp, 
			 double &edgThicknessTher,
			 double &colorScale,
			 char *ppifilename, 
			 bool &edgesBetween, 
			 bool &sharedGenes,
			 bool &go_info,
			 char *catfile, 
			 char *inputGoFile,
			 char *defaultGoFile 
){

	char temp[512],empty[512];
	int bool_i;
	FILE *fptr;
        if( (fptr = fopen( "settings.yaml", "r" ) ) == NULL ){
		printf( "\n Ini file can not be found\n" );
                        FILE *erptr;
#ifdef LINUX
                        erptr = fopen( "outputs/error.txt", "w" );
#else
                        erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "Ini file can not be found\n" );
                        fclose( erptr );
                        cout << "\nIni file can not be found\n";
                        exit(1);
	}
	else{
		for( int i = 0; i < 62; i++ ){
			fgets( temp, 512, fptr );
		}

//1 		CoFunctionality:
		fscanf( fptr, "%s", temp );
//2		    Input:
		fscanf( fptr, "%s", temp );
//3			ppifilename_go: sources/organisms/Yeast2993x173/ppi.txt *
		fscanf( fptr, "%s%s", temp, ppifilename );
//4			catfile_go: sources/organisms/Yeast2993x173/category.txt *
		fscanf( fptr, "%s%s", temp, catfile );
//5			gofile_go: sources/organisms/Yeast2993x173/go_simple.txt
		fscanf( fptr, "%s%s", temp, defaultGoFile );
//6			dataName_go: sources/organisms/Yeast2993x173/input_go.txt
		fscanf( fptr, "%s%s", temp, inputGoFile );
//7		    Drawing:
		fscanf( fptr, "%s", temp );
//8			removeRat: 0.15 *
		fscanf( fptr, "%s%lf", temp, &removeRat );
//9			nodeSize: 50.0 *
		fscanf( fptr, "%s%lf", temp, &nodeSize );
//1			edgeBendImp: 35.0
		fscanf( fptr, "%s%lf", temp, &edgeBendImp );
//2			colorScale: 130.0 *
		fscanf( fptr, "%s%lf", temp, &colorScale );
//3			edgThicknessTher: 4.0 *
		fscanf( fptr, "%s%lf", temp, &edgThicknessTher );
//4		    Algorithms:
		fscanf( fptr, "%s", temp );
//5			ffd_layout: 1*
// 		fscanf( fptr, "%s%d", empty, &bool_i ); 
// 		ffd_layout = ( bool_i == 0 ? 0 : 1 );
//6			go_info: 0 *
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		go_info = ( bool_i == 0 ? 0 : 1 );
//7			edgesBetween: 1 *
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		edgesBetween = ( bool_i == 0 ? 0 : 1 );
//8			sharedGenes: 0 *
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		sharedGenes = ( bool_i == 0 ? 0 : 1 );
	// 18 lines with one empty line
		fclose( fptr );
	}
}
