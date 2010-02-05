/*****************************************************************************************/
/*	Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>   	 */
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
/*    Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.         */
/*    This is free software, and you are welcome to redistribute it                      */
/*    under certain conditions; type `show c' for details.			         */
/*****************************************************************************************/


/*************************************************************************************

handlers.h is for the ppibiclusters.cpp. The main contribuiton of this
header is to functionate the main program. You can just call handlers for
each iterative modules.

Created

18 October 2009, by Melih

Modified

18 October 2009, by Melih
  Four main functions introduced, extracted from main executable file
22 October 2009, by Melih
  To obtain the results from our xcoordinate assignment using new identiftyXPositions
  functions from graphUnion.h. We addedd input parameter "ourMethodFlag" to the
  mainAlgHandlingForEachSubgraph function. RUN_SELF call is also changed since
  the call of our method is inside RUN_SELF.
23 October 2009, by Melih
  interactionHandling function has a property to get different ppi networks
  from main menu.
30 October 2009, by Melih
  mainGraphHandling function is created. It mainly does the layout of biclusters graphs,
  and window layout for clicking in order to see peripheral graphs.
08 December 2009 by Melih
 "interpretBiclusters.h" is added to this header. The results of other algorithms
 are interpreted with the functions of this header and then written to a conventional
 format of bicluster output. Then each result is interpreted using 
 getBiclustersFromFile2 function.
15 December 2009 by Melih
 New Layout Force Directed Function is added
*************************************************************************************/

