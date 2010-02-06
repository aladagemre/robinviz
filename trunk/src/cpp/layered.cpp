 /*******************************************************************************************************/
/*	Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>   	 	       */
/*											 	       */
/*    This program is free software: you can redistribute it and/or modify		 	       */
/*    it under the terms of the GNU General Public License as published by		 	       */
/*    the Free Software Foundation, either version 3 of the License, or			 	       */
/*    (at your option) any later version.						 	       */
/*											 	       */
/*    This program is distributed in the hope that it will be useful,			 	       */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of			 	       */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			 	       */
/*    GNU General Public License for more details.					 	       */
/*											 	       */
/*    You should have received a copy of the GNU General Public License			 	       */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.		 	       */
/*											 	       */
/*    Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 	       */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.         	       */
/*    This is free software, and you are welcome to redistribute it                      	       */
/*    under certain conditions; type `show c' for details.			         	       */
/*******************************************************************************************************/
/********************************************************************************************************
*                                                                               			*
*	The project team                                                        			*
*                                                                               			*
*  Assistant Professor Cesim Erten(Kadir Has University)                        			*
*  Ahmet Emre Aladağ(Kadir Has University)                                      			*
*  Melih Sözdinler(Boğaziçi University)                                         			*
*                                                                               			*
*	Contacts for Implementation Issues                                      			*
*                                                                               			*
*  Melih Sözdinler, melihsozdinler{at}gmail.com, melih.sozdinler{at}boun{dot}edu{dot}tr 		*
*  Ahmet Emre Aladağ, aladagemre{at}gmail{dot}com, emre.aladag{at}stu{dot}khas{dot}edu{dot}tr      	*
*                                                                               			*
********************************************************************************************************/

#include "incl/handlers.h"
#include "incl/handlers/_dataread.h"
#include "incl/handlers/_bicread.h"
#include "incl/handlers/_ppiread.h"
#include "incl/handlers/_maingraph.h"
#include "incl/bicalg/bimax/bimax.h"
#include "incl/bicalg/cc/cc.h"
#include "incl/bicalg/rleb/runleb.h"

#define ORDVARS
#define LEDAVARS

#ifdef ORDVARS
	/**
	* bool sharedGenes First option for the main graph layout, when it is true, edgesBetween must be false
	**/
		bool sharedGenes = false; 
	/**
	* bool edgesBetween Second option for the main graph layout, when it is true, sharedGenes must be false
	**/
		bool edgesBetween = true;
	/**
	* Variables for the visualization system, considering the proposed system of weighted and layered
        * drawings of graphs
	**/
                int minBicSize = 0; // minimum number of genes that bicluster can have
                int maxBicSize = 1000; // maximum number of genes that bicluster can have
		int width = 10;       // Layering max width
		int space = 150;      // yspace between layers
		int increment = 125;  // xspace between nodes
	/**
	* double removeRat this restricts the main layout to have less edges when the doube value increases
	**/
		double removeRat = 0.35;
	/**
	* variables for visualization clues
	**/
		double nodeSize=50.0;
		double edgeBendImp=35.0;
		double colorScale=130.0;
		double edgThicknessTher=3.0;
	/**
	* decision variables for running biclustering algorithms or obtaining default results
	**/
		bool ownFlag = false, lebFlag = true, ccFlag = true, msbeFlag = false, bimaxFlag = false, rlebFlag = false;
	/**
	* decision variables for xcoordinate algorithms
	**/
		bool brandFlag = true, brandFlag2= false, ourMethodFlag = false, ledaPostFlag=false;
	/**
	* decision variable when it is true,
	* assumes
	* n m // n => dim1, m => dim2
        * gene  cond0 cond1 cond2....
        * gene0   0     1    2   ....
        * gene1   1     2    3   ....
        * when it is false
	* assumes 
        * n m // n => dim1, m => dim2
        * 0	1	2
        * 1	2	3
	**/
		bool readOption = true;
	/**
	* destination of input when readOption is false
	**/
#ifdef LINUX
		char dataName[128] = "sources/usr_sources/microarray_data/input.txt";
