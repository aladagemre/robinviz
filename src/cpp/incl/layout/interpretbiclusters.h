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

interpretBiclusters.h is for the handlers.h. The main contribuiton of this
header is to functionate the main program. You can just call handlers for
each iterative modules.

Created

08 Aralık 2009, by Melih

Modified


*************************************************************************************/


#include "layeredlayout.h"

#define ESIT ==

struct genes4{
    leda::string gene;
    char GENE[ 12 ];
    int index;
};

struct GeneNames{
	char GENE[16];
};

typedef GeneNames GENENAMES;

typedef struct genes4 GENES2;

typedef struct Strings STRINGS;


struct genes2{
    char gene[ 14 ];
    char geneType[ 1 ];
};

typedef struct genes2 GNDATA;
GNDATA g[6241];


// FOR CC,OPSM,XMOTIF and OPSM cond is gene in this function
list<list<GENENAMES> > writeGenesConds2(  char algName[] ){
	list<list<GENENAMES> > geneList;
	int numberOfGenes = 0;
	int numberOfConditions = 0;
	list<leda::string> listOfFile,listOfFile2;
	leda::string temp,number;
	char getGenes[ 10 ];
	char getConds[ 10 ];
	list_item it,it2,it3;
	list<GENENAMES> indexGenes;
    char algFile[1024];
    sprintf( algFile, "%s%s", algName , "Result.txt" );
	FILE *fptr = fopen( algFile , "r" );
			fscanf( fptr, "%d%d", &numberOfGenes , &numberOfConditions);	
		int bicount = 0;

        int geneCount = 1;

                GENES2 allGenes[ 2994 ];
                FILE *yeastFptr = fopen( "GeneNamesYeast.txt", "r" );
                int fileCount = 0;
                leda::string genes_s;
                while( !feof( yeastFptr ) ){
                    fscanf( yeastFptr, "%s", allGenes[ fileCount ].GENE );
                    allGenes[ fileCount ].index = fileCount;
                    fileCount++;
                }
                fclose( yeastFptr );
		
		cout << endl << algName << "'s bicluster genes are processing" << endl;
		while( !feof( fptr ) ){
			if( bicount % 501 == 499 )
				cout << " 500 biclusters passed\n";
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
					GENENAMES tempx;
					sprintf( tempx.GENE, "%s", allGenes[ geneNumber ].GENE );
					indexGenes.append( tempx );
			}
			geneList.append( indexGenes );
			indexGenes.clear();
			for( int i = 0; i < numberOfConditions; i++ ){
				fscanf( fptr, "%s", getConds );
			}
			temp.del_all( temp );
			number.del_all( number );
			listOfFile.clear();

			fscanf( fptr, "%d%d", &numberOfGenes , &numberOfConditions);	
			//TEMP.print();
			bicount++;
            geneCount++;
		}
		fclose(fptr);
        return geneList;
}
// Bivusu Greedy Pcluster cond is gene in this function
list<list<GENENAMES> > writeGenesConds4( char algName[] ){
	list<list<GENENAMES> > geneList;
	
	int numberOfGenes = 0;
	int numberOfConditions = 0;
	list<leda::string> listOfFile,listOfFile2;
	leda::string temp,number;
	char getGenes[ 10 ];
	int getCondId;
	int getGeneId;
	char getConds[ 10 ];
	list_item it,it2,it3;
	list<int> indexCond;
	list<GENENAMES> indexGenes;
	double t_d;
    char algFile[1024];
    sprintf( algFile, "%s%s", algName , "Result.txt" );
	FILE *fptr = fopen( algFile , "r" );
		fscanf( fptr, "%s%d%d%lf", getGenes, &numberOfGenes , &numberOfConditions, &t_d );	
		int bicount = 0;

        int geneCount = 1;

                GENES2 allGenes[ 2994 ];
                FILE *yeastFptr = fopen( "GeneNamesYeast.txt", "r" );
                int fileCount = 0;
                leda::string genes_s;
                while( !feof( yeastFptr ) ){
                    fscanf( yeastFptr, "%s", allGenes[ fileCount ].GENE );
                    allGenes[ fileCount ].index = fileCount;
                    fileCount++;
                }
                fclose( yeastFptr );

		while( !feof( fptr ) ){
			
			fscanf( fptr, "%s", getConds );
			for( int i = 0; i < numberOfConditions; i++ ){
				fscanf( fptr, "%d", &getCondId );
				sprintf( getConds, "%s%d", "cond", getCondId );
				listOfFile.append( getConds );
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
					int condNumber = atoi( temp2 );
					indexCond.append( condNumber );
			}
			temp.del_all( temp );
			number.del_all( number );
			listOfFile.clear();

			fscanf( fptr, "%s", getGenes );
			for( int i = 0; i < numberOfGenes; i++ ){
				fscanf( fptr, "%d", &getGeneId );
				sprintf( getGenes, "%s%d", "cond", getGeneId );
				listOfFile.append( getGenes );
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
					int geneNumber = atoi( temp2 );
					GENENAMES tempx;
					sprintf( tempx.GENE, "%s", allGenes[ geneNumber ].GENE );
					indexGenes.append( tempx );
			}
			
			for( int i = 0; i < numberOfConditions; i++ )
				fscanf( fptr, "%s", getConds);
			for( int i = 0; i < numberOfGenes; i++ )
				fscanf( fptr, "%s", getGenes );

			temp.del_all( temp );
			number.del_all( number );
			listOfFile.clear();

			geneList.append( indexGenes );
			
			indexCond.clear();
			indexGenes.clear();
			fscanf( fptr, "%s%d%d%lf", getGenes, &numberOfGenes , &numberOfConditions, &t_d );	
			//TEMP.print();
			bicount++;
            geneCount++;
		}
		fclose(fptr);
        return geneList;
}
// MSBE RESULT cond and gene is parsed in this function
list<list<GENENAMES> > writeGenesConds5( char algName[], int modOrPure ){
	list<list<GENENAMES> > geneList;
			
	int numberOfGenes = 0;
	int numberOfConditions = 0;
	leda::string temp,number;
	char getGenes[ 10 ];
	int getCondId,t1;
	int getGeneId,t2;
	char getConds[ 10 ];
	list_item it,it2,it3;
	list<int> indexCond;
	list<GENENAMES> indexGenes;
	double t_d;
    char algFile[1024],temp_s[32],temp_s1[32],temp_s2[32],temp_s3[32],temp_s4[32],temp_s5[32],temp_s6[32],temp_s7[32],temp_s8[32],temp_s9[32],temp_s10[32];
    sprintf( algFile, "%s%s", algName , "Result.txt" );
	FILE *fptr = fopen( algFile , "r" );
	//No.1 Gene num: 90 Cond num: 96 Ref Gene: 2950 Ref Cond: 110 Avg Sim: 1.2

	fscanf( fptr, "%s%s%s%d%s%s%d%s%s%d%s%s%d%s%s%lf", temp_s, temp_s1,temp_s2, &numberOfGenes , temp_s3,temp_s4, &numberOfConditions, temp_s5,temp_s6, &t1, temp_s7,temp_s8, &t2, temp_s9, temp_s10 , &t_d );	
		int bicount = 0;

        int geneCount = 1;

                GENES2 allGenes[ 2994 ];
                FILE *yeastFptr = fopen( "GeneNamesYeast.txt", "r" );
                int fileCount = 0;
                leda::string genes_s;
                while( !feof( yeastFptr ) ){
                    fscanf( yeastFptr, "%s", allGenes[ fileCount ].GENE );
                    allGenes[ fileCount ].index = fileCount;
                    fileCount++;
                }
                fclose( yeastFptr );


		cout << endl << algName << "'s bicluster genes are processing" << endl;
		while( !feof( fptr ) ){
			//cout << bicount << " - ";
			if( bicount % 501 == 499 )
				cout << " 500 biclusters passed\n";
			GENENAMES tempx;
			fscanf( fptr, "%d", &getGeneId );
			for( int i = 0; i < numberOfGenes; i++ ){	
				if( modOrPure == 1 ){
					FILE *fptr2;
					if( (fptr2= fopen( "modifiedIdConversionGene.txt", "r" ) ) != NULL ){
						int count = 0;
						
						while( count != getGeneId ){
							if( feof( fptr2 ) == true )
								break;							
							fscanf( fptr2, "%d", &t1 );
							count++;
						}						
						fclose( fptr2 );
						sprintf( tempx.GENE, "%s", allGenes[ t1 ].GENE );
						indexGenes.append( tempx );
					}
				}		
				else{
					sprintf( tempx.GENE, "%s", allGenes[ getGeneId - 1 ].GENE );
					indexGenes.append( tempx );
				}
				fscanf( fptr, "%d", &getGeneId );
				//cout << getGeneId;
			}
			
			geneList.append( indexGenes );
			indexGenes.clear();
			fscanf( fptr, "%d", &getCondId );
			for( int i = 0; i < numberOfConditions; i++ ){				
				if( modOrPure == 1 ){
					FILE *fptr2;
					if( (fptr2= fopen( "modifiedIdConversionCond.txt", "r" ) ) != NULL ){
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
			indexCond.clear();
			//cout << endl;
			//forall_items( it, indexGenes )
			//	cout << indexGenes[ it ].GENE << " - ";
			//cout << endl;
			if( feof( fptr ) == true )
				break;
			fscanf( fptr, "%s%s%s%d%s%s%d%s%s%d%s%s%d%s%s%lf", temp_s, temp_s1,temp_s2, &numberOfGenes , temp_s3,temp_s4, &numberOfConditions, temp_s5,temp_s6, &t1, temp_s7,temp_s8, &t2, temp_s9, temp_s10 , &t_d );
			//TEMP.print();
			bicount++;
            geneCount++;
		}
		fclose(fptr);
        return 	geneList;
}

// FOR LAS
list<list<GENENAMES> > writeGenesConds6( char algName[] ){
	list<list<GENENAMES> > geneList;

	int numberOfGenes = 0;
	int numberOfConditions = 0;
	list<leda::string> listOfFile,listOfFile2;
	leda::string temp,number;
	char getGenes[ 10 ];
	char getConds[ 10 ];
	list_item it,it2,it3;
	list<int> indexCond;
	list<GENENAMES> indexGenes;
    char algFile[1024];
    sprintf( algFile, "LAS//%s%s", algName , "ResultRows.txt" );
	FILE *fptr = fopen( algFile , "r" );
	sprintf( algFile, "LAS//%s%s", algName , "ResultLabels.txt" );
	FILE *fptr2 = fopen( algFile , "r" );
	sprintf( algFile, "LAS//%s%s", algName , "ResultColumns.txt" );
	FILE *fptr3 = fopen( algFile , "r" );
	//One Instance In Labels
	//Red_1	bicluster of	676	x	113	size with average	0,821484102664456	and score	24076,9817123579
	char t1[16],t2[16],t3[16],t4[16],t5[16],t6[16],t7[16],t8[16],t9[16],t_c[8];
	char td1[32], td2[32];
	fscanf( fptr2, "%s%s%s%d%s%d%s%s%s%s%s%s%s", t1, t2, t3, &numberOfGenes , t_c, &numberOfConditions, t4, t5, t6, td1, t8, t9, td2 );	
		int bicount = 0;

        int geneCount = 1;

                GENES2 allGenes[ 2994 ];
                FILE *yeastFptr = fopen( "GeneNamesYeast.txt", "r" );
                int fileCount = 0;
                leda::string genes_s;
                while( !feof( yeastFptr ) ){
                    fscanf( yeastFptr, "%s", allGenes[ fileCount ].GENE );
                    allGenes[ fileCount ].index = fileCount;
                    fileCount++;
                }
                fclose( yeastFptr );

		while( !feof( fptr2 ) ){
			
            
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
					GENENAMES tempx;
					sprintf( tempx.GENE, "%s", allGenes[ geneNumber ].GENE );
					indexGenes.append( tempx );
			}


			temp.del_all( temp );

			number.del_all( number );
			listOfFile.clear();


            for( int i = 0; i < numberOfConditions; i++ ){
				fscanf( fptr3, "%s", getConds );
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
			geneList.append( indexGenes );
			indexCond.clear();
			indexGenes.clear();
			fscanf( fptr2, "%s%s%s%d%s%d%s%s%s%s%s%s%s", t1, t2, t3, &numberOfGenes , t_c, &numberOfConditions, t4, t5, t6, td1, t8, t9, td2 );	
		
			//TEMP.print();
			bicount++;
            geneCount++;
		}
		fclose(fptr);
		fclose(fptr2);
		fclose(fptr3);
        return 	geneList;
}