#include "layout/interpretbiclusters.h"
#include "layout/runFunctions.h"
// #include "kmeans.h"
#include "layout/misc/graphToMatrix.h"
#include "layout/misc/saveGraph.h"
#include "layout/misc/getbiclusters.h"
#include "othervis/heatmap.h"
#include "othervis/parallelCoordinate.h"

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
* Function that produce data structure of entries
* with size of the given input data.
* Each substructure has three attribute,
* GENENAME CATEGORY[] GO[]
* These resulting array of substructure will
* be processed by main program using leda::array<GENEONTO> variable
**/
array<GENEONTO> geneOntologyHandling( char gofile[256] ){
		cout << "/**************************************************/" << endl;
		cout << "\t" << " Parsing GO File as you wish" << endl;
		cout << "/**************************************************/" << endl;
		FILE *f;
		list<GENES> dataGenes;
		char line[ 100000 ];
		char *pc,*pend,*go,*cat;
		const char *strDelim = " \t";
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
			fprintf( erptr, "Error-id5: You did not specify gene name file, sources/usr_sources/visualization_data/genenames.txt \n" );
			fclose( erptr );
			cout << "\nError-id5: You did not specify gene name file, sources/usr_sources/visualization_data/genenames.txt \n"; 
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
			inputGenes[ i ].categories.resize( 200 );
			inputGenes[ i ].gos.resize( 200 );
			sprintf( inputGenes[ i ].genename, "%s", dataGenes[ dataGenes.get_item( i ) ].GENE );
			
		}
		//system( "PAUSE" );
		if( (f = fopen( filePath, "r" )) == NULL){
			FILE *erptr;
#ifdef LINUX
			erptr = fopen( "outputs/error.txt", "w" );
#else
			erptr = fopen( "outputs//error.txt", "w" );
#endif
			fprintf( erptr, "Error-id4: You did not specify GO based functional category file although you selected to use that file\n" );
			fclose( erptr );
			cout << "\nError-id4: You did not specify GO based functional category file although you selected to use that file\n"; 
			exit(1);
		}

		int line_i = 0;
		while( !feof( f ) ){
			fgets( line, 100000, f );
			line_i++;
		}
		cout << "\t Will Parse " << line_i << " lines, Parsing begins...\n";
		rewind( f );
		line_i = 0;
		while( !feof( f ) ){
			// count rows
			fgets( line, 100000, f );
			pc = strtok( line, strDelim );
			go = pc;
			if( feof( f ) )
				break;
			int count = 0;
			while( pc != NULL ){
				pc = strtok( NULL, strDelim );
				if( count == 0 )
					cat = pc;
				else{
					for( int i = 0; i < dataGenes.size(); i++ ){
						if( strcmp( pc, inputGenes[ i ].genename ) == 0 && inputGenes[ i ].index < 200 ){
							sprintf( inputGenes[ i ].categories[ inputGenes[ i ].index ].catName, "%s", cat );
							sprintf( inputGenes[ i ].gos[ inputGenes[ i ].index++ ].goName, "%s", go );
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
			if( line_i % 500 == 0 )
				cout << "\n Line " << line_i << " is parsed ";
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
	int i = 0,count=0,j = 0;
	char c;
	FILE *fptr,*fptr2;

	forall_items( it, biclusters ){
		//cout << " Bic\n";
		temp = biclusters[ it ];
#ifdef LINUX
		sprintf( filename, "outputs/go/gobicluster%d.html", count );
#else
		sprintf( filename, "outputs//go//gobicluster%d.html", count );
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

		fprintf( fptr, "\n\t<th scope=\"col\"> %s </th>\n", "Gene Names" );
		fprintf( fptr, "\n\t<th scope=\"col\"> %s </th>\n", "Gene Ontology Id" );
		fprintf( fptr, "\n\t<th scope=\"col\"> %s </th>\n", "Category" );
		fprintf( fptr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Gene Ontologies and Categories for each gene of Bicluster %d</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, count );
			
		forall_items( it2, temp ){
			for( i = 0; i < inputGenes.size(); i++ ){
				if( strcmp( temp[it2].GENE, inputGenes[ i ].genename ) == 0 ){
					fprintf( fptr, "\t<tr>\n" );
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
void biclusterHandling( matrix &INPUT, list<list<GENES> > &biclusters, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, int minBicSize, int maxBicSize, int biclustering, int dimension1, int dimension2  ){
        list_item it;
	leda::matrix inverseINPUT = INPUT.trans();
	list<list<CONDS> > conditions;
	if( biclustering == 1 ){
		getBiclustersFromFile( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, dimension1, dimension2 ); 
		analyseGenes2( "geneResult", categ, biclusters.size(), "", dimension1, dimension2  );
	}
	if( biclustering == 2 ){ 
		getBiclustersFromFile2( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, "BIMAX", dimension1, dimension2 );
		analyseGenes2( "geneResult", categ, biclusters.size(), "BIMAX", dimension1, dimension2  );
	}
	if( biclustering == 3 ){
		getBiclustersFromFile2( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, "CC", dimension1, dimension2 ); 
		analyseGenes2( "geneResult", categ, biclusters.size(), "CC", dimension1, dimension2  );
	}
	if( biclustering == 4 ){
		getBiclustersFromFile2( inverseINPUT, 1, minBicSize , maxBicSize, matrixBiclusters, biclusters, conditions, "RLEB", dimension1, dimension2 ); 
		analyseGenes2( "geneResult", categ, biclusters.size(), "RLEB" , dimension1, dimension2 );
	}
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
		fprintf( erptr, "Error-id1: No bicluster candidate to draw, make sure that you use correct parameters\n" );
		fclose( erptr );
		cout << "\nError-id1: No bicluster candidate to draw, make sure that you use correct parameters\n"; 
		exit(1);
	}
	cout << " GENE LIST size : " << biclusters.size() << endl;
	cout << " COND LIST size : " << conditions.size() << endl;
	cout << " MATRIX LIST size : " << matrixBiclusters.size() << endl;

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
		if( numberOfReleations % 1000 == 999 )
			cout << "* " << INTERACTIONS.number_of_edges() << " " << INTERACTIONS.number_of_nodes() << endl;

		fscanf( interactionsPtr, "%s%s%lf", sourceGene, targetGene, &value );
		
		// FOR SOURCE GENES
		int flag = 0;
		if( numberOfReleations > 0 ){
			forall_items( it, entered ){
				if( strcmp( entered[ it ].GENE, sourceGene ) == 0 ){
					flag = 1;
				}
			}
			if( flag == 1 ){
				forall_nodes( n, INTERACTIONS ){
					if( strcmp( GenesNode[ INTERACTIONS[ n ]].GENE , sourceGene ) == 0 ){
						sourceTemp = n;
						break;
					}
				}
			}
			else{
				sourceTemp = INTERACTIONS.new_node();
				INTERACTIONS[ sourceTemp ] = nodeAdded;
				nodeAdded++;
				GenesNode.resize( nodeAdded );
				sprintf( GenesNode[ INTERACTIONS[ sourceTemp ] ].GENE, "%s",sourceGene );
				GENENAMES tempx;
				sprintf( tempx.GENE, "%s", sourceGene );
				entered.append( tempx );
			}
		}
		else{
				sourceTemp = INTERACTIONS.new_node();
				INTERACTIONS[ sourceTemp ] = nodeAdded;
				nodeAdded++;
				GenesNode.resize( nodeAdded );
				sprintf( GenesNode[ INTERACTIONS[ sourceTemp ] ].GENE, "%s",sourceGene );
				GENENAMES tempx;
				sprintf( tempx.GENE, "%s", sourceGene );
				entered.append( tempx );
		}

		// FOR TARGET GENES
		flag = 0;
		forall_items( it, entered ){
			if( strcmp( entered[ it ].GENE , targetGene ) == 0 ){
				flag = 1;
			}
		}
		if( flag == 1 ){
			forall_nodes( n, INTERACTIONS ){
				if( strcmp( GenesNode[ INTERACTIONS[ n ] ].GENE, targetGene ) == 0 ){
					targetTemp = n;
					break;
				}
			}
		}
		else{
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
		INTERACTIONS[ e ] = fabs(log( value * 10000.0 ));
		numberOfReleations++;
		/*cout << endl;*/			
	}
	cout << endl;
// 	forall_edges( e, INTERACTIONS )
// 		cout << GenesNode[ INTERACTIONS[ INTERACTIONS.source( e ) ] ].GENE << " " << GenesNode[ INTERACTIONS[ INTERACTIONS.target( e )] ].GENE << endl;
// 	
	fclose( interactionsPtr );
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
		cout << "\n Error id3: You did not specify gene to function file in the path sources/usr_sources/visualization_data/\n";
		FILE *erptr;
#ifdef LINUX
		erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
		fprintf( erptr, "\n Error id3: You did not specify gene to function file in the path sources/usr_sources/visualization_data/\n" );
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
	/*forall_nodes( n, INTERACTIONS ){
	      cout << Categories[ INTERACTIONS[n] ] << " ";
	}*/
	fclose( categoryOfGenes );

	cout << endl << " * Gene File is loaded from ppi_sources" << endl;
	
	cout << endl << " * Loading biclusters to form their ppi graphs " << endl;
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
		
		cout << " GRAPH " << i << " WITH SIZE " << tmp1.size() << " || ";
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
		cout << " GRAPH IS PRODUCED ||";
		forall_nodes( m1, G ){
		    list<edge> inedges = G.in_edges( m1 );
		    list<edge> outedges = G.out_edges( m1 );
		    if( (inedges.size() + outedges.size() ) == 0 )
			G.del_node( m1 );
		}
		cout << endl;
		forall_nodes( m1, G ){
			cout <<  GenesNode[  G[ m1 ] ].GENE << "-";
		}
		cout << endl;
		listOfGraphs[ i ] = G;
		it_b1 = biclusters.succ( it_b1 );
		cout << " GRAPH IS CLEANED " << endl;
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
        FILE *wptr2,*wptr3;
#ifdef LINUX
	wptr3 = fopen( "sources/graph_sources/graph_infos.txt", "w" );
#else
	wptr3 = fopen( "sources//graph_sources//graph_infos.txt", "w" );
#endif
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
		    if( listOfGraphs[ i ].empty() != true )
			      H = RUN_SELF( listOfGraphs[ i ], layers, width, Xpos, Ypos, i + 1, pos, bends, algorithmFlag, space, xCoordFlag, increment, ledaPostFlag );

		    nPar = 0;
		    forall_nodes( n, listOfGraphs[ i ] ){
			  listOfGraphs[ i ][ n ] = nodePar[ nPar ];
// 			  cout << nodePar[ nPar ] << " - " << listOfGraphs[ i ][ n ] << endl;
			  nPar++;
		    }  

			GraphList.append( listOfGraphs[ i ] );
#ifdef LINUX
			sprintf( filename , "sources/graph_sources/Graph%d.txt" , i+1 );
#else
			sprintf( filename , "sources//graph_sources//Graph%d.txt" , i+1 );
#endif
			ofstream GRAPHS( filename , ios::in | ios::trunc );
		    
		        
			fprintf( wptr3, "%s\n", "_________________________________" );
			fprintf( wptr3, "%s\t%s\n", "File Name      :", filename ); 
			fprintf( wptr3, "%s\t%d\n", "Number of Graph:", i );
			fprintf( wptr3, "%s\n", "_________________________________" );
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
#ifdef LINUX
			sprintf( filename , "sources/graph_sources/Relation%d.txt" , i+1 );
#else
			sprintf( filename , "sources//graph_sources//Relation%d.txt" , i+1 );
#endif
			wptr2 = fopen( filename, "w" );
			forall_edges( e, listOfGraphs[ i ] ){
				node source = listOfGraphs[ i ].source( e );
				node target = listOfGraphs[ i ].target( e );
				fprintf( wptr2, "%s\t=>\t%s\t|\t|\n", nameS[ INDEX[ source ] ], nameS[ INDEX[target] ] );
			}

			fclose( wptr2 );

			leda::string m = "";
			listOfGraphs[ i ].write( GRAPHS );
			GRAPHS.close();

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
			cout << endl;
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
	fclose( wptr3 );
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
				     int cat_num,
				     array<char> &Categories
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
#ifdef LINUX
	wptr3 = fopen( "sources/graph_sources/graph_infos.txt", "w" );
#else
	wptr3 = fopen( "sources//graph_sources//graph_infos.txt", "w" );
#endif
	for(int i = 0; i < biclusters.size(); i++ ){	
		if( listOfGraphs[ i ].empty() != true ){
			pos.init( listOfGraphs[ i ] );
			bends.init( listOfGraphs[ i ] );   
			array<int> nodePar( listOfGraphs[ i ].number_of_nodes()+1 );
			int nPar = 0;
			forall_nodes( n, listOfGraphs[ i ] ){
			      nodePar[ nPar ] = listOfGraphs[ i ][ n ];
    // 			  cout << nodePar[ nPar ] << " - " << listOfGraphs[ i ][ n ] << endl;
			      nPar++;
			}

			H = RUN_FFD( listOfGraphs[ i ], layers, width, Xpos, Ypos, i + 1, pos, bends, algorithmFlag, space, xCoordFlag, increment, ledaPostFlag, abbv, cat_num, Categories );
		      
			nPar = 0;
			forall_nodes( n, listOfGraphs[ i ] ){
			      listOfGraphs[ i ][ n ] = nodePar[ nPar ];
    // 			  cout << nodePar[ nPar ] << " - " << listOfGraphs[ i ][ n ] << endl;
			      nPar++;
			}  

			GraphList.append( listOfGraphs[ i ] );
#ifdef LINUX
			sprintf( filename , "sources/graph_sources/Graph%d.txt" , i+1 );
#else
			sprintf( filename , "sources//graph_sources//Graph%d.txt" , i+1 );
#endif
			ofstream GRAPHS( filename , ios::in | ios::trunc );
		    
		    
			fprintf( wptr3, "%s\n", "_________________________________" );
			fprintf( wptr3, "%s\t%s\n", "File Name      :", filename ); 
			fprintf( wptr3, "%s\t%d\n", "Number of Graph:", i );
			fprintf( wptr3, "%s\n", "_________________________________" );
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
#ifdef LINUX
			sprintf( filename , "sources/graph_sources/Relation%d.txt" , i+1 );
#else
			sprintf( filename , "sources//graph_sources//Relation%d.txt" , i+1 );
#endif
			wptr2 = fopen( filename, "w" );
			forall_edges( e, listOfGraphs[ i ] ){
				node source = listOfGraphs[ i ].source( e );
				node target = listOfGraphs[ i ].target( e );
				fprintf( wptr2, "%s\t=>\t%s\t|\t|\n", nameS[ INDEX[ source ] ], nameS[ INDEX[target] ] );
			}

			fclose( wptr2 );


			leda::string m = "";
			listOfGraphs[ i ].write( GRAPHS );
			GRAPHS.close();

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
			cout << endl;
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
	fclose( wptr3 );
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
				  list<int> &categ
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
	if( hvalueWeighting == true ){
		counter = 0;
		forall_nodes( n, PROJECT ){
			HValues[ n ] = H_values[ H_values.get_item( counter )];
			//cout << counter << " - " << HValues[ n ] << endl;
			counter++;
		}
	}
	else{
		if( ppihitratioWeighting == true ){
			double max = 0;
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
					fprintf( erptr, "Error-id2: outputs/enrich/result.txt file does not exist\n" );
					fclose( erptr );
					cout << "\nError-id2: outputs/enrich/result.txt file does not exist\n"; 
					exit(1);
				}
				else{
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
					
					node_array<bool> mark( G1, false );
					node_array<bool> mark2( G2, false );
					if( it1 != it2 ){
						forall_nodes( n1, G1 ){
							forall_nodes( n2, G2 ){
								if( strcmp( G1[ n1 ],G2[ n2 ] ) == 0 && mark[ n1 ] == false && mark2[ n2 ] == false ){

									int flag_e = 0;
									edge e1;
// 									if( DEBUG_GIVING_NAMES )
// 										cout << "\n 5 gecti" << endl;
									edge ee;
									forall_edges( ee, PROJECT ){
										if( (PROJECT.source( ee ) == f1 && PROJECT.target( ee ) == f2) || (PROJECT.source( ee ) == f2 && PROJECT.target( ee ) == f1)  ){
											flag_e = 1;
											e = ee;
											break;
										}
									}
									if( flag_e == 0 && f1 != NULL && f2 != NULL ){
										e = PROJECT.new_edge( f1 ,f2 );
										PROJECT[ e ] = 1;
									}
									else{
										if( e != NULL )
											PROJECT[ e ]++;
									}
								}
								mark2[ n2 ] = true;
							}
							mark[ n1 ] = true;
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
		//cout << PROJECT[ e1 ] << " - " << " old " << endl;	
		n = PROJECT.source( e1 );
		list<edge> edges_l = G.out_edges( n );
		PROJECT[ e1 ] = (int)((double)PROJECT[ e1 ] * multiply);
		if( PROJECT[ e1 ] < (int)(removeRat * 100.0) /*&& edges_l.size() > 2*/ ){
// 			cout << multiply << " Deleted Since - " << PROJECT[ e1 ] << " < " << (removeRat * 100.0) << endl;
			PROJECT.del_edge( e1 );
		}
		else{
			old_edges.append( PROJECT[ e1 ] );			
			//cout << PROJECT[ e1 ] << " - " << " new " << endl;
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
// 	cout << " E4 -  " << PROJECT2.number_of_edges() << endl;
	i = 0;
	RUN_SELF2( PROJECT2, PARS, max_weight, 10, Xpos, Ypos, i + 1, pos, bends, HValues2, hided, algorithmFlag, brandFlag, brandFlag2, ourMethodFlag, space, increment,ledaPostFlag,nodeSize,edgeBendImp,colorScale,edgThicknessTher, categ );	
// 	cout << " E5 -  " << PROJECT2.number_of_edges() << endl;
	return PROJECT2;
}