#else
		char dataName[128] = "sources//usr_sources//microarray_data//input.txt";
#endif
	/**
	* destination of input when readOption is true
	**/
#ifdef LINUX
		char dataName2[128] = "sources/usr_sources/microarray_data/inputLabel.txt";
#else
		char dataName2[128] = "sources//usr_sources//microarray_data//inputLabel.txt";
#endif
	/**
	* decision variables for layout methods
	**/
		bool ffd_layout = false, layered_layout = true;
	/**
	* Specifies Biclustering algorithm 1 default files, 2 Bimax, 3 CC
	**/	
		int biclustering = 1;
	/**
	* Specifies the weighting scheme for nodes
	**/
		bool hvalueWeighting = true;
		bool enrichmentWeighting_o = false;
		bool enrichmentWeighting_f = false;
		bool ppihitratioWeighting = false;
	/**
	* BIMAX parameters
	**/
		int bimax_low_dim1 = 10;
		int bimax_low_dim2 = 10;
		double ther_disc = 0.50;
		int bic_num_bimax = 100;
	/**
	* CC parameters
	**/
		int bic_num_cc = 100;
		int maxScore_ = 5;
		int minHeight_ = 40;
		int minWidth_ = 10;
	/**
	* RLEB parameters
	**/
		int maxSizeSubMatrix_exp1_g = 100;
		int maxSizeSubMatrix_exp1_c = 20;
		int minSizeSubMatrix_exp1_g = 50;
		int minSizeSubMatrix_exp1_c = 10;
		int repeat = 100;
		double hvaluemin = 10.0;
		int increment_exp1_g = 10;
		int increment_exp1_c = 5;
	/**
	*	GO file read or not, please specify if it is true at sources/usr_sources/vis...
	**/
		bool go_info = false;
#ifdef LINUX
                char gofile[256] = "sources/usr_sources/visualization_data/funcassociate_go_associations.txt";
#else
                char gofile[256] = "sources//usr_sources//visualization_data//funcassociate_go_associations.txt";
#endif
        /**
        *	Category File
        **/
#ifdef LINUX
                char catfile[256] = "sources/usr_sources/visualization_data/category.txt";
#else
                char catfile[256] = "sources//usr_sources//visualization_data//category.txt";
#endif
	/* This flag for changing the layout using x-coordinate assignment  */
	/* 1 for python and 0 for old non including x-coordinate assignment */
	int algorithmFlag = 1;
	int crossings = 1;
	int simplify = 0;
	array<char> abbv(64);
	int cat_num;
	double Hmax;
	bool wolfFlag = true, baryFlag = false, medianFlag = false, pmFlag = false, greFlag = false;
	char empty[32];
        int colors[17][3] ={ {255,255,255},
                             {0,0,0},
                             {255,0,0},
                             {0,255,0},
                             {0,0,160},
                             {255,255,0},
                             {128,0,255},
                             {255,128,0},
                             {0,255,255},
                             {128,0,0},
                             {255,0,255},
                             {0,128,128},
                             {100,100,255},
                             {220,220,220},
                             {175,175,175},
                             {128,128,128},
                             {255,255,230}};
#endif ORDVARS

#ifdef LEDAVARS
	/**
	* PPI Graph source file, it should be in sources/usr_sources/visualization_data/ppi
	**/
#ifdef LINUX
        char ppifilename[256] = "sources/usr_sources/visualization_data/ppi/NewPPIs2.txt";
#else
        char ppifilename[256] = "sources//usr_sources//visualization_data//ppi//NewPPIs2.txt";
