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
*  Melih Sözdinler, melihsozdinler{at}gmail{dot}com, melih.sozdinler{at}boun{dot}edu{dot}tr 		*
*  Ahmet Emre Aladağ, aladagemre{at}gmail{dot}com, emre.aladag{at}stu{dot}khas{dot}edu{dot}tr      	*
*                                                                               			*
********************************************************************************************************/

#include "incl/handlers.h"
#include "incl/ehandlers.h"
#include "incl/handlers/_dataread.h"
#include "incl/handlers/_bicread.h"
#include "incl/handlers/_ppiread.h"
#include "incl/handlers/_maingraph.h"
#include "incl/bicalg/bimax/bimax.h"
#include "incl/bicalg/cc/cc.h"
#include "incl/bicalg/rleb/runleb.h"
#include "incl/handlers/_iniread.h"

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
                int maxBicSize = 2500; // maximum number of genes that bicluster can have
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
                bool circular_layout;
                bool circular_ffd_layout;
                bool is_ppi_directed;
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
        *	Color Method for journal paper
        **/
                bool hasColor = true;
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
        int colors[60][3] ={ {255,255,255}, {0,0,0},       {255,0,0},     {0,255,0},
                             {0,0,160},     {255,255,0},   {128,0,255},   {255,128,0},
                             {0,255,255},   {128,0,0},     {255,0,255},   {0,128,128},
                             {100,100,255}, {220,220,220}, {175,175,175}, {128,128,128},
                             {255,255,230}, {75,75,75},    {45,255,45},   {255,45,45},
                             {230,255,230}, {255,230,230}, {230,230,255}, {255,230,255},
                             {200,255,200}, {255,200,200}, {200,200,255}, {255,200,255},
                             {180,255,180}, {180,230,180}, {180,180,255}, {255,180,255},
                             {160,255,160}, {255,160,160}, {160,160,255}, {255,160,255},
                             {120,255,120}, {255,120,120}, {120,120,255}, {255,120,255},
                             {100,255,100}, {255,100,100}, {100,100,255}, {255,100,255},
                             {80,255,80},   {255,80,80},   {80,80,255},   {255,80,255},
                             {60,255,60},   {255,60,60},   {60,60,255},   {255,60,255},
                             {40,255,40},   {255,40,40},   {40,40,255},   {255,40,255},
                             {20,255,20},   {255,20,20},   {20,20,255},   {255,20,255} };
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
        array<list<char> > CategoriesXL;
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

