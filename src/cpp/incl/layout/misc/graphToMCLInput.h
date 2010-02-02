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

void convertor( GRAPH<int,int> &G ){
	
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
			M( Index[ n ], Index[ G.target( e ) ] ) = (double)( G[ e ] ) / (double)( weightCount );
		}
	}
	ofstream MarkovMatrix("C://GraphUnion//markov.txt", ios::in | ios::trunc);
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			MarkovMatrix << M( i, j ) << " ";
		}
		MarkovMatrix << "\n";
	}
	MarkovMatrix.close();
}