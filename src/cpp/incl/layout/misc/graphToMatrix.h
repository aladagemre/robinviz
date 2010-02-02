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


void convertor( GRAPH<int,int> &G, int row, int column ){
	
	int count = 0, weightCount;
	node n;
	edge e;
	forall_nodes( n, G )
		count++;
	leda::matrix M( count, count );
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			M( i, j ) = 0;
		}
	}
	list<edge> elist;
	list_item it;
	node_array<int> Index( G, 0 );

	count = 0;
	forall_nodes( n, G ){
		Index[ n ] = count;
		count++;
	}

	forall_nodes( n , G ){
		weightCount = 0;
		elist = G.out_edges( n );
		forall_items( it, elist ){
			e = elist[ it ];
			weightCount += G[ elist[it ] ];
		}
		forall_items( it, elist ){
			e = elist[ it ];
			//M( Index[ n ], Index[ G.target( e ) ] ) = (double)( G[ e ] ) / (double)( weightCount );
			M( Index[ n ], Index[ G.target( e ) ] ) = (double)( G[ e ] );
		}
	}
	//ofstream MarkovMatrix("C://GraphUnion//markov.txt", ios::in | ios::trunc);
	ofstream MarkovMatrix("C://GraphUnion//Bimax//bimax.txt", ios::in | ios::trunc);
	MarkovMatrix << M.dim1() << " " << M.dim2() << " " << row << " " << column;
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			MarkovMatrix << (int) M( i, j ) << " ";
		}
		MarkovMatrix << "\n";
	}
	MarkovMatrix.close();
}

matrix graphToMatrix(GRAPH<leda::string,int>& G, list<node> A, list<node> B, bool weighted ){
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

		array<double> colSum( m.dim1() );
		array<double> colStats( m.dim1() );
		for( i = 0; i < m.dim1(); i++ ){
				colSum[ i ] = 0;
				colStats[ i ] = 0;
		}

		for( i = 0; i < m.dim1(); i++ ){
				for( j = 0; j < m.dim2(); j++ ){
						colSum[ i ] += m( i, j );
						if( m( i, j ) > 0 )
							colStats[ i ]++;
				}
		}
		for( i = 0; i < m.dim1(); i++ ){
				for( j = 0; j < m.dim2(); j++ ){
					if( m( i, j ) > 0 ){
						m( i, j ) = m( i, j ) / colSum[ i ];
					}
				}
		}
		m.print();
		return m;
}

matrix graphToMatrix(GRAPH<int,int>& G,list<node> A,list<node> B, int rowComing, int columnComing , char filename[ 1024 ], bool weighted=true){
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
		char file[1024] = "";
		sprintf( file , "%s%s", "C://GraphUnion//", filename ); 
		ofstream MarkovMatrix( file , ios::in | ios::trunc);
		for( int i = 0; i < m.dim1(); i++ ){
			for( int j = 0; j < m.dim2(); j++ ){
				MarkovMatrix << (int) m( i, j ) << " ";
			}
			MarkovMatrix << "\n";
		}
		MarkovMatrix.close();
		return m;
}