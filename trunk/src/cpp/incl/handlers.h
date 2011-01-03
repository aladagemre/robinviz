/*****************************************************************************************/
/*	Copyright (C) <2010>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>   	 */
/*											 */
/*    This program is free software: you can redistribute it and/or modify		 */
/*    it under the terms of the GNU General Public License as published by		 */
/*    the Free Software Foundation, either version 3 of the License, or			 */
/*    (at your option) any later version.						 */
/*											 */
/*    This program is distributed in the hope that it will be useful,			 */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of			 */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			 */
/*    GNU General Public License for more details.					 */
/*											 */
/*    You should have received a copy of the GNU General Public License			 */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.		 */
/*											 */
/*    Copyright (C) <2010>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.         */
/*    This is free software, and you are welcome to redistribute it                      */
/*    under certain conditions; type `show c' for details.			         */
/*****************************************************************************************/
/*****************************************************************************************

Created

18 October 2009, by Melih

*****************************************************************************************/

#include "layout/interpretbiclusters.h"
#include "layout/runFunctions.h"
#include "layout/proteinscreen.h"
// #include "kmeans.h"
#include "layout/misc/graphToMatrix.h"
#include "layout/misc/saveGraph.h"
#include "layout/misc/getbiclusters.h"
#include "othervis/heatmap.h"
#include "othervis/parallelCoordinate.h"

/** 
  ****************************************** STRUCTS *********************************************
**/
typedef struct Strings STRINGS;

struct catNames{
	char catName[256];
};

struct goNames{
	char goName[256];
};

typedef struct catNames CATNAMES;
typedef struct goNames GONAMES;

struct geneonto{
	char genename[20];
	array<CATNAMES> categories;
	array<GONAMES> gos;
	int index;
};

typedef struct geneonto GENEONTO;

/** 
  *************************************************************************************************************
  ***************************************     FUNCTION PROTOYPES    *******************************************
  *************************************************************************************************************
**/

/**
* Function that produce data structure of entries
* with size of the given input data.
* Each substructure has three attribute,
* GENENAME CATEGORY[] GO[]
* These resulting array of substructure will
* be processed by main program using leda::array<GENEONTO> variable
**/
array<GENEONTO> geneOntologyHandling( char gofile[256] ); 

/**
* Function that produce data structure of entries
* with size of the given input data.
* Each substructure has three attribute,
* GENENAME CATEGORY[] GO[]
* These resulting array of substructure will
* be processed by main program using leda::array<GENEONTO> variable
**/
array<GENEONTO> geneOntologyHandling( char gofile[256], array<GENES> &dataGenes, char filePath[256], list<two_tuple<CATNAMES,int> > &categories );

/**
* Function that produce data structure of entries
* with size of category queries. 
* Each substructure has three attribute,
* GENENAME CATEGORY[] GO[]
* These resulting array of substructure will
* be processed by main program using leda::array<GENEONTO> variable
**/
array<GENEONTO> geneOntologyHandling2( char gofile[256], list<CATNAMES> &inputCats, array<GENENAMES> &GenesNode,  list<two_tuple<CATNAMES,int> > &categories, list<list<GENES> > &categoryGenes );

/**
* goHandling is a function that handles the go input file
* and responds to the main program by having modifying the passed
* parameters.
* @param inputGoFile (char[256]) Go Category file to read
* @param defaultGoFile (char[256]) Go original file
* @param gocategories (leda::list<list<GENES>>)filled with the GENE names provided by the data file
* @param matrixCategories (leda::list<leda::matrix>)is used to score calculation we need this param to obtain
* @param Hmax (double) Hmax is a value that stores max H-value
*/
void goHandling( char inputGoFile[256], char defaultGoFile[256], list<list<GENES> > &gocategories, list<int> &categ, 
                 list<leda::matrix> &matrixCategories, list<double> &scores, double &Hmax, array<GENENAMES> &GenesNode, bool hasColor );

/*
  -----------------------------------------------------------------------------------------------------------
*/

/**
* cogFileHandling parse the given cogfile. It is called from cogHandling function.
* @param cogfile (char[256]) Go Category file to read
* @param categoryGenes (leda::list<list<GENES>>)filled with the GENE names provided by the data file
- @param GenesNode (array<GENENAMES>) stores gene names inside this array
*/
array<GENEONTO> cogFileHandling( char cogfile[256], char orgAbv[12], list<CATNAMES> &inputCats, array<GENENAMES> &GenesNode,  list<two_tuple<CATNAMES,int> > &categories, list<list<GENES> > &categoryGenes );

/**
* cogHandling is a function that handles the cog input file from COG's database
* and responds to the main program by having modifying the passed
* parameters.
* @param inputCogFile (char[256]) Go Category file to read
* @param defaultCogFile (char[256]) Go original file
* @param cogcategories (leda::list<list<GENES>>)filled with the GENE names provided by the data file
* @param matrixCategories (leda::list<leda::matrix>)is used to score calculation we need this param to obtain
* @param Hmax (double) Hmax is a value that stores max H-value
*/
void cogHandling( char inputCogFile[256], char defaultCogFile[256], list<list<GENES> > &cogcategories, list<int> &categ, 
	list<leda::matrix> &matrixCategories, list<double> &scores, double &Hmax, array<GENENAMES> &GenesNode );

/*
  -----------------------------------------------------------------------------------------------------------
*/


/**
* mainAlgHandlingForEachSubgraph is a function that handles the layering of
* peripheral graphs using weighted layered drawing concept
* @param abbv (char []) we need a categories for each organism. It may be too general as we did having n categories. abbv stores just a capital letter of categories.
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param LAYERS Layer Numbers of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param layers INTERACTION graph node layer numbers 
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
*/
void mainAlgHandlingForEachSubgraph( node_array<point> &pos, 
				     edge_array<list<point> > &bends, 
				     array<list<node> > &layers,
				     list<GRAPH<int,int> > &GraphList, 
				     list<GRAPH<leda::string,int> > &GraphList_S,
				     node_array<double> &Xpos,
				     node_array<double> &Ypos,
                                     list<node_array<point> > &POS,
				     list<edge_array<list<point> > > &BENDS,
				     list<array<list<node> > > &LAYERS,
                                     GRAPH<leda::string,int> &PROJECT,
				     array<GENENAMES> &GenesNode,
				     array<GRAPH<int,int> > &listOfGraphs,
				     list<list<Strings> > &namesForEachGraph,
				     list<list<GENES> > &biclusters,
				     int &width,
				     int algorithmFlag,
				     int space,
				     bool xCoordFlag,
				     int increment,
				     bool ledaPostFlag
				  );

/**
* mainAlgHandlingForEachSubgraph2 is a function that handles the force directed and circular layout of
* peripheral graphs
* @param abbv (char []) we need a categories for each organism. It may be too general as we did having n categories. abbv stores just a capital letter of categories.
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
*/
void mainAlgHandlingForEachSubgraph2( node_array<point> &pos, 
				     edge_array<list<point> > &bends, 
				     array<list<node> > &layers,
				     list<GRAPH<int,int> > &GraphList, 
				     list<GRAPH<leda::string,int> > &GraphList_S,
				     node_array<double> &Xpos,
				     node_array<double> &Ypos,
                                     list<node_array<point> > &POS,
				     list<edge_array<list<point> > > &BENDS,
				     list<array<list<node> > > &LAYERS,
                                     GRAPH<leda::string,int> &PROJECT,
				     array<GENENAMES> &GenesNode,
				     array<GRAPH<int,int> > &listOfGraphs,
				     list<list<Strings> > &namesForEachGraph,
				     list<list<GENES> > &biclusters,
				     int &width,
				     int algorithmFlag,
				     int space,
				     bool xCoordFlag,
				     int increment,
				     bool ledaPostFlag,
				     array<char> &abbv,
				     int cat_num
				  );

/*
  -----------------------------------------------------------------------------------------------------------
*/

/**
* mainGraphHandling is a function that handles the ISB 2010 submitted main layout on the 
* peripheral graphs
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
* @param removeRat(double) operational param 7; removes edge weights when they are smaller than this ratio
* @param sharedGenes(bool) operational param 8: options for having a methodology to use shared Genes options specifed in the paper
* @param max_weight(int) largets weight
* @param edgesBetween(bool) operational param 9: the second method rather than shared Genes expained in the paper
* @param hvalueWeighting(bool) Main graph weighting scheme according to H-values
* @param enrichmentWeighting_o(bool) Main graph weighting scheme according to defined categories
* @param enrichmentWeighting_f(bool) Main graph weighting scheme according to funcassociate results
* @param ppihitratioWeighting(bool) Main graph weighting scheme according to Hit Ratios
*/
GRAPH<int,int> mainGraphHandling( GRAPH<leda::string,int> &PROJECT,
				  node_array<int> &nodesOfProjectStr,
				  array<node> &nodesOfProjectInt,
				  list<GRAPH<int,int> > &GraphList,
				  list<GRAPH<leda::string,int> > &GraphList_S, 
				  list<list<Strings> > &namesForEachGraph,
				  array<GENENAMES> &GenesNode,
				  node_array<double> &HValues,
				  list<double> &H_values,
				  array<char> &abbv,
				  int cat_num,
				  node_array<double> &Xpos,
				  node_array<double> &Ypos,		
				  node_array<point> &pos,
				  edge_array<list<point> > &bends,
				  edge_array<int> &edgeNumbersForStr_,
				  array<edge> &edgeNumbersForInt_,
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
				  int max_weight,
				  bool edgesBetween,
				  bool hvalueWeighting,
				  bool enrichmentWeighting_o,
				  bool enrichmentWeighting_f,
				  bool ppihitratioWeighting,
				  list<int> &categ,
				  int width
				);

/**
* mainGraphHandling2 is a function that handles layout for GO categories
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
* @param removeRat(double) operational param 7; removes edge weights when they are smaller than this ratio
* @param sharedGenes(bool) operational param 8: options for having a methodology to use shared Genes options specifed in the paper
* @param max_weight(int) largets weight
* @param edgesBetween(bool) operational param 9: the second method rather than shared Genes expained in the paper
* @param hvalueWeighting(bool) Main graph weighting scheme according to H-values
* @param enrichmentWeighting_o(bool) Main graph weighting scheme according to defined categories
* @param enrichmentWeighting_f(bool) Main graph weighting scheme according to funcassociate results
* @param ppihitratioWeighting(bool) Main graph weighting scheme according to Hit Ratios
*/
GRAPH<int,int> mainGraphHandling2( GRAPH<leda::string,int> &PROJECT,
				  node_array<int> &nodesOfProjectStr,
				  array<node> &nodesOfProjectInt,
				  list<GRAPH<int,int> > &GraphList,
				  list<GRAPH<leda::string,int> > &GraphList_S, 
				  list<list<Strings> > &namesForEachGraph,
				  array<GENENAMES> &GenesNode,
				  node_array<double> &HValues,
				  list<double> &H_values,
				  array<char> &abbv,
				  int cat_num,
				  node_array<double> &Xpos,
				  node_array<double> &Ypos,		
				  node_array<point> &pos,
				  edge_array<list<point> > &bends,
				  edge_array<int> &edgeNumbersForStr_,
				  array<edge> &edgeNumbersForInt_,
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
				  int max_weight,
				  bool edgesBetween,
				  bool hvalueWeighting,
				  bool enrichmentWeighting_o,
				  bool enrichmentWeighting_f,
				  bool ppihitratioWeighting,
				  list<int> &categ,
				  int width
				);
/*
  -----------------------------------------------------------------------------------------------------------
*/

/**
    Does handling of obtaining colors via first level of Molecular Function Reads from gofile and
    finds protein to list of categories for each proteins and write into catfile where robinviz main
    reads from main file.
    File format first we write
    Cat Number generally 
    18 
    % then abrrv and category names
    A antioxidant activity
    .
    .
    .
    S transporter activity
    % then gene to category abbrvs.
    gene1 ABC
    gene2 PRS
    ...
    @param catfile(char[256]) :saved for post processing in bicluster and go handling
    @param gofile(char[256]) : needed for parsing and forming colors for each genes
**/
void colorHandling( char catfile[256], char gofile[256] );

/*
  -----------------------------------------------------------------------------------------------------------
*/

/**
* biclusterHandling is a function that handles the bicluster output
* and responds to the main program by having modifying the passed
* parameters.
* @param INPUT (leda::matrix) input matrix obtained from the given input file sources/data_sources
* @param biclusters (leda::list<list<GENES>>)filled with the GENE names provided by the data file
* @param matrixBiclusters (leda::list<leda::matrix>)is used H-value calculation we need this param to obtain submatrices of biclusters
* @param H-values (leda::list) are used in order to have a corresponding value obtained with the id of biclusters       
* @param Hmax (double) Hmax is a value that stores max H-value
* @param minBicSize to control bicluster sizes
* @param maxBicSize to control bicluster sizes
* @param biclustering (int) is for choosing different biclustering outputs, We have to do that since each alg. has different output from its tool.
* @param dimension1(int) size of the data row dimension
* @param dimension2(int) size of the data column dimension
* @param hasColor(bool) reimplementation when the new coloring methodology is used.
*/
void biclusterHandling( matrix &INPUT, char defaultBicFile[256], list<list<GENES> > &biclusters, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, int minBicSize, int maxBicSize, int biclustering, int dimension1, int dimension2, bool hasColor  );

/*
  -----------------------------------------------------------------------------------------------------------
*/

/**
* interactionHandling is a function that handles the PPI extraction from
* given file
* @param temp (leda::node_array<GENENAMES>) temp file for proceessing
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param INTERACTIONS (leda::GRAPH<int,int>) is PPI graph extracted from sources/ppi_sources/<ppiFile> 
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
*/
void interactionHandling( node_array<GENENAMES> &temp, array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, GRAPH<int,int> &TEMPINT, char ppiFile[256] );

/*
  -----------------------------------------------------------------------------------------------------------
*/


/**
* inpGraphProdHandling is a function that handles the production of
* peripheral graphs
* @param abbv (char []) we need a categories for each organism. It may be too general as we did having n categories. abbv stores just a capital letter of categories.
* @param Categories (leda::array<list<char> >) stores each categorie(s) for each genes rather than other inpGraphProdHandling several categs may be owned by each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param INTERACTIONS (leda::GRAPH<int,int>) is PPI graph extracted from sources/ppi_sources/<ppiFile> it is original graph obtained with interactionHandling
* @param biclusters we do not lose the track of bicluster genes
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu

*/
void inpGraphProdHandling( GRAPH<int,int> &G, array<GRAPH<int,int> > &listOfGraphs, array<char> &abbv,  array<list<char> > &Categories, node_array<GENENAMES> &temp, array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, GRAPH<int,int> &TEMPINT, list<list<GENES> > &biclusters, int cat_num );


/** 
  ***********************************************************************************************************
  *************************************** FUNCTION DEFINITIONS    *******************************************
  ***********************************************************************************************************
**/

/**
* Function that produce data structure of entries
* with size of the given input data.
* Each substructure has three attribute,
* GENENAME CATEGORY[] GO[]
* These resulting array of substructure will
* be processed by main program using leda::array<GENEONTO> variable
* and returned.
* @param  gofile ( char[256] ) 
**/
array<GENEONTO> geneOntologyHandling( char gofile[256] ){
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Parsing GO File as you wish" << endl;
		cout << "/**************************************************/" << endl;
		FILE *f;
		list<GENES> dataGenes;
		char line[ 1000000 ];
		char *pc,*pend,*go,*cat;
		const char *strDelim = " \t";
                const char *strDelim2 = " \t";
#ifdef LINUX
		char filePath[256] = "sources/usr_sources/visualization_data/funcassociate_go_associations.txt";
		char filePath2[256] = "sources/usr_sources/visualization_data/genenames.txt";
#else
		char filePath[256] = "sources//usr_sources//visualization_data//funcassociate_go_associations.txt";
		char filePath2[256] = "sources//usr_sources//visualization_data//genenames.txt";
#endif
		sprintf( filePath, "%s", gofile );
		if( (f = fopen( filePath2, "r" )) == NULL){
			FILE *erptr;
#ifdef LINUX
			erptr = fopen( "outputs/error.txt", "w" );
#else
			erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "Error-301: You did not specify gene name file, sources/usr_sources/visualization_data/genenames.txt \n" );
			fclose( erptr );
                        cout << "\nError-301: You did not specify gene name file, sources/usr_sources/visualization_data/genenames.txt \n";
			exit(1);
		}else{
			while( !feof( f ) ){
				GENES tempg;
				fscanf( f, "%s", tempg.GENE );
				dataGenes.append( tempg );
			}
		}
                fclose( f );
		array<GENEONTO> inputGenes( dataGenes.size()+1 );
		for( int i = 0; i < dataGenes.size(); i++ ){
			inputGenes[ i ].index = 0;
                        inputGenes[ i ].categories.resize( 10 );
                        inputGenes[ i ].gos.resize( 10 );
			for( int k = 0; k < 10; k++ ){
				sprintf( inputGenes[ i ].categories[k].catName, "%s", "" );
				sprintf( inputGenes[ i ].gos[k].goName, "%s", "" );
			}
			sprintf( inputGenes[ i ].genename, "%s", dataGenes[ dataGenes.get_item( i ) ].GENE );
			
                }
		if( (f = fopen( filePath, "r" )) == NULL){
			FILE *erptr;
#ifdef LINUX
			erptr = fopen( "outputs/error.txt", "w" );
#else
			erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "Error 302: You did not specify GO based functional category file although you selected to use that file\n" );
			fclose( erptr );
                        cout << "\nError 302: You did not specify GO based functional category file although you selected to use that file\n";
			exit(1);
		}

		int line_i = 0;
		while( !feof( f ) ){
			fgets( line, 1000000, f );
//cout << line_i << endl;
			line_i++;
		}
		cout << "\t Will Parse " << line_i << " lines, Parsing begins...\n";
		rewind( f );
		line_i = 0;
		while( !feof( f ) ){
			// count rows
			fgets( line, 1000000, f );
//                        cout << "-" << line << endl;
			pc = strtok( line, strDelim );

//                        cout << line_i << " : " << pc << endl;
			go = pc;
			if( feof( f ) )
				break;
			int count = 0;
			while( pc != NULL ){
				if( count == 0 ){
					pc = strtok( NULL, strDelim );
					cat = pc;
// 					cout << cat << endl;
				}
				else{
					pc = strtok( NULL, strDelim2 );
//                                        cout << pc << " ";
					for( int i = 0; i < dataGenes.size(); i++ ){
                                                if( strcmp( pc, inputGenes[ i ].genename ) == 0 && inputGenes[ i ].index < 10 ){
							sprintf( inputGenes[ i ].categories[ inputGenes[ i ].index ].catName, "%s", cat );
							sprintf( inputGenes[ i ].gos[ inputGenes[ i ].index++ ].goName, "%s", go );
//                                                        cout << "|";
							break;
						}
					}
				}
				//cout << pc << "\t";
				pend=pc+strlen(pc)-1; /* check last char for newline terminator */
				if (*pend=='\n') 
					pc = NULL;
				count++;
			}
			line_i++;
                        if( line_i % 1 == 0 )
				cout << "\n Line " << line_i << " is parsed ";
		}
		cout << "\n/**************************************************/" << endl;
		cout << "\t" << " Parsing GO File ends " << endl;
		cout << "/**************************************************/" << endl;
                fclose( f );
		return inputGenes;
}