#endif
	list<list<GENES> > biclusters; 
	list<leda::matrix> matrixBiclusters;	      
	list<double> H_values;
	GRAPH<int,int> INTERACTIONS,TEMPINT;
	node_array<GENENAMES> temp;
	array<GENENAMES> GenesNode;
	matrix INPUT;
	GRAPH<int,int> G;
	array<GRAPH<int,int> > listOfGraphs; 
	array<char> Categories;
	GRAPH<leda::string,int> PROJECT,G1,G2;
	GRAPH<int,int> H;
	node_array<double> Xpos;
	node_array<double> Ypos;
	list<list<Strings> > namesForEachGraph;
	list<node_array<point> > POS;
	list<edge_array<list<point> > > BENDS;
	list<array<list<node> > > LAYERS;
	node_array<point> pos;
	edge_array<list<point> > bends;
	array<list<node> > layers;
	list<GRAPH<int,int> > GraphList;
	list<GRAPH<leda::string,int> > GraphList_S;
	array<GENES> geneArray;  
	array<CONDS> condArray;
	list<CAT> categoriesOfGenes;
        list<int> categ;
#endif LEDAVARS

int main(){
	
	FILE *fptr;
	if( (fptr = fopen( "settings.ini", "r" ) ) == NULL ){
		printf( "\n Ini file can not be found\n" );
		return 0;
	}
	else{
		// 	//RobinVizInitializationFile
// 		fscanf( fptr, "%s", empty );
// 		//cout << empty << endl;
		// 	minBicSize = 150
		fscanf( fptr, "%s%d", empty, &minBicSize );
		//cout << empty << " " << minBicSize << endl;
		// 	maxBicSize = 160
		fscanf( fptr, "%s%d", empty, &maxBicSize );
		//cout << empty << " " << maxBicSize << endl;
		// 	width = 10
		fscanf( fptr, "%s%d", empty, &width );
		//cout << empty << " " << width << endl;
		// 	space = 150 
		fscanf( fptr, "%s%d", empty, &space );
		//cout << empty << " " << space << endl;
		// 	increment = 225
		fscanf( fptr, "%s%d", empty, &increment );
		//cout << empty << " " << increment << endl;
		// 	removeRat = 0.35
		fscanf( fptr, "%s%lf", empty, &removeRat );
		//cout << empty << " " << removeRat << endl;
		// 	nodeSize = 50.0
		fscanf( fptr, "%s%lf", empty, &nodeSize );
		//cout << empty << " " << nodeSize << endl;
		// 	edgeBendImp = 35.0
		fscanf( fptr, "%s%lf", empty, &edgeBendImp );
		//cout << empty << " " << edgeBendImp << endl;
		// 	colorScale = 130.0
		fscanf( fptr, "%s%lf", empty, &edgeBendImp );
		//cout << empty << " " << edgeBendImp << endl;
		// 	edgThicknessTher = 4.0
		fscanf( fptr, "%s%lf", empty, &edgThicknessTher );
		//cout << empty << " " << edgThicknessTher << endl;
		// 	dataName = "sources/usr_sources/microarray_data/input.txt"
		fscanf( fptr, "%s%s", empty, dataName );
		//cout << empty << " " << dataName << endl;
		// 	dataName2 = "sources/usr_sources/microarray_data/inputLabel.txt"
		fscanf( fptr, "%s%s", empty, dataName2 );
                //cout << empty << " " << dataName2 << endl;
                fscanf( fptr, "%s%s", empty, ppifilename );
                //cout << empty << " " << ppifilename << endl;
                fscanf( fptr, "%s%s", empty, catfile );
                //cout << empty << " " << ppifilename << endl;
		// 	biclustering = 1
		fscanf( fptr, "%s%d", empty, &biclustering );
		//cout << empty << " " << biclustering << endl;
		// 	bimax_low_dim1 = 10
		fscanf( fptr, "%s%d", empty, &bimax_low_dim1 );
		//cout << empty << " " << bimax_low_dim1 << endl;
		// 	bimax_low_dim2 = 10
		fscanf( fptr, "%s%d", empty, &bimax_low_dim2 );
		//cout << empty << " " << bimax_low_dim2 << endl;
		// 	ther_disc = 0.5
		fscanf( fptr, "%s%lf", empty, &ther_disc );
		//cout << empty << " " << ther_disc << endl;
		// 	bic_num_bimax = 50
		fscanf( fptr, "%s%d", empty, &bic_num_bimax );
		//cout << empty << " " << bic_num_bimax << endl;
		// 	bic_num_cc = 100
		fscanf( fptr, "%s%d", empty, &bic_num_cc );
		//cout << empty << " " << bic_num_cc << endl;
		// 	maxScore_ = 10
		fscanf( fptr, "%s%d", empty, &maxScore_ );
		//cout << empty << " " << maxScore_ << endl;
		// 	minHeight_ = 5
		fscanf( fptr, "%s%d", empty, &minHeight_ );
		//cout << empty << " " << minHeight_ << endl;
		// 	minWidth_ = 10
		fscanf( fptr, "%s%d", empty, &minWidth_ );
		//cout << empty << " " << minWidth_ << endl;
 		fscanf( fptr, "%s%d", empty, &maxSizeSubMatrix_exp1_g );
		//cout << empty << " " << maxSizeSubMatrix_exp1_g << endl;
		fscanf( fptr, "%s%d", empty, &maxSizeSubMatrix_exp1_c );
		//cout << empty << " " << maxSizeSubMatrix_exp1_c << endl;
		fscanf( fptr, "%s%d", empty, &minSizeSubMatrix_exp1_g ); 
		//cout << empty << " " << minSizeSubMatrix_exp1_g << endl;
		fscanf( fptr, "%s%d", empty, &minSizeSubMatrix_exp1_c );
		//cout << empty << " " << minSizeSubMatrix_exp1_c << endl;
		fscanf( fptr, "%s%d", empty, &repeat );
		//cout << empty << " " << repeat << endl;
		fscanf( fptr, "%s%lf", empty, &hvaluemin );
		//cout << empty << " " << hvaluemin << endl;
		fscanf( fptr, "%s%d", empty, &increment_exp1_g );
		//cout << empty << " " << increment_exp1_g << endl;
		fscanf( fptr, "%s%d", empty, &increment_exp1_c );
		//cout << empty << " " << increment_exp1_c << endl;

		int bool_i;
		// 	ownFlag = 0 
		fscanf( fptr, "%s%d", empty, &bool_i ); 
		ownFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << ownFlag << endl;
		// 	lebFlag = 1 
		fscanf( fptr, "%s%d", empty, &bool_i );
		lebFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << lebFlag << endl;
		// 	ccFlag = 0 
		fscanf( fptr, "%s%d", empty, &bool_i );
		ccFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << ccFlag << endl;
		// 	msbeFlag = 0 
		fscanf( fptr, "%s%d", empty, &bool_i );
		msbeFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << msbeFlag << endl;
		// 	bimaxFlag = 1
		fscanf( fptr, "%s%d", empty, &bool_i );
		bimaxFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << bimaxFlag << endl;
		// 	rlebFlag = 0
		fscanf( fptr, "%s%d", empty, &bool_i );
		rlebFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << rlebFlag << endl;
		// 	brandFlag = 1 
		fscanf( fptr, "%s%d", empty, &bool_i );
		brandFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << brandFlag << endl;
		// 	brandFlag2 = 0 
		fscanf( fptr, "%s%d", empty, &bool_i );
		brandFlag2 = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << brandFlag2 << endl;
		// 	ourMethodFlag = 0 
		fscanf( fptr, "%s%d", empty, &bool_i );
		ourMethodFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << ourMethodFlag << endl;
		// 	ledaPostFlag = 0
		fscanf( fptr, "%s%d", empty, &bool_i );
		ledaPostFlag = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << ledaPostFlag << endl;
		// 	readOption = 1
		fscanf( fptr, "%s%d", empty, &bool_i );
		readOption = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << readOption << endl;
		// 	ffd_layout = 0
		fscanf( fptr, "%s%d", empty, &bool_i );
		ffd_layout = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << ffd_layout << endl;
		// 	layered_layout = 1
		fscanf( fptr, "%s%d", empty, &bool_i );
		layered_layout = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << layered_layout << endl;
		// 	go_info = 1
		fscanf( fptr, "%s%d", empty, &bool_i );
		go_info = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << go_info << endl;
                fscanf( fptr, "%s%s", empty, gofile );
                //cout << empty << " " << ppifilename << endl;
		// 	edgesBetween = 1
		fscanf( fptr, "%s%d", empty, &bool_i );
		edgesBetween = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << edgesBetween << endl;
		// 	sharedGenes = 0 
		fscanf( fptr, "%s%d", empty, &bool_i );
		sharedGenes = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << sharedGenes << endl;
		fscanf( fptr, "%s%d", empty, &bool_i );
		hvalueWeighting = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << hvalueWeighting << endl;
		fscanf( fptr, "%s%d", empty, &bool_i );
		enrichmentWeighting_o = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << enrichmentWeighting_o << endl;
		fscanf( fptr, "%s%d", empty, &bool_i );
		enrichmentWeighting_f = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << enrichmentWeighting_f << endl;
		fscanf( fptr, "%s%d", empty, &bool_i );
		ppihitratioWeighting = ( bool_i == 0 ? 0 : 1 );
		//cout << empty << " " << ppihitratioWeighting << endl;
		fclose( fptr );

                FILE *kfptr;
                if( (fptr = fopen( catfile , "r" )) !=NULL ){
                    if( (kfptr = fopen( "sources/usr_sources/visualization_data/functions.txt", "w" )) != NULL || (kfptr = fopen( "sources//usr_sources//visualization_data//functions.txt", "w" )) != NULL ){
                        int number_of_category;
                        char tc_array[20], tc_array2[256];
                        fscanf( fptr, "%d", &number_of_category );
                        fprintf( kfptr, "%d\n", number_of_category );
                        for( int i = 0; i < number_of_category; i++ ){
                            fscanf( fptr, "%s%s", tc_array, tc_array2 );
                            if( i != number_of_category - 1 )
                                fprintf( kfptr, "%s\t%s\n", tc_array, tc_array2 );
                            else
                                fprintf( kfptr, "%s\t%s", tc_array, tc_array2 );
                        }
                        fclose(kfptr);
                        if( (kfptr = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "w" )) != NULL || (kfptr = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "w" )) != NULL ){
                               char cat_c;
                               while( !feof( fptr ) ){
                                   fscanf( fptr, "%s%c", tc_array, &cat_c );
                                   fprintf( kfptr, "%s\t%c\n", tc_array, cat_c );
                               }
                               fclose(kfptr);
                               fclose(fptr);
                        }
                        else{
                            FILE *erptr;
#ifdef LINUX
                            erptr = fopen( "outputs/error.txt", "w" );
#else
                            erptr = fopen( "outputs//error.txt", "w" );
#endif
                            fprintf( erptr, "You Probably Deleted genefunctions.txt\n" );
                            fclose( erptr );
                            cout << "\nYou Probably Deleted genefunctions.txt\n";
                            exit(1);
                        }
                    }
                    else{
                        FILE *erptr;
#ifdef LINUX
                        erptr = fopen( "outputs/error.txt", "w" );
#else
                        erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "You Probably Deleted functions.txt\n" );
                        fclose( erptr );
                        cout << "\nYou Probably Deleted functions.txt\n";
                        exit(1);

                    }
                }
                else{
                    FILE *erptr;
#ifdef LINUX
                    erptr = fopen( "outputs/error.txt", "w" );
#else
                    erptr = fopen( "outputs//error.txt", "w" );
#endif
                    fprintf( erptr, "Check that you specifiy the correct category file\n" );
                    fclose( erptr );
                    cout << "\nCheck that you specifiy the correct category file\n";
                    exit(1);
                }

		if( readOption == false ){
			INPUT = dataRead( dataName );
		}
		else{
                        FILE *defaultRunTo;
                        char chr;
			INPUT = dataRead( dataName2, geneArray, condArray );
                         //cout << "\nDONE";
#ifdef LINUX
                         defaultRunTo = fopen( "sources/usr_sources/visualization_data/genenames.txt", "w" );
#else
                         defaultRunTo = fopen( "sources//usr_sources//visualization_data//genenames.txt", "w" );
#endif
                         for( int i = 0; i < INPUT.dim1(); i++ ){
                             //cout << geneArray[ i ].GENE << "\t" << i << endl;
                             fprintf( defaultRunTo, "%s\n", geneArray[ i ].GENE );
                         }
                         fclose( defaultRunTo );
#ifdef LINUX
                         defaultRunTo = fopen( "sources/usr_sources/visualization_data/geneNameConversion.txt", "w" );
#else
                         defaultRunTo = fopen( "sources//usr_sources//visualization_data//geneNameConversion.txt", "w" );
#endif
                         for( int i = 0; i < INPUT.dim1(); i++ ){
                             fprintf( defaultRunTo, "%s%d\t%s\n", "gene", i, geneArray[ i ].GENE );
                         }
                         fclose( defaultRunTo );
                         //cout << "\nDONE\n";
	// 		drawHeatmap( INPUT, geneArray, condArray, "outputs/heatmap/out.html" );
		}
		if( bimaxFlag == true ){
			dataWriteBimax( INPUT, bimax_low_dim1, bimax_low_dim2, ther_disc );
			biclustering = 2;
                        if( bimax_low_dim1 > INPUT.dim1() || bimax_low_dim2 > INPUT.dim2() ){
                            FILE *erptr;
#ifdef LINUX
                            erptr = fopen( "outputs/error.txt", "w" );
#else
                            erptr = fopen( "outputs//error.txt", "w" );
#endif
                            fprintf( erptr, "Check the desired dimensions of BIMAX algorithm\n" );
                            fclose( erptr );
                            cout << "\nCheck the desired dimensions of BIMAX algorithm\n";
                            exit(1);
                        }
			#ifdef LINUX
                                bimaxMain( "src/cpp/incl/bicalg/bimax/matrix_robin.txt", bic_num_bimax );
			#else
                                bimaxMain( "src//cpp//incl//bicalg//bimax//matrix_robin.txt", bic_num_bimax );
			#endif
		}else{
			if( ccFlag == true ){
				dataWriteCC( INPUT ); 
                                if( minHeight_ > INPUT.dim1() || minWidth_ > INPUT.dim2() ){
                                    FILE *erptr;
#ifdef LINUX
                                    erptr = fopen( "outputs/error.txt", "w" );
#else
                                    erptr = fopen( "outputs//error.txt", "w" );
#endif
                                    fprintf( erptr, "Check the desired dimensions of CC algorithm\n" );
                                    fclose( erptr );
                                    cout << "\nCheck the desired dimensions of CC algorithm\n";
                                    exit(1);
                                }
				biclustering = 3;
				#ifdef LINUX
                                        ccMain( "src/cpp/incl/bicalg/cc/matrix_robin.txt", INPUT.dim1(), INPUT.dim2(), 10, maxScore_, minHeight_, minWidth_ );
				#else
                                        ccMain( "src//cpp//incl//bicalg//cc//matrix_robin.txt", INPUT.dim1(), INPUT.dim2(), 10, maxScore_, minHeight_, minWidth_ );
				#endif
			}else{
				if( rlebFlag == true ){
                                        if( maxSizeSubMatrix_exp1_g > INPUT.dim1() || minSizeSubMatrix_exp1_g > INPUT.dim1()  || minSizeSubMatrix_exp1_c > INPUT.dim2() || maxSizeSubMatrix_exp1_c > INPUT.dim2() || maxSizeSubMatrix_exp1_g < minSizeSubMatrix_exp1_g || maxSizeSubMatrix_exp1_c < minSizeSubMatrix_exp1_c ){
                                            FILE *erptr;
#ifdef LINUX
                                            erptr = fopen( "outputs/error.txt", "w" );
#else
                                            erptr = fopen( "outputs//error.txt", "w" );
#endif
                                            fprintf( erptr, "Check the desired dimensions of RLEB algorithm\n" );
                                            fclose( erptr );
                                            cout << "\nCheck the desired dimensions of RLEB algorithm\n";
                                            exit(1);
                                        }
					biclustering = 4;
                                        rlebmain_m( INPUT, maxSizeSubMatrix_exp1_g, maxSizeSubMatrix_exp1_c, minSizeSubMatrix_exp1_g, minSizeSubMatrix_exp1_c, repeat, hvaluemin, increment_exp1_g, increment_exp1_c );
				}
                                else{
                                         //cout << "\nDONE2\n";
                                         FILE *defaultRunFrom,*defaultRunTo;                                         
                                         char chr;
                                         if( readOption == false ){
#ifdef LINUX
                                             defaultRunFrom = fopen( "sources/ppi_sources/genenames.txt", "r" );
                                             defaultRunTo = fopen( "sources/usr_sources/visualization_data/genenames.txt", "w" );
#else
                                             defaultRunFrom = fopen( "sources//ppi_sources//genenames.txt", "r" );
                                             defaultRunTo = fopen( "sources//usr_sources//visualization_data//genenames.txt", "w" );
#endif
                                             while( !feof( defaultRunFrom )){
                                                 fscanf( defaultRunFrom, "%c", &chr );
                                                 fprintf( defaultRunTo, "%c", chr );
                                             }
                                             fclose( defaultRunFrom );
                                             fclose( defaultRunTo );
                                         }
                                         //cout << "\nDONE2\n";
                                         if( readOption == false ){
#ifdef LINUX
                                             defaultRunFrom = fopen( "sources/ppi_sources/geneNameConversion.txt", "r" );
                                             defaultRunTo = fopen( "sources/usr_sources/visualization_data/geneNameConversion.txt", "w" );
#else
                                             defaultRunFrom = fopen( "sources//ppi_sources//geneNameConversion.txt", "r" );
                                             defaultRunTo = fopen( "sources//usr_sources//visualization_data//geneNameConversion.txt", "w" );
#endif
                                             while( !feof( defaultRunFrom )){
                                                 fscanf( defaultRunFrom, "%c", &chr );
                                                 fprintf( defaultRunTo, "%c", chr );
                                             }
                                             fclose( defaultRunFrom );
                                             fclose( defaultRunTo );
                                         }
                                         //cout << "\nDONE2\n";
#ifdef LINUX
                                         defaultRunFrom = fopen( "sources/ppi_sources/genefunctions.txt", "r" );
                                         defaultRunTo = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "w" );
#else
                                         defaultRunFrom = fopen( "sources//ppi_sources//genefunctions.txt", "r" );
                                         defaultRunTo = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "w" );
#endif
                                         while( !feof( defaultRunFrom )){
                                             fscanf( defaultRunFrom, "%c", &chr );
                                             fprintf( defaultRunTo, "%c", chr );
                                         }
                                         fclose( defaultRunFrom );
                                         fclose( defaultRunTo );
#ifdef LINUX
                                         defaultRunFrom = fopen( "sources/ppi_sources/functions.txt", "r" );
                                         defaultRunTo = fopen( "sources/usr_sources/visualization_data/functions.txt", "w" );
#else
                                         defaultRunFrom = fopen( "sources//ppi_sources//functions.txt", "r" );
                                         defaultRunTo = fopen( "sources//usr_sources//visualization_data//functions.txt", "w" );
#endif
                                         while( !feof( defaultRunFrom )){
                                             fscanf( defaultRunFrom, "%c", &chr );
                                             fprintf( defaultRunTo, "%c", chr );
                                         }
                                         fclose( defaultRunFrom );
                                         fclose( defaultRunTo );
                                         //cout << "\nDONE\n";
                                }
			}
		}
		
                bicRead( INPUT, biclusters, categ, matrixBiclusters, H_values, Hmax, minBicSize, maxBicSize, biclustering, INPUT.dim1(), INPUT.dim2() );
		cat_num = functionalCategoryFinder( categoriesOfGenes, abbv );
		if( go_info == 1 ){
                        array<GENEONTO> geneOntoForData = geneOntologyHandling(gofile);
			geneOntologyToBiclusterHandling( biclusters, geneOntoForData );
		}
                FILE *cfptr;
                if( (cfptr =fopen("outputs/colors_func.txt", "w"))==NULL && (cfptr=fopen("outputs//colors_func.txt", "w")) ==NULL ){
                    cout << " Error: Cannot write into outputs folder";
                    return 0;
                }
                else{
                    list_item cit;
                    int colorCount = 0;
                    forall_items( cit, categoriesOfGenes){
                        fprintf( cfptr, "%s %d %d %d\n",categoriesOfGenes[cit].categ, colors[colorCount][0], colors[colorCount][1], colors[colorCount][2] );
                        //printf( "%s %d %d %d\n",categoriesOfGenes[cit].categ, colors[colorCount][0], colors[colorCount][1], colors[colorCount][2] );
                        colorCount++;
                    }
                    fclose( cfptr );
                }
                interactionRead( temp, GenesNode, INTERACTIONS, TEMPINT, ppifilename );
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Produce Bicluster Graphs" << endl;
		cout << "/**************************************************/" << endl;
		Categories.resize( INTERACTIONS.number_of_nodes() );
                node n;
                for( int i = 0; i < INTERACTIONS.number_of_nodes(); i++ )
                    Categories[ i ] = 'X';
		listOfGraphs.resize( 0, biclusters.size() );
		inpGraphProdHandling( G, listOfGraphs, abbv, Categories, temp, GenesNode, INTERACTIONS, TEMPINT, biclusters, cat_num );
		cout << "/**************************************************/" << endl;
		cout << "\t" << " End Producing Bicluster Graphs" << endl;
		cout << "/**************************************************/" << endl;
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Running Layering Algorithm" << endl;
		cout << "/**************************************************/" << endl;
		if( layered_layout ){
			mainAlgHandlingForEachSubgraph( pos, bends, layers, GraphList, GraphList_S, Xpos, Ypos, POS, BENDS, LAYERS, PROJECT, GenesNode, listOfGraphs, namesForEachGraph, biclusters, width, algorithmFlag, space, ourMethodFlag, increment, ledaPostFlag );
		}
		else{
			if( ffd_layout ){
				mainAlgHandlingForEachSubgraph2( pos, bends, layers, GraphList, GraphList_S, Xpos, Ypos, POS, BENDS, LAYERS, PROJECT, GenesNode, listOfGraphs, namesForEachGraph, biclusters, width, algorithmFlag, space, ourMethodFlag, increment, ledaPostFlag, abbv, cat_num, Categories );
			}
		}
		cout << "/**************************************************/" << endl;
		cout << "\t" << " End Running Layering Algorithm" << endl;
		cout << "/**************************************************/" << endl;
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Building High Level Graph " << endl;
		cout << "/**************************************************/" << endl;
		cout << endl;
		if( layered_layout ){
			mainGraph( PROJECT,GraphList,GraphList_S,namesForEachGraph,GenesNode,H_values,Xpos,Ypos,pos,bends,algorithmFlag,brandFlag,brandFlag2,ourMethodFlag,space,
				  increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher,G1,G2,TEMPINT,G,simplify,removeRat,sharedGenes,Hmax,edgesBetween,
                                  POS, BENDS, LAYERS, abbv, cat_num, Categories, width, hvalueWeighting, enrichmentWeighting_o, enrichmentWeighting_f, ppihitratioWeighting, categ );
		}
		else{
			if( ffd_layout ){
				mainGraph2( PROJECT,GraphList,GraphList_S,namesForEachGraph,GenesNode,H_values,Xpos,Ypos,pos,bends,algorithmFlag,brandFlag,brandFlag2,ourMethodFlag,space,
				  increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher,G1,G2,TEMPINT,G,simplify,removeRat,sharedGenes,Hmax,edgesBetween,
                                  POS, BENDS, LAYERS, abbv, cat_num, Categories, width, hvalueWeighting, enrichmentWeighting_o, enrichmentWeighting_f, ppihitratioWeighting, categ );
			}
		}
		cout << "/**************************************************/" << endl;
		cout << "\t" << " End Building High Level Graph " << endl;
		cout << "/**************************************************/" << endl;
	}
	return 0;
}
