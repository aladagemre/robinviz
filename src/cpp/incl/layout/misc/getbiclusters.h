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


#include "declerations.h"
#define ESIT ==

struct genes{
    leda::string gene;
    char GENE[ 512 ];
    int index;
};

struct conds{
    char COND[ 512 ];
    int index;
};

typedef struct genes GENES;
typedef struct conds CONDS;

struct genesx{
    char gene[ 20 ];
    char geneType[ 1 ];
};
typedef struct genesx GNDATAx;
GNDATAx gx[50000];

struct genes3{
    leda::string gene;
    char GENE[ 20 ];
    int index;
};

struct category{
	char categ[ 256 ];
};

typedef struct category CAT;


leda::integer factorial( integer x ){
        integer m = 1;
        for( int i = 2; i <= x; i++ ){
                m *= i;
        }
        return m;
}

// Calcalates P-Value Bonferroni Corrected
double pvalueFinder( int x, int n, int N, int M, int L ){
        double result1,result2,result3;
        leda::rational res1( factorial( n ) , ( factorial( n - x ) * factorial( x ) ));// n, x
        leda::rational res2( factorial( M-n ) , ( factorial( M-n - (N-x) ) * factorial( N-x ) ));// M-n, N-x,
        leda::rational res3( factorial( M ) , ( factorial( M - N ) * factorial( N ) ));// M, N
        leda::rational final( res1 * res2 / res3 );
        double result = final.to_double();
        return result /* L*/;
}


matrix getMatrixFromFile( char filename[ 128 ], int &dimension1, int &dimension2, array<GENES> &geneArray,  array<CONDS> &condArray ){
// 	cout << " MATRIX " << endl;
	FILE *fcptr;
	fcptr = fopen( filename, "r" );
        int dim1,dim2,count = 0,count2 = 0, count3 = 0;
	double value;
	fscanf( fcptr, "%d%d", &dim1, &dim2 );
	char probeset[128];
	fscanf( fcptr, "%s", probeset );
	geneArray.resize( dim1 );
	condArray.resize( dim2 );
	for( int i = 0; i < dim2; i++ ){
		fscanf( fcptr, "%s", condArray[ i ].COND );
// 		cout << condArray[ i ].COND << endl;
		condArray[ i ].index = i+1;
	}
        count3++;
        fscanf( fcptr, "%s", geneArray[ count2 ].GENE );
        while( true ){
            if( strcmp( geneArray[ count2 ].GENE , "None" ) != 0 )
                break;
            for( int i = 0; i < dim2; i++ ){
                fscanf( fcptr, "%lf" , &value );
            }
            count3++;
            fscanf( fcptr, "%s", geneArray[ count2 ].GENE );
            if( count3 == dim1 - 1 )
                    break;
        }
        geneArray[ count2 ].index = count2+1;
//        cout << geneArray[ count2 ].GENE << endl;
	matrix TEMP( dim1, dim2 );
        while( !feof( fcptr ) ){
		fscanf( fcptr, "%lf" , &value );
		TEMP( count2, count ) = value;
// 		cout << TEMP( count2, count ) << "\t";
		count++;
		if( count == dim2 ){
                        count3++;
			count2++;
			fscanf( fcptr, "%s", geneArray[ count2 ].GENE );
                        while( true ){
                            if( strcmp( geneArray[ count2 ].GENE , "None" ) != 0 )
                                break;
                            for( int i = 0; i < dim2; i++ ){
                                fscanf( fcptr, "%lf" , &value );
                            }
                            count3++;
                            fscanf( fcptr, "%s", geneArray[ count2 ].GENE );
                            if( count3 == dim1 - 1 )
                                    break;
                        }
			geneArray[ count2 ].index = count2+1;
//                        cout << geneArray[ count2 ].GENE << " - " << count2 << " - " << count3 <<  " - " << dim1 <<endl;
			count = 0;
// 			cout << endl;
		}
                if( count3 == dim1 - 1 )
			break;
	}
//        cout << "DONE\n";
        matrix TEMP2( count2, dim2 );
        for( int i=0; i < count2; i++ ){
            for( int j=0; j < dim2; j++ ){
                TEMP2(i,j) = TEMP(i,j);
            }
//            cout << geneArray[ i ].GENE << endl;
        }
//        cout << dim1 << " - " << dim2 << endl;
//        cout << count2 << " - " << dim2 << endl;
        geneArray.resize( count2 );
        dimension1 = count2;
	dimension2 = dim2;
        return TEMP2;
}

matrix getMatrixFromFile( char filename[ 128 ] , double multiplyParameter, int &dimension1, int &dimension2 ){
// 	cout << " MATRIX " << endl;
	FILE *fcptr;
	fcptr = fopen( filename, "r" );
	int dim1,dim2,count = 0,count2 = 0;
	double value;
	fscanf( fcptr, "%d%d", &dim1, &dim2 );
	//cout << "Dimensions " << dim1 << "  " << dim2 << endl;
	matrix TEMP( dim1, dim2 );
	while( feof( fcptr ) != true ){
		fscanf( fcptr, "%lf" , &value );
		//cout << value << "  ";
		//value = value * 15;
		//if( value < -1 )
		//	value = leda::log2_abs( value );
		//if( value < 0 && value > -1 ){
		//	value = 0;
		//}
			//value = value * -1;
		//TEMP( count2, count ) = floorl( value );
		TEMP( count2, count ) = value * multiplyParameter;
		if( TEMP( count2, count ) < -1.0 ){
			TEMP( count2, count ) = fabs(log( TEMP( count2, count )) ) ;
		}
		else{
			if( TEMP( count2, count ) > -1.0 && TEMP( count2, count ) < 0 )
				TEMP( count2, count ) = 0;
		}
		count++;
		
		if( count == dim2 ){
			count2++;
			count = 0;
			//cout << count << " - " << count2 << "\n";
		}
		if( count2 == dim1 )
			break;
		//cout << count << "  " << count2 << endl;
	}
	//cout << " MATRIX " << endl;
	dimension1 = dim1;
	dimension2 = dim2;
	return TEMP;
}


matrix getMatrixFromFile( char filename[ 128 ], int forOtherAlgorithms, int &dimension1, int &dimension2  ){
// 	cout << " MATRIX \n" << filename << endl;
	FILE *fcptr;
	fcptr = fopen( filename, "r" );
	int dim1,dim2,count = 0,count2 = 0;
	double value;
	fscanf( fcptr, "%d%d", &dim1, &dim2 );
	//cout << "Dimensions " << dim1 << "  " << dim2 << endl;
	matrix TEMP( dim1, dim2 );
	while( feof( fcptr ) != true ){
		fscanf( fcptr, "%lf" , &value );
		//cout << value << "  ";
		//value = value * 15;
		//if( value < -1 )
		//	value = leda::log2_abs( value );
		//if( value < 0 && value > -1 ){
		//	value = 0;
		//}
			//value = value * -1;
		//TEMP( count2, count ) = floorl( value );
		TEMP( count2, count ) = value;
		count++;
		if( count == dim2 ){
			count2++;
			count = 0;
			//cout << count << " - " << count2 << "\n";
		}
		if( count2 == dim1 )
			break;
		//cout << count << "  " << count2 << endl;
	}
	//cout << " MATRIX " << endl;
	dimension1 = dim1;
	dimension2 = dim2;
	return TEMP;
}




typedef struct genes3 GNDATA2;
/* 
    This function does analysis on files of each bicluster
    Finds each corresponding value in Yeast Gene Function File
    and calculates statistics for each of them.
    E - energy production
    G - amino acid metabolism
    M - other metabolism
    P - translation
    T - transcription
    B - transcriptional control
    F - protein fate
    O - cellular organization
    A - transport and sensing
    R - stress and defense
    D - genome maintenance
    C - cellular fate / organization
    U - uncharacterized
*/


/**
* inputs the categories to the program with its label and abbv 
* returns abbv char array
* @param categ (list<CAT>) labels
* @param Cat_Num (int) number of categories
**/
int functionalCategoryFinder( list<CAT> &categ, array<char> &abbreviation ){
	FILE *fptr;
	int cat_num,i=0;
	CAT tmp;
#ifdef __linux__
	fptr = fopen( "sources/usr_sources/visualization_data/functions.txt", "r" );
#else
	fptr = fopen( "sources//usr_sources//visualization_data//functions.txt", "r" );
#endif
	fscanf( fptr, "%d", &cat_num );
 	//cout << cat_num << endl;
	while( !feof( fptr ) ){
		char temp[20];
		fscanf( fptr, "%s%s", temp, tmp.categ );
		categ.append( tmp );
 		//cout << i << "\t" << temp[1] << "\t" << tmp.categ << endl;
		abbreviation[ i ] = temp[1];
		i++;
	}
 	//cout << endl << i << endl;
	fclose( fptr );
	return i;
}


void analyseGenes2( char fileName[], list<int> &categoriesBicluster, int biNumber, char algName[], int dimension1, int dimension ){

    list<CAT> categoriesOfGenes;
    FILE *fptr,*resultPtr;
    GNDATA2 *allGenes = (GNDATA2*)malloc( sizeof( GNDATA2 ) * dimension1 );
    char fileName2[ 1024 ];
    int count = 0, cat_num;
    int genesAnnotated = 0;
    char geneTemp[ 1024 ];
    array<char> abbv(64); 
    cat_num = functionalCategoryFinder( categoriesOfGenes, abbv );
    array<int> inCategory( cat_num );
    for( int i = 0; i < cat_num; i++ ){
	  inCategory[ i ] = 0;
    } 
    int fileCount = 0;

// 	cout << " Gecti1 " << endl;
#ifdef __linux__
    FILE *gptr = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "r");
#else
	FILE *gptr = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "r");
#endif
    while( !feof( gptr )){
        fscanf( gptr, "%s%s", gx[count].gene, gx[count].geneType );       
        count++;
    }
    fclose( gptr );
// 	cout << " Gecti2 " << endl;
#ifdef __linux__
    FILE *yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" );
#else
	FILE *yFptr = fopen( "sources//usr_sources//visualization_data//genenames.txt", "r" );
#endif

    array<int> checkAnnot1( count + 1 );
    for( int j = 0; j < count; j++ )
        checkAnnot1[ j ] = 0;
    while( !feof( yFptr ) ){
        fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
        for( int j = 0; j < count; j++ ){
            if( strcmp( allGenes[ fileCount ].GENE, gx[ j ].gene ) == 0 ){
                for( int k = 0; k < cat_num; k++ ){
                    if( gx[ j ].geneType[ 0 ] == abbv[ k ] ){
                        inCategory[ k ]++;
                        if( checkAnnot1[ j ] == 0 ){
                            genesAnnotated++;
                            checkAnnot1[ j ] = 1;
                        }
                    }
                }
            }
        }
    }
    fclose( yFptr );
    
//     	cout << " Gecti3 " << endl;
#ifdef __linux__
        resultPtr = fopen( "outputs/enrich/result.txt", "w" );
#else
	resultPtr = fopen( "outputs//enrich//result.txt", "w" );