/**
* Function that produce data structure of entries
* with size of the given input data.
* Each substructure has three attribute,
* GENENAME CATEGORY[] GO[]
* These resulting array of substructure will
* be processed by main program using leda::array<GENEONTO> variable
**/
array<GENEONTO> geneOntologyHandling( char gofile[256], array<GENES> &dataGenes, char filePath[256], list<two_tuple<CATNAMES,int> > &categories ){
		cout << "/**************************************************/" << endl;
                cout << "\t" << " Parsing GO File" << endl;
		cout << "/**************************************************/" << endl;
		FILE *f;
		char line[ 1000000 ];
		char *pc,*pend,*go,*cat;
                const char *strDelim = " \t";
		const char *strDelim2 = " ";

		array<GENEONTO> inputGenes( dataGenes.size()+1 );
		for( int i = 0; i < dataGenes.size(); i++ ){
			inputGenes[ i ].index = 0;
                        inputGenes[ i ].categories.resize( 10 );
                        inputGenes[ i ].gos.resize( 10 );
			for( int k = 0; k < 10; k++ ){
				sprintf( inputGenes[ i ].categories[k].catName, "%s", "" );
				sprintf( inputGenes[ i ].gos[k].goName, "%s", "" );
			}
			sprintf( inputGenes[ i ].genename, "%s", dataGenes[ i ].GENE );			
		}

		if( (f = fopen( filePath, "r" )) == NULL){
			FILE *erptr;
#ifdef LINUX
			erptr = fopen( "outputs/error.txt", "w" );
#else
			erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "Error 303: You did not specify GO based functional category file although you selected to use that file\n" );
			fclose( erptr );
                        cout << "\nError 303: You did not specify GO based functional category file although you selected to use that file\n";
			exit(1);
		}

                int line_i = 0;
//		while( !feof( f ) ){
//			fgets( line, 100000, f );
//			line_i++;
//		}
//		cout << "\t Will Parse " << line_i << " lines, Parsing begins...\n";
                rewind( f );
//		line_i = 0;
		while( !feof( f ) ){
			// count rows
			two_tuple<CATNAMES,int> tup;
			tup.second() = 0;
			fgets( line, 1000000, f );
                        cout << line << endl;
			pc = strtok( line, strDelim );
			go = pc;
			if( feof( f ) )
				break;
			int count = 0;
			while( pc != NULL ){
				if( count == 0 ){
					pc = strtok( NULL, strDelim );
					cat = pc;
					sprintf( tup.first().catName, "%s", cat );
// 					cout << cat << endl;
				}
				else{
					pc = strtok( NULL, strDelim2 );
					tup.second()++;
					for( int i = 0; i < dataGenes.size(); i++ ){
                                                if( strcmp( pc, inputGenes[ i ].genename ) == 0 && inputGenes[ i ].index < 10 ){
							sprintf( inputGenes[ i ].categories[ inputGenes[ i ].index ].catName, "%s", cat );
							sprintf( inputGenes[ i ].gos[ inputGenes[ i ].index++ ].goName, "%s", go );
                                                        cout << "|";
							break;
						}
					}
				}
				//cout << pc << "\t";
				pend=pc+strlen(pc)-1; /* check last char for newline terminator */
				if (*pend=='\n') 
					pc = NULL;
				count++;
			}
			line_i++;
                        if( line_i % 1 == 0 )
				cout << "\n Line " << line_i << " is parsed ";
			categories.append( tup );
		}
		cout << "\n/**************************************************/" << endl;
		cout << "\t" << " Parsing GO File ends " << endl;
		cout << "/**************************************************/" << endl;
		fclose( f );
		return inputGenes;
}

array<GENEONTO> geneOntologyHandling2( char gofile[256], list<CATNAMES> &inputCats, array<GENENAMES> &GenesNode,  list<two_tuple<CATNAMES,int> > &categories, list<list<GENES> > &categoryGenes ){
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Parsing GO File as you wish" << endl;
		cout << "/**************************************************/" << endl;
		FILE *f;
		list<GENES> tempGenes;
		char line[ 1000000 ];
		char *pc,*pend,*go,*cat;
                const char *strDelim = "\t";
                const char *strDelim2 = " \t";

		for( int i = 0; i < inputCats.size(); i++ ){
			categoryGenes.append( tempGenes );
		}

		array<GENEONTO> inputGenes( GenesNode.size()+1 );
		for( int i = 0; i < GenesNode.size(); i++ ){
			inputGenes[ i ].index = 0;
                        inputGenes[ i ].categories.resize( 25 );
                        inputGenes[ i ].gos.resize( 25 );
			sprintf( inputGenes[ i ].genename, "%s", GenesNode[ i ].GENE );
			for( int k = 0; k < 25; k++ ){
				sprintf( inputGenes[ i ].categories[k].catName, "%s", "" );
				sprintf( inputGenes[ i ].gos[k].goName, "%s", "" );
			}
		}

		if( (f = fopen( gofile, "r" )) == NULL){
			FILE *erptr;
#ifdef LINUX
			erptr = fopen( "outputs/error.txt", "w" );
#else
			erptr = fopen( "outputs//error.txt", "w" );
#endif
                        fprintf( erptr, "Error 304: You did not specify GO based functional category file although you selected to use that file\n" );
			fclose( erptr );
                        cout << "\nError 304: You did not specify GO based functional category file although you selected to use that file\n";
			exit(1);
		}

		int line_i = 0;
		while( !feof( f ) ){
			fgets( line, 1000000, f );
//                        cout << line << endl;
			line_i++;
		}
		cout << "\t Will Parse " << line_i << " lines, Parsing begins...\n" << "\n";
		rewind( f );
		line_i = 0;
		while( !feof( f ) ){
			// count rows
			two_tuple<CATNAMES,int> tup;
			tup.second() = 0;
                        fgets( line, 1000000, f );
			pc = strtok( line, strDelim );
			go = pc;
			if( feof( f ) )
				break;
			int count = 0;
			int foundedone = -1;
			while( pc != NULL ){
				if( count == 0 ){
					pc = strtok( NULL, strDelim );
                                        sprintf( tup.first().catName, "%s", pc );
					CATNAMES cats;
					sprintf( cats.catName, "%s", pc );
					cat = pc;
					list_item catit;
					int count_cat = 0;
					forall_items( catit, inputCats ){
						int max = strlen( inputCats[ catit ].catName ) > strlen( cats.catName ) ? strlen( inputCats[ catit ].catName ) : strlen( cats.catName );
						max = max - 1;
						if( strncmp( inputCats[ catit ].catName, cats.catName, max ) == 0 ){
// 							cout << "/**************************************************/" << endl;
// 							cout << inputCats[ catit ].catName << "\n" << cats.catName << endl;
// 							cout << "/**************************************************/" << endl;
							foundedone = count_cat;
						}
						count_cat++;
					}
				}
				else{
					pc = strtok( NULL, strDelim2 );
					tup.second()++;
					for( int i = 0; i < inputGenes.size(); i++ ){
						if( strcmp( pc, inputGenes[ i ].genename ) == 0 && inputGenes[ i ].index < 25 ){
							sprintf( inputGenes[ i ].categories[ inputGenes[ i ].index ].catName, "%s", cat );
							sprintf( inputGenes[ i ].gos[ inputGenes[ i ].index++ ].goName, "%s", go );
							break;
						}
					}
					if( foundedone != -1 ){
						GENES now;
						sprintf( now.GENE, "%s", pc );
						categoryGenes[ categoryGenes.get_item(foundedone) ].append( now );
//                                                cout << pc << "\t";
					}
				}
//                                cout << pc << "\t";
				pend=pc+strlen(pc)-1; /* check last char for newline terminator */
				if (*pend=='\n') 
                                        pc = NULL;
				count++;
			}
			line_i++;
			if( line_i % 1 == 0 )
				cout << "\n Category " << line_i << " is parsed ";
			categories.append( tup );
		}
		cout << "\n/**************************************************/" << endl;
		cout << "\t" << " Parsing GO File ends " << endl;
		cout << "/**************************************************/" << endl;
		fclose( f );
		return inputGenes;
}

void geneOntologyToBiclusterHandling( list<list<GENES> > &biclusters, array<GENEONTO> &inputGenes ){
	list_item it,it2;
	list<GENES> temp;
	char filename[256];
        int i = 0,count=1,j = 0;
	char c;
	FILE *fptr,*fptr2;

	forall_items( it, biclusters ){
		//cout << " Bic\n";
		temp = biclusters[ it ];
#ifdef LINUX
                sprintf( filename, "outputs/go/gobicluster%d.html", count - 1 );
#else
                sprintf( filename, "outputs//go//gobicluster%d.html", count - 1);
#endif		
		fptr = fopen( filename, "w" );

#ifdef LINUX
	    fptr2 = fopen( "outputs/go/src/header.txt", "r" );
#else
	    fptr2 = fopen( "outputs//go//src//header.txt", "r" );
#endif
	    while( !feof( fptr2 ) ){
		    fscanf( fptr2, "%c", &c );
		    fprintf( fptr, "%c", c );
	    }
	    fclose( fptr2 );

                fprintf( fptr, "\n\t<th scope=\"col\"> %s </th>\n", "Gene(Protein)" );
                fprintf( fptr, "\n\t<th scope=\"col\"> %s </th>\n", "Category Name" );
                fprintf( fptr, "\n\t<th scope=\"col\"> %s </th>\n", "Gene Ontology Id" );
                fprintf( fptr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">GO Id %d, can view html file at ouptuts/go/ </td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, count );
			
		forall_items( it2, temp ){
			for( i = 0; i < inputGenes.size(); i++ ){
				if( strcmp( temp[it2].GENE, inputGenes[ i ].genename ) == 0 ){
					fprintf( fptr, "\t<tr>\n" );
                                        if( inputGenes[ i ].index == 0 )
                                            inputGenes[ i ].index = 1;
					fprintf( fptr, "\t\t<td rowspan=%d>%s</td>\n", inputGenes[ i ].index, temp[it2].GENE );
					fprintf( fptr, "\t\t<td>%s</td>\n", inputGenes[ i ].categories[ 0 ].catName );
					fprintf( fptr, "\t\t<td>%s</td>\n", inputGenes[ i ].gos[ 0 ].goName );
					fprintf( fptr, "\t</tr>\n" );
					for( j = 1; j < inputGenes[ i ].index; j++ ){
						fprintf( fptr, "\t<tr>\n" );
						fprintf( fptr, "\t\t<td>%s</td>\n", inputGenes[ i ].categories[ j ].catName );
						fprintf( fptr, "\t\t<td>%s</td>\n", inputGenes[ i ].gos[ j ].goName );
						fprintf( fptr, "\t</tr>\n" );
					}
					break;
				}
			}
		}
		fprintf( fptr, "\t</tbody>\n</table>\n</center>\n</body>\n</html>" );
		fclose( fptr );
		count++;
		//cout << " Bic ends\n";
	}
	inputGenes.resize( 1 );
}

/**
* goHandling is a function that handles the go input file
* and responds to the main program by having modifying the passed
* parameters.
* @param inputGoFile (char[256]) Go Category file to read
* @param defaultGoFile (char[256]) Go original file
* @param gocategories (leda::list<list<GENES>>)filled with the GENE names provided by the data file
* @param matrixCategories (leda::list<leda::matrix>)is used to score calculation we need this param to obtain
* @param Hmax (double) Hmax is a value that stores max H-value
*/
void goHandling( char inputGoFile[256], char defaultGoFile[256], list<list<GENES> > &gocategories, list<int> &categ, 
                 list<leda::matrix> &matrixCategories, list<double> &scores, double &Hmax, array<GENENAMES> &GenesNode, bool hasColor ){
	FILE *fptr;
	char *pc;
	list<two_tuple<CATNAMES,int> > categories;
	list<CATNAMES> inputCats;
        list_item it,it2;

	if( (fptr = fopen( inputGoFile, "r") ) == NULL ){
		FILE *erptr;
		#ifdef LINUX
					erptr = fopen( "outputs/error.txt", "w" );
		#else
					erptr = fopen( "outputs//error.txt", "w" );
		#endif
                fprintf( erptr, "Error 305: You did not specify GO input file correctly\n" );
		fclose( erptr );
                cout << "\nError 305: You did not specify GO input file correctly\n";
		exit(1);
	}
	else{
		CATNAMES cats;
		int count = 0;
		while( !feof( fptr ) ){
			fgets( cats.catName, 256, fptr );
			pc = strtok( cats.catName, "\n" );
			sprintf( cats.catName, "%s", pc );
// 			cout << pc << endl;
			inputCats.append( cats );
			count++;
		}
	}	
// 	cout << "\nWEDONE\n";
	array<GENEONTO> geneOntoForData = geneOntologyHandling2( defaultGoFile, inputCats, GenesNode, categories, gocategories );
// 	cout << "\nWEDONE\n";
        //gocategories.del_item( gocategories.first_item() );
	geneOntologyToBiclusterHandling( gocategories, geneOntoForData );
        int catCount = 0;
        forall_items( it, gocategories ){
            FILE *saveGene;
            if( gocategories[it].size() >= 0 ){
//                    cout << catCount << " - \t";
                    char geneFile[ 1024 ];
#ifdef LINUX
                    sprintf( geneFile, "%s%d%s", "outputs/bicgenes/geneResult", catCount, ".txt" );
// 				cout << " created\n";
#else
                    sprintf( geneFile, "%s%d%s", "outputs//bicgenes//geneResult", catCount, ".txt" );
// 				cout << " created\n";
#endif
                    if( (saveGene = fopen( geneFile , "w" ) ) != NULL ){
                        forall_items( it2, gocategories[ it ] ){
//                            cout << gocategories[ it ][ it2 ].GENE << " ";
                            fprintf( saveGene, "%s\n", gocategories[ it ][ it2 ].GENE );
                        }
//                        cout << endl;
                    }
//                    cout << "\n**********************************\n";
            }
            catCount++;
        }
#ifdef LINUX
        FILE *yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "w" );
#else
        FILE *yFptr = fopen( "sources//usr_sources//visualization_data//genenames.txt", "w" );
#endif
        for( int i = 0; i < GenesNode.size(); i++ )
            fprintf( yFptr, "%s\n", GenesNode[i].GENE );
        fclose( yFptr );
        cout << "||||||||||||||||||||||||||||||||\n";
        char headerFileName[64];
        sprintf( headerFileName, "geneResult" );
        char algName[64] = "";
        if( hasColor )
            analyseGenes2( headerFileName, categ, gocategories.size(), algName, GenesNode.size(), 0, hasColor  );
        else
            analyseGenes2( headerFileName, categ, gocategories.size(), algName, GenesNode.size(), 0  );
        cout << "||||||||||||||||||||||||||||||||\n";
}


array<GENEONTO> cogFileHandling( char cogfile[256], char orgAbv[12], list<CATNAMES> &inputCats, array<GENENAMES> &GenesNode,  list<two_tuple<CATNAMES,int> > &categories, list<list<GENES> > &categoryGenes ){
	cout << "/**************************************************/" << endl;
	cout << "\t" << " Parsing COG File as you wish" << endl;
	cout << "/**************************************************/" << endl;
	FILE *f;
	list<GENES> tempGenes;
	char line[ 1000 ];
	char read[ 256 ];
	char *pc,*pend,*go,*cogid,*cogfunct,cat[512];
	const char *strDelim = "\t";
	const char *strDelim2 = " ";

	for( int i = 0; i < inputCats.size(); i++ ){
		categoryGenes.append( tempGenes );
	}

	array<GENEONTO> inputGenes( GenesNode.size()+1 );
	for( int i = 0; i < GenesNode.size(); i++ ){
		inputGenes[ i ].index = 0;
		inputGenes[ i ].categories.resize( 100 );
		inputGenes[ i ].gos.resize( 100 );
		sprintf( inputGenes[ i ].genename, "%s", GenesNode[ i ].GENE );
	}

	if( (f = fopen( cogfile, "r" )) == NULL){
		FILE *erptr;
#ifdef LINUX
		erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
                fprintf( erptr, "Error 306: You did not specify COG based functional category file although you selected to use that file\n" );
		fclose( erptr );
                cout << "\nError 306: You did not specify COG based functional category file although you selected to use that file\n";
		exit(1);
	}

	int line_i = 0;
	while( !feof( f ) ){
		fgets( line, 1000, f );
		if( strcmp( line, "_______" ) == 0 )
		  line_i++;
	}
	cout << "\t Will Parse " << line_i << " cog categories, Parsing begins...\n" << "\n";
	rewind( f );
	line_i = 0;
	while( !feof( f ) ){
		// count rows
		two_tuple<CATNAMES,int> tup;
		tup.second() = 0;
		fgets( line, 1000, f );
		pc = strtok( line, strDelim2 );
		cogfunct = pc;
		pc = strtok( line, strDelim2 );
		cogid = pc;
		pc = strtok( NULL, strDelim2 );
		CATNAMES cats;
		sprintf( cats.catName, "%s", pc );
		list_item catit;

		int count = 0;
		int foundedone = -1;
		int count_cat = 0;
		forall_items( catit, inputCats ){
			int max = strlen( inputCats[ catit ].catName ) > strlen( cats.catName ) ? strlen( inputCats[ catit ].catName ) : strlen( cats.catName );
			max = max - 1;
			if( strncmp( inputCats[ catit ].catName, cats.catName, max ) == 0 ){
// 							cout << "/**************************************************/" << endl;
// 							cout << inputCats[ catit ].catName << "\n" << cats.catName << endl;
// 							cout << "/**************************************************/" << endl;
				foundedone = count_cat;
			}
			count_cat++;
		}
		if( feof( f ) )
			break;
		fscanf( f, "%s", read );
		while( strcmp( read, "_______" ) != 0 ){
			fscanf( f, "%s", read );
			if( strcmp( read, orgAbv ) == 0 ){
				fscanf( f, "%s", read );
				while( read[3] != ':' ){
					for( int i = 0; i < inputGenes.size(); i++ ){
						if( strcmp( pc, inputGenes[ i ].genename ) == 0 && inputGenes[ i ].index < 100 ){
							sprintf( inputGenes[ i ].categories[ inputGenes[ i ].index ].catName, "%s", cats.catName );
							sprintf( inputGenes[ i ].gos[ inputGenes[ i ].index++ ].goName, "%s", cogid );
							break;
						}
					}
					if( foundedone != -1 ){
						GENES now;
						sprintf( now.GENE, "%s", read );
						categoryGenes[ categoryGenes.get_item(foundedone) ].append( now );
		// 						cout << pc << "\t";
					}
					fscanf( f, "%s", read );
				}
			}
			count++;
		}
		categories.append( tup );
	}
	cout << "\n/**************************************************/" << endl;
	cout << "\t" << " Parsing COG File ends " << endl;
	cout << "/**************************************************/" << endl;
	fclose( f );
	return inputGenes;
}