int main( int argc, char **argv ){
        if( argc != 2 ){
            mainOntologyFlagCheckerEXP();
        }
	FILE *fptr;
        if( (fptr = fopen( "settings.yaml", "r" ) ) == NULL ){
		printf( "\n settings.yaml file can not be found\n" );
                settingsYamlChecker();
		return 0;
	}
	else{
                fclose( fptr );

                _iniReadCoExpression(    minBicSize,
                                         maxBicSize,
                                         width,
                                         space,
                                         increment,
                                         removeRat,
                                         nodeSize,
                                         edgeBendImp,
                                         colorScale,
                                         edgThicknessTher,
                                         dataName,
                                         dataName2,
                                         ppifilename,
                                         catfile,
                                         gofile,
                                         biclustering,
                                         bimax_low_dim1,
                                         bimax_low_dim2,
                                         ther_disc,
                                         bic_num_bimax,
                                         bic_num_cc,
                                         maxScore_,
                                         minHeight_,
                                         minWidth_,
                                         maxSizeSubMatrix_exp1_g,
                                         maxSizeSubMatrix_exp1_c,
                                         minSizeSubMatrix_exp1_g,
                                         minSizeSubMatrix_exp1_c,
                                         repeat,
                                         hvaluemin,
                                         increment_exp1_g,
                                         increment_exp1_c,
                                         ownFlag,
                                         lebFlag,
                                         ccFlag,
                                         msbeFlag,
                                         bimaxFlag,
                                         rlebFlag,
                                         brandFlag,
                                         brandFlag2,
                                         ourMethodFlag,
                                         ledaPostFlag,
                                         readOption,
                                         layered_layout,
                                         go_info,
                                         edgesBetween,
                                         sharedGenes,
                                         hvalueWeighting,
                                         enrichmentWeighting_o,
                                         enrichmentWeighting_f,
                                         ppihitratioWeighting,
                                         ffd_layout,
                                         circular_layout,
                                         circular_ffd_layout,
                                         is_ppi_directed
                );               
                if( hasColor == true ){
                        if( strcmp( argv[1], "-f" ) == 0 ){

                            cout << "/**************************************************/" << endl;
                            cout << "\t" << " Color Processing based on Molecular Function" << endl;
                            cout << "/**************************************************/" << endl;
                            char molecularF[18][128] = {
                                        "antioxidant activity",
                                        "binding",
                                        "catalytic activity",
                                        "channel regulator activity",
                                        "chemoattractant activity",
                                        "chemorepellent activity",
                                        "electron carrier activity",
                                        "enzyme regulator activity",
                                        "metallochaperone activity",
                                        "molecular transducer activity",
                                        "nutrient reservoir activity",
                                        "protein binding transcription factor activity",
                                        "protein tag",
                                        "sequence-specific DNA binding transcription factor activity",
                                        "structural molecule activity",
                                        "transcription regulator activity",
                                        "translation regulator activity",
                                        "transporter activity"
                            };
                            colorHandling( catfile, gofile, molecularF, 18 );
                        }
                        else{
                            if( strcmp( argv[1], "-l" ) == 0 ){
                                cout << "/**************************************************/" << endl;
                                cout << "\t" << " Color Processing based on Cellular Component" << endl;
                                cout << "/**************************************************/" << endl;
                                char molecularF[13][128] = {
                                            "cell",
                                            "cell part",
                                            "extracellular region",
                                            "extracellular region part",
                                            "macromolecular complex",
                                            "membrane-enclosed lumen",
                                            "organelle",
                                            "organelle part",
                                            "symplast",
                                            "synapse",
                                            "synapse part",
                                            "virion",
                                            "virion part"
                                };
                                colorHandling( catfile, gofile, molecularF, 13 );
                            }
                            else{
                                if( strcmp( argv[1], "-p" ) == 0 ){
                                    cout << "/**************************************************/" << endl;
                                    cout << "\t" << " Color Processing based on Biological Process " << endl;
                                    cout << "/**************************************************/" << endl;
                                    char molecularF[33][128] = {
                                                "biological adhesion",
                                                "biological regulation",
                                                "carbohydrate utilization",
                                                "carbon utilization",
                                                "cell killing",
                                                "cell proliferation",
                                                "cellular component organization or biogenesis",
                                                "cellular process",
                                                "death",
                                                "developmental process",
                                                "establishment of localization",
                                                "growth",
                                                "immune system process",
                                                "localization",
                                                "locomotion",
                                                "metabolic process",
                                                "multi-organism process",
                                                "multicellular organismal process",
                                                "negative regulation of biological process",
                                                "nitrogen utilization",
                                                "phosphorus utilization",
                                                "pigmentation",
                                                "positive regulation of biological process",
                                                "regulation of biological process",
                                                "reproduction",
                                                "reproductive process",
                                                "response to stimulus",
                                                "rhythmic process",
                                                "signaling",
                                                "signaling process",
                                                "sugar utilization",
                                                "sulfur utilization",
                                                "viral reproduction"
                                    };
                                    colorHandling( catfile, gofile, molecularF, 33 );
                                }
                                else{
                                    if( strcmp( argv[1], "-o" ) == 0 ){
                                        cout << "/**************************************************/" << endl;
                                        cout << "\t" << " Color Processing based on Wholo Ontology " << endl;
                                        cout << "/**************************************************/" << endl;
                                        char molecularF[33][128] = {
                                        "biological regulation",
                                        "cellular component organization or biogenesis",
                                        "cellular process",
                                        "developmental process",
                                        "localization",
                                        "metabolic process",
                                        "multi-organism process",
                                        "multicellular organismal process",
                                        "negative regulation of biological process",
                                        "positive regulation of biological process",
                                        "regulation of biological process",
                                        "reproductive process",
                                        "response to stimulus",
                                        "signaling process",
                                        "cell part",
                                        "extracellular region part",
                                        "macromolecular complex",
                                        "membrane-enclosed lumen",
                                        "organelle part",
                                        "symplast",
                                        "synapse",
                                        "synapse part",
                                        "virion part",
                                        "binding",
                                        "catalytic activity",
                                        "electron carrier activity",
                                        "enzyme regulator activity",
                                        "metallochaperone activity",
                                        "molecular transducer activity",
                                        "nutrient reservoir activity",
                                        "structural molecule activity",
                                        "transcription regulator activity",
                                        "transporter activity"
                                        };
                                        colorHandling( catfile, gofile, molecularF, 33 );
                                    }
                                    else{
                                        cout << "/**************************************************/" << endl;
                                        cout << "\t" << " Color Processing" << endl;
                                        cout << "/**************************************************/" << endl;
                                        colorHandling( catfile, gofile );
                                    }
                                }
                            }
                        }
                }
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
                                   if( hasColor != true ){
                                       fscanf( fptr, "%s%c", tc_array, &cat_c );
                                       fprintf( kfptr, "%s\t%c\n", tc_array, cat_c );
                                   }
                                   else{
                                       char cat_c_a[64];
                                       fscanf( fptr, "%s%s", tc_array, cat_c_a );
                                       fprintf( kfptr, "%s\t%s\n", tc_array, cat_c_a );
                                   }
                               }
                               fclose(kfptr);
                               fclose(fptr);
                        }
                        else{
                           geneFunctionFileChecker();
                        }
                    }
                    else{
                        functionsFileChecker();
                    }
                }
                else{
                    categoryFileChecker();
                }

                char defaultBicFile[256];

		if( readOption == false ){
			INPUT = dataRead( dataName );
		}
		else{
                        FILE *defaultRunTo;
                        char chr;
			INPUT = dataRead( dataName2, geneArray, condArray );
                        if( rlebFlag == true )
                            minHvalueErrorChecker( INPUT, hvaluemin );
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
                         leda::string FILENAME( "%s", dataName2 );
                         if( FILENAME.contains( "Yeast" ) ){
#ifdef LINUX
                             sprintf( defaultBicFile, "%s%s%s", "sources/bicluster_sources/biclusteringresult", "Yeast" ,".txt" );
#else
                             sprintf( defaultBicFile, "%s%s%s", "sources//bicluster_sources//biclusteringresult", "Yeast" ,".txt" );
#endif
                         }
                         if( FILENAME.contains( "Thaliana" ) ){
#ifdef LINUX
                             sprintf( defaultBicFile, "%s%s%s", "sources/bicluster_sources/biclusteringresult", "Thaliana" ,".txt" );
#else
                             sprintf( defaultBicFile, "%s%s%s", "sources//bicluster_sources//biclusteringresult", "Thaliana" ,".txt" );
#endif
                         }

                         //cout << "\nDONE\n";
	// 		drawHeatmap( INPUT, geneArray, condArray, "outputs/heatmap/out.html" );
		}
                char algName[64];
		if( bimaxFlag == true ){
			dataWriteBimax( INPUT, bimax_low_dim1, bimax_low_dim2, ther_disc );
			biclustering = 2;
                        if( bimax_low_dim1 > INPUT.dim1() || bimax_low_dim2 > INPUT.dim2() ){
                            sprintf( algName, "BIMAX" );
                            dimensionChecker( algName );
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
                                    sprintf( algName, "ChengAndChurch" );
                                    dimensionChecker( algName );
                                }
				biclustering = 3;
				#ifdef LINUX
                                        ccMain( "src/cpp/incl/bicalg/cc/matrix_robin.txt", INPUT.dim2(), INPUT.dim1(), bic_num_cc, maxScore_, minHeight_, minWidth_ );
				#else
                                        ccMain( "src//cpp//incl//bicalg//cc//matrix_robin.txt", INPUT.dim2(), INPUT.dim1(), bic_num_cc, maxScore_, minHeight_, minWidth_ );
				#endif
			}else{
				if( rlebFlag == true ){
                                        if( maxSizeSubMatrix_exp1_g > INPUT.dim1() || minSizeSubMatrix_exp1_g > INPUT.dim1()  || minSizeSubMatrix_exp1_c > INPUT.dim2() || maxSizeSubMatrix_exp1_c > INPUT.dim2() || maxSizeSubMatrix_exp1_g < minSizeSubMatrix_exp1_g || maxSizeSubMatrix_exp1_c < minSizeSubMatrix_exp1_c ){
                                            sprintf( algName, "REAL" );
                                            dimensionChecker( algName );
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
//#ifdef LINUX
//                                         defaultRunFrom = fopen( "sources/ppi_sources/genefunctions.txt", "r" );
//                                         defaultRunTo = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "w" );
//#else
//                                         defaultRunFrom = fopen( "sources//ppi_sources//genefunctions.txt", "r" );
//                                         defaultRunTo = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "w" );
//#endif
//                                         while( !feof( defaultRunFrom )){
//                                             fscanf( defaultRunFrom, "%c", &chr );
//                                             fprintf( defaultRunTo, "%c", chr );
//                                         }
//                                         fclose( defaultRunFrom );
//                                         fclose( defaultRunTo );
//#ifdef LINUX
//                                         defaultRunFrom = fopen( "sources/ppi_sources/functions.txt", "r" );
//                                         defaultRunTo = fopen( "sources/usr_sources/visualization_data/functions.txt", "w" );
//#else
//                                         defaultRunFrom = fopen( "sources//ppi_sources//functions.txt", "r" );
//                                         defaultRunTo = fopen( "sources//usr_sources//visualization_data//functions.txt", "w" );
//#endif
//                                         while( !feof( defaultRunFrom )){
//                                             fscanf( defaultRunFrom, "%c", &chr );
//                                             fprintf( defaultRunTo, "%c", chr );
//                                         }
//                                         fclose( defaultRunFrom );
//                                         fclose( defaultRunTo );
//                                         //cout << "\nDONE\n";
                                }
			}
		}
                if( hasColor == false )
                    bicRead( INPUT, defaultBicFile, biclusters, categ, matrixBiclusters, H_values, Hmax, minBicSize, maxBicSize, biclustering, INPUT.dim1(), INPUT.dim2() );
                else
                    bicRead( INPUT, defaultBicFile, biclusters, categ, matrixBiclusters, H_values, Hmax, minBicSize, maxBicSize, biclustering, INPUT.dim1(), INPUT.dim2(), hasColor );
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
                INTERACTIONS.write_gml( "whole.gml" );
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Produce Bicluster Graphs" << endl;
		cout << "/**************************************************/" << endl;
                if( hasColor == false ){
                    Categories.resize( INTERACTIONS.number_of_nodes() );
                    node n;
                    for( int i = 0; i < INTERACTIONS.number_of_nodes(); i++ )
                        Categories[ i ] = 'X';
                }
                else{
                    CategoriesXL.resize( INTERACTIONS.number_of_nodes() );
//                    node n;
//                    for( int i = 0; i < INTERACTIONS.number_of_nodes(); i++ )
//                        CategoriesXL[ i ].append('X');
                }
		listOfGraphs.resize( 0, biclusters.size() );