#endif
    for( int i = -1; i < cat_num; i++ ){
	    if( i == -1 )
		    fprintf( resultPtr, "%20s\t", "categories" );
	    else{
		    if( i != cat_num - 1 )
			    fprintf( resultPtr, "%20s\t", categoriesOfGenes[ categoriesOfGenes.get_item( i ) ].categ );
		    else
			    fprintf( resultPtr, "%20s\n", categoriesOfGenes[ categoriesOfGenes.get_item( i ) ].categ );
	    }
    }
    
    for( int i = -1; i < cat_num; i++ ){
	    if( i == -1 )
		    fprintf( resultPtr, "%20d\t", 0 );
	    else{
		    if( i != cat_num - 1 )
			    fprintf( resultPtr, "%20d\t", inCategory[ i ] );
		    else
			    fprintf( resultPtr, "%20d\n", inCategory[ i ] );
	    }
    }
    array<list<double> > PValues( biNumber );

    for( int i = 0; i < biNumber - 1; i++ ){
        int total = 0;
#ifdef __linux__
        sprintf( fileName2, "outputs/bicgenes/%s%s%d%s", fileName, algName, i+1, ".txt" );
#else
	sprintf( fileName2, "outputs//bicgenes//%s%s%d%s", fileName, algName, i+1, ".txt" );
#endif
		if( (fptr = fopen( fileName2, "r" )) == NULL ){
                        cout << "List " << i << " has 0 prots" << "\n";
                        for( int j = 0; j < cat_num; j++ ){
                                PValues[ i ].push_back( 1.0 );
                        }
                        for( int j = -1; j < cat_num; j++ ){
                                if( j == -1 )
                                        fprintf( resultPtr, "%20d\t", i+1 );
                                else{
                                        if( j != cat_num - 1 )
                                                fprintf( resultPtr, "%20d\t", 0 );
                                        else
                                                fprintf( resultPtr, "%20d\n", 0 );
                                        //cout << PValues[ i ][ PValues[ i ].get_item( j ) ] << " ";
                                }
                        }
		}
		else{
			array<int> countAbbv( 64 );
                        for( int j = 0; j < cat_num; j++ ){
                                countAbbv[ j ] = 0;
                                PValues[ i ].push_back( 1.0 );
                        }
                        for( int j = 0; j < count; j++ )
                            checkAnnot1[ j ] = 0;
			while( !feof( fptr ) ){
				fscanf( fptr, "%s", geneTemp );
                                for( int j = 0; j < count; j++ ){
					if( strcmp( geneTemp, gx[ j ].gene ) == 0 ){
                                                for( int k = 0; k < cat_num; k++ ){
                                                        if( gx[ j ].geneType[ 0 ] == abbv[ k ] ){
                                                                countAbbv[ k ] = countAbbv[ k ] + 1;
                                                                if( checkAnnot1[ j ] == 0 ){
                                                                    total++;
                                                                    checkAnnot1[ j ] = 1;
                                                                }
                                                        }
						}
					}
				}
			}
			for( int j = -1; j < cat_num; j++ ){
				if( j == -1 )
					fprintf( resultPtr, "%20d\t", i+1 );
				else{
					if( j != cat_num - 1 )
						fprintf( resultPtr, "%20d\t", countAbbv[ j ] );
					else
						fprintf( resultPtr, "%20d\n", countAbbv[ j ] );
                                        PValues[ i ][ PValues[ i ].get_item( j ) ] = pvalueFinder( countAbbv[ j ], inCategory[ j ], total, genesAnnotated, cat_num );
				}
			}
// 			fclose( fptr );
		}
    }

//  	cout << " Gecti4 " << endl;
    fclose( resultPtr );

    FILE *efptr;
    if( ( efptr = fopen( "outputs/enrich/result.txt", "r" )) == NULL && ( efptr = fopen( "outputs/enrich//result.txt", "r" )) == NULL ){
	    FILE *erptr;
#ifdef __linux__
	    erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
	    fprintf( erptr, "Error-401: outputs/enrich/result.txt file does not exist in directory, enrichment process can not continue\n" );
	    fclose( erptr );
	    cout << "Error-401: outputs/enrich/result.txt file does not exist in directory, enrichment process can not continue\n"; 
	    exit(1);
    }
    else{
	    char readCatName[128];
	    for( int i = 0; i <= cat_num; i++ ){
		    fscanf( efptr, "%s", readCatName );
	    }
	    int numberOfBiclusters = biNumber;
	    array<int> categoryMaxGenes( cat_num );
	    array< list<int> > categoryPerGenes( numberOfBiclusters );
	    array< list<double> > categoryPerGenesRatio( numberOfBiclusters );
	    int bic_id;
// 	    cout << " Beginning to read first row of enrichments\n" << endl;
	    for( int i = 0; i <= cat_num; i++ ){
		    if( i == 0 ){
			    fscanf( efptr, "%d", &bic_id );
		    }else{
			    fscanf( efptr, "%d", &categoryMaxGenes[ i-1 ] );
		    }
	    }
// 	    cout << " Beginning to read enrichments\n" << endl;
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
// 	    cout << " Beginning to Calculate Ratio\n" << endl;
	    for( int counter = 0; counter < numberOfBiclusters; counter++ ){
		    for( int i = 0; i < cat_num; i++ )
			    categoryPerGenesRatio[ counter ].append( 0.0 );
		    int maxCategId = 0; 
		    double gene_sum = 0;
		    for( int k = 0; k < cat_num; k++ ){
			    gene_sum += categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( k )];
		    }
		    double maxValue = 0;
		    for( int i = 0; i < cat_num; i++ ){
			    categoryPerGenesRatio[ counter ][ categoryPerGenesRatio[ counter ].get_item( i )] = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / (double)categoryMaxGenes[ i ];
			    if( maxValue <  (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / gene_sum ){
				maxValue = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / gene_sum;
				maxCategId = i;
			    }
		    }
		    categoriesBicluster.append( maxCategId ); 
	    }

	    fclose( efptr );
	    char c;
// 	    cout << " Beginning to html file\n" << endl;
#ifdef __linux__
	    efptr = fopen( "outputs/enrich/result.html", "w" );
	    resultPtr = fopen( "outputs/enrich/src/header.txt", "r" );
#else
		efptr = fopen( "outputs//enrich//result.html", "w" );
	    resultPtr = fopen( "outputs//enrich//src//header.txt", "r" );
#endif
	    while( !feof( resultPtr ) ){
		    fscanf( resultPtr, "%c", &c );
		    fprintf( efptr, "%c", c );
	    }
	    fclose( resultPtr );

            fprintf( efptr, "\n\t<th scope=\"col\">GO or Bicluster Id</th>\n" );
	    list<CAT> categoriesOfGenes2 = categoriesOfGenes;
	    for( int i = 0; i < cat_num; i++ ){
			for( int j = 0; j < 64; j++ ){
				if( categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ[ j ] == '_' )
					categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ[ j ] = ' ';
			}
	    }
		FILE *oneResultPtr;
#ifdef __linux__
		resultPtr = fopen( "outputs/enrich/src/header.txt", "r" );
		for( int i = 0; i < numberOfBiclusters; i++ ){
			char outputFile[256];
			sprintf( outputFile, "%s%d%s", "outputs/enrich/resultbic", i, ".html" );
			oneResultPtr = fopen( outputFile, "w" );
			while( !feof( resultPtr ) ){
				fscanf( resultPtr, "%c", &c );
				fprintf( oneResultPtr, "%c", c );
			}
			rewind( resultPtr );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Categories" );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Number of Genes" );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to All Category Genes" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to GO or Bicluster Gene Space" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "P-values" );
			fprintf( oneResultPtr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for Bicluster %d</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, i );
			for( int j = 0; j < cat_num; j++ ){
				fprintf( oneResultPtr, "\t<tr>\n" );
				fprintf( oneResultPtr, "\t\t<td>%s</td>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( j ) ].categ );
				fprintf( oneResultPtr, "\t\t<td>%d</td>\n", categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] );
				fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ i ][ categoryPerGenesRatio[ i ].get_item( j )] );
				double gene_sum = 0;
				for( int k = 0; k < cat_num; k++ ){
					gene_sum += categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( k )];
				}
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", PValues[ i ][ PValues[ i ].get_item( j )]);
				fprintf( oneResultPtr, "\t</tr>\n" );
			}
			fprintf( oneResultPtr, "\t</tbody>\n</table>\n</body>\n</html>" );
			fclose( oneResultPtr );
		}
		fclose( resultPtr );
#else
		resultPtr = fopen( "outputs//enrich//src//header.txt", "r" );
		for( int i = 0; i < numberOfBiclusters; i++ ){
			char outputFile[256];
			sprintf( outputFile, "%s%d%s", "outputs//enrich//resultbic", i, ".html" );
			oneResultPtr = fopen( outputFile, "w" );
			while( !feof( resultPtr ) ){
				fscanf( resultPtr, "%c", &c );
				fprintf( oneResultPtr, "%c", c );
			}
			rewind( resultPtr );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Categories" );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Number of Genes" );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to All Category Genes" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to GO or Bicluster Gene Space" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "P-values" );
			fprintf( oneResultPtr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for Bicluster %d</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, i );
			for( int j = 0; j < cat_num; j++ ){
				fprintf( oneResultPtr, "\t<tr>\n" );
				fprintf( oneResultPtr, "\t\t<td>%s</td>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( j ) ].categ );
				fprintf( oneResultPtr, "\t\t<td>%d</td>\n", categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] );
				fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ i ][ categoryPerGenesRatio[ i ].get_item( j )] );
				double gene_sum = 0;
				for( int k = 0; k < cat_num; k++ ){
					gene_sum += categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( k )];
				}
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", PValues[ i ][ PValues[ i ].get_item( j )]);
				fprintf( oneResultPtr, "\t</tr>\n" );
			}
			fprintf( oneResultPtr, "\t</tbody>\n</table>\n</body>\n</html>" );
			fclose( oneResultPtr );
		}
		fclose( resultPtr );
#endif
	    for( int i = 0; i < cat_num; i++ ){
		    fprintf( efptr, "\n\t<th scope=\"col\"> %s </th>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ );
	    }
//     </tr>
//     </thead>
//     <tfoot>
//     	<tr>
//         	<td colspan="4"> Table shows the enrichment results for the specified file</td>
//         </tr>
//     </tfoot>
//     <tbody>
	    fprintf( efptr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for the specified file</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", cat_num+1 );
// 	<tr>
// 	    <td rowspan="2">Stephen C. Cox</td>
// 	    <td>Marketing</td>
// 	    <td>Make discount offers</td>
// 	    <td>3/10</td>
// 	</tr>
// 	<tr>
// 	    <td>Marketing</td>
// 	    <td>Make discount offers</td>
// 	    <td>3/10</td>
// 	</tr>
	    for( int counter = 0; counter < numberOfBiclusters-1; counter++ ){
		  fprintf( efptr, "\t<tr>\n" );
		  fprintf( efptr, "\t\t<td rowspan=\"2\">Bicluster %d</td>\n", counter+1 );
		  for( int i = 0; i < cat_num; i++ ){
			  fprintf( efptr, "\t\t<td>%d</td>\n", categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] );
		  }
		  fprintf( efptr, "\t</tr>\n" );
		  fprintf( efptr, "\t<tr>\n" );
		  for( int i = 0; i < cat_num; i++ ){
			  fprintf( efptr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ counter ][ categoryPerGenesRatio[ counter ].get_item( i )] );
		  }
		  fprintf( efptr, "\t</tr>\n");
	    }