/**
* cogHandling is a function that handles the cog input file from COG's database
* and responds to the main program by having modifying the passed
* parameters.
* @param inputGoFile (char[256]) Go Category file to read
* @param defaultGoFile (char[256]) Go original file
* @param gocategories (leda::list<list<GENES>>)filled with the GENE names provided by the data file
* @param matrixCategories (leda::list<leda::matrix>)is used to score calculation we need this param to obtain
* @param Hmax (double) Hmax is a value that stores max H-value
*/
void cogHandling( char inputCogFile[256], char defaultCogFile[256], list<list<GENES> > &cogcategories, list<int> &categ, 
	list<leda::matrix> &matrixCategories, list<double> &scores, double &Hmax, array<GENENAMES> &GenesNode ){
	FILE *fptr;
	char *pc;
	list<two_tuple<CATNAMES,int> > categories;
	list<CATNAMES> inputCats;

	if( (fptr = fopen( inputCogFile, "r") ) == NULL ){
		FILE *erptr;
		#ifdef LINUX
                    erptr = fopen( "outputs/error.txt", "w" );
		#else
                    erptr = fopen( "outputs//error.txt", "w" );
		#endif
                fprintf( erptr, "Error 307: You did not specify COG input file correctly\n" );
		fclose( erptr );
                cout << "\nError 307: You did not specify COG input file correctly\n";
		exit(1);
	}
	else{
		CATNAMES cats;
		int count = 0;
		while( !feof( fptr ) ){
			fgets( cats.catName, 256, fptr );
			pc = strtok( cats.catName, "\n" );
			sprintf( cats.catName, "%s", pc );
// 			cout << pc << endl;
			inputCats.append( cats );
			count++;
		}
	}	
// 	cout << "\nWEDONE\n";
	array<GENEONTO> geneOntoForData = cogFileHandling( defaultCogFile, "src", inputCats, GenesNode, categories, cogcategories );
// 	cout << "\nWEDONE\n";
	geneOntologyToBiclusterHandling( cogcategories, geneOntoForData );
}

/**
* biclusterHandling is a function that handles the bicluster output
* and responds to the main program by having modifying the passed
* parameters.
* @param INPUT (leda::matrix) input matrix obtained from the given input file sources/data_sources
* @param biclusters (leda::list<list<GENES>>)filled with the GENE names provided by the data file
* @param matrixBiclusters (leda::list<leda::matrix>)is used H-value calculation we need this param to obtain submatrices of biclusters
* @param H-values (leda::list) are used in order to have a corresponding value obtained with the id of biclusters       
* @param Hmax (double) Hmax is a value that stores max H-value
* @param minBicSize to control bicluster sizes
* @param maxBicSize to control bicluster sizes
* @param biclustering (int) is for choosing different biclustering outputs, We have to do that since each alg. has different output from its tool.
* @param dimension1(int) size of the data row dimension
* @param dimension2(int) size of the data column dimension
*/
void biclusterHandling( matrix &INPUT, char defaultBicFile[256], list<list<GENES> > &biclusters, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, int minBicSize, int maxBicSize, int biclustering, int dimension1, int dimension2  ){
        list_item it;
	leda::matrix inverseINPUT = INPUT.trans();
	list<list<CONDS> > conditions;
        char headerFileName[64] = "geneResult";
        char algname[64];
        if( biclustering == 1 ){
                sprintf( algname, "" );
        }
        if( biclustering == 2 ){
                sprintf( algname, "BIMAX" );
        }
        if( biclustering == 3 ){
                sprintf( algname, "CC" );
        }
        if( biclustering == 4 ){
                sprintf( algname, "REAL" );
        }
        getBiclustersFromFile2( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, algname, dimension1, dimension2 ); 
        analyseGenes2( headerFileName, categ, biclusters.size(), algname, dimension1, dimension2  );
// 	if( biclustering == 5 ){
// 		getBiclustersFromFile2( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, "SAMBA", dimension1, dimension2 ); 
// 		analyseGenes2( "geneResult", biclusters.size(), "SAMBA" , dimension1, dimension2 );
// 	}
// 	if( biclustering == 6 ){
// 		getBiclustersFromFile5( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, "MSBE", 1, dimension1, dimension2 ); 
// 		analyseGenes2( "geneResult", biclusters.size(), "MSBE" , dimension1, dimension2 );
// 	}
	if( biclusters.size() == 0 ) { 
		FILE *erptr;
		erptr = fopen( "outputs/error.txt", "w" );
                fprintf( erptr, "Error 301: No bicluster candidate to draw, make sure that you use correct parameters\n" );
		fclose( erptr );
                cout << "\nError 301: No bicluster candidate to draw, make sure that you use correct parameters\n";
		exit(1);
	}
// 	cout << " GENE LIST size : " << biclusters.size() << endl;
// 	cout << " COND LIST size : " << conditions.size() << endl;
// 	cout << " MATRIX LIST size : " << matrixBiclusters.size() << endl;

	for( int i=0; i < biclusters.size(); i++ ){
#ifdef LINUX
		char out[ 64 ] = "outputs/heatmap/out";
#else
		char out[ 64 ] = "outputs//heatmap//out";
#endif
		sprintf( out, "%s%d.html", out, i );
		drawHeatmap( matrixBiclusters[ matrixBiclusters.get_item( i )], biclusters[ biclusters.get_item( i ) ], conditions[ conditions.get_item( i ) ], out );
#ifdef LINUX
		char out2[ 64 ] = "outputs/parallel/out";
#else
		char out2[ 64 ] = "outputs//parallel//out";
#endif
		sprintf( out2, "%s%d.html", out2, i );
		drawParallelCoordinate( matrixBiclusters[ matrixBiclusters.get_item( i )], biclusters[ biclusters.get_item( i ) ], conditions[ conditions.get_item( i ) ], out2 );
#ifdef LINUX
		char out3[ 64 ] = "outputs/heatmap/out";
#else
		char out3[ 64 ] = "outputs//heatmap//out";
#endif
		sprintf( out3, "%s%d.txt", out3, i );
		produceHeatmapInput( matrixBiclusters[ matrixBiclusters.get_item( i )], biclusters[ biclusters.get_item( i ) ], conditions[ conditions.get_item( i ) ], out3 );
	}

	matrix newI2;
	int k = 1;
	bool loopflg = false;
	forall_items( it , matrixBiclusters ){
		char filename2[1024] = "";
		int count_i = 0;
		newI2 = matrixBiclusters[ it ];
		double rs_value;
		double bagMean = 0;
		for( int bag1_int = 0; bag1_int <  newI2.dim1(); bag1_int++ ){
			for( int bag2_int = 0; bag2_int <  newI2.dim2(); bag2_int++ ){
			    bagMean += newI2( bag1_int, bag2_int );
			}
		}
		bagMean = bagMean / (double)( newI2.dim1() * newI2.dim2() );
		double rowMean = 0;
		double colMean = 0;
		double residueScore = 0;
		double residueScoreWhole = 0;
		/*cout << endl;
		for( int bag1_int = i_int; bag1_int < i_int + bagDim1_int; bag1_int++ ){
			for( int bag2_int = j_int; bag2_int < j_int + bagDim2_int; bag2_int++ ){
				cout << extract_mx( bag1_int, bag2_int ) << " ";
			}
			cout << endl;
		}*/
		/*****************************************************************************/
		for( int bag1_int = 0; bag1_int <  newI2.dim1(); bag1_int++ ){
			for( int bag2_int = 0; bag2_int <  newI2.dim2(); bag2_int++ ){
			    /* For each value inside the sub matrix calculate row mean, col mean, and bicluster mean*/
			    /* Calculate row mean */
			    for( int bag2_int2 = 0; bag2_int2 < newI2.dim2(); bag2_int2++ ){
				    rowMean += newI2( bag1_int, bag2_int2 );
			    }
			    rowMean = rowMean / newI2.dim2();
			    /* Calculate col mean */
			    for( int bag1_int2 = 0; bag1_int2 < newI2.dim1(); bag1_int2++ ){
				    colMean += newI2( bag1_int2, bag2_int );
			    }
			    colMean = colMean / newI2.dim1();
			    residueScore += (double)newI2( bag1_int, bag2_int ) + - rowMean - colMean + bagMean;
			    residueScore *= residueScore;
			    residueScoreWhole += residueScore;

		
			    residueScore = 0;
			    colMean = 0;
			    rowMean = 0;
			}
		}
		/*****************************************************************************/
		double rs_Value = residueScoreWhole / (double)( newI2.dim1() * newI2.dim2() ) ;
		H_values.append( rs_Value );
// 		cout << rs_Value << endl;
// 		if( loopflg == false ){
// 			Hmax = rs_Value;
// 			loopflg = true;
// 		}
// 		else{
// 			if( rs_Value > Hmax ){
// 				Hmax = rs_Value;
// 			}
// 		}
		Hmax = rs_Value;
	}
}

/**
* interactionHandling is a function that handles the PPI extraction from
* given file
* @param temp (leda::node_array<GENENAMES>) temp file for proceessing
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param INTERACTIONS (leda::GRAPH<int,int>) is PPI graph extracted from sources/ppi_sources/<ppiFile> 
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
*/
void interactionHandling( node_array<GENENAMES> &temp, array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, GRAPH<int,int> &TEMPINT, char ppiFile[256] ){

	list_item it;
	FILE *interactionsPtr;
	char ppiFileToOpen[256];


	sprintf( ppiFileToOpen, "%s", ppiFile );

// 	cout <<  "Bitti " << endl << ppiFileToOpen << endl;
	//For Sample Data
	//interactionsPtr = fopen( "DenemeYeastProteinInteractions.txt", "r" );
	//For Real Data
	interactionsPtr = fopen( ppiFileToOpen, "r" );		
	node n,sourceTemp,targetTemp;
	edge e;
	char sourceGene[1024], targetGene[1024], relation[1024];
	double value;
	list<GENENAMES> entered;
	int numberOfReleations = 0;
        int nodeAdded = 0;
	while( !feof( interactionsPtr ) ){
                if( numberOfReleations % 5000 == 4999 )
			cout << "* " << INTERACTIONS.number_of_edges() << " " << INTERACTIONS.number_of_nodes() << endl;

		fscanf( interactionsPtr, "%s%s%lf", sourceGene, targetGene, &value );
		
//		// FOR SOURCE GENES
//		int flag = 0;
//		if( numberOfReleations > 0 ){
//			forall_items( it, entered ){
//				if( strcmp( entered[ it ].GENE, sourceGene ) == 0 ){
//					flag = 1;
//				}
//			}
//			if( flag == 1 ){
//				forall_nodes( n, INTERACTIONS ){
//					if( strcmp( GenesNode[ INTERACTIONS[ n ]].GENE , sourceGene ) == 0 ){
//						sourceTemp = n;
//						break;
//					}
//				}
//			}
//			else{
//				sourceTemp = INTERACTIONS.new_node();
//				INTERACTIONS[ sourceTemp ] = nodeAdded;
//				nodeAdded++;
//				GenesNode.resize( nodeAdded );
//				sprintf( GenesNode[ INTERACTIONS[ sourceTemp ] ].GENE, "%s",sourceGene );
//				GENENAMES tempx;
//				sprintf( tempx.GENE, "%s", sourceGene );
//				entered.append( tempx );
//			}
//		}
//		else{
//				sourceTemp = INTERACTIONS.new_node();
//				INTERACTIONS[ sourceTemp ] = nodeAdded;
//				nodeAdded++;
//				GenesNode.resize( nodeAdded );
//				sprintf( GenesNode[ INTERACTIONS[ sourceTemp ] ].GENE, "%s",sourceGene );
//				GENENAMES tempx;
//				sprintf( tempx.GENE, "%s", sourceGene );
//				entered.append( tempx );
//		}
//
//		// FOR TARGET GENES
//		flag = 0;
//		forall_items( it, entered ){
//			if( strcmp( entered[ it ].GENE , targetGene ) == 0 ){
//				flag = 1;
//			}
//		}
//		if( flag == 1 ){
//			forall_nodes( n, INTERACTIONS ){
//				if( strcmp( GenesNode[ INTERACTIONS[ n ] ].GENE, targetGene ) == 0 ){
//					targetTemp = n;
//					break;
//				}
//			}
//		}
//		else{
//			targetTemp = INTERACTIONS.new_node();
//			INTERACTIONS[ targetTemp ] = nodeAdded;
//			nodeAdded++;
//			GenesNode.resize( nodeAdded );
//			sprintf( GenesNode[ INTERACTIONS[ targetTemp ] ].GENE, "%s", targetGene );
//			GENENAMES tempx;
//			sprintf( tempx.GENE, "%s", targetGene );
//			entered.append( tempx );
//		}

                int flag_s = 0,flag_t = 0;
                forall_nodes( n, INTERACTIONS ){
                        if( strcmp( GenesNode[ INTERACTIONS[ n ]].GENE , sourceGene ) == 0 ){
                                sourceTemp = n;
                                flag_s = 1;
                                if( flag_t == 1 )
                                    break;
                        }
                        if( strcmp( GenesNode[ INTERACTIONS[ n ] ].GENE, targetGene ) == 0 ){
                                targetTemp = n;
                                flag_t = 1;
                                if( flag_s == 1 )
                                    break;
                        }
                }
                // FOR SOURCE GENES
                if( flag_s == 0 ){
                        sourceTemp = INTERACTIONS.new_node();
                        INTERACTIONS[ sourceTemp ] = nodeAdded;
                        nodeAdded++;
                        GenesNode.resize( nodeAdded );
                        sprintf( GenesNode[ INTERACTIONS[ sourceTemp ] ].GENE, "%s",sourceGene );
                        GENENAMES tempx;
                        sprintf( tempx.GENE, "%s", sourceGene );
                        entered.append( tempx );
                }
                if( strcmp( sourceGene, targetGene ) == 0 ){
                    e = INTERACTIONS.new_edge( sourceTemp, sourceTemp );
                    INTERACTIONS[ e ] = (int)(value * 100.0);
                }
                else{
                    // FOR TARGET GENES
                    if( flag_t == 0 ){
                            targetTemp = INTERACTIONS.new_node();
                            INTERACTIONS[ targetTemp ] = nodeAdded;
                            nodeAdded++;
                            GenesNode.resize( nodeAdded );
                            sprintf( GenesNode[ INTERACTIONS[ targetTemp ] ].GENE, "%s", targetGene );
                            GENENAMES tempx;
                            sprintf( tempx.GENE, "%s", targetGene );
                            entered.append( tempx );
                    }
                    e = INTERACTIONS.new_edge( sourceTemp, targetTemp );
                    INTERACTIONS[ e ] = (int)(value * 100.0);
                }
		if( INTERACTIONS[ e ] == 0 )
			INTERACTIONS[ e ] = 1;
// 		cout << GenesNode[ INTERACTIONS[ sourceTemp ] ].GENE << " - " << GenesNode[ INTERACTIONS[ targetTemp ] ].GENE << " : " << INTERACTIONS[ e ] << endl;
		numberOfReleations++;
		/*cout << endl;*/			
	}
// 	exit(1);
        cout << endl;
// 	forall_edges( e, INTERACTIONS )
// 		cout << GenesNode[ INTERACTIONS[ INTERACTIONS.source( e ) ] ].GENE << " " << GenesNode[ INTERACTIONS[ INTERACTIONS.target( e )] ].GENE << endl;
//
	fclose( interactionsPtr );
}

/**
* handleZeroDegreeNodes is a function that adds zero degree nodes due to categories
* given file
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param INTERACTIONS (leda::GRAPH<int,int>) is PPI graph extracted from sources/ppi_sources/<ppiFile>
* @param biclusters (leda::list<list<GENES> >) is list ofe genes in category
*/
void handleZeroDegreeNodes( array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, list<list<GENES> > &categories ){
    list_item it1,it2;
    list<GENES> genes;
    node n;
    int nodeAdded = INTERACTIONS.number_of_nodes();
    forall_items( it1, categories ){
        genes = categories[ it1 ];
        forall_items( it2, genes ){
            bool flag = false;
            for( int i = 0; i < GenesNode.size(); i++ ){
                if( strcmp( GenesNode[ i ].GENE, genes[ it2 ].GENE ) == 0 ){
                    flag = true;
                    break;
                }
            }
            if( flag == false ){
                n = INTERACTIONS.new_node();
                INTERACTIONS[ n ] = nodeAdded;
                nodeAdded++;
                GenesNode.resize( nodeAdded  );
                sprintf( GenesNode[ INTERACTIONS[ n ] ].GENE, "%s",  genes[ it2 ].GENE );
            }
        }
    }
}

/**
* inpGraphProdHandling is a function that handles the production of
* peripheral graphs
* @param abbv (char []) we need a categories for each organism. It may be too general as we did having n categories. abbv stores just a capital letter of categories.
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param INTERACTIONS (leda::GRAPH<int,int>) is PPI graph extracted from sources/ppi_sources/<ppiFile> it is original graph obtained with interactionHandling
* @param biclusters we do not lose the track of bicluster genes
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu

*/
void inpGraphProdHandling( GRAPH<int,int> &G, array<GRAPH<int,int> > &listOfGraphs, array<char> &abbv,  array<char> &Categories, node_array<GENENAMES> &temp, array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, GRAPH<int,int> &TEMPINT, list<list<GENES> > &biclusters, int cat_num ){
	FILE *categoryOfGenes;
	node n,m1,m2;
#ifdef LINUX
	categoryOfGenes = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "r" );
#else
	categoryOfGenes = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "r" );
#endif
	if( categoryOfGenes == NULL ){
                cout << "\n Error 303: You did not specify gene to function file in the path sources/usr_sources/visualization_data/\n";
		FILE *erptr;
#ifdef LINUX
		erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
                fprintf( erptr, "\n Error 303: You did not specify gene to function file in the path sources/usr_sources/visualization_data/\n" );
		fclose( erptr );
	}
	int geneCount = 0;
	while( !feof( categoryOfGenes ) ){
	    fscanf( categoryOfGenes, "%s%s", g[ geneCount ].gene, g[ geneCount ].geneType );
	    forall_nodes( n, INTERACTIONS ){
		if( strcmp( GenesNode[ INTERACTIONS[ n ] ].GENE , g[ geneCount ].gene) == 0 ){
		    for( int i = 0; i < cat_num; i++ ){
			if( g[ geneCount ].geneType[ 0 ] == abbv[ i ] ){
			    Categories[ INTERACTIONS[n] ] = abbv[ i ];                              
			}
		    }
		}
	    }
	    //cout << geneCount << " ";
	    geneCount++;
	}

        gwSave( GenesNode, INTERACTIONS );
	/*forall_nodes( n, INTERACTIONS ){
	      cout << Categories[ INTERACTIONS[n] ] << " ";
	}*/
	fclose( categoryOfGenes );
#ifdef DEBUG_ROBINVIZ
	cout << endl << " * Gene File is loaded from ppi_sources" << endl;
	
	cout << endl << " * Loading biclusters to form their ppi graphs " << endl;
