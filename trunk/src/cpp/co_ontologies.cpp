 /*******************************************************************************************************/
/*	Copyright (C) <2010>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>   	 	       */
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
/*    Copyright (C) <2010>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 	       */
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

#include <LEDA/graphics/graphwin.h>
#include "incl/handlers.h"
#include "incl/ehandlers.h"
#include "incl/handlers/_dataread.h"
#include "incl/handlers/_bicread.h"
#include "incl/handlers/_ppiread.h"
#include "incl/handlers/_maingraph.h"
#include "incl/handlers/_iniread.h"
//#include "incl/bicalg/bimax/bimax.h"
//#include "incl/bicalg/cc/cc.h"
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
	* double removeRat this restricts the main layout to have less edges when the doube value increases
	**/
		double removeRat = 0.05;
	/**
	* variables for visualization clues
	**/
                double nodeSize=25.0;
		double edgeBendImp=35.0;
		double colorScale=130.0;
		double edgThicknessTher=3.0;
	/**
	* Specifies the weighting scheme for nodes
	**/
		bool hvalueWeighting = true;
		bool enrichmentWeighting_o = false;
		bool enrichmentWeighting_f = false;
		bool ppihitratioWeighting = false;
    /**
	* destination of input when readOption is false
	**/
	/**
	* decision variables for xcoordinate algorithms
	**/
		bool brandFlag = true, brandFlag2= false, ourMethodFlag = false, ledaPostFlag=false;
	/**
#ifdef LINUX
		char dataName[128] = "sources/usr_sources/microarray_data/input.txt";
#else
		char dataName[128] = "sources//usr_sources//microarray_data//input.txt";
#endif
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
	*	GO input file read or not, please specify if it is true at sources/usr_sources/vis...
	**/
#ifdef LINUX
                char inputGoFile[256] = "sources/usr_sources/visualization_data/cat_input.txt";
#else
                char inputGoFile[256] = "sources//usr_sources//visualization_data//cat_input.txt";
#endif
        /**
        *	Color Method for journal paper
        **/
                bool hasColor = true;
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
	char empty[32];
	int width = 10;       // Layering max width
	int space = 150;      // yspace between layers
	int increment = 125;  // xspace between nodes
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
        char ppifilename[256] = "sources/usr_sources/visualization_data/ppi.txt";
#else
        char ppifilename[256] = "sources//usr_sources//visualization_data//ppi.txt";
#endif
#ifdef LINUX
        char defaultGoFile[256] = "sources/usr_sources/visualization_data/go.txt";
#else
        char defaultGoFile[256] = "sources//usr_sources//visualization_data//go.txt";
#endif
	list<list<GENES> > categories;       
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

int main(int argc, char **argv){
        if( argc != 2 ){
            mainOntologyFlagCheckerONT();
        }
	FILE *fptr;
        if( (fptr = fopen( "settings.yaml", "r" ) ) == NULL ){
                printf( "\n Ini file can not be found\n" );
                settingsYamlChecker();
                return 0;
        }
        else{
                int bool_i;
                fclose( fptr );
                _iniReadCoFunctionality( removeRat,
                                         nodeSize,
                                         edgeBendImp,
                                         edgThicknessTher,
                                         colorScale,
                                         ppifilename,
                                         edgesBetween,
                                         sharedGenes,
                                         go_info,
                                         catfile,
                                         inputGoFile,
                                         defaultGoFile );

                FILE *kfptr;
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
                        colorHandling( catfile, defaultGoFile, molecularF, 18 );
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
                            colorHandling( catfile, defaultGoFile, molecularF, 13 );
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
                                colorHandling( catfile, defaultGoFile, molecularF, 33 );
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
                                    colorHandling( catfile, defaultGoFile, molecularF, 33 );
                                }
                                else{
                                    cout << "/**************************************************/" << endl;
                                    cout << "\t" << " Color Processing" << endl;
                                    cout << "/**************************************************/" << endl;
                                    colorHandling( catfile, defaultGoFile );
                                }
                            }
                        }
                    }
                }
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

                list<leda::matrix> matrixCategories;
                list<GENES> ppiGenes;

                interactionRead( temp, GenesNode, INTERACTIONS, TEMPINT, ppifilename );
// 		INTERACTIONS.print();
                goCatRead( inputGoFile, defaultGoFile, categories, categ, matrixCategories, H_values, Hmax, GenesNode, hasColor );
                cat_num = functionalCategoryFinder( categoriesOfGenes, abbv );

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
                    fprintf( cfptr, "%s %d %d %d\n","None", colors[colorCount][0], colors[colorCount][1], colors[colorCount][2] );
                    fclose( cfptr );
                }

                cout << "/**************************************************/" << endl;
                cout << "\t" << " Produce GO Graphs" << endl;
                cout << "/**************************************************/" << endl;
//                Categories.resize( INTERACTIONS.number_of_nodes() );
//                node n;
//                for( int i = 0; i < INTERACTIONS.number_of_nodes(); i++ )
//                    Categories[ i ] = 'X';
                listOfGraphs.resize( 0, categories.size() );
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
                listOfGraphs.resize( 0, categories.size() );
                if( hasColor == false ){
                    inpGraphProdHandling( G, listOfGraphs, abbv, Categories, temp, GenesNode, INTERACTIONS, TEMPINT, categories, cat_num );
                }
                else{
                    inpGraphProdHandling( G, listOfGraphs, abbv, CategoriesXL, temp, GenesNode, INTERACTIONS, TEMPINT, categories, cat_num );
                    list_item it;
//                    for( int i = 0; i < INTERACTIONS.number_of_nodes(); i++ ){
//                        forall_items( it, CategoriesXL[ i ] ){
//                            cout << CategoriesXL[ i ][ it ] << "-";
//                        }
//                        cout << endl;
//                    }
                }
