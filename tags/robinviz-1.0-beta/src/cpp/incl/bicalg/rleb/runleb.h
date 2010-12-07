/*************************************************************************************/
/*	Copyright (C) <2008>  <Melih Sozdinler>											 */
/*																					 */
/*    This program is free software: you can redistribute it and/or modify			 */
/*    it under the terms of the GNU General Public License as published by			 */
/*    the Free Software Foundation, either version 3 of the License, or				 */
/*    (at your option) any later version.											 */
/*																					 */
/*    This program is distributed in the hope that it will be useful,				 */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of				 */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the					 */
/*    GNU General Public License for more details.									 */
/*																					 */
/*    You should have received a copy of the GNU General Public License				 */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.			 */
/*																					 */																							
/*    <biclustering.cpp>  Copyright (C) <2008>  <Melih Sozdinler>					 */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.     */
/*    This is free software, and you are welcome to redistribute it                  */
/*    under certain conditions; type `show c' for details.							 */
/**************************************************************************************/

#include "extract.h"
#define IS =
#define DRAW 0
#define debugANH 0

matrix graphToMatrix(GRAPH<int,int>& G,list<node> A,list<node> B, char filename[ 1024 ], bool weighted=true ){
		int i,j;
		double d = 0.0;
		int row=A.size(),col=B.size();
		matrix m(row,col);
		node_array<int> A1(G),B1(G);

		edge e;
		node n;
		for ( i=0 ; i<row ; i++ ){
				for ( j=0 ; j<col ; j++ ){
					m(i,j) = (double)0;
				}
		}
		i=0;
		forall(n,A){
				A1[n] = i;
				i++;
		}

		i=0;
		forall(n,B){
				B1[n] = i;
				i++;
		}

		node sn,tn;
		forall_edges(e,G){
				sn = G.source(e);
				tn = G.target(e);

				if ( weighted ){
					m(A1[sn],B1[tn]) = G[e];
				}else{
					m(A1[sn],B1[tn]) = m(A1[sn],B1[tn]) + (double)1.0;
				}
		}
// 		char file[1024] = "";
// 		sprintf( file , "%s", filename ); 
// 		ofstream MarkovMatrix( file , ios::in | ios::trunc);
// 		
// 		MarkovMatrix << m.dim2() << " " << m.dim1() << "\n";
// 		for( int i = 0; i < m.dim1(); i++ ){
// 			for( int j = 0; j < m.dim2(); j++ ){
// 				if( j != m.dim2() - 1 )
// 					MarkovMatrix << (int) m( i, j ) << "\t";
// 				else
// 					MarkovMatrix << (int) m( i, j );
// 			}
// 			MarkovMatrix << "\n";
// 		}
// 		MarkovMatrix.close();
		return m;
}

matrix graphToMatrix(GRAPH<int,int>& G,list<node> A,list<node> B, bool weighted=true){
		int i,j;
		double d = 0.0;
		int row=A.size(),col=B.size();
		matrix m(row,col);
		node_array<int> A1(G),B1(G);

		edge e;
		node n;
		for ( i=0 ; i<row ; i++ ){
				for ( j=0 ; j<col ; j++ ){
					m(i,j) = (double)0;
				}
		}
		i=0;
		forall(n,A){
				A1[n] = i;
				i++;
		}

		i=0;
		forall(n,B){
				B1[n] = i;
				i++;
		}

		node sn,tn;
		forall_edges(e,G){
				sn = G.source(e);
				tn = G.target(e);

				if ( weighted ){
					m(A1[sn],B1[tn]) = G[e];
				}else{
					m(A1[sn],B1[tn]) = m(A1[sn],B1[tn]) + (double)1.0;
				}
		}
		return m;
}

matrix graphToMatrix(GRAPH<int,int>& G,list<node> A,list<node> B, edge_array<double> Doubles, bool weighted=true){
		int i,j;
		double d = 0.0;
		int row=A.size(),col=B.size();
		matrix m(row,col);
		node_array<int> A1(G),B1(G);

		edge e;
		node n;
		for ( i=0 ; i<row ; i++ ){
				for ( j=0 ; j<col ; j++ ){
					m(i,j) = (double)0;
				}
		}
		i=0;
		forall(n,A){
				A1[n] = i;
				i++;
		}

		i=0;
		forall(n,B){
				B1[n] = i;
				i++;
		}

		node sn,tn;
		forall_edges(e,G){
				sn = G.source(e);
				tn = G.target(e);

				if ( weighted ){
					m(A1[sn],B1[tn]) = Doubles[e];
				}else{
					m(A1[sn],B1[tn]) = m(A1[sn],B1[tn]) + (double)1.0;
				}
		}
		return m;
}