#endif
		    TEMPINT = INTERACTIONS;
	array<int> nodePar( INTERACTIONS.number_of_nodes()+1 );
	int nPar = 0;
	forall_nodes( n, INTERACTIONS ){
		nodePar[ nPar ] = INTERACTIONS[ n ];
		nPar++;
	}
	for( int i = 0; i < biclusters.size(); i++ ){
		listOfGraphs[ i ] = INTERACTIONS;
		nPar = 0;
		forall_nodes( n,listOfGraphs[ i ] ){
			listOfGraphs[ i ][ n ] = nodePar[ nPar ];
			nPar++;
		}
	}

	list<edge> list_E;
	list_item it_b1 = biclusters.first_item(),it_b2;
	for( int i = 0; i < biclusters.size(); i++ ){
		G = listOfGraphs[ i ];				                                       
		list<GENES> tmp1 = biclusters[ it_b1 ];
#ifdef DEBUG_ROBINVIZ
		cout << " GRAPH " << i << " WITH SIZE " << tmp1.size() << " || ";
#endif
		forall_nodes( m1, G ){
			if( tmp1.size() == 0 ){
				G.clear();					
				break;
			}
			bool isEnter = false;
			forall_items( it_b2, tmp1 ){
				if( strcmp( GenesNode[  G[ m1 ] ].GENE, tmp1[ it_b2 ].GENE ) == 0 ){
					isEnter = true;
					break;
				}
			}
			if( isEnter == false ){
				G.del_node( m1 );
			}
		}
#ifdef DEBUG_ROBINVIZ
		cout << " GRAPH IS PRODUCED ||";
#endif
		forall_nodes( m1, G ){
		    list<edge> inedges = G.in_edges( m1 );
		    list<edge> outedges = G.out_edges( m1 );
		    if( (inedges.size() + outedges.size() ) == 0 )
			G.del_node( m1 );
		}
#ifdef DEBUG_ROBINVIZ
		cout << endl;
		forall_nodes( m1, G ){
			cout <<  GenesNode[  G[ m1 ] ].GENE << "-";
		}
		cout << endl;
#endif
		listOfGraphs[ i ] = G;
		it_b1 = biclusters.succ( it_b1 );
#ifdef DEBUG_ROBINVIZ
		cout << " GRAPH IS CLEANED " << endl;
#endif
	}
}


/**
* mainAlgHandlingForEachSubgraph is a function that handles the layering of
* peripheral graphs using weighted layered drawing concept
* @param abbv (char []) we need a categories for each organism. It may be too general as we did having n categories. abbv stores just a capital letter of categories.
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param LAYERS Layer Numbers of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param layers INTERACTION graph node layer numbers 
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
*/
void mainAlgHandlingForEachSubgraph( node_array<point> &pos, 
				     edge_array<list<point> > &bends, 
				     array<list<node> > &layers,
				     list<GRAPH<int,int> > &GraphList, 
				     list<GRAPH<leda::string,int> > &GraphList_S,
				     node_array<double> &Xpos,
				     node_array<double> &Ypos,
                                     list<node_array<point> > &POS,
				     list<edge_array<list<point> > > &BENDS,
				     list<array<list<node> > > &LAYERS,
                                     GRAPH<leda::string,int> &PROJECT,
				     array<GENENAMES> &GenesNode,
				     array<GRAPH<int,int> > &listOfGraphs,
				     list<list<Strings> > &namesForEachGraph,
				     list<list<GENES> > &biclusters,
				     int &width,
				     int algorithmFlag,
				     int space,
				     bool xCoordFlag,
				     int increment,
				     bool ledaPostFlag
				  ){
	node projectNode,projectNode2,m,m1,m2,zzz,n1,n2,n;
	edge e1,e2,e;
	list_item it1,it;
	GRAPH<int,int> H;
	int node_number;
	int edge_number;
	int ncount=0,ecount=0;
	int fileIndex = 0;
	char filename[ 1024 ];
//         FILE *wptr2,*wptr3;
// #ifdef LINUX
// 	wptr3 = fopen( "sources/graph_sources/graph_infos.txt", "w" );
// #else
// 	wptr3 = fopen( "sources//graph_sources//graph_infos.txt", "w" );
// #endif
	for(int i = 0; i < biclusters.size(); i++ ){	
// 		if( listOfGraphs[ i ].empty() != true ){
		    pos.init( listOfGraphs[ i ] );
		    bends.init( listOfGraphs[ i ] );   
		    array<int> nodePar( listOfGraphs[ i ].number_of_nodes()+1 );
		    int nPar = 0;
		    forall_nodes( n, listOfGraphs[ i ] ){
			  nodePar[ nPar ] = listOfGraphs[ i ][ n ];
// 			  cout << nodePar[ nPar ] << " - " << listOfGraphs[ i ][ n ] << endl;
			  nPar++;
		    }
                    if( listOfGraphs[ i ].empty() != true ){
                              cout << " Graph " << i << " is processed\n";
			      H = RUN_SELF( listOfGraphs[ i ], layers, width, Xpos, Ypos, i + 1, pos, bends, algorithmFlag, space, xCoordFlag, increment, ledaPostFlag );
                              node_array<bool> markDegreeZero( listOfGraphs[ i ], false );
                              list<node> justOnes;
                              forall_nodes( n, listOfGraphs[ i ] ){
                                  if( listOfGraphs[ i ].degree( n ) == 0 ){
                                      justOnes.append( n );
//                                      markDegreeZero[ n ] = true;
                                  }
                              }
                              cout << "\ndone";
                              if( justOnes.size() > 0 ){
                                  double averageDistance = 0;
                                  forall_edges( e, listOfGraphs[ i ] ){
                                      point p( Xpos[ listOfGraphs[ i ].source( e ) ], Ypos[ listOfGraphs[ i ].source( e ) ] );
                                      point q( Xpos[ listOfGraphs[ i ].target( e ) ], Ypos[ listOfGraphs[ i ].target( e ) ] );
                                      averageDistance += p.distance( q );
                                  }
                                  averageDistance = averageDistance / (double)(listOfGraphs[ i ].number_of_edges());

                                  cout << "\ndone";
                                  double xmin, xmax, ymin, ymax;
                                  forall_nodes( n, listOfGraphs[ i ] ){
    //                                  if( markDegreeZero[ n ] != true ){
                                          if( ncount == 0 ){
                                                  xmin = Xpos[ n ];
                                                  xmax = Xpos[ n ];
                                                  ymin = Ypos[ n ];
                                                  ymax = Ypos[ n ];
                                          }
                                          else{
                                              if( Xpos[ n ] < xmin )
                                                    xmin = Xpos[ n ];
                                              if( Xpos[ n ] > xmax )
                                                    xmax = Xpos[ n ];
                                              if( Ypos[ n ] < ymin )
                                                    ymin = Ypos[ n ];
                                              if( Ypos[ n ] > ymax )
                                                    ymax = Ypos[ n ];
                                          }
    //                                  }
                                  }
                                  cout << "\ndone";
                                  double xpos1 = xmin;
                                  double ypos1 = ymin - space;

                                  forall_items( it, justOnes ){
                                      if( xpos1 < xmax ){
                                          Xpos[ justOnes[ it ] ] = xpos1;
                                          Ypos[ justOnes[ it ] ] = ypos1;
                                      }
                                      else{
                                          xpos1 = xmin;
                                          ypos1 -= space;
                                          Xpos[ justOnes[ it ] ] = xpos1;
                                          Ypos[ justOnes[ it ] ] = ypos1;
                                      }
                                      xpos1 += increment;
                                  }
                                  cout << "OK\n";
                             }
                          }

		    nPar = 0;
		    forall_nodes( n, listOfGraphs[ i ] ){
			  listOfGraphs[ i ][ n ] = nodePar[ nPar ];
// 			  cout << nodePar[ nPar ] << " - " << listOfGraphs[ i ][ n ] << endl;
			  nPar++;
		    }  

			GraphList.append( listOfGraphs[ i ] );
// #ifdef LINUX
// 			sprintf( filename , "sources/graph_sources/Graph%d.txt" , i+1 );
// #else
// 			sprintf( filename , "sources//graph_sources//Graph%d.txt" , i+1 );
// #endif
// 			ofstream GRAPHS( filename , ios::in | ios::trunc );
		    
		        
// 			fprintf( wptr3, "%s\n", "_________________________________" );
// 			fprintf( wptr3, "%s\t%s\n", "File Name      :", filename ); 
// 			fprintf( wptr3, "%s\t%d\n", "Number of Graph:", i );
// 			fprintf( wptr3, "%s\n", "_________________________________" );
			projectNode = PROJECT.new_node();
			char nameS[ 200 ][ 16 ];
			list<Strings> nameGraph;
		    
			int nn = 0;
			node_array<int> INDEX( listOfGraphs[ i ], 0 );
			forall_nodes( n, listOfGraphs[ i ]){
				ncount = 0;
				INDEX[ n ] = nn;
				STRINGS temp_S;
				strcpy( temp_S.name, GenesNode[ listOfGraphs[ i ][ n ] ].GENE);
// 					cout << GenesNode[ listOfGraphs[ i ][ n ] ].GENE << endl;
				nameGraph.append( temp_S  );
				nn++;	
			}
			/*forall_items( it, nameGraph )
				printf( "%s\n", (char*)nameGraph[ it ].name );*/
			namesForEachGraph.append(nameGraph);
// #ifdef LINUX
// 			sprintf( filename , "sources/graph_sources/Relation%d.txt" , i+1 );
// #else
// 			sprintf( filename , "sources//graph_sources//Relation%d.txt" , i+1 );
// #endif
// 			wptr2 = fopen( filename, "w" );
			forall_edges( e, listOfGraphs[ i ] ){
				node source = listOfGraphs[ i ].source( e );
				node target = listOfGraphs[ i ].target( e );
// 				fprintf( wptr2, "%s\t=>\t%s\t|\t|\n", nameS[ INDEX[ source ] ], nameS[ INDEX[target] ] );
			}

// 			fclose( wptr2 );

			leda::string m = "";
// 			listOfGraphs[ i ].write( GRAPHS );
// 			GRAPHS.close();

			/************************************************/
			/*    Copy Graph as leda::string type Graph     */
			/************************************************/
			GRAPH<leda::string,int> HAsString;		
			HAsString.clear();
			leda::string temp;
			int count = 0;
			forall_nodes( n, listOfGraphs[ i ] ){
				zzz = HAsString.new_node();
				for( ; count < 16; count++ );
					temp.insert( count, nameS[ INDEX[ n ] ][ count ] );
				HAsString[ zzz ] = temp;
			}
			node_array<int> INDEX_S( HAsString, 0 );
			count = 0;
			forall_nodes( n, HAsString ){
				INDEX_S[ n ] = count;
				count++;
			}
			forall_edges( e, listOfGraphs[ i ] ){
				int a = INDEX[ listOfGraphs[ i ].source( e ) ];
				int b = INDEX[ listOfGraphs[ i ].target( e ) ];

				forall_nodes( m1, HAsString ){
					if( INDEX_S[ m1 ] == a )
						break;
				}
				forall_nodes( m2, HAsString ){
					if( INDEX_S[ m2 ] == b )
						break;
				}
				//cout << m1 << " " << a << " - " << m2 << b << endl; 
				HAsString.new_edge( m1, m2, 1 );
			}

			node_array<point> pos_S(HAsString);
			edge_array<list<point> > bends_S(HAsString);
			array <list<node> > layer_S( layers.size());
			/*forall_nodes( n1, HAsString ){
			    cout << endl<< INDEX_S[ n1 ];
			}
			forall_nodes( n2, listOfGraphs[ i ] ){
			    cout << endl<< INDEX[ n2 ];
			}*/
			forall_nodes( n1, HAsString ){
			    forall_nodes( n2, listOfGraphs[ i ] ){
				//cout << endl<< INDEX_S[ n1 ] << "  " << INDEX[ n2 ]<<endl;
				if( INDEX_S[ n1 ] == INDEX[ n2 ] ){
				    //cout << endl << pos_S[ n1 ] << "  " << pos[ n2 ] << endl;        
				    pos_S[ n1 ] = pos[ n2 ];
				}
			    }
			}
			forall_edges( e1, HAsString ){
			    forall_edges( e2, listOfGraphs[ i ] ){
				if( INDEX_S[ HAsString.source( e1 ) ] == INDEX[ listOfGraphs[ i ].source( e2 ) ] &&
				      INDEX_S[ HAsString.target( e1 ) ] == INDEX[ listOfGraphs[ i ].target( e2 )  ] )
				      bends_S[ e1 ] = bends[ e2 ];
			    }
			}
			int j;
			for( j = 0; j < layers.size(); j++ ){
			    list<node> layerTemp = layers[ j ];
			    forall_items( it, layerTemp ){
				forall_nodes( n1, HAsString ){
				    if( INDEX_S[ n1 ] == INDEX[ layerTemp[ it ] ] ){
				      layer_S[ j ].push_back( n1 );
				    }
				}
			    }
			}
			for( j = 0; j < layer_S.size(); j++ ){
			    list<node> layerTemp = layer_S[ j ];
// 				  forall_items( it, layerTemp ){
// 					cout << INDEX_S[ layerTemp[ it ] ] << " ";
// 				  }
			}
#ifdef DEBUG_ROBINVIZ
			cout << endl;
#endif
			for( j = 0; j < layers.size(); j++ ){
			    list<node> layerTemp = layers[ j ];
// 				  forall_items( it, layerTemp ){
// 					cout << INDEX[ layerTemp[ it ] ] << " ";
// 				  }
			}


			POS.push_back( pos_S );
			BENDS.push_back( bends_S );
			LAYERS.push_back( layer_S );
			GraphList_S.append( HAsString );
			//cout << " Size 1 : " << HAsString.number_of_nodes() << " Size 2 : " << listOfGraphs[ i ].number_of_nodes() << " Size 3 : " << nameGraph.size() << endl;
			HAsString.clear();
// 		}
			    /************************************************/
			    /* End of Copy Graph as leda::string type Graph */
			    /************************************************/			
	}
// 	fclose( wptr3 );
}


/**
* mainAlgHandlingForEachSubgraph is a function that handles the force directed layout of
* peripheral graphs
* @param abbv (char []) we need a categories for each organism. It may be too general as we did having n categories. abbv stores just a capital letter of categories.
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
*/
void mainAlgHandlingForEachSubgraph2( node_array<point> &pos, 
				     edge_array<list<point> > &bends, 
				     array<list<node> > &layers,
				     list<GRAPH<int,int> > &GraphList, 
				     list<GRAPH<leda::string,int> > &GraphList_S,
				     node_array<double> &Xpos,
				     node_array<double> &Ypos,
                                     list<node_array<point> > &POS,
				     list<edge_array<list<point> > > &BENDS,
				     list<array<list<node> > > &LAYERS,
                                     GRAPH<leda::string,int> &PROJECT,
				     array<GENENAMES> &GenesNode,
				     array<GRAPH<int,int> > &listOfGraphs,
				     list<list<Strings> > &namesForEachGraph,
				     list<list<GENES> > &biclusters,
				     int &width,
				     int algorithmFlag,
				     int space,
				     bool xCoordFlag,
				     int increment,
				     bool ledaPostFlag,
				     array<char> &abbv,
				     int cat_num
				  ){
	node projectNode,projectNode2,m,m1,m2,zzz,n1,n2,n;
	edge e1,e2,e;
	list_item it1,it;
	GRAPH<int,int> H;
	int node_number;
	int edge_number;
	int ncount=0,ecount=0;
	int fileIndex = 0;
	char filename[ 1024 ];
        FILE *wptr2,*wptr3;
// #ifdef LINUX
// 	wptr3 = fopen( "sources/graph_sources/graph_infos.txt", "w" );
// #else
// 	wptr3 = fopen( "sources//graph_sources//graph_infos.txt", "w" );
// #endif
	for(int i = 0; i < biclusters.size(); i++ ){	
                    projectNode = PROJECT.new_node();
                    pos.init( listOfGraphs[ i ] );
                    bends.init( listOfGraphs[ i ] );
//                    array<int> nodePar( listOfGraphs[ i ].number_of_nodes()+1 );
//                    int nPar = 0;
//                    forall_nodes( n, listOfGraphs[ i ] ){
//                          nodePar[ nPar ] = listOfGraphs[ i ][ n ];
//// 			  cout << nodePar[ nPar ] << " - " << listOfGraphs[ i ][ n ] << endl;
//                          nPar++;
//                    }
                    cout << " Graph " << i << " in process \n";
                    if( listOfGraphs[ i ].empty() != true ){
//                            if( listOfGraphs[ i ].number_of_nodes() < 1500 ){
//                                    //H = RUN_CIRCLEALONE( listOfGraphs[ i ], layers, width, Xpos, Ypos, i + 1, pos, bends, algorithmFlag, space, xCoordFlag, increment, ledaPostFlag, abbv, cat_num, Categories, 100 );
//                                    H = RUN_FFDANDCIRCLE( listOfGraphs[ i ], layers, width, Xpos, Ypos, i, pos, bends, algorithmFlag, space, xCoordFlag, increment, ledaPostFlag, abbv, cat_num, 100, 100.0 );
//                            }
//                            else
//                                    H = RUN_CIRCULARKC( listOfGraphs[ i ], layers, width, Xpos, Ypos, i, pos, bends, algorithmFlag, space, xCoordFlag, increment, ledaPostFlag, abbv, cat_num, 100, 2, 0.20 );
                            forall_edges( e, listOfGraphs[ i ] )
                            {
                                    if( listOfGraphs[ i ].source( e ) == listOfGraphs[ i ].target( e ) )
                                            listOfGraphs[ i ].del_edge( e );
                            }
                        if( listOfGraphs[ i ].number_of_edges() < 1 )
                            H = RUN_CIRCLEALONE( listOfGraphs[ i ], Xpos, Ypos, pos, i, 100 );
                        else
                            H = RUN_SPRING_EMBEDDER( listOfGraphs[ i ], Xpos, Ypos, pos, 50, i );
                    }
                    cout << " Graph " << i << " layout is done \n";
//                    nPar = 0;
//                    forall_nodes( n, listOfGraphs[ i ] ){
//                          listOfGraphs[ i ][ n ] = nodePar[ nPar ];
//// 			  cout << nodePar[ nPar ] << " - " << listOfGraphs[ i ][ n ] << endl;
//                          nPar++;
//                    }

                    GraphList.append( listOfGraphs[ i ] );
// #ifdef LINUX
// 			sprintf( filename , "sources/graph_sources/Graph%d.txt" , i+1 );
// #else
// 			sprintf( filename , "sources//graph_sources//Graph%d.txt" , i+1 );
// #endif
// 			ofstream GRAPHS( filename , ios::in | ios::trunc );


// 			fprintf( wptr3, "%s\n", "_________________________________" );
// 			fprintf( wptr3, "%s\t%s\n", "File Name      :", filename ); 
// 			fprintf( wptr3, "%s\t%d\n", "Number of Graph:", i );
// 			fprintf( wptr3, "%s\n", "_________________________________" );
                    char nameS[ 10000 ][ 16 ];
                    list<Strings> nameGraph;

                    int nn = 0;
                    node_array<int> INDEX( listOfGraphs[ i ], 0 );
                    forall_nodes( n, listOfGraphs[ i ]){
                            ncount = 0;
                            INDEX[ n ] = nn;
                            STRINGS temp_S;
                            strcpy( temp_S.name, GenesNode[ listOfGraphs[ i ][ n ] ].GENE);
// 					cout << GenesNode[ listOfGraphs[ i ][ n ] ].GENE << endl;
                            nameGraph.append( temp_S  );
                            nn++;
                    }
                    /*forall_items( it, nameGraph )
                            printf( "%s\n", (char*)nameGraph[ it ].name );*/
                    namesForEachGraph.append(nameGraph);
// #ifdef LINUX
// 			sprintf( filename , "sources/graph_sources/Relation%d.txt" , i+1 );
// #else
// 			sprintf( filename , "sources//graph_sources//Relation%d.txt" , i+1 );
// #endif
// 			wptr2 = fopen( filename, "w" );
                    forall_edges( e, listOfGraphs[ i ] ){
                            node source = listOfGraphs[ i ].source( e );
                            node target = listOfGraphs[ i ].target( e );
// 				fprintf( wptr2, "%s\t=>\t%s\t|\t|\n", nameS[ INDEX[ source ] ], nameS[ INDEX[target] ] );
                    }

// 			fclose( wptr2 );

                    leda::string m = "";
// 			listOfGraphs[ i ].write( GRAPHS );
// 			GRAPHS.close();

                    /************************************************/
                    /*    Copy Graph as leda::string type Graph     */
                    /************************************************/
                    GRAPH<leda::string,int> HAsString;
                    HAsString.clear();
                    leda::string temp;
                    int count = 0;
                    forall_nodes( n, listOfGraphs[ i ] ){
                            zzz = HAsString.new_node();
                            temp = leda::string( nameS[ INDEX[ n ] ][ count ] );
                            HAsString[ zzz ] = temp;
                    }
                    node_array<int> INDEX_S( HAsString, 0 );
                    count = 0;
                    forall_nodes( n, HAsString ){
                            INDEX_S[ n ] = count;
                            count++;
                    }
                    forall_edges( e, listOfGraphs[ i ] ){
                            int a = INDEX[ listOfGraphs[ i ].source( e ) ];
                            int b = INDEX[ listOfGraphs[ i ].target( e ) ];

                            forall_nodes( m1, HAsString ){
                                    if( INDEX_S[ m1 ] == a )
                                            break;
                            }
                            forall_nodes( m2, HAsString ){
                                    if( INDEX_S[ m2 ] == b )
                                            break;
                            }
                            //cout << m1 << " " << a << " - " << m2 << b << endl;
                            HAsString.new_edge( m1, m2, 1 );
                    }

                    node_array<point> pos_S(HAsString);
                    edge_array<list<point> > bends_S(HAsString);
                    array <list<node> > layer_S( layers.size());
                    /*forall_nodes( n1, HAsString ){
                        cout << endl<< INDEX_S[ n1 ];
                    }
                    forall_nodes( n2, listOfGraphs[ i ] ){
                        cout << endl<< INDEX[ n2 ];
                    }*/
                    forall_nodes( n1, HAsString ){
                        forall_nodes( n2, listOfGraphs[ i ] ){
                            //cout << endl<< INDEX_S[ n1 ] << "  " << INDEX[ n2 ]<<endl;
                            if( INDEX_S[ n1 ] == INDEX[ n2 ] ){
                                //cout << endl << pos_S[ n1 ] << "  " << pos[ n2 ] << endl;
                                pos_S[ n1 ] = pos[ n2 ];
                            }
                        }
                    }
                    /*forall_edges( e1, HAsString ){
                        forall_edges( e2, listOfGraphs[ i ] ){
                            if( INDEX_S[ HAsString.source( e1 ) ] == INDEX[ listOfGraphs[ i ].source( e2 ) ] &&
                                  INDEX_S[ HAsString.target( e1 ) ] == INDEX[ listOfGraphs[ i ].target( e2 )  ] )
                                  bends_S[ e1 ] = bends[ e2 ];
                        }
                    }*/
                    int j;
                    for( j = 0; j < layers.size(); j++ ){
                        list<node> layerTemp = layers[ j ];
                        forall_items( it, layerTemp ){
                            forall_nodes( n1, HAsString ){
                                if( INDEX_S[ n1 ] == INDEX[ layerTemp[ it ] ] ){
                                  layer_S[ j ].push_back( n1 );
                                }
                            }
                        }
                    }
                    for( j = 0; j < layer_S.size(); j++ ){
                        list<node> layerTemp = layer_S[ j ];
// 				  forall_items( it, layerTemp ){
// 					cout << INDEX_S[ layerTemp[ it ] ] << " ";
// 				  }
                    }
#ifdef DEBUG_ROBINVIZ
                    cout << endl;
#endif
                    for( j = 0; j < layers.size(); j++ ){
                        list<node> layerTemp = layers[ j ];
// 				  forall_items( it, layerTemp ){
// 					cout << INDEX[ layerTemp[ it ] ] << " ";
// 				  }
                    }


                    POS.push_back( pos_S );
                    BENDS.push_back( bends_S );
                    LAYERS.push_back( layer_S );
                    GraphList_S.append( HAsString );
                    //cout << " Size 1 : " << HAsString.number_of_nodes() << " Size 2 : " << listOfGraphs[ i ].number_of_nodes() << " Size 3 : " << nameGraph.size() << endl;
                    HAsString.clear();
            }
            /************************************************/
            /* End of Copy Graph as leda::string type Graph */
            /************************************************/
}