// 	      </tbody>
// 	  </table>
// 
// 	  </body>
// 	  </html>
	    fprintf( efptr, "\t</tbody>\n</table>\n</body>\n</html>" );
	    fclose( efptr );
	      
    }
}

void analyseGenesInPPI( char fileName[64], list<int> &categoriesBicluster, int biNumber, char algName[64], int dimension1, int dimension, bool hasColor ){

    list<CAT> categoriesOfGenes;
    FILE *fptr,*resultPtr;
    GNDATA2 *allGenes = (GNDATA2*)malloc( sizeof( GNDATA2 ) * dimension1 );
    char fileName2[ 1024 ];
    int count = 0, cat_num;
    int genesAnnotated = 0;
    char geneTemp[ 1024 ];
    array<char> abbv(64);
    cat_num = functionalCategoryFinder( categoriesOfGenes, abbv );
    array<int> inCategory( cat_num );
    for( int i = 0; i < cat_num; i++ ){
          inCategory[ i ] = 0;
    }
    int fileCount = 0;

//        cout << " Gecti1 " << endl;
#ifdef __linux__
        FILE *gptr = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "r");
#else
        FILE *gptr = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "r");
#endif
    while( !feof( gptr )){
        fscanf( gptr, "%s%s", gx[count].gene, gx[count].geneType );
        count++;
    }
    fclose( gptr );
//        cout << " Gecti2 " << endl;
#ifdef __linux__
    FILE *yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" );
#else
    FILE *yFptr = fopen( "sources//usr_sources//visualization_data//genenames.txt", "r" );
#endif

    array<int> checkAnnot1( count + 1 );
    for( int j = 0; j < count; j++ )
        checkAnnot1[ j ] = 0;

    while( !feof( yFptr ) ){
        fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
        for( int j = 0; j < count; j++ ){
            if( strcmp( allGenes[ fileCount ].GENE, gx[ j ].gene ) == 0 ){
                for( int k = 0; k < cat_num; k++ ){
                    int ii = 0;
                    while( gx[ j ].geneType[ ii ] != '\0' ){
                        if( gx[ j ].geneType[ ii ] == abbv[ k ] ){
                            inCategory[ k ]++;
                            if( checkAnnot1[ j ] == 0 ){
                                genesAnnotated++;
                                checkAnnot1[ j ] = 1;
                            }
                        }
                        ii++;
                    }
                }
            }
        }
        fileCount++;
    }
    fclose( yFptr );

//        cout << " Gecti3 " << endl;
#ifdef __linux__
        resultPtr = fopen( "outputs/enrich/result.txt", "w" );
#else
        resultPtr = fopen( "outputs//enrich//result.txt", "w" );
#endif
    for( int i = -1; i < cat_num; i++ ){
            if( i == -1 )
                    fprintf( resultPtr, "%20s\t", "categories" );
            else{
                    if( i != cat_num - 1 )
                            fprintf( resultPtr, "%20s\t", categoriesOfGenes[ categoriesOfGenes.get_item( i ) ].categ );
                    else
                            fprintf( resultPtr, "%20s\n", categoriesOfGenes[ categoriesOfGenes.get_item( i ) ].categ );
            }
    }

    for( int i = -1; i < cat_num; i++ ){
            if( i == -1 )
                    fprintf( resultPtr, "%20d\t", 0 );
            else{
                    if( i != cat_num - 1 )
                            fprintf( resultPtr, "%20d\t", inCategory[ i ] );
                    else
                            fprintf( resultPtr, "%20d\n", inCategory[ i ] );
            }
    }
    array<list<double> > PValues( biNumber+1 );

        for( int i = 0; i < biNumber /*- 1*/; i++ ){
            int total = 0;
#ifdef __linux__
        sprintf( fileName2, "outputs/bicgenes/%s%s%d%s", fileName, algName, i, ".txt" );
#else
        sprintf( fileName2, "outputs//bicgenes//%s%s%d%s", fileName, algName, i, ".txt" );
#endif
                if( (fptr = fopen( fileName2, "r" )) == NULL ){
                        cout << "List " << i << " has 0 prots" << "\n";
                        for( int j = 0; j < cat_num; j++ ){
                                PValues[ i ].push_back( 1.0 );
                        }
                        for( int j = -1; j < cat_num; j++ ){
                                if( j == -1 )
                                        fprintf( resultPtr, "%20d\t", i+1 );
                                else{
                                        if( j != cat_num - 1 )
                                                fprintf( resultPtr, "%20d\t", 0 );
                                        else
                                                fprintf( resultPtr, "%20d\n", 0 );
                                        //cout << PValues[ i ][ PValues[ i ].get_item( j ) ] << " ";
                                }
                        }
                }
                else{
                        array<int> countAbbv( 64 );
                        for( int j = 0; j < cat_num; j++ ){
                                countAbbv[ j ] = 0;
                                PValues[ i ].push_back( 1.0 );
                        }
                        for( int j = 0; j < count; j++ )
                            checkAnnot1[ j ] = 0;
                        while( !feof( fptr ) ){
                                fscanf( fptr, "%s", geneTemp );
                                for( int j = 0; j < count; j++ ){
                                        if( strcmp( geneTemp, gx[ j ].gene ) == 0 ){
                                                for( int k = 0; k < cat_num; k++ ){
                                                        int ii = 0;
                                                        while( gx[ j ].geneType[ ii ] != '\0' ){
                                                            if( gx[ j ].geneType[ ii ] == abbv[ k ] ){
                                                                    countAbbv[ k ] = countAbbv[ k ] + 1;
                                                                    if( checkAnnot1[ j ] == 0 ){
                                                                        total++;
                                                                        checkAnnot1[ j ] = 1;
                                                                    }
                                                            }
                                                            ii++;
                                                        }
                                                }
                                        }
                                }
                        }
                        for( int j = -1; j < cat_num; j++ ){
                                if( j == -1 )
                                        fprintf( resultPtr, "%20d\t", i+1 );
                                else{
                                        if( j != cat_num - 1 )
                                                fprintf( resultPtr, "%20d\t", countAbbv[ j ] );
                                        else
                                                fprintf( resultPtr, "%20d\n", countAbbv[ j ] );
                                        cout << "\n*******************\n";
                                        cout << "\tI:" << inCategory[ j ] << "\n\ty:" << countAbbv[ j ] << "\n\tm:" << genesAnnotated << "\n\tn:" << total << endl;
                                        cout << "\tPValue:";
                                        PValues[ i ][ PValues[ i ].get_item( j ) ] = pvalueFinder( countAbbv[ j ], inCategory[ j ], total, genesAnnotated, cat_num );
                                        cout << PValues[ i ][ PValues[ i ].get_item( j ) ] << endl;
                                        //cout << PValues[ i ][ PValues[ i ].get_item( j ) ] << " ";
                                }
                        }

// 			fclose( fptr );
                }
    }

//        cout << " Gecti4 " << endl;
    fclose( resultPtr );

    FILE *efptr;
    if( ( efptr = fopen( "outputs/enrich/result.txt", "r" )) == NULL && ( efptr = fopen( "outputs/enrich//result.txt", "r" )) == NULL ){
            FILE *erptr;
#ifdef __linux__
            erptr = fopen( "outputs/error.txt", "w" );
#else
                erptr = fopen( "outputs//error.txt", "w" );
#endif
            fprintf( erptr, "Error-401: outputs/enrich/result.txt file does not exist in directory, enrichment process can not continue\n" );
            fclose( erptr );
            cout << "Error-401: outputs/enrich/result.txt file does not exist in directory, enrichment process can not continue\n";
            exit(1);
    }
    else{
            char readCatName[128];
            for( int i = 0; i <= cat_num; i++ ){
                    fscanf( efptr, "%s", readCatName );
            }
            int numberOfBiclusters = biNumber;
            array<int> categoryMaxGenes( cat_num );
            array< list<int> > categoryPerGenes( numberOfBiclusters );
            array< list<double> > categoryPerGenesRatio( numberOfBiclusters );
            int bic_id;
// 	    cout << " Beginning to read first row of enrichments\n" << endl;
            for( int i = 0; i <= cat_num; i++ ){
                    if( i == 0 ){
                            fscanf( efptr, "%d", &bic_id );
                    }else{
                            fscanf( efptr, "%d", &categoryMaxGenes[ i-1 ] );
                    }
            }
// 	    cout << " Beginning to read enrichments\n" << endl;
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
// 	    cout << " Beginning to Calculate Ratio\n" << endl;
            for( int counter = 0; counter < numberOfBiclusters; counter++ ){
                    for( int i = 0; i < cat_num; i++ )
                            categoryPerGenesRatio[ counter ].append( 0.0 );
                    int maxCategId = 0;
                    double gene_sum = 0;
                    for( int k = 0; k < cat_num; k++ ){
                            gene_sum += categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( k )];
                    }
                    double maxValue = 0;
                    for( int i = 0; i < cat_num; i++ ){
                            categoryPerGenesRatio[ counter ][ categoryPerGenesRatio[ counter ].get_item( i )] = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / (double)categoryMaxGenes[ i ];
                            if( maxValue <  (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / gene_sum ){
                                maxValue = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / gene_sum;
                                maxCategId = i;
                            }
                    }
                    categoriesBicluster.append( maxCategId );
            }

            fclose( efptr );
            char c;
// 	    cout << " Beginning to html file\n" << endl;
#ifdef __linux__
            efptr = fopen( "outputs/enrich/result.html", "w" );
            resultPtr = fopen( "outputs/enrich/src/header.txt", "r" );
#else
                efptr = fopen( "outputs//enrich//result.html", "w" );
            resultPtr = fopen( "outputs//enrich//src//header.txt", "r" );
#endif
            while( !feof( resultPtr ) ){
                    fscanf( resultPtr, "%c", &c );
                    fprintf( efptr, "%c", c );
            }
            fclose( resultPtr );

            fprintf( efptr, "\n\t<th scope=\"col\">Category(GO or Bicluster) Id</th>\n" );
            list<CAT> categoriesOfGenes2 = categoriesOfGenes;
            for( int i = 0; i < cat_num; i++ ){
                        for( int j = 0; j < 64; j++ ){
                                if( categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ[ j ] == '_' )
                                        categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ[ j ] = ' ';
                        }
            }
                FILE *oneResultPtr, *pieNode;
#ifdef __linux__
                resultPtr = fopen( "outputs/enrich/src/header.txt", "r" );
                for( int i = 0; i < numberOfBiclusters; i++ ){
                        char outputFile[256];
                        sprintf( outputFile, "%s%s%d%s", "outputs/enrich/resultbic", algName, i, ".html" );
                        oneResultPtr = fopen( outputFile, "w" );
                        while( !feof( resultPtr ) ){
                                fscanf( resultPtr, "%c", &c );
                                fprintf( oneResultPtr, "%c", c );
                        }
                        rewind( resultPtr );

//                        char pie[256];
//                        sprintf( pie, "%s%d.txt", "outputs/enrich/pie_", i );
//                        pieNode = fopen( pie, "w" );

                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Categories" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Number of Genes" );
                        //fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to All Category Genes" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to GO or Bicluster Gene Space" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "P-values" );
                        fprintf( oneResultPtr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for Categories %d</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, i );
                        for( int j = 0; j < cat_num; j++ ){
                                fprintf( oneResultPtr, "\t<tr>\n" );
                                fprintf( oneResultPtr, "\t\t<td>%s</td>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( j ) ].categ );
                                fprintf( oneResultPtr, "\t\t<td>%d</td>\n", categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] );
                                //fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ i ][ categoryPerGenesRatio[ i ].get_item( j )] );
                                double gene_sum = 0;
                                for( int k = 0; k < cat_num; k++ ){
                                        gene_sum += categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( k )];
                                }
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", PValues[ i ][ PValues[ i ].get_item( j )]);
                                fprintf( oneResultPtr, "\t</tr>\n" );
//                                if( categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] > 0 )
//                                    fprintf( pieNode, "%c %lf\n", abbv[ j ], (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
                        }
                        fprintf( oneResultPtr, "\t</tbody>\n</table>\n</body>\n</html>" );
                        fclose( oneResultPtr );
