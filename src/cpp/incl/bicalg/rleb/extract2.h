//************************************************************************************/
//*	Copyright (C) <2009>  <Melih Sozdinler,Emre Aladag,Cesim Erten>					 */
//*																					 */
//*    This program is free software: you can redistribute it and/or modify			 */
//*    it under the terms of the GNU General Public License as published by			 */
//*    the Free Software Foundation, either version 3 of the License, or			 */
//*    (at your option) any later version.											 */
//*																					 */
//*    This program is distributed in the hope that it will be useful,				 */
//*    but WITHOUT ANY WARRANTY; without even the implied warranty of				 */
//*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				 */
//*    GNU General Public License for more details.									 */
//*																					 */
//*    You should have received a copy of the GNU General Public License			 */
//*    along with this program.  If not, see <http://www.gnu.org/licenses/>.		 */
//*																					 */
//*    <microarray.cpp>Copyright (C)<2009><Melih Sozdinler,Emre Aladag,Cesim Erten>  */
//*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.    */
//*    This is free software, and you are welcome to redistribute it                 */
//*    under certain conditions; type `show c' for details.							 */
//************************************************************************************/


#include "declerations.h"

struct genes{
    char GENE[ 12 ];
    int index;
};

struct conds{
    char COND[ 12 ];
    int index;
};

typedef struct genes GENES;
typedef struct conds CONDS;

matrix getMatrixFromFile( char filename[ 10 ], int forOtherAlgorithms, array<GENES> &geneArray_Yeast,  array<CONDS> &condArray_Yeast ){
	cout << " MATRIX " << endl;
	FILE *fcptr;
	fcptr = fopen( filename, "r" );
	int dim1,dim2,count = 0,count2 = 0;
	double value;
	fscanf( fcptr, "%d%d", &dim1, &dim2 );
	char probeset[32];
	fscanf( fcptr, "%s", probeset );
	for( int i = 0; i < dim2; i++ ){
		fscanf( fcptr, "%s", condArray_Yeast[ i ].COND );
		condArray_Yeast[ i ].index = i+1;
	}
	fscanf( fcptr, "%s", geneArray_Yeast[ count2 ].GENE );
	geneArray_Yeast[ count2 ].index = count2+1;
	matrix TEMP( dim1, dim2 );
	while( feof( fcptr ) != true ){
		fscanf( fcptr, "%lf" , &value );
		TEMP( count2, count ) = value;
		count++;
		if( count == dim2 ){
			count2++;
			fscanf( fcptr, "%s", geneArray_Yeast[ count2 ].GENE );
			geneArray_Yeast[ count2 ].index = count2+1;
			count = 0;
		}
		if( count2 == dim1 )
			break;
	}
	return TEMP;
}

matrix getSubmatrixAtIndexs( matrix &INPUT, int x1, int x2, int y1, int y2 ){
	matrix result( x2 - x1, y2 - y1 );
	int count_i = 0, count_j = 0;
	for( int i = x1; i < x2; i++ ){
		for( int j = y1; j < y2; j++ ){
			result( count_i, count_j ) = INPUT( i, j ); 
			count_j++;
		}
		count_j = 0;
		count_i++;
	}
	return result;
}

void hvalueCalculator( list<four_tuple<int,int,double,matrix> > &matrices, char *filename ){
		double HAverage = 0;
		FILE *fptrResults;
		list_item it;
		matrix newI2;
		char fileToOpen[ 32 ];
		sprintf( fileToOpen, "%sresultsAll.txt", filename );
		if( ( fptrResults = fopen( fileToOpen , "a" ) ) != NULL ){
			sprintf( fileToOpen, "%sAverages.txt", filename );
			FILE *fptrOnlyHvalueAverages = fopen( fileToOpen , "a" );
			fprintf( fptrResults, "\n%s\n%d\t%d\n", "*********************", 
												matrices[matrices.first_item()].fourth().dim1(), 
												matrices[matrices.first_item()].fourth().dim2() 
			);
			int k = 1;
			forall_items( it , matrices ){
    			char filename2[1024] = "";
				int count_i = 0;
				newI2 = matrices[ it ].fourth();
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
					fprintf( fptrResults, "%10f\t%10f\t%10d\t%10d\n", rs_Value, residueScoreWhole, newI2.dim1(), newI2.dim2() );
					matrices[matrices.first_item()].third() = rs_Value;
					HAverage+= rs_Value;
					residueScoreWhole = 0;
			}
			fprintf( fptrOnlyHvalueAverages, "%d\t%d\t%f\n", matrices[matrices.first_item()].fourth().dim1(), 
												matrices[matrices.first_item()].fourth().dim2(),
												HAverage / (double) matrices.size()
			);
			fclose( fptrResults );
			fclose( fptrOnlyHvalueAverages );
		}
}