/**
* mainGraphHandling is a function that handles the ISB 2010 submitted main layout on the 
* peripheral graphs
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
* @param removeRat(double) operational param 7; removes edge weights when they are smaller than this ratio
* @param sharedGenes(bool) operational param 8: options for having a methodology to use shared Genes options specifed in the paper
* @param max_weight(int) largets weight
* @param edgesBetween(bool) operational param 9: the second method rather than shared Genes expained in the paper
* @param hvalueWeighting(bool) Main graph weighting scheme according to H-values
* @param enrichmentWeighting_o(bool) Main graph weighting scheme according to defined categories
* @param enrichmentWeighting_f(bool) Main graph weighting scheme according to funcassociate results
* @param ppihitratioWeighting(bool) Main graph weighting scheme according to Hit Ratios
*/
GRAPH<int,int> mainGraphHandling( GRAPH<leda::string,int> &PROJECT,
				  node_array<int> &nodesOfProjectStr,
				  array<node> &nodesOfProjectInt,
				  list<GRAPH<int,int> > &GraphList,
				  list<GRAPH<leda::string,int> > &GraphList_S, 
				  list<list<Strings> > &namesForEachGraph,
				  array<GENENAMES> &GenesNode,
				  node_array<double> &HValues,
				  list<double> &H_values,
				  array<char> &abbv,
				  int cat_num,
				  node_array<double> &Xpos,
				  node_array<double> &Ypos,		
				  node_array<point> &pos,
				  edge_array<list<point> > &bends,
				  edge_array<int> &edgeNumbersForStr_,
				  array<edge> &edgeNumbersForInt_,
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
				  int max_weight,
				  bool edgesBetween,
				  bool hvalueWeighting,
				  bool enrichmentWeighting_o,
				  bool enrichmentWeighting_f,
				  bool ppihitratioWeighting,
				  list<int> &categ,
				  int width
				)
{

	node n,n1,n2,projectNode;
	edge e,e1,e2;
	list_item it,it1,it2,it3;
	int count = 0,i;
	int counter = 0;
// 	forall_nodes( n, PROJECT ){
// 		cout << counter << " - " << H_values[ H_values.get_item( counter )] << endl;
// 		counter++;
// 	}
	FILE *reportScoring;
#ifdef LINUX
	char scoringFile[256] = "outputs/biclusters/scoring.txt";
#else
	char scoringFile[256] = "outputs//biclusters//scoring.txt";
#endif

        cout << " BEFORE SCORING\n";

	reportScoring = fopen( scoringFile, "w" );
	if( hvalueWeighting == true ){
		counter = 0;
		fprintf( reportScoring, "Scoring Scheme: H-value\n" );
		forall_nodes( n, PROJECT ){
			HValues[ n ] = H_values[ H_values.get_item( counter )];
			//cout << counter << " - " << HValues[ n ] << endl;
			fprintf( reportScoring, "Bicluster %d %lf\n", counter, H_values[ H_values.get_item( counter )] );
			counter++;
		}
	}
	else{
		if( ppihitratioWeighting == true ){
			double max = 0;
			fprintf( reportScoring, "Scoring Scheme: PPI-HitRatio\n" );
			forall_items( it, GraphList_S){
				if( (double)(GraphList_S[ it ].number_of_edges() / (double)(GraphList_S[ it ].number_of_nodes() * GraphList_S[ it ].number_of_nodes())) > max )
					max = (double)((double)GraphList_S[ it ].number_of_edges() / (double)(GraphList_S[ it ].number_of_nodes() * GraphList_S[ it ].number_of_nodes()));
			}
			counter = 0;
// 			Hmax = max;
			forall_nodes( n, PROJECT ){
				HValues[ n ] = (double)((double)( GraphList_S[ GraphList_S.get_item( counter )].number_of_edges() ) /
							(double)( GraphList_S[ GraphList_S.get_item( counter )].number_of_nodes() * GraphList_S[ GraphList_S.get_item( counter )].number_of_nodes() ) ) /
							max * Hmax;
				H_values[ H_values.get_item( counter )] = HValues[ n ];
				fprintf( reportScoring, "Bicluster %d %lf\n", counter, H_values[ H_values.get_item( counter )] );
// 				cout << counter << " - " << HValues[ n ] << max;
// 				cout << " - " << GraphList_S[ GraphList_S.get_item( counter )].number_of_nodes();
// 				cout << " - "  << GraphList_S[ GraphList_S.get_item( counter )].number_of_edges() << endl;
				counter++;
			}
		}
		else{
			if( enrichmentWeighting_o == true ){
				// User defined categories produced
				FILE *efptr;
				if( ( efptr = fopen( "outputs/enrich/result.txt", "r" )) == NULL && ( efptr = fopen( "outputs//enrich//result.txt", "r" )) == NULL ){
					FILE *erptr;
#ifdef LINUX
					erptr = fopen( "outputs/error.txt", "w" );
#else
					erptr = fopen( "outputs//error.txt", "w" );
#endif
                                        fprintf( erptr, "Error 302: outputs/enrich/result.txt file does not exist\n" );
					fclose( erptr );
                                        cout << "\nError 302: outputs/enrich/result.txt file does not exist\n";
					exit(1);
				}
				else{
					fprintf( reportScoring, "Scoring Scheme: Enrichment-Ratio\n" );
					char readCatName[128];
					for( int i = 0; i <= cat_num; i++ ){
						fscanf( efptr, "%s", readCatName );
					}
					int numberOfBiclusters = GraphList_S.size();
					array<int> categoryMaxGenes( cat_num );
					array< list<int> > categoryPerGenes( numberOfBiclusters );
					
					int bic_id;
					for( int i = 0; i <= cat_num; i++ ){
						if( i == 0 ){
							fscanf( efptr, "%d", &bic_id );
						}else{
							fscanf( efptr, "%d", &categoryMaxGenes[ i-1 ] );
						}
					}
					for( int i = 0; i < numberOfBiclusters; i++ ){
						for( int j = 0; j < cat_num; j++ )
							categoryPerGenes[ i ].append( 0 );
						for( int j = 0; j <= cat_num; j++ ){
							if( j == 0 ){
								fscanf( efptr, "%d", &bic_id );
							}else{
								fscanf( efptr, "%d", &categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j-1 )] );
							}
						}
					}
					double enrich = 0, max;
					counter = 0;
					forall_nodes( n, PROJECT ){
						max = 0;
// 						cout << endl;
						for( int i = 0; i < cat_num; i++ ){
							enrich = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / (double)categoryMaxGenes[ i ];
// 							cout << enrich << "\t";
							if( max < enrich ){
								max = enrich;
								HValues[ n ] = max;
							}
						}
// 						cout << endl;
						counter++;
					}
					max = 0;
					forall_nodes( n, PROJECT ){
						if( max < HValues[ n ] )
							max = HValues[ n ];
					}
					counter = 0;
// 					cout << " Hmax - "<< Hmax << endl;
					forall_nodes( n, PROJECT ){
						HValues[ n ] = HValues[ n ] / max * Hmax;
						H_values[ H_values.get_item( counter )] = HValues[ n ];
						fprintf( reportScoring, "Bicluster %d %lf\n", counter, H_values[ H_values.get_item( counter )] );
// 						cout << counter << " - " << HValues[ n ] << " - " << max << endl;
						counter++;
					}
// 					forall_nodes( n, PROJECT ){
// 						if( Hmax < HValues[ n ] )
// 							Hmax = HValues[ n ];
// 					}
				}
			}
		}
	}
        cout << " AFTER SCORING\n";
	fclose( reportScoring );
// 	cout << " E1 -  " << PROJECT.number_of_edges() << endl;
	it2 = namesForEachGraph.first_item();
	forall_items( it, GraphList_S){
		//cout << GraphList_S[ it ].number_of_nodes() << " --- " << namesForEachGraph[ it2 ].size() << endl;
		it2 = namesForEachGraph.succ( it2 );
	}

	//cout << " Graphlist size " << GraphList_S.size() << endl;

	int count1 = 0, count2 = 0, innerCount = 0, innerCount2 = 0;

	int DEBUG_GIVING_NAMES = 0;
	node f1,f2;
	list_item it_n1,it_n2;
	/*  To iterate over graph list for n^2 loop we need iterators */
	list_item graph_item1,graph_item2;
	/*  To iterate over graph list for n^2 loop we need two iterators and initialise first one*/
	graph_item1 = namesForEachGraph.first_item();
	int comparisonNumber = 0;
	list<Strings> nameGraph1,nameGraph2;
	forall_items( it1, GraphList_S){
		nameGraph1 = namesForEachGraph[ graph_item1 ];
		G1 = GraphList_S[ it1 ];
		it_n1 = nameGraph1.first_item();
		forall_nodes( n, G1 ){
			G1[ n ] = nameGraph1[ it_n1 ].name;
			//cout << G1[ n ] << endl; 
			it_n1 = nameGraph1.succ( it_n1 );
		}
		GraphList_S[ it1 ] = G1;
		graph_item1 = namesForEachGraph.succ( graph_item1 );

	}


  	int countIN = 0;
	int countOUT = 0;
	if( sharedGenes == true ){
		/********************************************************************/
		/*								    */
		/* If you are doing the first model as it is mentioned in the paper */
		/* The main graph composed with the idea using shared genes among   */
                /*  biclusters     						    */
		/*								    */
		/********************************************************************/	
		forall_items( it1, GraphList_S){
			G1 = GraphList_S[ it1 ];
			innerCount = 0;
			forall_nodes( f1, PROJECT ){
				if( innerCount == countOUT )
					break;
				innerCount++;
			}
			int flg = 0;
			forall_items( it2, GraphList_S){
					G2 = GraphList_S[ it2 ];
					innerCount2 = 0;
					forall_nodes( f2, PROJECT ){
						if( innerCount2 == countIN )
							break;
						innerCount2++;
					}
					/*cout << endl << "--------------------------" << endl << "IN " << countIN << endl << "OUT" << countOUT << endl;
					forall_nodes( n, G1 ){
						cout << G1[ n ] << "  ";
					}
					cout << endl;
					forall_nodes( n, G2 ){
						cout << G2[ n ] << "  ";
					}
					cout << endl << "--------------------------" << endl;*/
					
					if( it1 != it2 ){
						int countedges = 0;
						forall_nodes( n1, G1 ){
							forall_nodes( n2, G2 ){
								if( strcmp( G1[ n1 ],G2[ n2 ] ) == 0 ){
									
									edge e1;
// 									if( DEBUG_GIVING_NAMES )
// 										cout << "\n 5 gecti" << endl;
									countedges++;
								}
// 								mark2[ n2 ] = true;
							}
// 							mark[ n1 ] = true;
						}
						edge ee;
						int flag_e = 0;
						forall_edges( ee, PROJECT ){
							if( (PROJECT.source( ee ) == f1 && PROJECT.target( ee ) == f2) || (PROJECT.source( ee ) == f2 && PROJECT.target( ee ) == f1)  ){
								flag_e = 1;
								e = ee;
								break;
							}
						}
						if( flag_e == 0 && f1 != NULL && f2 != NULL ){
							e = PROJECT.new_edge( f1 ,f2 );
							PROJECT[ e ] = countedges;
						}
						else{
							if( e != NULL )
								PROJECT[ e ] += countedges;
						}
					}
					countIN++;
					if( countIN == GraphList_S.size() )
						break;
			}
			countIN = 0;
			countOUT++;
			if( countOUT == GraphList_S.size() + 1 )
				break;
		}
// 		forall_edges( e, PROJECT ){
// 			cout << " e[]\t" << PROJECT[ e ] << endl;
// 		}
	}
	else{
		if( edgesBetween == true ){
		/********************************************************************/	
		/*								    */
		/* Else the second method that is visualized in the paper           */
		/*								    */
		/********************************************************************/	
			node_array<list<node> > neighbors( TEMPINT );
			forall_nodes( n, TEMPINT ){
				list<edge> elist = TEMPINT.out_edges( n );
				forall_items( it, elist ){
					neighbors[ n ].append( TEMPINT.target( elist[ it ]  ) );
				}
			}
			forall_items( it1, GraphList_S){
				G1 = GraphList_S[ it1 ];
				innerCount = 0;
				forall_nodes( f1, PROJECT ){
					if( innerCount == countOUT )
						break;
					innerCount++;
				}
				int flg = 0;
                                if( GraphList_S.succ( it1 ) != NULL )
                                it2 = GraphList_S.succ( it1 );
                                countIN = countOUT;
                                while( GraphList_S.succ( it2 ) != NULL ){
						G2 = GraphList_S[ it2 ];
						innerCount2 = 0;
						forall_nodes( f2, PROJECT ){
							if( innerCount2 == countIN )
								break;
							innerCount2++;
						}			
						if( it1 != it2 ){
							int edgeCount = 0;
							forall_nodes( n1, G1 ){
								forall_nodes( n2, TEMPINT ){
									if( strcmp( GenesNode[ TEMPINT[ n2 ] ].GENE, G1[ n1 ] ) == 0 ){
										n = n2;
										break;
									}
								}
								list<node> between = neighbors[ n ]; 
								forall_items( it3, between ){
									forall_nodes( n2, G2 ){
										if( strcmp( GenesNode[ TEMPINT[ between[ it3 ] ] ].GENE , G2[ n2 ] ) == 0 ){
											edgeCount++;
										}
									}
								}
							}
							e = PROJECT.new_edge( f1 ,f2 );
							PROJECT[ e ] = edgeCount;
						}	
						countIN++;
						if( countIN == GraphList_S.size() )
							break;
                                                it2 = GraphList_S.succ( it2 );
				}
				countIN = 0;
				countOUT++;
				if( countOUT == GraphList_S.size() + 1 )
					break;
			}
		}
		else{
			forall_items( it1, GraphList_S){
				G1 = GraphList_S[ it1 ];
				innerCount = 0;
				forall_nodes( f1, PROJECT ){
					if( innerCount == countOUT )
						break;
					innerCount++;
				}
				int flg = 0;
				forall_items( it2, GraphList_S){
						G2 = GraphList_S[ it2 ];
						innerCount2 = 0;
						forall_nodes( f2, PROJECT ){
							if( innerCount2 == countIN )
								break;
							innerCount2++;
						}			
						if( it1 != it2 ){
							int edgeCount = 0;
							forall_nodes( n1, G1 ){
								forall_nodes( n2, G2 ){
									if( strcmp( G1[ n1 ],G2[ n2 ] ) == 0 ){
										edgeCount++;
									}
								}
							}
							e = PROJECT.new_edge( f1 ,f2 );
							PROJECT[ e ] = edgeCount;
						}
						countIN++;
						if( countIN == GraphList_S.size() )
							break;
				}
				countIN = 0;
				countOUT++;
				if( countOUT == GraphList_S.size() + 1 )
					break;
			}
		}
	}