void saveToFile( matrix M, GRAPH<int,int> &G, list<node> &A, list<node> &B, node_array<int> &genesConds, char filename[] ){

	FILE *fptr;
	fptr = fopen( filename, "w" );
	node n;

	M = graphToMatrix( G, A, B, true );
	M = M.trans();
	fprintf( fptr, "%d\t%d\n\t", A.size(), B.size() );
	forall( n, A ){
		fprintf( fptr, "%s%d\t", "cond", genesConds[ n ] );		
	}
	for( int i = 0; i < M.dim1(); i++ ){ 
		fprintf( fptr, "\n%s%d\t", "gene", genesConds[ B[ B.get_item( i ) ] ] );
		for( int j = 0; j < M.dim2(); j++ ){
			fprintf( fptr, "%d\t", (int)M( i, j ) );
		}
	}
	fclose( fptr );
}

void saveToFile( matrix M, array<int> &genes_i, array<int> &conditions_i, char filename[] ){

	FILE *fptr;
	fptr = fopen( filename, "w" );
	node n;

	fprintf( fptr, "%d\t%d\n%s\t", M.dim2(), M.dim1(), "probeset" );
	for( int i = 0; i < M.dim2(); i++ ){
		if( i != M.dim2() - 1 )
			fprintf( fptr, "%s%d\t", "cond", conditions_i[ i ] );
		else
			fprintf( fptr, "%s%d", "cond", conditions_i[ i ] );
	}
	for( int i = 0; i < M.dim1(); i++ ){ 
		fprintf( fptr, "\n%s%d\t", "gene", genes_i[ i ] );
		for( int j = 0; j < M.dim2(); j++ ){
			fprintf( fptr, "%lf\t", M( genes_i[ i ], conditions_i[ j ] ) );
		}
	}
	fclose( fptr );
}

void saveToFile( matrix M, GRAPH<int,int> &G, list<node> &A, list<node> &B, node_array<int> &genesConds, edge_array<double> &Doubles, char filename[] ){

	FILE *fptr;
// 	cout << filename << endl;
	fptr = fopen( filename, "w" );
	node n;

	M = graphToMatrix( G, A, B, Doubles, true );

	M = M.trans();
	fprintf( fptr, "%d\t%d\n\t", A.size(), B.size() );
	fprintf( fptr, "%s\t", "probeset" );
	forall( n, A ){
		fprintf( fptr, "%s%d\t", "cond", genesConds[ n ] );		
	}
	for( int i = 0; i < M.dim1(); i++ ){ 
		fprintf( fptr, "\n%s%d\t", "gene", genesConds[ B[ B.get_item( i ) ] ] );
		for( int j = 0; j < M.dim2(); j++ ){
			fprintf( fptr, "%f\t", M( i, j ) );
		}
	}
	fclose( fptr );
}

void matrixToGraph(GRAPH<int,int> &G, list<node> &A, list<node> &B, matrix m, edge_array<double> &realValues, bool weighted=true){


    int i,j;
    int k;
    int row=m.dim1(),col=m.dim2();
    array<node> A1(row),B1(col);
    edge e;
    node sn,tn;
    A.clear();
    B.clear();
    G.clear();

    for ( i=0 ; i<row ; i++ ){
        sn = G.new_node();
        A1[i] = sn;
        A.append(sn);
    }

    for ( j=0 ; j<col ; j++ ){
        tn = G.new_node();
        B1[j] = tn;
        B.append(tn);
    }

	list<double> storeAll;
    if ( weighted ){
        for ( i=0 ; i<row ; i++ ){
            for ( j=0 ; j<col ; j++ ){
                if ( (int)m(i,j) != 0 ){
                    e = G.new_edge(A1[i],B1[j],(int)m(i,j));
                    G[e] = (int)m(i,j);
					storeAll.push_back( m(i,j) );
				}
            }
        }
    }else{
        for ( i=0 ; i<row ; i++ ){
            for ( j=0 ; j<col ; j++ ){
                for ( k=0 ; k<(int)m(i,j) ; k++ ){
                	e = G.new_edge(A1[i],B1[j]);
                	G[e] = 0;
                }
            }
        }
    }
	//m.print();
	realValues.init( G );
	forall_edges( e, G ){
		realValues[ e ] = storeAll.pop_front();
		//cout << realValues[ e ] << " - ";
	}
}