void hvvalueCalculator( list<four_tuple<int,int,double,matrix> > &matrices, char *filename ){
		double HAverage = 0;
		FILE *fptrResults;
		list_item it;
		matrix newI2;
		char fileToOpen[ 32 ];
		sprintf( fileToOpen, "%sresultsAll_hv.txt", filename );
		if( ( fptrResults = fopen( fileToOpen , "a" ) ) != NULL ){
			sprintf( fileToOpen, "%sAverages_hv.txt", filename );
			FILE *fptrOnlyHvalueAverages = fopen( fileToOpen , "a" );
			fprintf( fptrResults, "\n%s\n%d\t%d\n", "*********************", 
												matrices[matrices.first_item()].fourth().dim1(), 
												matrices[matrices.first_item()].fourth().dim2() 
			);
			int k = 1;
			forall_items( it , matrices ){
    			char filename2[1024] = "";
				int count_i = 0;
				newI2 = matrices[ it ].fourth();
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
					double residueScore2 = 0;
                    double residueScoreWhole = 0;
					double residueScoreWhole2 = 0;
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
                           residueScore += (double)newI2( bag1_int, bag2_int ) - rowMean - colMean + bagMean;
                           residueScore *= residueScore;
						   residueScore2 += (double)newI2( bag1_int, bag2_int ) - rowMean;
                           residueScore2 *= residueScore2;
                           residueScoreWhole += residueScore;
						   residueScoreWhole2 += residueScore2;
                           
                           residueScore = 0;
						   residueScore2 = 0;
                           colMean = 0;
                           rowMean = 0;
					    }
				    }
                    /*****************************************************************************/
					double rs_Value = residueScoreWhole / residueScoreWhole2;
                    fprintf( fptrResults, "%10f\t%10f\t%10d\t%10d\n", rs_Value, residueScoreWhole, newI2.dim1(), newI2.dim2() );
                    matrices[matrices.first_item()].third() = rs_Value;
                    HAverage+= rs_Value;
					residueScoreWhole = 0;
					residueScoreWhole2 = 0;
			}
			fprintf( fptrOnlyHvalueAverages, "%d\t%d\t%f\n", matrices[matrices.first_item()].fourth().dim1(), 
												matrices[matrices.first_item()].fourth().dim2(),
												HAverage / (double) matrices.size()
			);
			fclose( fptrResults );
			fclose( fptrOnlyHvalueAverages );
		}
}

#pragma region MAIN