// 	cout << " FORM MAIN GRAPH\n";
// 	cout << " E2 -  " << PROJECT.number_of_edges() << endl;
	list<int> old_edges;
	list<edge> old_edges_e;
// 	old_edges_e = Make_Simple( PROJECT );
// 	PROJECT.hide_edges( old_edges_e );
	//cout << endl << " Comparison Number " << comparisonNumber << endl;
	int max = 0;
// 	forall_edges( e1, PROJECT ){
// 		forall_edges( e2, PROJECT ){
// 			if( e1 != e2 ){
// 				if( PROJECT.source( e1 ) == PROJECT.source( e2 ) && PROJECT.target( e1 ) == PROJECT.target( e2 ) ){
// 					PROJECT.del_edge( e1 );
// 				}
// 			}
// 		}
// 	}

	forall_edges( e1, PROJECT ){
// 		cout << "- " << PROJECT[ e1 ] << " - " <<  ( 1.0 / ( HValues[ PROJECT.source( e1 ) ] / Hmax * HValues[ PROJECT.target( e1 ) ] / Hmax  ) ) <<endl;
// 		PROJECT[ e1 ] =  ( (int)( 1.0 / ( HValues[ PROJECT.source( e1 ) ] / Hmax * HValues[ PROJECT.target( e1 ) ] / Hmax  ) ) * PROJECT[ e1 ] );
// 		cout << "- " << PROJECT[ e1 ] << endl;
		if( PROJECT[ e1 ] > max )
			max = PROJECT[ e1 ];
	}
	double multiply = 100.0 / (double)( max );
	
	node_array<int> indexsP( PROJECT, 0 );
	count1 = 0;
	forall_nodes( projectNode, PROJECT ){
		indexsP[ projectNode ] = count1;
		count1++;
	}

	/********************************************************************/
	/*								    */
	/*	Make some filtrations					    */
	/*								    */
	/********************************************************************/
	forall_edges( e1, PROJECT ){
// 		cout << PROJECT[ e1 ] << " - " << " old " << endl;	
		n = PROJECT.source( e1 );
		list<edge> edges_l = G.out_edges( n );
		PROJECT[ e1 ] = (int)((double)PROJECT[ e1 ] * multiply);
		if( PROJECT[ e1 ] < (int)(removeRat * 100.0) /*&& edges_l.size() > 2*/ ){
// 			cout << multiply << " Deleted Since - " << PROJECT[ e1 ] << " < " << (removeRat * 100.0) << endl;
			PROJECT.del_edge( e1 );
		}
		else{
			old_edges.append( PROJECT[ e1 ] );			
// 			cout << PROJECT[ e1 ] << " - " << " new " << endl;
		}
	}

	list<node> hided;
	if( simplify == 1 ){
		forall_nodes( n, PROJECT ){
			if( HValues[ n ] < 0.20 && PROJECT.outdeg(n) == 0 && PROJECT.indeg(n) == 0 )
				PROJECT.del_node( n );
				//hided.append( n );//PROJECT.del_node( n );
		}
	}
// 	cout << " E3 -  " << PROJECT.number_of_edges() << endl;
	nodesOfProjectStr.init( PROJECT );

	GRAPH<int,int> PROJECT2;

	count = 0;
	array<node> Nodes( PROJECT.number_of_nodes() + 1);
	forall_nodes( n1, PROJECT ){
		n2 = PROJECT2.new_node();
		Nodes[ count ] = n1;
		nodesOfProjectStr[ n1 ] = count;
		nodesOfProjectInt[ count ] = n2;
		count++;
	}	
	node_array<double> HValues2( PROJECT2 );
	count = 0;
	node_array<int> PARS( PROJECT2 );
	forall_nodes( n1, PROJECT2 ){
		HValues2[ n1 ] = HValues[ Nodes[ count ] ];
		PARS[ n1 ] = count;
		count++;
	}
	count = 0;
	edge_array<int> edgeNumbersForStr( PROJECT );
	array<edge> edgeNumbersForInt( PROJECT2.number_of_edges() );
	forall_edges( e1, PROJECT ){
		e2 = PROJECT2.new_edge( nodesOfProjectInt[ nodesOfProjectStr[ PROJECT.source( e1 ) ] ], nodesOfProjectInt[ nodesOfProjectStr[ PROJECT.target( e1 ) ] ] );
		PROJECT2[ e2 ]= PROJECT[ e1 ];
		edgeNumbersForStr[ e1 ] = count;
		edgeNumbersForInt.resize( PROJECT2.number_of_edges() );
		edgeNumbersForInt[ count ] = e2;
		count++;
	}

	/********************************************************************/
	/*								    */
	/*	Run our weighted layering method			    */
	/*	Settings are imported from main menu			    */
	/*								    */
	/********************************************************************/
	Xpos.init( PROJECT2 );
	Ypos.init( PROJECT2 );
	pos.init( PROJECT2 );
	bends.init( PROJECT2 );
	edgeNumbersForStr_ = edgeNumbersForStr;
	edgeNumbersForInt_ = edgeNumbersForInt;
        cout << " E4 -  " << PROJECT2.number_of_edges() << endl;
	i = 0;
        cout << " RUN LAYOUT\n";
        if( PROJECT2.number_of_edges() != 0 )
            RUN_SELF2( PROJECT2, PARS, max_weight, width, Xpos, Ypos, i + 1, pos, bends, HValues2, hided, algorithmFlag, brandFlag, brandFlag2, ourMethodFlag, space, increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher, categ );
        else{
                    FILE *erptr;
#ifdef LINUX
                    erptr = fopen( "outputs/error.txt", "w" );
#else
                    erptr = fopen( "outputs//error.txt", "w" );
#endif
                    fprintf( erptr, "Error 302.1: central graph has no edges\n" );
                    fclose( erptr );
                    cout << "\nError 302.1: central graph has no edges\n";
                    exit(1);
        }
        cout << " E5 -  " << PROJECT2.number_of_edges() << endl;
	return PROJECT2;
}


/**
* mainGraphHandling is a function that handles layout for GO categories
* @param Categories (leda::array<char>) stores each categories for each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param biclusters we do not lose the track of bicluster genes(GENES)
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu
* @param POS Positions of all graphs stored in leda list
* @param BENDS Edge bends meanly the points where edge passes and for all graphs
* @param pos INTERACTION graph node positions
* @param bends INTERACTION graph edge bends
* @param namesForEachGraph have the gene names(GENENAMES) for each bicluster
* @param width (int) operational param 1; max width for layering
* @param algorithmFlag (int) operational param 2; chooses x-coordinate alg.
* @param space (int) operational param 3; space between nodes
* @param xCoordFlag (bool) operational param 4; our preliminary method for runnin layering
* @param increment operational param 5; space between layers
* @param ledaPostFlag (bool) operational param 6; if you need some post-process after x-coordinate assignment
* @param removeRat(double) operational param 7; removes edge weights when they are smaller than this ratio
* @param sharedGenes(bool) operational param 8: options for having a methodology to use shared Genes options specifed in the paper
* @param max_weight(int) largets weight
* @param edgesBetween(bool) operational param 9: the second method rather than shared Genes expained in the paper
* @param hvalueWeighting(bool) Main graph weighting scheme according to H-values
* @param enrichmentWeighting_o(bool) Main graph weighting scheme according to defined categories
* @param enrichmentWeighting_f(bool) Main graph weighting scheme according to funcassociate results
* @param ppihitratioWeighting(bool) Main graph weighting scheme according to Hit Ratios
*/
GRAPH<int,int> mainGraphHandling2( GRAPH<leda::string,int> &PROJECT,
				  node_array<int> &nodesOfProjectStr,
				  array<node> &nodesOfProjectInt,
				  list<GRAPH<int,int> > &GraphList,
				  list<GRAPH<leda::string,int> > &GraphList_S, 
				  list<list<Strings> > &namesForEachGraph,
				  array<GENENAMES> &GenesNode,
				  node_array<double> &HValues,
				  list<double> &H_values,
				  array<char> &abbv,
				  int cat_num,
				  node_array<double> &Xpos,
				  node_array<double> &Ypos,		
				  node_array<point> &pos,
				  edge_array<list<point> > &bends,
				  edge_array<int> &edgeNumbersForStr_,
				  array<edge> &edgeNumbersForInt_,
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
				  int max_weight,
				  bool edgesBetween,
				  bool hvalueWeighting,
				  bool enrichmentWeighting_o,
				  bool enrichmentWeighting_f,
				  bool ppihitratioWeighting,
				  list<int> &categ,
				  int width
				)
{

	node n,n1,n2,projectNode;
	edge e,e1,e2;
	list_item it,it1,it2,it3;
	int count = 0,i;
	int counter = 0;
// 	forall_nodes( n, PROJECT ){
// 		cout << counter << " - " << H_values[ H_values.get_item( counter )] << endl;
// 		counter++;
// 	}
	FILE *reportScoring;
#ifdef LINUX
	char scoringFile[256] = "outputs/biclusters/scoring.txt";
#else
	char scoringFile[256] = "outputs//biclusters//scoring.txt";
#endif

//	cout << " BEFORE SCORING\n";

	reportScoring = fopen( scoringFile, "w" );
	if( hvalueWeighting == true ){
		counter = 0;
		fprintf( reportScoring, "Scoring Scheme: H-value\n" );
		forall_nodes( n, PROJECT ){
			HValues[ n ] = H_values[ H_values.get_item( counter )];
			//cout << counter << " - " << HValues[ n ] << endl;
			if( H_values[ H_values.get_item( counter )] >= 0 ){
				fprintf( reportScoring, "Bicluster %d %lf\n", counter, H_values[ H_values.get_item( counter )] );
			}
			else{
				fprintf( reportScoring, "Bicluster %d %lf\n", counter, 0.0 );
			}
			counter++;
		}
                double max = 0,min;
                forall_nodes( n, PROJECT ){
                        if( max < HValues[ n ] )
                                max = HValues[ n ];
                }
                min = max;
                forall_nodes( n, PROJECT ){
                    if( min > HValues[ n ] )
                        min = HValues[ n ];
                }

                counter = 0;
// 					cout << " Hmax - "<< Hmax << endl;
                forall_nodes( n, PROJECT ){
                        if( max - HValues[ n ] >= 0 )
                            HValues[ n ] = ( max - HValues[ n ] )/ (max - min);
                        else
                            HValues[ n ] = 0;
                        H_values[ H_values.get_item( counter )] = HValues[ n ];
                        counter++;
                }
	}
	else{
		if( ppihitratioWeighting == true ){
			double max = 0;
			fprintf( reportScoring, "Scoring Scheme: PPI-HitRatio\n" );
			forall_items( it, GraphList_S){
				if( (double)(GraphList_S[ it ].number_of_edges() / (double)(GraphList_S[ it ].number_of_nodes() * GraphList_S[ it ].number_of_nodes())) > max )
					max = (double)((double)GraphList_S[ it ].number_of_edges() / (double)(GraphList_S[ it ].number_of_nodes() * GraphList_S[ it ].number_of_nodes()));
			}
			counter = 0;
// 			Hmax = max;
			forall_nodes( n, PROJECT ){
				HValues[ n ] = (double)((double)( GraphList_S[ GraphList_S.get_item( counter )].number_of_edges() ) /
							(double)( GraphList_S[ GraphList_S.get_item( counter )].number_of_nodes() * GraphList_S[ GraphList_S.get_item( counter )].number_of_nodes() ) ) /
							max * Hmax;
				H_values[ H_values.get_item( counter )] = HValues[ n ];
				fprintf( reportScoring, "Bicluster %d %lf\n", counter, H_values[ H_values.get_item( counter )] );
// 				cout << counter << " - " << HValues[ n ] << max;
// 				cout << " - " << GraphList_S[ GraphList_S.get_item( counter )].number_of_nodes();
// 				cout << " - "  << GraphList_S[ GraphList_S.get_item( counter )].number_of_edges() << endl;
				counter++;
			}
		}
		else{
			if( enrichmentWeighting_o == true ){
				// User defined categories produced
				FILE *efptr;
				if( ( efptr = fopen( "outputs/enrich/result.txt", "r" )) == NULL && ( efptr = fopen( "outputs//enrich//result.txt", "r" )) == NULL ){
					FILE *erptr;
#ifdef LINUX
					erptr = fopen( "outputs/error.txt", "w" );
#else
					erptr = fopen( "outputs//error.txt", "w" );
#endif
                                        fprintf( erptr, "Error 302: outputs/enrich/result.txt file does not exist\n" );
					fclose( erptr );
                                        cout << "\nError 302: outputs/enrich/result.txt file does not exist\n";
					exit(1);
				}
				else{
					fprintf( reportScoring, "Scoring Scheme: Enrichment-Ratio\n" );
					char readCatName[128];
					for( int i = 0; i <= cat_num; i++ ){
						fscanf( efptr, "%s", readCatName );
					}
					int numberOfBiclusters = GraphList_S.size();
					array<int> categoryMaxGenes( cat_num );
					array< list<int> > categoryPerGenes( numberOfBiclusters );
					
					int bic_id;
					for( int i = 0; i <= cat_num; i++ ){
						if( i == 0 ){
							fscanf( efptr, "%d", &bic_id );
						}else{
							fscanf( efptr, "%d", &categoryMaxGenes[ i-1 ] );
						}
					}
					for( int i = 0; i < numberOfBiclusters; i++ ){
						for( int j = 0; j < cat_num; j++ )
							categoryPerGenes[ i ].append( 0 );
						for( int j = 0; j <= cat_num; j++ ){
							if( j == 0 ){
								fscanf( efptr, "%d", &bic_id );
							}else{
								fscanf( efptr, "%d", &categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j-1 )] );
							}
						}
					}
					double enrich = 0, max;
					counter = 0;
					forall_nodes( n, PROJECT ){
						max = 0;
// 						cout << endl;
						for( int i = 0; i < cat_num; i++ ){
							enrich = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / (double)categoryMaxGenes[ i ];
// 							cout << enrich << "\t";
							if( max < enrich ){
								max = enrich;
								HValues[ n ] = max;
							}
						}
// 						cout << endl;
						counter++;
					}
					max = 0;
					forall_nodes( n, PROJECT ){
						if( max < HValues[ n ] )
							max = HValues[ n ];
					}
					counter = 0;
// 					cout << " Hmax - "<< Hmax << endl;
					forall_nodes( n, PROJECT ){
						HValues[ n ] = HValues[ n ] / max * Hmax;
						H_values[ H_values.get_item( counter )] = HValues[ n ];
						fprintf( reportScoring, "Bicluster %d %lf\n", counter, H_values[ H_values.get_item( counter )] );
// 						cout << counter << " - " << HValues[ n ] << " - " << max << endl;
						counter++;
					}
// 					forall_nodes( n, PROJECT ){
// 						if( Hmax < HValues[ n ] )
// 							Hmax = HValues[ n ];
// 					}
				}
			}
                        else{
                                counter = 0;
                                fprintf( reportScoring, "Scoring Scheme: Reliabilities\n" );
                                double Hmax = 0;
                                forall_nodes( n, PROJECT ){
                                        HValues[ n ] = H_values[ H_values.get_item( counter )];
                                        if( HValues[ n ] > Hmax )
                                            Hmax = HValues[ n ];
                                        counter++;
                                }
                                counter = 0;
                                forall_nodes( n, PROJECT ){
                                        //HValues[ n ] = (Hmax - HValues[ n ])+0.01;
                                        HValues[ n ] = (HValues[ n ] / Hmax) + 0.01;
//                                        cout << counter << " - " << HValues[ n ] << endl;
                                        if( HValues[ n ] >= 0 ){
                                                fprintf( reportScoring, "Bicluster %d %lf\n", counter, HValues[ n ] );
                                        }
                                        else{
                                                fprintf( reportScoring, "Bicluster %d %lf\n", counter, 0.0 );
                                        }
                                        counter++;
                                }
                        }
		}
	}
//	cout << " AFTER SCORING\n";
	fclose( reportScoring );