//                inpGraphProdHandling( G, listOfGraphs, abbv, Categories, temp, GenesNode, INTERACTIONS, TEMPINT, categories, cat_num );


                for( int i = 0; i < categories.size(); i++ ){
                        list<node> A = listOfGraphs[ i ].all_nodes();
                        matrixCategories.append( graphToMatrix( listOfGraphs[ i ], A, A, true ) );
                        //graphToMatrix( listOfGraphs[ i ], A, A, true ).print();
                }

                for( int i=0; i < categories.size(); i++ ){
        #ifdef LINUX
                        char out3[ 64 ] = "outputs/heatmap/out";
        #else
                        char out3[ 64 ] = "outputs//heatmap//out";
        #endif
                        sprintf( out3, "%s%d.txt", out3, i );
                        if( matrixCategories[ matrixCategories.get_item( i )].dim1() < 100 )
                                produceHeatmapInput( matrixCategories[ matrixCategories.get_item( i )], categories[ categories.get_item( i ) ], categories[ categories.get_item( i ) ], out3 );
                }

                list_item it;
                matrix newI2;
                Hmax = 0;
                forall_items( it , matrixCategories ){
                        char filename2[1024] = "";
                        double count_i = 0;
                        newI2 = matrixCategories[ it ];
                        double rs_value;
                        double bagMean = 0;
                        for( int bag1_int = 0; bag1_int <  newI2.dim1(); bag1_int++ ){
                                for( int bag2_int = 0; bag2_int <  newI2.dim2(); bag2_int++ ){
                                        bagMean += newI2( bag1_int, bag2_int );
                                        if( newI2( bag1_int, bag2_int ) > 0 )
                                            count_i+=1.0;
                                }
                        }
                        if( count_i > 10 )
                            bagMean = sqrt(newI2.dim1()) * count_i / (double)( newI2.dim1() * newI2.dim2() );
                        else
                            if( count_i != 0 )
                                bagMean = 0.01;
                            else
                                bagMean = 0;
                        H_values.append( bagMean );
                        if( Hmax < bagMean )
                                Hmax = bagMean;
//                        cout << count_i << " - " << bagMean << endl;
                }

                cout << "/**************************************************/" << endl;
                cout << "\t" << " End Producing GO Graphs" << endl;
                cout << "/**************************************************/" << endl;
                cout << "/**************************************************/" << endl;
                cout << "\t" << " Running Layering Algorithm" << endl;
                cout << "/**************************************************/" << endl;

                mainAlgHandlingForEachSubgraph2( pos, bends, layers, GraphList, GraphList_S, Xpos, Ypos, POS, BENDS, LAYERS, PROJECT, GenesNode, listOfGraphs, namesForEachGraph, categories, width, algorithmFlag, space, ourMethodFlag, increment, ledaPostFlag, abbv, cat_num );
                cout << "\n Main Graph has " << PROJECT.number_of_nodes() << endl;
                cout << "\n*******************************\n";
//                int count_i = 0;
//                forall_items( it, GraphList ){
//                    cout << endl << count_i << " - " << GraphList[it].number_of_nodes() << " - " << GraphList[it].number_of_edges() << endl;
//                    count_i++;
//                }
                cout << "\n*******************************\n";
                cout << "/**************************************************/" << endl;
                cout << "\t" << " End Running Layering Algorithm" << endl;
                cout << "/**************************************************/" << endl;
                cout << "/**************************************************/" << endl;
                cout << "\t" << " Building High Level Graph " << endl;
                cout << "/**************************************************/" << endl;
                cout << endl;

                node n;
                if( hasColor == false )
                    mainGraph2( PROJECT,GraphList,GraphList_S,namesForEachGraph,GenesNode,H_values,Xpos,Ypos,pos,bends,algorithmFlag,brandFlag,brandFlag2,ourMethodFlag,space,
                                        increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher,G1,G2,TEMPINT,G,simplify,removeRat,sharedGenes,Hmax,edgesBetween,
                    POS, BENDS, LAYERS, abbv, cat_num, Categories, width, hvalueWeighting, enrichmentWeighting_o, enrichmentWeighting_f, ppihitratioWeighting, categ );
                else{
                    hvalueWeighting = false;
                    mainGraph2( PROJECT,GraphList,GraphList_S,namesForEachGraph,GenesNode,H_values,Xpos,Ypos,pos,bends,algorithmFlag,brandFlag,brandFlag2,ourMethodFlag,space,
                                        increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher,G1,G2,TEMPINT,G,simplify,removeRat,sharedGenes,Hmax,edgesBetween,
                    POS, BENDS, LAYERS, abbv, cat_num, CategoriesXL, width, hvalueWeighting, enrichmentWeighting_o, enrichmentWeighting_f, ppihitratioWeighting, categ );
                }
                cout << "/**************************************************/" << endl;
                cout << "\t" << " End Building High Level Graph " << endl;
                cout << "/**************************************************/" << endl;
        }
	return 0;
}