int main( int argc, char *argv[] ){
	list_item it,it2;

	#pragma region MENU	
		panel P( 420,200,"Microarray Analysis");
		// Variables Experiment 1
		int maxSizeSubMatrix_exp1_g = 100, maxSizeSubMatrix_exp1_c = 20;
		int minSizeSubMatrix_exp1_g = 50, minSizeSubMatrix_exp1_c=10;
		int increment_exp1_g = 10, increment_exp1_c = 5;
		int repeat = 100,report=10;
		int data_dim1 = 0, data_dim2 = 0;
		bool exp1 = true, exp2 = true, min_hvalue = true;
		double hvaluemin = 100.0;
		for(;;)
		{
			P.load_button_font("lucidasans-12");
			P.text_item("\\bf\\blue Experimental Setup \\");
			P.text_item("");			
			P.int_item( "REPEAT", repeat);
			P.text_item("");
			P.int_item( "REPORTED BIC AT EACH REPEAT", report);
			P.text_item("");
			P.int_item( "Data dim1", data_dim1 );
			P.text_item("");
			P.int_item( "Data dim2", data_dim2 );
			P.text_item("");
			P.int_item( "Max sized submatrix in genes",maxSizeSubMatrix_exp1_g);
			P.text_item("");
			P.int_item( "Max sized submatrix in conds",maxSizeSubMatrix_exp1_c);
			P.text_item("");
			P.int_item( "Min sized submatrix in genes",minSizeSubMatrix_exp1_g);
			P.text_item("");
			P.int_item( "Min sized submatrix in conds",minSizeSubMatrix_exp1_c);
			P.text_item("");
			P.int_item( "Incrementing size 1",increment_exp1_g);
			P.text_item("");
			P.int_item( "Incrementing size 2",increment_exp1_c);
			//P.text_item("");
			//P.bool_item( "H-value Min(T/F)", min_hvalue );
			//P.text_item("");
			//P.double_item( "Minimize Value", hvaluemin );
			P.open(); 
			if ( P.button_press_time() > 0 ) break;
			P.display();
			P.clear();		
		}
	#pragma endregion MENU

	matrix BEFORE;								// pure dataset
	matrix AFTER;								// modified dataset
	array<GENES> geneArrayBefore_Yeast( data_dim1 + 1 ); // one unit larger than original
	array<GENES> geneArrayAFTER_Yeast( data_dim1 + 1 );	// one unit larger than original
	array<CONDS> condArrayBefore_Yeast( data_dim2 + 1 );	// one unit larger than original
	array<CONDS> condArrayAFTER_Yeast( data_dim2 + 1  );	// one unit larger than original

	#pragma region DATASET_RETRIEVAL												
		AFTER = getMatrixFromFile( "after.txt", 1, geneArrayAFTER_Yeast, condArrayAFTER_Yeast );
	#pragma endregion DATASET_RETRIEVAL		

	#pragma region EXTRACTION
		list<two_tuple<int,int> > randomSubmatrixIndexs;
		list<four_tuple<int,int,double,matrix> > results;
	    list<four_tuple<int,int,double,matrix> > output;

		for( int i = minSizeSubMatrix_exp1_g; i <= maxSizeSubMatrix_exp1_g; i+= increment_exp1_g ){
			for( int j = minSizeSubMatrix_exp1_c; j <= maxSizeSubMatrix_exp1_c; j+= increment_exp1_c ){
				two_tuple<int,int> tuples( i,j );
				randomSubmatrixIndexs.append( tuples );
			}
		}

		list<matrix> submatricesForProcessing_Modifed, submatricesForProcessing_Pure;
		matrix temp_m;
		forall_items( it, randomSubmatrixIndexs ){
			for( int count = 0; count < repeat; count++ ){
				random_source G( 0, data_dim1 - randomSubmatrixIndexs[ it ].first() - 1 );
				random_source C( 0, data_dim2 - randomSubmatrixIndexs[ it ].second() - 1 );
				int t_g, t_c;
				G >> t_g;
				C >> t_c;
				temp_m = getSubmatrixAtIndexs( AFTER, t_g, t_g + randomSubmatrixIndexs[ it ].first(),
													  t_c, t_c +  randomSubmatrixIndexs[ it ].second() );
				four_tuple<int,int,double,matrix> tuples( t_g, t_c, 0, temp_m ); 
				results.append( tuples );
			}
			hvalueCalculator( results, "Modified" );
			double hvalue_max = 0;
			forall_items( it2, results ){
				four_tuple<int,int,double,matrix> tuples = results[ it2 ];
				if( tuples.third() > hvalue_max ){
					hvalue_max = tuples.third(); 
				}
			}
			double hvalue_min = hvalue_max;
			for( int i = 0; i < report; i++ ){		
				list_item it_min;
				forall_items( it2, results ){
					four_tuple<int,int,double,matrix> tuples = results[ it2 ];
					if( hvalue_min > tuples.third() ){
						it_min = it2;
						hvalue_min = tuples.third();
					}
				}
				output.append( results[ it_min ] );
				results[ it_min ].third() = hvalue_max;
				hvalue_min = hvalue_max;
			}
			//hvvalueCalculator( results, "Modified" );
			results.clear();
			FILE *fptr;
			fptr = fopen( "RLEBResults_thaliana.txt" , "a" );
			
			forall_items( it2, output ){
				fprintf( fptr, "%d\t%d\n", output[ it2 ].fourth().dim1(), output[ it2 ].fourth().dim2() );
				for( int i = output[ it2 ].first(); i < output[ it2 ].first() + randomSubmatrixIndexs[ it ].first(); i++ ){
					//cout << i << "\t";
					fprintf( fptr, "%s\t", geneArrayAFTER_Yeast[ i ].GENE );
				}
				fprintf( fptr, "\n" );
				//cout << endl;
				for( int i = output[ it2 ].second(); i < output[ it2 ].second() + randomSubmatrixIndexs[ it ].second(); i++ ){
					//cout << i << "\t";
					fprintf( fptr, "%s\t", condArrayAFTER_Yeast[ i ].COND );
				}
				fprintf( fptr, "\n" );
				//cout << endl;
			}
			fclose( fptr );
			output.clear();
		}
	#pragma endregion EXTRACTION

	return 0;
}

#pragma endregion MAIN