// 	cout << " E1 -  " << PROJECT.number_of_edges() << endl;
	it2 = namesForEachGraph.first_item();
	forall_items( it, GraphList_S){
		//cout << GraphList_S[ it ].number_of_nodes() << " --- " << namesForEachGraph[ it2 ].size() << endl;
		it2 = namesForEachGraph.succ( it2 );
	}

	//cout << " Graphlist size " << GraphList_S.size() << endl;

	int count1 = 0, count2 = 0, innerCount = 0, innerCount2 = 0;

	int DEBUG_GIVING_NAMES = 0;
	node f1,f2;
	list_item it_n1,it_n2;
	/*  To iterate over graph list for n^2 loop we need iterators */
	list_item graph_item1,graph_item2;
	/*  To iterate over graph list for n^2 loop we need two iterators and initialise first one*/
	graph_item1 = namesForEachGraph.first_item();
	int comparisonNumber = 0;
	list<Strings> nameGraph1,nameGraph2;
	forall_items( it1, GraphList_S){
		nameGraph1 = namesForEachGraph[ graph_item1 ];
		G1 = GraphList_S[ it1 ];
		it_n1 = nameGraph1.first_item();
		forall_nodes( n, G1 ){
			G1[ n ] = nameGraph1[ it_n1 ].name;
			//cout << G1[ n ] << endl; 
			it_n1 = nameGraph1.succ( it_n1 );
		}
		GraphList_S[ it1 ] = G1;
		graph_item1 = namesForEachGraph.succ( graph_item1 );

	}


  	int countIN = 0;
	int countOUT = 0;
	if( sharedGenes == true ){
		/********************************************************************/
		/*								    */
		/* If you are doing the first model as it is mentioned in the paper */
		/* The main graph composed with the idea using shared genes among   */
                /*  biclusters     						    */
		/*								    */
		/********************************************************************/	
		forall_items( it1, GraphList_S){
			G1 = GraphList_S[ it1 ];
			innerCount = 0;
			forall_nodes( f1, PROJECT ){
				if( innerCount == countOUT )
					break;
				innerCount++;
			}
			int flg = 0;
			forall_items( it2, GraphList_S){
					G2 = GraphList_S[ it2 ];
					innerCount2 = 0;
					forall_nodes( f2, PROJECT ){
						if( innerCount2 == countIN )
							break;
						innerCount2++;
					}
					/*cout << endl << "--------------------------" << endl << "IN " << countIN << endl << "OUT" << countOUT << endl;
					forall_nodes( n, G1 ){
						cout << G1[ n ] << "  ";
					}
					cout << endl;
					forall_nodes( n, G2 ){
						cout << G2[ n ] << "  ";
					}
					cout << endl << "--------------------------" << endl;*/
					
					if( it1 != it2 ){
						int countedges = 0;
						forall_nodes( n1, G1 ){
							forall_nodes( n2, G2 ){
								if( strcmp( G1[ n1 ],G2[ n2 ] ) == 0 ){
									
									edge e1;
// 									if( DEBUG_GIVING_NAMES )
// 										cout << "\n 5 gecti" << endl;
									countedges++;
								}
// 								mark2[ n2 ] = true;
							}
// 							mark[ n1 ] = true;
						}
						edge ee;
						int flag_e = 0;
						forall_edges( ee, PROJECT ){
							if( (PROJECT.source( ee ) == f1 && PROJECT.target( ee ) == f2) || (PROJECT.source( ee ) == f2 && PROJECT.target( ee ) == f1)  ){
								flag_e = 1;
								e = ee;
								break;
							}
						}
						if( flag_e == 0 && f1 != NULL && f2 != NULL ){
							e = PROJECT.new_edge( f1 ,f2 );
							PROJECT[ e ] = countedges;
						}
						else{
							if( e != NULL )
								PROJECT[ e ] += countedges;
						}
					}
					countIN++;
					if( countIN == GraphList_S.size() )
						break;
			}
			countIN = 0;
			countOUT++;
			if( countOUT == GraphList_S.size() + 1 )
				break;
		}
// 		forall_edges( e, PROJECT ){
// 			cout << " e[]\t" << PROJECT[ e ] << endl;
// 		}
	}
	else{
		if( edgesBetween == true ){
		/********************************************************************/	
		/*								    */
		/* Else the second method that is visualized in the paper           */
		/*								    */
		/********************************************************************/	
			node_array<list<node> > neighbors( TEMPINT );
			forall_nodes( n, TEMPINT ){
				list<edge> elist = TEMPINT.out_edges( n );
				forall_items( it, elist ){
					neighbors[ n ].append( TEMPINT.target( elist[ it ]  ) );
				}
			}
			forall_items( it1, GraphList_S){
				G1 = GraphList_S[ it1 ];
				innerCount = 0;
				forall_nodes( f1, PROJECT ){
					if( innerCount == countOUT )
						break;
					innerCount++;
				}
				int flg = 0;
				forall_items( it2, GraphList_S){
						G2 = GraphList_S[ it2 ];
						innerCount2 = 0;
						forall_nodes( f2, PROJECT ){
							if( innerCount2 == countIN )
								break;
							innerCount2++;
						}			
						if( it1 != it2 ){
							int edgeCount = 0;
							forall_nodes( n1, G1 ){
								forall_nodes( n2, TEMPINT ){
									if( strcmp( GenesNode[ TEMPINT[ n2 ] ].GENE, G1[ n1 ] ) == 0 ){
										n = n2;
										break;
									}
								}
								list<node> between = neighbors[ n ]; 
								forall_items( it3, between ){
									forall_nodes( n2, G2 ){
										if( strcmp( GenesNode[ TEMPINT[ between[ it3 ] ] ].GENE , G2[ n2 ] ) == 0 ){
											edgeCount++;
										}
									}
								}
							}

                                                        edge ee;
                                                        int flag_e = 0;
                                                        forall_edges( ee, PROJECT ){
                                                                if( (PROJECT.source( ee ) == f1 && PROJECT.target( ee ) == f2) || (PROJECT.source( ee ) == f2 && PROJECT.target( ee ) == f1)  ){
                                                                        flag_e = 1;
                                                                        e = ee;
                                                                        break;
                                                                }
                                                        }
                                                        if( flag_e == 0 && f1 != NULL && f2 != NULL ){
                                                                e = PROJECT.new_edge( f1 ,f2 );
                                                                PROJECT[ e ] = edgeCount;
                                                        }
                                                        else{
                                                                if( e != NULL )
                                                                        PROJECT[ e ] += edgeCount;
                                                        }
						}	
						countIN++;
						if( countIN == GraphList_S.size() )
							break;
				}
				countIN = 0;
				countOUT++;
				if( countOUT == GraphList_S.size() + 1 )
					break;
			}
		}
		else{
			forall_items( it1, GraphList_S){
				G1 = GraphList_S[ it1 ];
				innerCount = 0;
				forall_nodes( f1, PROJECT ){
					if( innerCount == countOUT )
						break;
					innerCount++;
				}
				int flg = 0;
				forall_items( it2, GraphList_S){
						G2 = GraphList_S[ it2 ];
						innerCount2 = 0;
						forall_nodes( f2, PROJECT ){
							if( innerCount2 == countIN )
								break;
							innerCount2++;
						}			
						if( it1 != it2 ){
							int edgeCount = 0;
							forall_nodes( n1, G1 ){
								forall_nodes( n2, G2 ){
									if( strcmp( G1[ n1 ],G2[ n2 ] ) == 0 ){
										edgeCount++;
									}
								}
							}
                                                        if( edgeCount != 0 ){
                                                            e = PROJECT.new_edge( f1 ,f2 );
                                                            PROJECT[ e ] = edgeCount;
                                                        }
						}
						countIN++;
						if( countIN == GraphList_S.size() )
							break;
				}
				countIN = 0;
				countOUT++;
				if( countOUT == GraphList_S.size() + 1 )
					break;
			}
		}
	}
//	cout << " FORM MAIN GRAPH\n";
// 	cout << " E2 -  " << PROJECT.number_of_edges() << endl;
	list<int> old_edges;
	list<edge> old_edges_e;
// 	old_edges_e = Make_Simple( PROJECT );
// 	PROJECT.hide_edges( old_edges_e );
	//cout << endl << " Comparison Number " << comparisonNumber << endl;
	int max = 0;
        forall_edges( e, PROJECT ){
            if( PROJECT.source( e ) == PROJECT.target( e ) )
                PROJECT.del_edge( e );
        }

	forall_edges( e1, PROJECT ){
// 		cout << "- " << PROJECT[ e1 ] << " - " <<  ( 1.0 / ( HValues[ PROJECT.source( e1 ) ] / Hmax * HValues[ PROJECT.target( e1 ) ] / Hmax  ) ) <<endl;
// 		PROJECT[ e1 ] =  ( (int)( 1.0 / ( HValues[ PROJECT.source( e1 ) ] / Hmax * HValues[ PROJECT.target( e1 ) ] / Hmax  ) ) * PROJECT[ e1 ] );
// 		cout << "- " << PROJECT[ e1 ] << endl;
		if( PROJECT[ e1 ] > max )
			max = PROJECT[ e1 ];
	}
        //double multiply = 100.0 / (double)( max );
        int min = max;
        forall_edges( e1, PROJECT ){
            if( PROJECT[ e1 ] < min )
                    min = PROJECT[ e1 ];
        }

	node_array<int> indexsP( PROJECT, 0 );
	count1 = 0;
	forall_nodes( projectNode, PROJECT ){
		indexsP[ projectNode ] = count1;
		count1++;
	}

	/********************************************************************/
	/*								    */
	/*	Make some filtrations					    */
	/*								    */
	/********************************************************************/
        forall_edges( e1, PROJECT ){
                if( max - min != 0 )
                    PROJECT[ e1 ] = (int)((double)( PROJECT[ e1 ] - min) / (double)( max - min ) * 100.0);
                else
                    PROJECT[ e1 ] = 1;
                if( PROJECT[ e1 ] < (int)(removeRat * 100.0) || PROJECT[ e1 ] == 0 ){
// 			cout << multiply << " Deleted Since - " << PROJECT[ e1 ] << " < " << (removeRat * 100.0) << endl;
			PROJECT.del_edge( e1 );
                }
	}

	list<node> hided;
	if( simplify == 1 ){
		forall_nodes( n, PROJECT ){
			if( HValues[ n ] < 0.20 && PROJECT.outdeg(n) == 0 && PROJECT.indeg(n) == 0 )
				PROJECT.del_node( n );
				//hided.append( n );//PROJECT.del_node( n );
		}
                cout << " S Done\n";
	}
// 	cout << " E3 -  " << PROJECT.number_of_edges() << endl;
	nodesOfProjectStr.init( PROJECT );

	GRAPH<int,int> PROJECT2;

	count = 0;
	array<node> Nodes( PROJECT.number_of_nodes() + 1);
	forall_nodes( n1, PROJECT ){
		n2 = PROJECT2.new_node();
		Nodes[ count ] = n1;
		nodesOfProjectStr[ n1 ] = count;
		nodesOfProjectInt[ count ] = n2;
		count++;
	}	
	node_array<double> HValues2( PROJECT2 );
	count = 0;
	node_array<int> PARS( PROJECT2 );
	forall_nodes( n1, PROJECT2 ){
		HValues2[ n1 ] = HValues[ Nodes[ count ] ];
		PARS[ n1 ] = count;
		count++;
	}
	count = 0;
	edge_array<int> edgeNumbersForStr( PROJECT );
	array<edge> edgeNumbersForInt( PROJECT2.number_of_edges() );
	forall_edges( e1, PROJECT ){
		e2 = PROJECT2.new_edge( nodesOfProjectInt[ nodesOfProjectStr[ PROJECT.source( e1 ) ] ], nodesOfProjectInt[ nodesOfProjectStr[ PROJECT.target( e1 ) ] ] );
		PROJECT2[ e2 ]= PROJECT[ e1 ];
		edgeNumbersForStr[ e1 ] = count;
		edgeNumbersForInt.resize( PROJECT2.number_of_edges() );
		edgeNumbersForInt[ count ] = e2;
		count++;
	}

	/********************************************************************/
	/*																	*/
	/*	Run our force directed method								    */
	/*	Settings are imported from main menu							*/
	/*																	*/
	/********************************************************************/
	Xpos.init( PROJECT2 );
	Ypos.init( PROJECT2 );
	pos.init( PROJECT2 );
	bends.init( PROJECT2 );
	edgeNumbersForStr_ = edgeNumbersForStr;
	edgeNumbersForInt_ = edgeNumbersForInt;
// 	cout << " E4 -  " << PROJECT2.number_of_edges() << endl;
	i = 0;
//	cout << " RUN LAYOUT\n";
	RUN_FFD_SELF( PROJECT2, PARS, max_weight, width, Xpos, Ypos, i + 1, pos, bends, HValues2, hided, algorithmFlag, brandFlag, brandFlag2, ourMethodFlag, space, increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher, categ );	
// 	cout << " E5 -  " << PROJECT2.number_of_edges() << endl;
	return PROJECT2;
}

/**
    Does handling of obtaining colors via first level of Molecular Function(For this function) Reads from gofile and
    finds protein to list of categories for each proteins and write into catfile where robinviz main
    reads from main file.
    File format first we write
    Cat Number generally 
    18 
    % then abrrv and category names
    A antioxidant activity
    .
    .
    .
    S transporter activity
    % then gene to category abbrvs.
    gene1 ABC
    gene2 PRS
    ...
    @param catfile(char[256]) :saved for post processing in bicluster and go handling
    @param gofile(char[256]) : needed for parsing and forming colors for each genes
**/
void colorHandling( char catfile[256], char gofile[256] ){
	int GOSIZE = 18;
        char abbrv[34] = "ABCDEFGHIJKLMNOPQRSTUVWYZ01234567";
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
	array<list<GENES> > molecularFunction( GOSIZE );
	list<GENES> allGenes;

	FILE *f;
	list_item it,it2;
	char line[ 1000000 ];
	char *pc,*pend,*go,*cat;
	const char *strDelim = "\t\n";
        const char *strDelim2 = " \t";

	if( (f = fopen( gofile, "r" )) == NULL){
		FILE *erptr;
#ifdef LINUX
		erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
                fprintf( erptr, "Error color: You did not specify GO based functional category file hence we use it for color processing\n" );
                fclose( erptr );
                cout << "\nError color: You did not specify GO based functional category file hence we use it for color processing\n";
		exit(1);
	}

	/* First we parse gofile to see how many lines of categories */
	int line_i = 0;
	while( !feof( f ) ){
		fgets( line, 1000000, f );
		line_i++;
	}
        if( line_i == 0 ){
            FILE *erptr;
#ifdef LINUX
            erptr = fopen( "outputs/error.txt", "w" );
#else
            erptr = fopen( "outputs//error.txt", "w" );
#endif
            fprintf( erptr, "Error-color-2:GO based functional category file has no category hence we can not continue\n" );
            fclose( erptr );
            cout << "\nError-color-2:GO based functional category file has no category hence we can not continue\n";
            exit(1);
        }
// 		cout << "\t Will Parse " << line_i << " lines, Parsing begins...\n";
	rewind( f );
	line_i = 0;
	/* We rewind and based on line by line parsing we check that is gofile contains
	   the predefined categories in molecularF double char array
	*/
	while( !feof( f ) ){
		// count rows
// 		two_tuple<CATNAMES,int> tup;
// 		tup.second() = 0;
		fgets( line, 1000000, f );
		pc = strtok( line, strDelim );
		go = pc;
		if( feof( f ) )
			break;
		int count = 0;
		int categIndex = -1;
		while( pc != NULL ){
			if( count == 0 ){
				pc = strtok( NULL, strDelim );
				cat = pc;
// 				sprintf( tup.first().catName, "%s", cat );
				for( int i = 0; i < GOSIZE; i++ ){
					if( strcmp( molecularF[ i ], cat ) == 0 ){
						categIndex = i;
// 						cout <<  molecularF[ i ] << " is found\n";
					}
				}
			}
			else{
				if( categIndex != -1 ){
					pc = strtok( NULL, strDelim2 );
//                                        cout << pc << " - ";
					GENES temporary;
					sprintf( temporary.GENE, "%s", pc );
					molecularFunction[ categIndex ].append( temporary );
					allGenes.append( temporary );
				}
				else
					break;
			}
                        pend = pc; /* check last char for newline terminator */
                        int chCount = 0;
                        while( *(pend+chCount) != '\0' ){
                            if( *(pend+chCount) == '\n' ){
                                    pc = NULL;
                                    break;
                            }
                            chCount++;
                        }
			count++;
		}
//                cout << endl << "************************\n";
		line_i++;
// 		if( line_i % 10 == 0 )
// 			cout << "\t Parsed " << line_i << " ...\n";
	}
//        cout << " We are done \n";
	fclose( f );
	/* With the next double loop we obtain pure gene list obtained with 18 categories */
        array<bool> marked( allGenes.size() + 1 );
        int count = 0;
        forall_items( it, allGenes ){
            marked[ count ] = false;
            count++;
        }
        count = 0;
	forall_items( it, allGenes ){
//                cout << allGenes[it].GENE << " - " << count << endl;
//                it2 = allGenes.succ(it);
//                while( it2 != NULL ){
//			if( it != it2 ){
////                                cout << allGenes[it].GENE << " - " << allGenes[it2].GENE << endl;
//				if( strcmp( allGenes[ it ].GENE, allGenes[ it2 ].GENE ) == 0 ){
//                                        list_item it3 = allGenes.succ(it2);
//					allGenes.del_item( it2 );
//                                        if( it3 != NULL )
//                                            it2 = it3;
//                                        else
//                                            break;
//				}
//			}
//                        it2 = allGenes.succ(it2);
//		}
                if( marked[ count ] == false ){
                    int count2 = 0;
                    forall_items( it2, allGenes ){
                        if( it != it2 ){
                            if( marked[ count2 ] == false ){
                                if( strcmp( allGenes[ it ].GENE, allGenes[ it2 ].GENE ) == 0 ){
                                    marked[ count2 ] = true;
                                }
                            }
                        }
                        count2++;
                    }
                }
                count++;
	}
        list<list_item> deleted;
        count = 0;
        forall_items( it, allGenes ){
            if( marked[ count ] == true ){
                deleted.append( it );
            }
            count++;
        }
        forall_items( it, deleted ){
            allGenes.del_item(deleted[ it ]);
        }

//        cout << " We are done \n";
	/* we now begin saving catfile. We collect the genes for each categories. */
	f = fopen( catfile, "w" );
	fprintf( f, "%d\n", GOSIZE );
	/* Saving category header: abbrv and name */
	for( int i = 0; i < GOSIZE; i++ ){
		for( int j = 0; molecularF[ i ][ j ] != '\0'; j++ ){
			if( molecularF[ i ][ j ] == ' ' ){
				  molecularF[ i ][ j ] = '_';
			}
		}
		fprintf( f, "\"%c\"\t%s\n", abbrv[ i ], molecularF[ i ] );
	}
//        cout << " We are done \n";
	/* Saving gene to abbrvs */
	forall_items( it, allGenes ){
		bool one = false;
		char funcCateg[18]="",funcCateg2[18]="";
		for( int i = 0; i < GOSIZE; i++ ){
			forall_items( it2, molecularFunction[ i ] ){
				if( strcmp( allGenes[ it ].GENE, molecularFunction[ i ][ it2 ].GENE ) == 0 ){
					one = true;
					sprintf( funcCateg2, "%c", abbrv[ i ] );
					strcat( funcCateg, funcCateg2 );
					break;
				}
			}
		}
		if( one == false ){
// 			fprintf( f, "X\n" );
		}
		else{
			int j = 0;
			while( allGenes[ it ].GENE[ j ] != '\0' ){
				if( allGenes[ it ].GENE[ j ] == '\n' ){
					allGenes[ it ].GENE[ j ] = '\0';
				}
				j++;
			}
			fprintf( f, "%s\t%s\n", allGenes[ it ].GENE, funcCateg );
		}
	}
//        cout << " We are done \n";
	fclose( f );
}