//                uncomment to see genes
//                list_item it1,it2;
//                forall_items( it1, biclusters ){
//                    forall_items( it2, biclusters[it1] ){
//                        cout << biclusters[it1][it2].GENE << " ";
//                    }
//                    cout << endl;
//                }

                if( hasColor == false ){
                    inpGraphProdHandling( G, listOfGraphs, abbv, Categories, temp, GenesNode, INTERACTIONS, TEMPINT, biclusters, cat_num );
                }
                else{
                    inpGraphProdHandling( G, listOfGraphs, abbv, CategoriesXL, temp, GenesNode, INTERACTIONS, TEMPINT, biclusters, cat_num );
                    list_item it;
//                    for( int i = 0; i < INTERACTIONS.number_of_nodes(); i++ ){
//                        forall_items( it, CategoriesXL[ i ] ){
//                            cout << CategoriesXL[ i ][ it ] << "-";
//                        }
//                        cout << endl;
//                    }
                }
		cout << "/**************************************************/" << endl;
		cout << "\t" << " End Producing Bicluster Graphs" << endl;
		cout << "/**************************************************/" << endl;
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Running Layering Algorithm" << endl;
		cout << "/**************************************************/" << endl;

		if( layered_layout ){
			mainAlgHandlingForEachSubgraph( pos, bends, layers, GraphList, GraphList_S, Xpos, Ypos, POS, BENDS, LAYERS, PROJECT, GenesNode, listOfGraphs, namesForEachGraph, biclusters, width, algorithmFlag, space, ourMethodFlag, increment, ledaPostFlag );
		}