//                        fclose( pieNode );
                }
                fclose( resultPtr );
#else
                resultPtr = fopen( "outputs//enrich//src//header.txt", "r" );
                for( int i = 0; i < numberOfBiclusters; i++ ){
                        char outputFile[256];
                        sprintf( outputFile, "%s%s%d%s", "outputs//enrich//resultbic", algName, i, ".html" );
                        oneResultPtr = fopen( outputFile, "w" );
                        while( !feof( resultPtr ) ){
                                fscanf( resultPtr, "%c", &c );
                                fprintf( oneResultPtr, "%c", c );
                        }
                        rewind( resultPtr );

//                        char pie[256];
//                        sprintf( pie, "%s%d.txt", "outputs//enrich//pie_", i );
//                        pieNode = fopen( pie, "w" );

                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Categories" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Number of Genes" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to All Category Genes" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to GO or Bicluster Gene Space" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "P-values" );
                        fprintf( oneResultPtr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for Categories %d</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, i );
                        for( int j = 0; j < cat_num; j++ ){
                                fprintf( oneResultPtr, "\t<tr>\n" );
                                fprintf( oneResultPtr, "\t\t<td>%s</td>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( j ) ].categ );
                                fprintf( oneResultPtr, "\t\t<td>%d</td>\n", categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] );
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ i ][ categoryPerGenesRatio[ i ].get_item( j )] );
                                double gene_sum = 0;
                                for( int k = 0; k < cat_num; k++ ){
                                        gene_sum += categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( k )];
                                }
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", PValues[ i ][ PValues[ i ].get_item( j )]);
                                fprintf( oneResultPtr, "\t</tr>\n" );
//                                if( categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] > 0 )
//                                    fprintf( pieNode, "%c %lf\n", abbv[ j ], (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum  ));
                        }
                        fprintf( oneResultPtr, "\t</tbody>\n</table>\n</body>\n</html>" );
                        fclose( oneResultPtr );
//                        fclose( pieNode );
                }
                fclose( resultPtr );
#endif
            for( int i = 0; i < cat_num; i++ ){
                    fprintf( efptr, "\n\t<th scope=\"col\"> %s </th>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ );
            }
//     </tr>
//     </thead>
//     <tfoot>
//     	<tr>
//         	<td colspan="4"> Table shows the enrichment results for the specified file</td>
//         </tr>
//     </tfoot>
//     <tbody>
            fprintf( efptr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for the specified file</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", cat_num+1 );
// 	<tr>
// 	    <td rowspan="2">Stephen C. Cox</td>
// 	    <td>Marketing</td>
// 	    <td>Make discount offers</td>
// 	    <td>3/10</td>
// 	</tr>
// 	<tr>
// 	    <td>Marketing</td>
// 	    <td>Make discount offers</td>
// 	    <td>3/10</td>
// 	</tr>
            for( int counter = 0; counter < numberOfBiclusters-1; counter++ ){
                  fprintf( efptr, "\t<tr>\n" );
                  fprintf( efptr, "\t\t<td rowspan=\"1\">Category %d</td>\n", counter+1 );
                  for( int i = 0; i < cat_num; i++ ){
                          fprintf( efptr, "\t\t<td>%d</td>\n", categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] );
                  }
                  fprintf( efptr, "\t</tr>\n" );
//		  fprintf( efptr, "\t<tr>\n" );
//		  for( int i = 0; i < cat_num; i++ ){
//			  fprintf( efptr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ counter ][ categoryPerGenesRatio[ counter ].get_item( i )] );
//		  }
//		  fprintf( efptr, "\t</tr>\n");
            }
// 	      </tbody>
// 	  </table>
//
// 	  </body>
// 	  </html>
            fprintf( efptr, "\t</tbody>\n</table>\n</body>\n</html>" );
            fclose( efptr );

    }
}

