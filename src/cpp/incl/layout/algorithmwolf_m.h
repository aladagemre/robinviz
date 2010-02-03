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
/*    Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.         */
/*    This is free software, and you are welcome to redistribute it                      */
/*    under certain conditions; type `show c' for details.			         */
/*****************************************************************************************/

int column_id;

class cmp_rows_in_matrix: public leda_cmp_base<int> { 
  const matrix& M;
public:
  cmp_rows_in_matrix(const matrix &m): M(m){}

  int operator()(const int &u, const int& v) const
  { 
    double uSum_b = 0, uSum_a = 0, vSum_b = 0, vSum_a = 0;
	for( int i = 0; i <= column_id; i++ ){
		uSum_b += M(u,i);
		vSum_b += M(v,i);
	}
	for( int i = column_id; i < M.dim2(); i++ ){
		uSum_a += M(u,i);
		vSum_a += M(v,i);
	}
	return compare( vSum_b * uSum_a, vSum_a * uSum_b );
    /*if( vSum_b * uSum_a <= vSum_a * uSum_b )
		return -1;
	if( vSum_b * uSum_a > vSum_a * uSum_b )
		return 1;*/
  } 
};


matrix wolf_m( matrix &M );
matrix wolf_m( matrix &M, array<int> &labelsIndex );

matrix wolf_m( matrix &M ){
	list_item it;
	int r,c,i;
	double leftsum = 0;
	double rightsum = 0;
	array<list<int> > partitions( M.dim2() );

	for( r = 0; r < M.dim1(); r++ ){
		leftsum = 0; rightsum = 0;
		for( c = 1; c < M.dim2(); c++ ){
			rightsum+=M(r,c);
		}
		for( c = 0; c < M.dim2(); c++ ){
			if( leftsum >= rightsum ){
				break;
			}
			leftsum+=M(r,c); rightsum-=M(r,c+1);
		}
		partitions[ c ].append( r );
	}
	cmp_rows_in_matrix cmp(M);
	for( c = 0; c < M.dim2(); c++ ){
		if( partitions[ c ].size() > 0 ){
			column_id = c;
			partitions[ c ].merge_sort( cmp );
		}
	}
	matrix M2 = M;
	i = 0;
	for( c = 0; c < M.dim2(); c++ ){
		forall_items( it, partitions[ c ] ){
			M2.row( i ) = M.row( partitions[ c ][ it ] );
			i++;
		}
	}
	return M;
}

matrix wolf_m( matrix &M, array<int> &labelsIndex ){
	list_item it;
	int r,c,i;
	double leftsum = 0;
	double rightsum = 0;
	array<list<int> > partitions( M.dim2() );

	for( r = 0; r < M.dim1(); r++ ){
		leftsum = 0; rightsum = 0;
		for( c = 1; c < M.dim2(); c++ ){
			rightsum+=M(r,c);
		}
		for( c = 0; c < M.dim2(); c++ ){
			if( leftsum >= rightsum ){
				break;
			}
			leftsum+=M(r,c); rightsum-=M(r,c+1);
		}
		partitions[ c ].append( r );
	}
	cmp_rows_in_matrix cmp(M);
	for( c = 0; c < M.dim2(); c++ ){
		if( partitions[ c ].size() > 0 ){
			column_id = c;
			partitions[ c ].merge_sort( cmp );
		}
	}
	matrix M2 = M;
	i = 0;
	array<int> labelsIndex_t( labelsIndex.size() );
	for( c = 0; c < M.dim2(); c++ ){
		forall_items( it, partitions[ c ] ){
			M2.row( i ) = M.row( partitions[ c ][ it ] );
			labelsIndex_t[ i ] = labelsIndex[ partitions[ c ][ it ] ];
			i++;
		}
	}
	labelsIndex = labelsIndex_t;
	return M;
}