//                else{
//                        if( ffd_layout && hasColor == false ){
//                                cout << " pirasa************************************* \n";
//                                mainAlgHandlingForEachSubgraph2( pos, bends, layers, GraphList, GraphList_S, Xpos, Ypos, POS, BENDS, LAYERS, PROJECT, GenesNode, listOfGraphs, namesForEachGraph, biclusters, width, algorithmFlag, space, ourMethodFlag, increment, ledaPostFlag, abbv, cat_num, Categories );
//                        }
//                }
		cout << "/**************************************************/" << endl;
		cout << "\t" << " End Running Layering Algorithm" << endl;
		cout << "/**************************************************/" << endl;
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Building High Level Graph " << endl;
		cout << "/**************************************************/" << endl;
		cout << endl;

		if( layered_layout ){
                    if( hasColor == false )
			mainGraph( PROJECT,GraphList,GraphList_S,namesForEachGraph,GenesNode,H_values,Xpos,Ypos,pos,bends,algorithmFlag,brandFlag,brandFlag2,ourMethodFlag,space,
				  increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher,G1,G2,TEMPINT,G,simplify,removeRat,sharedGenes,Hmax,edgesBetween,
                                  POS, BENDS, LAYERS, abbv, cat_num, Categories, width, hvalueWeighting, enrichmentWeighting_o, enrichmentWeighting_f, ppihitratioWeighting, categ );
                    else
                        mainGraph( PROJECT,GraphList,GraphList_S,namesForEachGraph,GenesNode,H_values,Xpos,Ypos,pos,bends,algorithmFlag,brandFlag,brandFlag2,ourMethodFlag,space,
                                  increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher,G1,G2,TEMPINT,G,simplify,removeRat,sharedGenes,Hmax,edgesBetween,
                                  POS, BENDS, LAYERS, abbv, cat_num, CategoriesXL, width, hvalueWeighting, enrichmentWeighting_o, enrichmentWeighting_f, ppihitratioWeighting, categ, hasColor );
		}
		else{
                        if( ffd_layout && hasColor == false ){
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