void analyseGenes2( char fileName[64], list<int> &categoriesBicluster, int biNumber, char algName[64], int dimension1, int dimension, bool hasColor ){

    list<CAT> categoriesOfGenes;
    FILE *fptr,*resultPtr;
    GNDATA2 *allGenes = (GNDATA2*)malloc( sizeof( GNDATA2 ) * dimension1 );
    char fileName2[ 1024 ];
    int count = 0, cat_num;
    int genesAnnotated = 0;
    char geneTemp[ 1024 ];
    array<char> abbv(64); 
    cat_num = functionalCategoryFinder( categoriesOfGenes, abbv );
    array<int> inCategory( cat_num );
    for( int i = 0; i < cat_num; i++ ){
	  inCategory[ i ] = 0;
    } 
    int fileCount = 0;

//        cout << " Gecti1 " << endl;
#ifdef __linux__
        FILE *gptr = fopen( "sources/usr_sources/visualization_data/genefunctions.txt", "r");
#else
	FILE *gptr = fopen( "sources//usr_sources//visualization_data//genefunctions.txt", "r");
#endif
    while( !feof( gptr )){
        fscanf( gptr, "%s%s", gx[count].gene, gx[count].geneType );       
        count++;
    }
    fclose( gptr );
//        cout << " Gecti2 " << endl;
#ifdef __linux__
    FILE *yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" );
#else
    FILE *yFptr = fopen( "sources//usr_sources//visualization_data//genenames.txt", "r" );
#endif

    array<int> checkAnnot1( count + 1 );
    for( int j = 0; j < count; j++ )
        checkAnnot1[ j ] = 0;

    while( !feof( yFptr ) ){
        fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
        for( int j = 0; j < count; j++ ){
            if( strcmp( allGenes[ fileCount ].GENE, gx[ j ].gene ) == 0 ){
                for( int k = 0; k < cat_num; k++ ){
		    int ii = 0;
		    while( gx[ j ].geneType[ ii ] != '\0' ){
                        if( gx[ j ].geneType[ ii ] == abbv[ k ] ){
                            inCategory[ k ]++;
                            if( checkAnnot1[ j ] == 0 ){
                                genesAnnotated++;
                                checkAnnot1[ j ] = 1;
                            }
                        }
			ii++;
		    }
                }
            }
        }
    }
    fclose( yFptr );
    
//        cout << " Gecti3 " << endl;
#ifdef __linux__
        resultPtr = fopen( "outputs/enrich/result.txt", "w" );
#else
	resultPtr = fopen( "outputs//enrich//result.txt", "w" );
#endif
    for( int i = -1; i < cat_num; i++ ){
	    if( i == -1 )
		    fprintf( resultPtr, "%20s\t", "categories" );
	    else{
		    if( i != cat_num - 1 )
			    fprintf( resultPtr, "%20s\t", categoriesOfGenes[ categoriesOfGenes.get_item( i ) ].categ );
		    else
			    fprintf( resultPtr, "%20s\n", categoriesOfGenes[ categoriesOfGenes.get_item( i ) ].categ );
	    }
    }
    
    for( int i = -1; i < cat_num; i++ ){
	    if( i == -1 )
		    fprintf( resultPtr, "%20d\t", 0 );
	    else{
		    if( i != cat_num - 1 )
			    fprintf( resultPtr, "%20d\t", inCategory[ i ] );
		    else
			    fprintf( resultPtr, "%20d\n", inCategory[ i ] );
	    }
    }
    array<list<double> > PValues( biNumber+1 );

        for( int i = 0; i < biNumber /*- 1*/; i++ ){
            int total = 0;
#ifdef __linux__
        sprintf( fileName2, "outputs/bicgenes/%s%s%d%s", fileName, algName, i, ".txt" );
#else
        sprintf( fileName2, "outputs//bicgenes//%s%s%d%s", fileName, algName, i, ".txt" );
#endif
		if( (fptr = fopen( fileName2, "r" )) == NULL ){
                        cout << "List " << i << " has 0 prots" << "\n";
                        for( int j = 0; j < cat_num; j++ ){
                                PValues[ i ].push_back( 1.0 );
                        }
                        for( int j = -1; j < cat_num; j++ ){
                                if( j == -1 )
                                        fprintf( resultPtr, "%20d\t", i+1 );
                                else{
                                        if( j != cat_num - 1 )
                                                fprintf( resultPtr, "%20d\t", 0 );
                                        else
                                                fprintf( resultPtr, "%20d\n", 0 );
                                        //cout << PValues[ i ][ PValues[ i ].get_item( j ) ] << " ";
                                }
                        }
		}
		else{
			array<int> countAbbv( 64 );
                        for( int j = 0; j < cat_num; j++ ){
				countAbbv[ j ] = 0;
                                PValues[ i ].push_back( 1.0 );
                        }
                        for( int j = 0; j < count; j++ )
                            checkAnnot1[ j ] = 0;
			while( !feof( fptr ) ){
				fscanf( fptr, "%s", geneTemp );
                                for( int j = 0; j < count; j++ ){
					if( strcmp( geneTemp, gx[ j ].gene ) == 0 ){
						for( int k = 0; k < cat_num; k++ ){
							int ii = 0;
                                                        while( gx[ j ].geneType[ ii ] != '\0' ){
                                                            if( gx[ j ].geneType[ ii ] == abbv[ k ] ){
                                                                    countAbbv[ k ] = countAbbv[ k ] + 1;
                                                                    if( checkAnnot1[ j ] == 0 ){
                                                                        total++;
                                                                        checkAnnot1[ j ] = 1;
                                                                    }
                                                            }
							    ii++;
							}	
						}
					}
				}
			}
			for( int j = -1; j < cat_num; j++ ){
				if( j == -1 )
					fprintf( resultPtr, "%20d\t", i+1 );
				else{
					if( j != cat_num - 1 )
						fprintf( resultPtr, "%20d\t", countAbbv[ j ] );
					else
                                                fprintf( resultPtr, "%20d\n", countAbbv[ j ] );
                                        PValues[ i ][ PValues[ i ].get_item( j ) ] = pvalueFinder( countAbbv[ j ], inCategory[ j ], total, genesAnnotated, cat_num );
                                        //cout << PValues[ i ][ PValues[ i ].get_item( j ) ] << " ";
				}
                        }

// 			fclose( fptr );
		}
    }

//        cout << " Gecti4 " << endl;
    fclose( resultPtr );

    FILE *efptr;
    if( ( efptr = fopen( "outputs/enrich/result.txt", "r" )) == NULL && ( efptr = fopen( "outputs/enrich//result.txt", "r" )) == NULL ){
	    FILE *erptr;
#ifdef __linux__
	    erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
	    fprintf( erptr, "Error-401: outputs/enrich/result.txt file does not exist in directory, enrichment process can not continue\n" );
	    fclose( erptr );
	    cout << "Error-401: outputs/enrich/result.txt file does not exist in directory, enrichment process can not continue\n"; 
	    exit(1);
    }
    else{
	    char readCatName[128];
	    for( int i = 0; i <= cat_num; i++ ){
		    fscanf( efptr, "%s", readCatName );
	    }
	    int numberOfBiclusters = biNumber;
	    array<int> categoryMaxGenes( cat_num );
	    array< list<int> > categoryPerGenes( numberOfBiclusters );
	    array< list<double> > categoryPerGenesRatio( numberOfBiclusters );
	    int bic_id;
// 	    cout << " Beginning to read first row of enrichments\n" << endl;
	    for( int i = 0; i <= cat_num; i++ ){
		    if( i == 0 ){
			    fscanf( efptr, "%d", &bic_id );
		    }else{
			    fscanf( efptr, "%d", &categoryMaxGenes[ i-1 ] );
		    }
	    }
// 	    cout << " Beginning to read enrichments\n" << endl;
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
// 	    cout << " Beginning to Calculate Ratio\n" << endl;
	    for( int counter = 0; counter < numberOfBiclusters; counter++ ){
		    for( int i = 0; i < cat_num; i++ )
			    categoryPerGenesRatio[ counter ].append( 0.0 );
		    int maxCategId = 0; 
		    double gene_sum = 0;
		    for( int k = 0; k < cat_num; k++ ){
			    gene_sum += categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( k )];
		    }
		    double maxValue = 0;
		    for( int i = 0; i < cat_num; i++ ){
			    categoryPerGenesRatio[ counter ][ categoryPerGenesRatio[ counter ].get_item( i )] = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / (double)categoryMaxGenes[ i ];
			    if( maxValue <  (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / gene_sum ){
				maxValue = (double)categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] / gene_sum;
				maxCategId = i;
			    }
		    }
		    categoriesBicluster.append( maxCategId ); 
	    }

	    fclose( efptr );
	    char c;
// 	    cout << " Beginning to html file\n" << endl;
#ifdef __linux__
	    efptr = fopen( "outputs/enrich/result.html", "w" );
	    resultPtr = fopen( "outputs/enrich/src/header.txt", "r" );
#else
		efptr = fopen( "outputs//enrich//result.html", "w" );
	    resultPtr = fopen( "outputs//enrich//src//header.txt", "r" );
#endif
	    while( !feof( resultPtr ) ){
		    fscanf( resultPtr, "%c", &c );
		    fprintf( efptr, "%c", c );
	    }
	    fclose( resultPtr );

            fprintf( efptr, "\n\t<th scope=\"col\">Category(GO or Bicluster) Id</th>\n" );
	    list<CAT> categoriesOfGenes2 = categoriesOfGenes;
	    for( int i = 0; i < cat_num; i++ ){
			for( int j = 0; j < 64; j++ ){
				if( categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ[ j ] == '_' )
					categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ[ j ] = ' ';
			}
	    }
                FILE *oneResultPtr, *pieNode;
#ifdef __linux__
		resultPtr = fopen( "outputs/enrich/src/header.txt", "r" );
		for( int i = 0; i < numberOfBiclusters; i++ ){
			char outputFile[256];
			sprintf( outputFile, "%s%d%s", "outputs/enrich/resultbic", i, ".html" );
			oneResultPtr = fopen( outputFile, "w" );
			while( !feof( resultPtr ) ){
				fscanf( resultPtr, "%c", &c );
				fprintf( oneResultPtr, "%c", c );
			}
			rewind( resultPtr );

                        char pie[256];
                        sprintf( pie, "%s%d.txt", "outputs/enrich/pie_", i );
                        pieNode = fopen( pie, "w" );

			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Categories" );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Number of Genes" );
                        //fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to All Category Genes" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to GO or Bicluster Gene Space" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "P-values" );
                        fprintf( oneResultPtr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for Categories %d</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, i );
			for( int j = 0; j < cat_num; j++ ){
				fprintf( oneResultPtr, "\t<tr>\n" );
				fprintf( oneResultPtr, "\t\t<td>%s</td>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( j ) ].categ );
				fprintf( oneResultPtr, "\t\t<td>%d</td>\n", categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] );
                                //fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ i ][ categoryPerGenesRatio[ i ].get_item( j )] );
				double gene_sum = 0;
				for( int k = 0; k < cat_num; k++ ){
					gene_sum += categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( k )];
				}
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", PValues[ i ][ PValues[ i ].get_item( j )]);
				fprintf( oneResultPtr, "\t</tr>\n" );
                                if( categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] > 0 )
                                    fprintf( pieNode, "%c %lf\n", abbv[ j ], (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
			}
			fprintf( oneResultPtr, "\t</tbody>\n</table>\n</body>\n</html>" );
			fclose( oneResultPtr );
                        fclose( pieNode );
		}
		fclose( resultPtr );
#else
		resultPtr = fopen( "outputs//enrich//src//header.txt", "r" );
		for( int i = 0; i < numberOfBiclusters; i++ ){
			char outputFile[256];
			sprintf( outputFile, "%s%d%s", "outputs//enrich//resultbic", i, ".html" );
			oneResultPtr = fopen( outputFile, "w" );
			while( !feof( resultPtr ) ){
				fscanf( resultPtr, "%c", &c );
				fprintf( oneResultPtr, "%c", c );
			}
			rewind( resultPtr );

                        char pie[256];
                        sprintf( pie, "%s%d.txt", "outputs//enrich//pie_", i );
                        pieNode = fopen( pie, "w" );

			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Categories" );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Number of Genes" );
			fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to All Category Genes" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "Ratio Respect to GO or Bicluster Gene Space" );
                        fprintf( oneResultPtr, "\n\t<th scope=\"col\"> %s </th>\n", "P-values" );
                        fprintf( oneResultPtr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for Categories %d</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", 3, i );
			for( int j = 0; j < cat_num; j++ ){
				fprintf( oneResultPtr, "\t<tr>\n" );
				fprintf( oneResultPtr, "\t\t<td>%s</td>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( j ) ].categ );
				fprintf( oneResultPtr, "\t\t<td>%d</td>\n", categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] );
				fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ i ][ categoryPerGenesRatio[ i ].get_item( j )] );
				double gene_sum = 0;
				for( int k = 0; k < cat_num; k++ ){
					gene_sum += categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( k )];
				}
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum ) );
                                fprintf( oneResultPtr, "\t\t<td>%lf</td>\n", PValues[ i ][ PValues[ i ].get_item( j )]);
				fprintf( oneResultPtr, "\t</tr>\n" );
                                if( categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] > 0 )
                                    fprintf( pieNode, "%c %lf\n", abbv[ j ], (double)(categoryPerGenes[ i ][ categoryPerGenes[ i ].get_item( j )] / gene_sum  ));
			}
			fprintf( oneResultPtr, "\t</tbody>\n</table>\n</body>\n</html>" );
			fclose( oneResultPtr );                        
                        fclose( pieNode );
		}
		fclose( resultPtr );
#endif
	    for( int i = 0; i < cat_num; i++ ){
		    fprintf( efptr, "\n\t<th scope=\"col\"> %s </th>\n", categoriesOfGenes2[ categoriesOfGenes2.get_item( i ) ].categ );
	    }
//     </tr>
//     </thead>
//     <tfoot>
//     	<tr>
//         	<td colspan="4"> Table shows the enrichment results for the specified file</td>
//         </tr>
//     </tfoot>
//     <tbody>
	    fprintf( efptr, "\n\t</tr>\n\t</thead>\n\t<tfoot>\n\t<tr>\n\t\t<td colspan=\"%d\">Table shows the enrichment results for the specified file</td>\n\t<tr>\n\t</tfoot>\n<tbody>\n", cat_num+1 );
// 	<tr>
// 	    <td rowspan="2">Stephen C. Cox</td>
// 	    <td>Marketing</td>
// 	    <td>Make discount offers</td>
// 	    <td>3/10</td>
// 	</tr>
// 	<tr>
// 	    <td>Marketing</td>
// 	    <td>Make discount offers</td>
// 	    <td>3/10</td>
// 	</tr>
	    for( int counter = 0; counter < numberOfBiclusters-1; counter++ ){
		  fprintf( efptr, "\t<tr>\n" );
                  fprintf( efptr, "\t\t<td rowspan=\"1\">Category %d</td>\n", counter+1 );
		  for( int i = 0; i < cat_num; i++ ){
			  fprintf( efptr, "\t\t<td>%d</td>\n", categoryPerGenes[ counter ][ categoryPerGenes[ counter ].get_item( i )] );
		  }
		  fprintf( efptr, "\t</tr>\n" );
//		  fprintf( efptr, "\t<tr>\n" );
//		  for( int i = 0; i < cat_num; i++ ){
//			  fprintf( efptr, "\t\t<td>%lf</td>\n", categoryPerGenesRatio[ counter ][ categoryPerGenesRatio[ counter ].get_item( i )] );
//		  }
//		  fprintf( efptr, "\t</tr>\n");
	    }
// 	      </tbody>
// 	  </table>
// 
// 	  </body>
// 	  </html>
	    fprintf( efptr, "\t</tbody>\n</table>\n</body>\n</html>" );
	    fclose( efptr );
	      
    }
}