/**
    Does handling of obtaining colors via first level of X Function( General Functional terms) Reads from gofile and
    finds protein to list of categories for each proteins and write into catfile where robinviz main
    reads from main file.
    File format first we write
    Cat Number generally
    18
    % then abrrv and category names
    A antioxidant activity
    .
    .
    .
    S transporter activity
    % then gene to category abbrvs.
    gene1 ABC
    gene2 PRS
    ...
    @param catfile(char[256]) :saved for post processing in bicluster and go handling
    @param gofile(char[256]) : needed for parsing and forming colors for each genes
    @param char molecularF(char[18][128]) : different from calling with other option than molecular function
**/
void colorHandling( char catfile[256], char gofile[256], char molecularF[][128], int size ){
        int GOSIZE = size;
        char abbrv[34] = "ABCDEFGHIJKLMNOPQRSTUVWYZ01234567";
        array<list<GENES> > molecularFunction( GOSIZE );
        list<GENES> allGenes;

        FILE *f;
        list_item it,it2;
        char line[ 100000 ];
        char *pc,*pend,*go,*cat;
        const char *strDelim = "\t\n";
        const char *strDelim2 = " \t";

        if( (f = fopen( gofile, "r" )) == NULL){
                FILE *erptr;
#ifdef LINUX
                erptr = fopen( "outputs/error.txt", "w" );
#else
                erptr = fopen( "outputs//error.txt", "w" );
#endif
                fprintf( erptr, "Error-color: You did not specify GO based functional category file hence we use it for color processing\n" );
                fclose( erptr );
                cout << "\nError-color: You did not specify GO based functional category file hence we use it for color processing\n";
                exit(1);
        }

        /* First we parse gofile to see how many lines of categories */
        int line_i = 0;
        while( !feof( f ) ){
                fgets( line, 100000, f );
                line_i++;
        }
        if( line_i == 0 ){
            FILE *erptr;
#ifdef LINUX
            erptr = fopen( "outputs/error.txt", "w" );
#else
            erptr = fopen( "outputs//error.txt", "w" );
#endif
            fprintf( erptr, "Error-color-2:GO based functional category file has no category hence we can not continue\n" );
            fclose( erptr );
            cout << "\nError-color-2:GO based functional category file has no category hence we can not continue\n";
            exit(1);
        }
// 		cout << "\t Will Parse " << line_i << " lines, Parsing begins...\n";
        rewind( f );
        line_i = 0;
        /* We rewind and based on line by line parsing we check that is gofile contains
           the predefined categories in molecularF double char array
        */
        while( !feof( f ) ){
                // count rows
// 		two_tuple<CATNAMES,int> tup;
// 		tup.second() = 0;
                fgets( line, 100000, f );
                pc = strtok( line, strDelim );
                go = pc;
                if( feof( f ) )
                        break;
                int count = 0;
                int categIndex = -1;
                while( pc != NULL ){
                        if( count == 0 ){
                                pc = strtok( NULL, strDelim );
                                cat = pc;
// 				sprintf( tup.first().catName, "%s", cat );
                                for( int i = 0; i < GOSIZE; i++ ){
                                        if( strcmp( molecularF[ i ], cat ) == 0 ){
                                                categIndex = i;
// 						cout <<  molecularF[ i ] << " is found\n";
                                        }
                                }
                        }
                        else{
                                if( categIndex != -1 ){
                                        pc = strtok( NULL, strDelim2 );
//                                        cout << pc << " - ";
                                        GENES temporary;
                                        sprintf( temporary.GENE, "%s", pc );
                                        molecularFunction[ categIndex ].append( temporary );
                                        allGenes.append( temporary );
                                }
                                else
                                        break;
                        }
                        pend = pc; /* check last char for newline terminator */
                        int chCount = 0;
                        while( *(pend+chCount) != '\0' ){
                            if( *(pend+chCount) == '\n' ){
                                    pc = NULL;
                                    break;
                            }
                            chCount++;
                        }
                        count++;
                }
//                cout << endl << "************************\n";
                line_i++;
// 		if( line_i % 10 == 0 )
// 			cout << "\t Parsed " << line_i << " ...\n";
        }
//        cout << " We are done \n";
        fclose( f );
        /* With the next double loop we obtain pure gene list obtained with 18 categories */
        array<bool> marked( allGenes.size() + 1 );
        int count = 0;
        forall_items( it, allGenes ){
            marked[ count ] = false;
            count++;
        }
        count = 0;
        forall_items( it, allGenes ){
//                cout << allGenes[it].GENE << " - " << count << endl;
//                it2 = allGenes.succ(it);
//                while( it2 != NULL ){
//			if( it != it2 ){
////                                cout << allGenes[it].GENE << " - " << allGenes[it2].GENE << endl;
//				if( strcmp( allGenes[ it ].GENE, allGenes[ it2 ].GENE ) == 0 ){
//                                        list_item it3 = allGenes.succ(it2);
//					allGenes.del_item( it2 );
//                                        if( it3 != NULL )
//                                            it2 = it3;
//                                        else
//                                            break;
//				}
//			}
//                        it2 = allGenes.succ(it2);
//		}
                if( marked[ count ] == false ){
                    int count2 = 0;
                    forall_items( it2, allGenes ){
                        if( it != it2 ){
                            if( marked[ count2 ] == false ){
                                if( strcmp( allGenes[ it ].GENE, allGenes[ it2 ].GENE ) == 0 ){
                                    marked[ count2 ] = true;
                                }
                            }
                        }
                        count2++;
                    }
                }
                count++;
        }
        list<list_item> deleted;
        count = 0;
        forall_items( it, allGenes ){
            if( marked[ count ] == true ){
                deleted.append( it );
            }
            count++;
        }
        forall_items( it, deleted ){
            allGenes.del_item(deleted[ it ]);
        }

//        cout << " We are done \n";
        /* we now begin saving catfile. We collect the genes for each categories. */
        f = fopen( catfile, "w" );
        fprintf( f, "%d\n", GOSIZE );
        /* Saving category header: abbrv and name */
        for( int i = 0; i < GOSIZE; i++ ){
                for( int j = 0; molecularF[ i ][ j ] != '\0'; j++ ){
                        if( molecularF[ i ][ j ] == ' ' ){
                                  molecularF[ i ][ j ] = '_';
                        }
                }
                fprintf( f, "\"%c\"\t%s\n", abbrv[ i ], molecularF[ i ] );
        }
//        cout << " We are done \n";
        /* Saving gene to abbrvs */
        forall_items( it, allGenes ){
                bool one = false;
                char funcCateg[18]="",funcCateg2[18]="";
                for( int i = 0; i < GOSIZE; i++ ){
                        forall_items( it2, molecularFunction[ i ] ){
                                if( strcmp( allGenes[ it ].GENE, molecularFunction[ i ][ it2 ].GENE ) == 0 ){
                                        one = true;
                                        sprintf( funcCateg2, "%c", abbrv[ i ] );
                                        strcat( funcCateg, funcCateg2 );
                                        break;
                                }
                        }
                }
                if( one == false ){
// 			fprintf( f, "X\n" );
                }
                else{
                        int j = 0;
                        while( allGenes[ it ].GENE[ j ] != '\0' ){
                                if( allGenes[ it ].GENE[ j ] == '\n' ){
                                        allGenes[ it ].GENE[ j ] = '\0';
                                }
                                j++;
                        }
                        fprintf( f, "%s\t%s\n", allGenes[ it ].GENE, funcCateg );
                }
        }
//        cout << " We are done \n";
        fclose( f );
}

/**
* biclusterHandling is a function that handles the bicluster output
* and responds to the main program by having modifying the passed
* parameters.
* @param INPUT (leda::matrix) input matrix obtained from the given input file sources/data_sources
* @param biclusters (leda::list<list<GENES>>)filled with the GENE names provided by the data file
* @param matrixBiclusters (leda::list<leda::matrix>)is used H-value calculation we need this param to obtain submatrices of biclusters
* @param H-values (leda::list) are used in order to have a corresponding value obtained with the id of biclusters       
* @param Hmax (double) Hmax is a value that stores max H-value
* @param minBicSize to control bicluster sizes
* @param maxBicSize to control bicluster sizes
* @param biclustering (int) is for choosing different biclustering outputs, We have to do that since each alg. has different output from its tool.
* @param dimension1(int) size of the data row dimension
* @param dimension2(int) size of the data column dimension
* @param hasColor(bool) reimplementation when the new coloring methodology is used.
*/
void biclusterHandling( matrix &INPUT, char defaultBicFile[256], list<list<GENES> > &biclusters, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, int minBicSize, int maxBicSize, int biclustering, int dimension1, int dimension2, bool hasColor  ){
        list_item it;
	leda::matrix inverseINPUT = INPUT.trans();
	list<list<CONDS> > conditions;
	/* Bicluster results can be saved with several different methods but the first four option is the most common one 
	   For our robinviz tool we use the following format
	   Gene 1#(int) Cond 2#(int)
	   Gene_id1 Gene_id2 ... Gene_id1#
	   Cond_id1 Cond_id2 ... Cond_id2#
	   ...
	*/
        char headerFileName[64] = "geneResult";
        char algname[64];
	if( biclustering == 1 ){
                sprintf( algname, "" );
	}
	if( biclustering == 2 ){ 
                sprintf( algname, "BIMAX" );
	}
	if( biclustering == 3 ){
                sprintf( algname, "CC" );
	}
	if( biclustering == 4 ){
                sprintf( algname, "REAL" );
	}
        getBiclustersFromFile2( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, algname, dimension1, dimension2 ); 
        analyseGenes2( headerFileName, categ, biclusters.size(), algname, dimension1, dimension2, hasColor  );
// 	if( biclustering == 5 ){
// 		getBiclustersFromFile2( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, "SAMBA", dimension1, dimension2 ); 
// 		analyseGenes2( "geneResult", biclusters.size(), "SAMBA" , dimension1, dimension2, hasColor );
// 	}
// 	if( biclustering == 6 ){
// 		getBiclustersFromFile5( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, "MSBE", 1, dimension1, dimension2 ); 
// 		analyseGenes2( "geneResult", biclusters.size(), "MSBE" , dimension1, dimension2, hasColor );
// 	}
	if( biclusters.size() == 0 ) { 
		/* Error checking if there is no reported bicluster */
		FILE *erptr;
		erptr = fopen( "outputs/error.txt", "w" );
                fprintf( erptr, "Error 301: No bicluster candidate to draw, make sure that you use correct parameters\n" );
		fclose( erptr );
                cout << "\nError 301: No bicluster candidate to draw, make sure that you use correct parameters\n";
		exit(1);
	}
// 	cout << " GENE LIST size : " << biclusters.size() << endl;
// 	cout << " COND LIST size : " << conditions.size() << endl;
// 	cout << " MATRIX LIST size : " << matrixBiclusters.size() << endl;

	/* If the program escapes from prev error check this means there are some reported biclusters
	   so we form heatmap and pc outputs. The visualization of these outputs are done within the
	   python side with PyQT. Make sure the format you save is correct */
	for( int i=0; i < biclusters.size(); i++ ){
#ifdef LINUX
		char out[ 64 ] = "outputs/heatmap/out";
#else
		char out[ 64 ] = "outputs//heatmap//out";
#endif
		sprintf( out, "%s%d.html", out, i );
		drawHeatmap( matrixBiclusters[ matrixBiclusters.get_item( i )], biclusters[ biclusters.get_item( i ) ], conditions[ conditions.get_item( i ) ], out );
#ifdef LINUX
		char out2[ 64 ] = "outputs/parallel/out";
#else
		char out2[ 64 ] = "outputs//parallel//out";
#endif
		sprintf( out2, "%s%d.html", out2, i );
		drawParallelCoordinate( matrixBiclusters[ matrixBiclusters.get_item( i )], biclusters[ biclusters.get_item( i ) ], conditions[ conditions.get_item( i ) ], out2 );
#ifdef LINUX
		char out3[ 64 ] = "outputs/heatmap/out";
#else
		char out3[ 64 ] = "outputs//heatmap//out";
#endif
		sprintf( out3, "%s%d.txt", out3, i );
		produceHeatmapInput( matrixBiclusters[ matrixBiclusters.get_item( i )], biclusters[ biclusters.get_item( i ) ], conditions[ conditions.get_item( i ) ], out3 );
	}

	matrix newI2;
	int k = 1;
	bool loopflg = false;
	/* Now we are calculating a H-value for each bicluster. In essence each biclusters have a submatrix from
	   original gene expression. So the reported genes and conds form this submatrix. Based on the function given
	   by Cheng and Church 2000 we do the calculations. This is vital when the user selected(with robinviz gui) that
	   for seeing size of nodes with respect to lower H-values. */
	forall_items( it , matrixBiclusters ){
		char filename2[1024] = "";
		int count_i = 0;
		newI2 = matrixBiclusters[ it ];
		double rs_value;
		double bagMean = 0;
		for( int bag1_int = 0; bag1_int <  newI2.dim1(); bag1_int++ ){
			for( int bag2_int = 0; bag2_int <  newI2.dim2(); bag2_int++ ){
			    bagMean += newI2( bag1_int, bag2_int );
			}
		}
		bagMean = bagMean / (double)( newI2.dim1() * newI2.dim2() );
		double rowMean = 0;
		double colMean = 0;
		double residueScore = 0;
		double residueScoreWhole = 0;
		/*cout << endl;
		for( int bag1_int = i_int; bag1_int < i_int + bagDim1_int; bag1_int++ ){
			for( int bag2_int = j_int; bag2_int < j_int + bagDim2_int; bag2_int++ ){
				cout << extract_mx( bag1_int, bag2_int ) << " ";
			}
			cout << endl;
		}*/
		/*****************************************************************************/
		for( int bag1_int = 0; bag1_int <  newI2.dim1(); bag1_int++ ){
			for( int bag2_int = 0; bag2_int <  newI2.dim2(); bag2_int++ ){
			    /* For each value inside the sub matrix calculate row mean, col mean, and bicluster mean*/
			    /* Calculate row mean */
			    for( int bag2_int2 = 0; bag2_int2 < newI2.dim2(); bag2_int2++ ){
				    rowMean += newI2( bag1_int, bag2_int2 );
			    }
			    rowMean = rowMean / newI2.dim2();
			    /* Calculate col mean */
			    for( int bag1_int2 = 0; bag1_int2 < newI2.dim1(); bag1_int2++ ){
				    colMean += newI2( bag1_int2, bag2_int );
			    }
			    colMean = colMean / newI2.dim1();
			    residueScore += (double)newI2( bag1_int, bag2_int ) + - rowMean - colMean + bagMean;
			    residueScore *= residueScore;
			    residueScoreWhole += residueScore;

		
			    residueScore = 0;
			    colMean = 0;
			    rowMean = 0;
			}
		}
		/*****************************************************************************/
		double rs_Value = residueScoreWhole / (double)( newI2.dim1() * newI2.dim2() ) ;
		H_values.append( rs_Value );
// 		cout << rs_Value << endl;
// 		if( loopflg == false ){
// 			Hmax = rs_Value;
// 			loopflg = true;
// 		}
// 		else{
// 			if( rs_Value > Hmax ){
// 				Hmax = rs_Value;
// 			}
// 		}
		Hmax = rs_Value;
	}
}

/**
* inpGraphProdHandling is a function that handles the production of
* peripheral graphs
* @param abbv (char []) we need a categories for each organism. It may be too general as we did having n categories. abbv stores just a capital letter of categories.
* @param Categories (leda::array<list<char> >) stores each categorie(s) for each genes rather than other inpGraphProdHandling several categs may be owned by each genes
* @param listOfGraphs (leda::array<GRAPH<int,int>>) stores all the graphs in this parameter
* @param GenesNode (leda::array<GENENAMES>) is used with the graph parameter to obtain the specific Genes corresponding nodes
* @param INTERACTIONS (leda::GRAPH<int,int>) is PPI graph extracted from sources/ppi_sources/<ppiFile> it is original graph obtained with interactionHandling
* @param biclusters we do not lose the track of bicluster genes
* @param TEMPINT (leda::GRAPH<int,int>) is andother copy PPI graph used for processing
* @param ppiFile (leda::string) is obtained from main menu

*/
void inpGraphProdHandling( GRAPH<int,int> &G, array<GRAPH<int,int> > &listOfGraphs, array<char> &abbv,  array<list<char> > &Categories, node_array<GENENAMES> &temp, array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, GRAPH<int,int> &TEMPINT, list<list<GENES> > &biclusters, int cat_num ){
        FILE *categoryOfGenes;
        node n,m1,m2;
        list_item it;
#ifdef LINUX
        categoryOfGenes = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "r" );
#else
        categoryOfGenes = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "r" );
#endif
        if( categoryOfGenes == NULL ){
                cout << "\n Error 303: You did not specify gene to function file in the path sources/usr_sources/visualization_data/\n";
                FILE *erptr;
#ifdef LINUX
                erptr = fopen( "outputs/error.txt", "w" );
#else
                erptr = fopen( "outputs//error.txt", "w" );
#endif
                fprintf( erptr, "\n Error 303: You did not specify gene to function file in the path sources/usr_sources/visualization_data/\n" );
                fclose( erptr );
        }
        int geneCount = 0;
	/* First we load categor file produced in main */
        while( !feof( categoryOfGenes ) ){
            fscanf( categoryOfGenes, "%s%s", g[ geneCount ].gene, g[ geneCount ].geneType );
            forall_nodes( n, INTERACTIONS ){
                if( strcmp( GenesNode[ INTERACTIONS[ n ] ].GENE , g[ geneCount ].gene) == 0 ){
                    for( int i = 0; i < cat_num; i++ ){
                        int ii = 0;
                        while( g[ geneCount ].geneType[ ii ] != '\0' ){
                            if( g[ geneCount ].geneType[ ii ] == abbv[ i ] ){
                                Categories[ INTERACTIONS[n] ].append( abbv[ i ] );
                            }
                            ii++;
                        }
                    }
                }
            }
            //cout << geneCount << " ";
            geneCount++;
        }

        gwSave( GenesNode, INTERACTIONS, Categories);
        /*forall_nodes( n, INTERACTIONS ){
              cout << Categories[ INTERACTIONS[n] ] << " ";
        }*/
        fclose( categoryOfGenes );
	/* Loading ends */
#ifdef DEBUG_ROBINVIZ
        cout << endl << " * Gene File is loaded from ppi_sources" << endl;

        cout << endl << " * Loading biclusters to form their ppi graphs " << endl;
#endif
        TEMPINT = INTERACTIONS;
        array<int> nodePar( INTERACTIONS.number_of_nodes()+1 );
        int nPar = 0;
        forall_nodes( n, INTERACTIONS ){
                nodePar[ nPar ] = INTERACTIONS[ n ];
                nPar++;
        }
//        for( int i = 0; i < biclusters.size(); i++ ){
//
//        }

        list<edge> list_E;
        list_item it_b1 = biclusters.first_item(),it_b2;
	/* For each gene list we detect its neigbors that are also in the gene group and we delete remaining edges
	   and nodes from the original interaction graph obtained with ppihandler function */
        for( int i = 0; i < biclusters.size(); i++ ){
                listOfGraphs[ i ] = INTERACTIONS;
                nPar = 0;
                forall_nodes( n,listOfGraphs[ i ] ){
                        listOfGraphs[ i ][ n ] = nodePar[ nPar ];
                        nPar++;
                }
                G = listOfGraphs[ i ];
                list<GENES> tmp1 = biclusters[ it_b1 ];
#ifdef DEBUG_ROBINVIZ
                cout << " GRAPH " << i << " WITH SIZE " << tmp1.size() << " || ";
#endif
                forall_nodes( m1, G ){
                        if( tmp1.size() == 0 ){
                                G.clear();
                                break;
                        }
                        bool isEnter = false;
                        forall_items( it_b2, tmp1 ){
                                if( strcmp( GenesNode[ G[ m1 ] ].GENE, tmp1[ it_b2 ].GENE ) == 0 ){
                                        isEnter = true;
                                        break;
                                }
                        }
                        if( isEnter == false ){
                                G.del_node( m1 );
                        }
                }
#ifdef DEBUG_ROBINVIZ
                cout << " GRAPH IS PRODUCED ||";
#endif
//                forall_nodes( m1, G ){
//                    list<edge> inedges = G.in_edges( m1 );
//                    list<edge> outedges = G.out_edges( m1 );
//                    if( (inedges.size() + outedges.size() ) == 0 )
//                        G.del_node( m1 );
//                }
#ifdef DEBUG_ROBINVIZ
                cout << endl;
                forall_nodes( m1, G ){
                        cout << G[ m1 ] << " - " << GenesNode[  G[ m1 ] ].GENE << "\n";
                }
#endif
                listOfGraphs[ i ] = G;
                it_b1 = biclusters.succ( it_b1 );
#ifdef DEBUG_ROBINVIZ
                cout << " GRAPH IS CLEANED " << endl;
#endif
        }
}