void rlebmain( matrix &INPUT, int maxSizeSubMatrix_exp1_g, int maxSizeSubMatrix_exp1_c, int minSizeSubMatrix_exp1_g, int minSizeSubMatrix_exp1_c, int repeat, double hvaluemin, int increment_exp1_g, int increment_exp1_c   ){

	int DEBUG = 1;
	GRAPH<int,int> G;
	list<node> A;
	list<node> B;

	node x,y;
	node n;
	edge e;

	matrix M;
	edge_array<double> realValues;

	matrixToGraph( G, A, B, INPUT.trans(), realValues, true );
	forall_edges( e, G ){
#ifdef LINUX
		if( G[ e ] < 0 )
			G[ e ] = log(abs( G[ e ] * 10000 ));
#else
		if( G[ e ] < 0 )
			G[ e ] = leda::log(abs( G[ e ] * 10000 ));
#endif
	}
 	M = graphToMatrix( G, A, B, "input.txt", true );
	
	cout << "/**************************************************/" << endl;
	cout << "         RLEB Algorithm Begins" << endl;
	cout << "/**************************************************/" << endl;
		
	
	array<int> genes_i( A.size() + 1);
	array<int> conditions_i( B.size() + 1 );
	node_array<int> genesConds( G, 0 );

	/***************************************/
	/*    Inserting Artificial Gene data   */
	/***************************************/

	int ncount_i = 0;
	forall( n,A ){
		genes_i[ ncount_i ] = ncount_i;
		genesConds[ n ] = ncount_i; 
		ncount_i++;
	}
	
	/***************************************/
	/* Inserting Artificia Conditions data */
	/***************************************/

	ncount_i = 0;
	forall( n,B ){
		conditions_i[ ncount_i ] = ncount_i;
		genesConds[ n ] = ncount_i;
		ncount_i++;
	}

	edge_array<int> control( G, 0);
	
	for( int a = 0; a != 10 ; a++ ){
		cout << endl << " Turn " << a + 1 << endl;
		if( debugANH )
			cout << "        * NOW CM begins  *\n";
		
		wolf( G , A , B );

		if( debugANH )
			cout << "****   CM is done over A, B      *****\n";
		G.rev_all_edges();

		wolf( G , B , A );

		if( debugANH ){
			cout << "****   CM is done over B, A      *****\n";
			cout << "        * NOW CM ends    *\n";
		}
		G.rev_all_edges();
	}	
#ifdef LINUX
	saveToFile( M, G, A, B, genesConds, realValues ,"outputs/localization/localized_input.txt" );
#else
	saveToFile( M, G, A, B, genesConds, realValues ,"outputs//localization//localized_input.txt" );
#endif

	int data_dim1 = M.dim2(), data_dim2 = M.dim1();
	runExtraction( repeat, data_dim1, data_dim2, maxSizeSubMatrix_exp1_g, maxSizeSubMatrix_exp1_c, minSizeSubMatrix_exp1_g, minSizeSubMatrix_exp1_c, increment_exp1_g, increment_exp1_c, hvaluemin );
}

void rlebmain_m( matrix &INPUT, int maxSizeSubMatrix_exp1_g, int maxSizeSubMatrix_exp1_c, int minSizeSubMatrix_exp1_g, int minSizeSubMatrix_exp1_c, int repeat, double hvaluemin, int increment_exp1_g, int increment_exp1_c   ){

	int DEBUG = 1;
	matrix M = INPUT;
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
#ifdef LINUX
			if( M(i,j) < 0 )
				M(i,j) = log(abs( M(i,j) * 10000 ));
#else
			if( M(i,j) < 0 )
				M(i,j) = leda::log(abs( M(i,j) * 10000 ));
#endif
		}
	}
	
	cout << "/**************************************************/" << endl;
        cout << "         REAL Algorithm Begins" << endl;
	cout << "/**************************************************/" << endl;
		
	
	array<int> genes_i( M.dim1() + 1 );
	array<int> conditions_i( M.dim2() + 1 );

	/***************************************/
	/*    Inserting Artificial Gene data   */
	/***************************************/

	for( int i = 0; i < M.dim1(); i++ ){
		genes_i[ i ] = i;
	}
	
	/***************************************/
	/* Inserting Artificia Conditions data */
	/***************************************/

	for( int j = 0; j < M.dim2(); j++ ){
		conditions_i[ j ] = j;
	}

	for( int a = 0; a != 10 ; a++ ){
		cout << endl << " Turn " << a + 1 << endl;
		if( debugANH )
			cout << "        * NOW CM begins  *\n";
		
		M = wolf_m( M, genes_i );

		if( debugANH )
			cout << "****   CM is done over A, B      *****\n";
		M = M.trans();
		M = wolf_m( M, conditions_i );

		if( debugANH ){
			cout << "****   CM is done over B, A      *****\n";
			cout << "        * NOW CM ends    *\n";
		}
		M = M.trans();
	}	
#ifdef LINUX
	saveToFile( INPUT, genes_i, conditions_i, "outputs/localization/localized_input.txt" );
#else
	saveToFile( INPUT, genes_i, conditions_i, "outputs//localization//localized_input.txt" );
#endif

	int data_dim1 = M.dim2(), data_dim2 = M.dim1();
	runExtraction( repeat, data_dim1, data_dim2, maxSizeSubMatrix_exp1_g, maxSizeSubMatrix_exp1_c, minSizeSubMatrix_exp1_g, minSizeSubMatrix_exp1_c, increment_exp1_g, increment_exp1_c, hvaluemin );
}