/* 
** Gets bicluster from file from entries of file
** format is
** number1 number2
** conditions number of number1
** genes number of number2 
*/
 void getBiclustersFromFile( leda::matrix &M , char defaultBicFile[256], int inp, int fraction, int high, list<leda::matrix> &matrixList_l, list<list<GENES> > &results, list<list<CONDS> > &conditions, int dimension1, int dimension2  ){
	random_source S(1,inp);
	int numberOfGenes = 0;
	int numberOfConditions = 0;
	list<leda::string> listOfFile;
	leda::string temp,number;
	char getGenes[ 20 ];
	char getConds[ 20 ];
	list_item it,it2,it3;
	list<int> indexCond;
	list<int> indexGenes;
	int numberOfBiclusters;
	FILE *fptr;

	if( ( fptr = fopen( defaultBicFile, "r") ) == NULL ){
	    FILE *erptr;
#ifdef __linux__
	    erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
	    fprintf( erptr, "Error-402: %s does not exist\n", defaultBicFile );
	    fclose( erptr );
	    cout << "\nError-402: " << defaultBicFile << " file does not exist\n"; 
	    exit(1);
	}
	else{
		fscanf( fptr, "%d", &numberOfBiclusters );
		fscanf( fptr, "%d%d", &numberOfConditions , &numberOfGenes );	
		int bicount = 0;

		int geneCount = 1;
                array<GENES> allGenes( dimension1 + 1 );
                int fileCount = 0;
                leda::string genes_s;
                FILE *yFptr;
				if( ( yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" )) == NULL && ( yFptr = fopen( "sources//usr_sources//visualization_data//genenames.txt", "r" )) == NULL  )
					cout << "\n Could not open specified file \n";
				else{
					while( !feof( yFptr ) ){
						fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
						allGenes[ fileCount ].index = fileCount;
						fileCount++;
					}
				}
                fclose( yFptr );
                char gene[1024];
                char realGeneName[1024];
		while( bicount < numberOfBiclusters -1 ){
			for( int i = 0; i < numberOfConditions; i++ ){
			    fscanf( fptr, "%s", getGenes );
			    listOfFile.append( getGenes );
			}
			forall_items( it, listOfFile ){
#ifdef __linux__
			    FILE *xptr = fopen( "sources/usr_sources/visualization_data/geneNameConversion.txt", "r" );
#else
			    FILE *xptr = fopen( "sources//usr_sources//visualization_data//geneNameConversion.txt", "r" );
#endif
			    while( !feof( xptr ) ){
					fscanf( xptr, "%s%s", gene, realGeneName );
					if( strcmp( listOfFile[ it ], realGeneName ) == 0 ){
						listOfFile[ it ] = gene;
					}
			    }
			    fclose( xptr );
			}
			forall_items( it, listOfFile ){
			    temp = listOfFile[ it ];
			    int count = 0;
			    char number[ 10 ];
			    number[ count ] = temp[ 4 ];
			    int i = 4;
				while( i != temp.length() ){
				    //number += temp( i + 1 );
				    //number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
				    number[ count + 1 ] = temp[ i + 1 ];
				    //cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
				    i++;
				    if( i == temp.length() )
					break;
				    count++;
				}       

				//cout << number << endl;
				char temp2[10];
				for( int j = 0; j <= count + 1; j++ ){
				    temp2[ j ] = number[ j ];
				}
				int geneNumber = atoi( temp2 );
				indexGenes.append( geneNumber );
			}


			temp.del_all( temp );

			number.del_all( number );
			listOfFile.clear();

			list<CONDS> condss;
			for( int i = 0; i < numberOfGenes; i++ ){
			    fscanf( fptr, "%s", getConds );
			    listOfFile.append( getConds );
			    CONDS condi;
			    sprintf( condi.COND, "%s", getConds);
			    condss.append( condi );
			}

			forall_items( it, listOfFile ){
			    temp = listOfFile[ it ];
			    //cout << temp << "  length " << temp.length();
			    int count = 0;
			    char number[ 10 ];
			    number[ count ] = temp[ 4 ];
			    int i = 4;
				while( i != temp.length() ){
				    //number += temp( i + 1 );
				    //number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
				    //number.insert( count + 1, temp[ i + 1 ] );
				    number[ count + 1 ] = temp[ i + 1 ];
				    //cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
				    i++;
				    if( i == temp.length() )
					break;
				    count++;
				}

				//cout << number << endl;
				char temp2[10];
				for( int j = 0; j <= count + 1; j++ ){
				    temp2[ j ] = number[ j ];
				}
				int condNumber = atoi( temp2 );
				indexCond.append( condNumber );
			}
		      
		      
			temp.del_all( temp );
			number.del_all( number );
			listOfFile.clear();
			list<leda::vector> V;

			
			FILE *yFptr, *saveGene;
			if( indexGenes.size() >= fraction && indexGenes.size() <= high ){
				char geneFile[ 1024 ];
#ifdef __linux__
				sprintf( geneFile, "%s%d%s", "outputs/bicgenes/geneResult", geneCount, ".txt" ); 
// 				cout << " created\n";
#else
				sprintf( geneFile, "%s%d%s", "outputs//bicgenes//geneResult", geneCount, ".txt" ); 
// 				cout << " created\n";
#endif
				saveGene = fopen( geneFile , "w" );            
			}            
// 			cout << indexGenes.size() << "\t" << indexCond.size() << endl;
			matrix TEMP( indexGenes.size(), indexCond.size() );
// 			cout << endl << M.dim1() << "\t" << M.dim2() << endl;
			int count = 0,count2 = 0;
			forall_items( it, indexGenes ){ 
				forall_items( it2, indexCond ){
					/*if( indexCond[ it2 ] >= dimension1 )
					    break;
					if( indexGenes[ it ] >= dimension2 )
					    break;*/
// 					cout << indexGenes[ it ] << "   " << indexCond[ it2 ] << "   " /*<< M( indexCond[ it2 ],indexGenes[ it ] ) */<< endl;
					TEMP( count, count2 ) = M( indexCond[ it2 ],indexGenes[ it ] );
					count2++;
				}
				//v.print();
				count++;
				count2 = 0;
			}

			list<GENES> geness;
			count = 0;	 

			forall_items( it2, indexGenes ){       
// 				cout << allGenes[ indexGenes[ it2 ] ].GENE << endl;
				if( indexGenes.size() >= fraction && indexGenes.size() <= high )   
					fprintf( saveGene, "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );     
				geness.append( allGenes[ indexGenes[ it2 ] ] );
				count++;
			}
			if( indexGenes.size() >= fraction && indexGenes.size() <= high ){
				fclose( saveGene );
				results.append( geness );
				conditions.append( condss );
				matrixList_l.append( TEMP );
				geneCount++;
			}
			indexCond.clear();
			indexGenes.clear();

			fscanf( fptr, "%d%d", &numberOfConditions , &numberOfGenes );
			//TEMP.print();
#ifdef DEBUG_ROBINVIZ
			cout << "\n Bicluster " << bicount << " is processed";
#endif
			bicount++;
// 			geneCount++;
		}
	}
#ifdef DEBUG_ROBINVIZ
 	cout << endl;
#endif
}

// FOR CC and BIMAX
void getBiclustersFromFile2( leda::matrix &M , int inp, int fraction, int high, list<leda::matrix> &matrixList_l , list<list<GENES> > &results, list<list<CONDS> > &conditions, char algName[], int dimension1, int dimension2  ){
	M = M.trans();
	random_source S(1,inp);
	int numberOfGenes = 0;
	int numberOfConditions = 0;
	list<leda::string> listOfFile;
	leda::string temp,number;
	char getGenes[ 20 ];
	char getConds[ 20 ];
	list_item it,it2,it3;
	list<int> indexCond;
	list<int> indexGenes;
	char nameFile[1024];
#ifdef __linux__
	sprintf( nameFile, "%s%s%s", "outputs/biclusters/", algName, "Result.txt" );
#else	
	sprintf( nameFile, "%s%s%s", "outputs//biclusters//", algName, "Result.txt" );
#endif
	FILE *fptr = fopen( nameFile, "r" );
        fscanf( fptr, "%d%d", &numberOfConditions , &numberOfGenes);
        int bicount = 0;

        int geneCount = 0;

	array<GENES> allGenes( dimension1 + 1 );
#ifdef __linux__
	FILE *yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" );
#else
	FILE *yFptr = fopen( "sources//usr_sources//visualization_data//genenames.txt", "r" );
#endif
			int fileCount = 0;
	leda::string genes_s;
	while( !feof( yFptr ) ){
	    fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
	    allGenes[ fileCount ].index = fileCount;
	    fileCount++;
	}
	fclose( yFptr );

	char gene[1024];
	char realGeneName[1024];
        while( !feof( fptr ) ){
           
           
            for( int i = 0; i < numberOfConditions; i++ ){
                fscanf( fptr, "%s", getGenes );
                listOfFile.append( getGenes );
            }
//            forall_items( it, listOfFile ){
//#ifdef __linux__
//                FILE *xptr = fopen( "sources/usr_sources/visualization_data/geneNameConversion.txt", "r" );
//#else
//                FILE *xptr = fopen( "sources//usr_sources//visualization_data//geneNameConversion.txt", "r" );
//#endif
//                while( !feof( xptr ) ){
//                    fscanf( xptr, "%s%s", gene, realGeneName );
//                    if( strcmp( listOfFile[ it ], realGeneName ) == 0 ){
//                        listOfFile[ it ] = gene;
//                        break;
//                    }
//                }
//                fclose( xptr );
//            }
            forall_items( it, listOfFile ){
                temp = listOfFile[ it ];
                int count = 0;
                char number[ 10 ];
                number[ count ] = temp[ 4 ];
                int i = 4;
                    while( i != temp.length() ){
                        //number += temp( i + 1 );
                        //number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
                        number[ count + 1 ] = temp[ i + 1 ];
                        //cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
                        i++;
                        if( i == temp.length() )
                            break;
                        count++;
                    }       

                    //cout << number << endl;
                    char temp2[10];
                    for( int j = 0; j <= count + 1; j++ ){
                        temp2[ j ] = number[ j ];
                    }
                    int geneNumber = atoi( temp2 );
                    indexGenes.append( geneNumber );
            }


            temp.del_all( temp );

            number.del_all( number );
            listOfFile.clear();

	    list<CONDS> condss;
            for( int i = 0; i < numberOfGenes; i++ ){
                fscanf( fptr, "%s", getConds );
                listOfFile.append( getConds );
		CONDS condi;
		sprintf( condi.COND, "%s", getConds);
		condss.append( condi );
            }

            forall_items( it, listOfFile ){
                temp = listOfFile[ it ];
                //cout << temp << "  length " << temp.length();
                int count = 0;
                char number[ 10 ];
                number[ count ] = temp[ 4 ];
                int i = 4;
                    while( i != temp.length() ){
                        //number += temp( i + 1 );
                        //number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
                        //number.insert( count + 1, temp[ i + 1 ] );
                        number[ count + 1 ] = temp[ i + 1 ];
                        //cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
                        i++;
                        if( i == temp.length() )
                            break;
                        count++;
                    }

                    //cout << number << endl;
                    char temp2[10];
                    for( int j = 0; j <= count + 1; j++ ){
                        temp2[ j ] = number[ j ];
                    }
                    int condNumber = atoi( temp2 );
                    indexCond.append( condNumber );
            }
           
           
            temp.del_all( temp );
            number.del_all( number );
            listOfFile.clear();
            list<leda::vector> V;

            //M.print();
            // For real data comment belov line
		FILE *yFptr, *saveGene;
		if( indexGenes.size() >= fraction && indexGenes.size() <= high ){
			char geneFile[ 1024 ];
#ifdef __linux__
			sprintf( geneFile, "%s%s%d%s", "outputs/bicgenes/geneResult", algName, geneCount, ".txt" ); 
#else
			sprintf( geneFile, "%s%s%d%s", "outputs//bicgenes//geneResult", algName, geneCount, ".txt" ); 
#endif
			saveGene = fopen( geneFile , "w" );            
		}
		matrix TEMP( indexGenes.size(), indexCond.size() );
		//****************************************************//
		//matrix TEMP( indexGenes.size() , indexCond.size() );
		//****************************************************//
		//cout << endl << M.dim1() << "      " <<  M.dim2() << " Vector : " << V.size() << endl;
		int count = 0,count2 = 0;
		/*	indexGenes.print();
			indexCond.print();*/
//  		cout << "M.dim1() " << M.dim1() << "  M.dim2()" << M.dim2() << "   TEMP.dim1() " << TEMP.dim1() << "  TEMP.dim2()" << TEMP.dim2() << endl;
//   		indexGenes.print();
//  		cout << "M.dim1() " << M.dim1() << "  M.dim2()" << M.dim2() << "   TEMP.dim1() " << TEMP.dim1() << "  TEMP.dim2()" << TEMP.dim2() << endl; 
// 		indexCond.print();

		forall_items( it, indexGenes ){ 
			forall_items( it2, indexCond ){
				/*if( indexCond[ it2 ] >= dimension1 )
				    break;
				if( indexGenes[ it ] >= dimension2 )
				    break;*/
// 				cout << indexGenes[ it ] << " - " << indexCond[ it2 ] << "   " << count << " - " << count2 << endl;
// 				cout << endl <<count <<"-" << count2 << " - " << indexCond[ it2 ] << " - " << indexGenes[ it ]  <<endl;
				TEMP( count, count2 ) = M( indexGenes[ it ] , indexCond[ it2 ]);
				count2++;
			}
                        //v.print();
                        count++;
                        count2 = 0;
		}

		list<GENES> geness;
		count = 0;	 
		forall_items( it2, indexGenes ){  
			if( indexGenes.size() >= fraction && indexGenes.size() <= high )
				fprintf( saveGene, "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );     
			//printf( "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );  
			geness.append( allGenes[ indexGenes[ it2 ] ] );
			count++;
		}
		//geness.print();

		if( indexGenes.size() >= fraction && indexGenes.size() <= high ){
                        fclose( saveGene );
                        results.append( geness );
			conditions.append( condss );
			matrixList_l.append( TEMP );
			geneCount++;
		}

		indexCond.clear();
		indexGenes.clear();
		fscanf( fptr, "%d%d", &numberOfConditions , &numberOfGenes );	
		//TEMP.print();
#ifdef DEBUG_ROBINVIZ
                cout << "\n Bicluster " << bicount << " is processed\n";
#endif
		bicount++;
// 		geneCount++;
	}
#ifdef DEBUG_ROBINVIZ
	cout << endl;
#endif
	fclose(fptr);
	M = M.trans();
}

// SAMBA cond is gene in this function
void getBiclustersFromFile3( leda::matrix &M , int inp, int fraction, int high, list<leda::matrix> &matrixList_l , list<list<GENES> > &results, char algName[], int dimension1, int dimension2  ){

	M = M.trans();
	random_source S(1,inp);
	int numberOfGenes = 0;
	int numberOfConditions = 0;
	list<leda::string> listOfFile;
	leda::string temp,number;
	char getGenes[ 10 ];
	char getConds[ 10 ];
	list_item it,it2,it3;
	list<int> indexCond;
	list<int> indexGenes;
	char nameFile[1024];
	sprintf( nameFile, "%s%s%s", "outputs/biclusters/", algName, "Result.txt" );
	FILE *fptr = fopen( nameFile, "r" );
		fscanf( fptr, "%d%d", &numberOfConditions , &numberOfGenes);	
		int bicount = 0;

        int geneCount = 1;

                GENES *allGenes = (GENES*)malloc( sizeof( GENES ) * dimension1 );
                FILE *yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" );
                               int fileCount = 0;
                leda::string genes_s;
                while( !feof( yFptr ) ){
                    fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
                    allGenes[ fileCount ].index = fileCount;
                    fileCount++;
                }
                fclose( yFptr );

                char gene[1024];
                char realGeneName[1024];

	while( !feof( fptr ) ){
            
		for( int i = 0; i < numberOfGenes; i++ ){
		    fscanf( fptr, "%s", getGenes );
		    listOfFile.append( getGenes );
		}
		forall_items( it, listOfFile ){
			temp = listOfFile[ it ];
			int count = 0;
			char number[ 10 ];
			number[ count ] = temp[ 4 ];
			int i = 4;
			while( i != temp.length() ){
				//number += temp( i + 1 );
				//number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
				number[ count + 1 ] = temp[ i + 1 ];
				//cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
				i++;
				if( i == temp.length() ) 
					break;
				count++;
			}		

			//cout << number << endl;
			char temp2[10];
			for( int j = 0; j <= count + 1; j++ ){
				temp2[ j ] = number[ j ];
			}
			int geneNumber = atoi( temp2 );
			indexGenes.append( geneNumber );
		}


		temp.del_all( temp );

		number.del_all( number );
		listOfFile.clear();


		for( int i = 0; i < numberOfConditions; i++ ){
			fscanf( fptr, "%s", getConds );
			listOfFile.append( getConds );
		}
		forall_items( it, listOfFile ){
			temp = listOfFile[ it ];
			//cout << temp << "  length " << temp.length();
			int count = 0;
			char number[ 10 ];
			number[ count ] = temp[ 4 ];
			int i = 4;
			while( i != temp.length() ){
				//number += temp( i + 1 );
				//number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
				//number.insert( count + 1, temp[ i + 1 ] );
				number[ count + 1 ] = temp[ i + 1 ];
				//cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
				i++;
				if( i == temp.length() ) 
					break;
				count++;
			}

			//cout << number << endl;
			char temp2[10]; 
			for( int j = 0; j <= count + 1; j++ ){
				temp2[ j ] = number[ j ];
			}
			int condNumber = atoi( temp2 );
			indexCond.append( condNumber );
		}
			    
		
		temp.del_all( temp );
		number.del_all( number );
		listOfFile.clear();
		list<leda::vector> V;

		//M.print();
		// For real data comment belov line
		FILE *yFptr, *saveGene;
		if( indexGenes.size() >= fraction && indexGenes.size() <= high ){
			char geneFile[ 1024 ];
			sprintf( geneFile, "%s%s%d%s", "sources/bicluster_sources/geneResult", algName, geneCount, ".txt" ); 
			saveGene = fopen( geneFile , "w" );            
		}
		matrix TEMP( indexGenes.size(), indexCond.size() );
		//****************************************************//
		//matrix TEMP( indexGenes.size() , indexCond.size() );
		//****************************************************//
// 		cout << endl << M.dim1() << "      " <<  M.dim2() << " Vector : " << V.size() << endl;
		int count = 0,count2 = 0;

		forall_items( it, indexGenes ){ 
			forall_items( it2, indexCond ){
				/*if( indexCond[ it2 ] >= dimension1 )
				    break;
				if( indexGenes[ it ] >= dimension2 )
				    break;*/
// 				cout << indexGenes[ it ] << "   " << indexCond[ it2 ] << endl;
				TEMP( count, count2 ) = M( indexCond[ it2 ]-1,  indexGenes[ it ]-1 );
				count2++;
			}
			//v.print();
			count++;
			count2 = 0;
		}

		list<GENES> geness;
		count = 0;	 
		forall_items( it2, indexGenes ){
			if( indexGenes.size() >= fraction && indexGenes.size() <= high )
				fprintf( saveGene, "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );     
			//printf( "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );  
			geness.append( allGenes[ indexGenes[ it2 ] ] );
			count++;
		}
		//geness.print();
		if( geness.size() >= fraction && geness.size() <= high ){
			fclose( saveGene );
			results.append( geness );
			matrixList_l.append( TEMP );
			geneCount++;
		}
		indexCond.clear();
		indexGenes.clear();
		fscanf( fptr, "%d%d", &numberOfConditions , &numberOfGenes );	
		//TEMP.print();
		bicount++;
	}
	fclose(fptr);
}

// // Bivusu Greedy Pcluster cond is gene in this function
// void getBiclustersFromFile4( leda::matrix &M , int inp, int fraction, int high, list<leda::matrix> &matrixList_l , list<list<GENES> > &results, char algName[], int dimension1, int dimension2  ){
//    
//     int numberOfGenes = 0;
//     int numberOfConditions = 0;
//     list<leda::string> listOfFile,listOfFile2;
//     leda::string temp,number;
//     char getGenes[ 10 ];
//     int getCondId;
//     int getGeneId;
//     char getConds[ 10 ],realGeneName[ 10 ];
//     list_item it,it2,it3;
//     list<int> indexCond;
//     list<int> indexGenes;
//     double t_d;
//     char algFile[1024];
//     char gene[ 12 ];
//     sprintf( algFile, "%s%s", algName , "Result.txt" );
//     FILE *fptr = fopen( algFile , "r" );
//         fscanf( fptr, "%s%d%d%lf", getGenes, &numberOfGenes , &numberOfConditions, &t_d );   
//         int bicount = 0;
// 
//         int geneCount = 1;
// 
//                 GENES allGenes[ dimension1 ];
//                 FILE *yFptr = fopen( "GeneNamesYeast.txt", "r" );
//                 int fileCount = 0;
//                 leda::string genes_s;
//                 while( !feof( yFptr ) ){
//                     fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
//                     allGenes[ fileCount ].index = fileCount;
//                     fileCount++;
//                 }
//                 fclose( yFptr );
// 
//         while( !feof( fptr ) ){
//            
//             fscanf( fptr, "%s", getConds );
//             for( int i = 0; i < numberOfConditions; i++ ){
//                 fscanf( fptr, "%d", &getCondId );
//                 sprintf( getConds, "%s%d", "cond", getCondId );
//                 listOfFile.append( getConds );
//             }
//             forall_items( it, listOfFile ){
//                 temp = listOfFile[ it ];
//                 int count = 0;
//                 char number[ 10 ];
//                 number[ count ] = temp[ 4 ];
//                 int i = 4;
//                     while( i != temp.length() ){
//                         //number += temp( i + 1 );
//                         //number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
//                         number[ count + 1 ] = temp[ i + 1 ];
//                         //cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
//                         i++;
//                         if( i == temp.length() )
//                             break;
//                         count++;
//                     }       
// 
//                     //cout << number << endl;
//                     char temp2[10];
//                     for( int j = 0; j <= count + 1; j++ ){
//                         temp2[ j ] = number[ j ];
//                     }
//                     int condNumber = atoi( temp2 );
//                     indexCond.append( condNumber );
//             }
//             temp.del_all( temp );
//             number.del_all( number );
//             listOfFile.clear();
// 
//             fscanf( fptr, "%s", getGenes );
//             for( int i = 0; i < numberOfGenes; i++ ){
//                 fscanf( fptr, "%d", &getGeneId );
//                 sprintf( getGenes, "%s%d", "cond", getGeneId );
//                 listOfFile.append( getGenes );
//             }
//             forall_items( it, listOfFile ){
//                 FILE *xptr = fopen( "sources/usr_sources/visualization_data/geneNameConversion.txt", "r" );
//                 while( !feof( xptr ) ){
//                     fscanf( xptr, "%s%s", gene, realGeneName );
//                     if( strcmp( listOfFile[ it ], realGeneName ) == 0 ){
//                         listOfFile[ it ] = gene;
//                     }
//                 }
//                 fclose( xptr );
//             }
//             forall_items( it, listOfFile ){
//                 temp = listOfFile[ it ];
//                 //cout << temp << "  length " << temp.length();
//                 int count = 0;
//                 char number[ 10 ];
//                 number[ count ] = temp[ 4 ];
//                 int i = 4;
//                     while( i != temp.length() ){
//                         //number += temp( i + 1 );
//                         //number.replace( 0, number.insert( count + 1, temp[ i + 1 ] ));
//                         //number.insert( count + 1, temp[ i + 1 ] );
//                         number[ count + 1 ] = temp[ i + 1 ];
//                         //cout << endl << number[ count + 1 ] << "         " << temp[ i + 1 ] << endl;
//                         i++;
//                         if( i == temp.length() )
//                             break;
//                         count++;
//                     }
// 
//                     //cout << number << endl;
//                     char temp2[10];
//                     for( int j = 0; j <= count + 1; j++ ){
//                         temp2[ j ] = number[ j ];
//                     }
//                     int geneNumber = atoi( temp2 );
//                     indexGenes.append( geneNumber );
//             }
//            
//             for( int i = 0; i < numberOfConditions; i++ )
//                 fscanf( fptr, "%s", getConds);
//             for( int i = 0; i < numberOfGenes; i++ )
//                 fscanf( fptr, "%s", getGenes );
// 
//             temp.del_all( temp );
//             number.del_all( number );
//             listOfFile.clear();
// 
// 	    FILE *saveGene;
// 	    char geneFile[ 1024 ];
// 	    int temp;
// 	    S >> temp;	
// 	    sprintf( geneFile, "%s%s%d%s", "sources/bicluster_sources/generesult", algName, geneCount, ".txt" ); 
// 	    saveGene = fopen( geneFile , "w" );            
// 
// 	    matrix TEMP( indexGenes.size(), indexCond.size() );
// 
// 	    int count = 0,count2 = 0;
// 	    forall_items( it, indexGenes ){ 
// 		    forall_items( it2, indexCond ){
// 			    /*if( indexCond[ it2 ] >= dimension1 )
// 				break;
// 			    if( indexGenes[ it ] >= dimension2 )
// 				break;*/
// 			    //cout << indexGenes[ it ] << "   " << indexCond[ it2 ] << "   " << M( indexCond[ it2 ],indexGenes[ it ] ) << endl;
// 			    TEMP( count, count2 ) = M( indexCond[ it2 ],indexGenes[ it ] );
// 			    count2++;
// 		    }
// 		    //v.print();
// 		    count++;
// 		    count2 = 0;
// 	    }
// 
// 	    list<GENES> geness;
// 	    count = 0;	 
// 	    forall_items( it2, indexGenes ){
// 			    if( indexGenes[ it2 ] <= dimension1 ){    
// 					    fprintf( saveGene, "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );            
// 					    geness.append( allGenes[ indexGenes[ it2 ] ] );
// 			    }
// 			    count++;
// 	    }
// 	    fclose( saveGene );
// 	    if( geness.size() >= fraction && geness.size() <= high ){
// 		    results.append( geness );
// 		    matrixList_l.append( TEMP );
// 	    }
// 
//             results.append( indexGenes );
//            
//             indexCond.clear();
//             indexGenes.clear();
//             fscanf( fptr, "%s%d%d%lf", getGenes, &numberOfGenes , &numberOfConditions, &t_d );   
//             //TEMP.print();
//             bicount++;
//             geneCount++;
//         }
//         fclose(fptr);
// }

// MSBE RESULT cond and gene is parsed in this function
void getBiclustersFromFile5( leda::matrix &M , int inp, int fraction, int high, list<leda::matrix> &matrixList_l , list<list<GENES> > &results, char algName[], int modOrPure, int dimension1, int dimension2  ){ 
    M = M.trans();
    int numberOfGenes = 0;
    int numberOfConditions = 0;
    leda::string temp,number;
    char getGenes[ 10 ];
    int getCondId,t1;
    int getGeneId,t2;
    char getConds[ 10 ];
    list_item it,it2,it3;
    list<int> indexCond;
    list<int> indexGenes;
    double t_d;
    char nameFile[1024],temp_s[32],temp_s1[32],temp_s2[32],temp_s3[32],temp_s4[32],temp_s5[32],temp_s6[32],temp_s7[32],temp_s8[32],temp_s9[32],temp_s10[32];
    sprintf( nameFile, "%s%s%s", "sources/bicluster_sources/", algName, "Result.txt" );
    FILE *fptr = fopen( nameFile, "r" );

    //No.1 Gene num: 90 Cond num: 96 Ref Gene: 2950 Ref Cond: 110 Avg Sim: 1.2

    fscanf( fptr, "%s%s%s%d%s%s%d%s%s%d%s%s%d%s%s%lf", temp_s, temp_s1,temp_s2, &numberOfGenes , temp_s3,temp_s4, &numberOfConditions, temp_s5,temp_s6, &t1, temp_s7,temp_s8, &t2, temp_s9, temp_s10 , &t_d );   

	int bicount = 0;
        int geneCount = 1;
                GENES *allGenes = (GENES*)malloc( sizeof( GENES ) * dimension1 );
                FILE *yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" );
                               int fileCount = 0;
                leda::string genes_s;
                while( !feof( yFptr ) ){
                    fscanf( yFptr, "%s", allGenes[ fileCount ].GENE );
                    allGenes[ fileCount ].index = fileCount;
                    fileCount++;
                }
                fclose( yFptr );

                char gene[1024];
                char realGeneName[1024];
        while( !feof( fptr ) ){
            if( bicount % 101 == 100 )
                cout << " 100 biclusters passed\n";
            fscanf( fptr, "%d", &getGeneId );
            for( int i = 0; i < numberOfGenes; i++ ){   
                if( modOrPure == 1 ){
                    FILE *fptr2;
                    if( (fptr2= fopen( "sources/bicluster_sources/MSBE/modifiedIdConversionGene.txt", "r" ) ) != NULL ){
                        int count = 0;
                       
                        while( count != getGeneId ){
                            if( feof( fptr2 ) == true )
                                break;                           
                            fscanf( fptr2, "%d", &t1 );
                            count++;
                        }                       
                        fclose( fptr2 );
                        indexGenes.append( t1 );
                    }
                }       
                else{
                    indexGenes.append( getGeneId );
                }
                fscanf( fptr, "%d", &getGeneId );
                //cout << getGeneId;
            }

            fscanf( fptr, "%d", &getCondId );
            for( int i = 0; i < numberOfConditions; i++ ){               
                if( modOrPure == 1 ){
                    FILE *fptr2;
                    if( (fptr2= fopen( "sources/bicluster_sources/MSBE/modifiedIdConversionCond.txt", "r" ) ) != NULL ){
                        int count = 0;
                        while( count != getCondId ){
                            if( feof( fptr2 ) == true )
                                break;
                            fscanf( fptr2, "%d", &t2 );
                            count++;
                        }
                        fclose( fptr2 );
                        indexCond.append( t2 );
                    }
                }       
                else
                    indexCond.append( getCondId );
                fscanf( fptr, "%d", &getCondId );
            }
            temp.del_all( temp );
            number.del_all( number );
            list<leda::vector> V;

            //M.print();
            // For real data comment belov line
	    FILE *yFptr, *saveGene;
	    if( indexGenes.size() >= fraction && indexGenes.size() <= high ){
		    char geneFile[ 1024 ];
		    sprintf( geneFile, "%s%s%d%s", "sources/bicluster_sources/geneResult",algName, geneCount, ".txt" ); 
		    saveGene = fopen( geneFile , "w" );            
	    }
		matrix TEMP( indexGenes.size(), indexCond.size() );
		//****************************************************//
		//matrix TEMP( indexGenes.size() , indexCond.size() );
		//****************************************************//
		//cout << endl << M.dim1() << "      " <<  M.dim2() << " Vector : " << V.size() << endl;
		int count = 0,count2 = 0;
		/*	indexGenes.print();
			indexCond.print();*/

		forall_items( it, indexGenes ){ 
			forall_items( it2, indexCond ){
				/*if( indexCond[ it2 ] >= dimension1 )
				    break;
				if( indexGenes[ it ] >= dimension2 )
				    break;*/
				//cout << indexGenes[ it ] << " - " << indexCond[ it2 ] << "   " << count << " - " << count2 << endl;
// 				cout << endl <<count <<"-" << count2 << " - " << indexCond[ it2 ] << " - " << indexGenes[ it ]  <<endl;
				TEMP( count, count2 ) = M( indexGenes[ it ] , indexCond[ it2 ]);
				count2++;
			}
                //v.print();
				count++;
				count2 = 0;
		}

		list<GENES> geness;
		count = 0;	 
		forall_items( it2, indexGenes ){  
			if( indexGenes.size() >= fraction && indexGenes.size() <= high )
				fprintf( saveGene, "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );     
			//printf( "%s\n", allGenes[ indexGenes[ it2 ] ].GENE );  
			geness.append( allGenes[ indexGenes[ it2 ] ] );
			count++;
		}
// 		forall_items( it, geness){
// 			cout << geness[it].GENE << " ";
// 		}
		if( geness.size() >= fraction && geness.size() <= high ){
			fclose( saveGene );
			results.append( geness );
			matrixList_l.append( TEMP );
			geneCount++;
		}		
		indexCond.clear();
		indexGenes.clear();
		fscanf( fptr, "%s%s%s%d%s%s%d%s%s%d%s%s%d%s%s%lf", temp_s, temp_s1,temp_s2, &numberOfGenes , temp_s3,temp_s4, &numberOfConditions, temp_s5,temp_s6, &t1, temp_s7,temp_s8, &t2, temp_s9, temp_s10 , &t_d );   
//  		TEMP.print();
		bicount++;
// 		geneCount++;
	}
// 	forall_items( it2, results ){ 
// 		forall_items( it, results[ it2 ] ){
// 			cout << results[it2][it].GENE << " ";
// 		}
// 		cout << endl;
// 	}
	fclose(fptr);
	M = M.trans();
}
