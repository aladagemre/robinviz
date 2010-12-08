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
#include<fstream>


GRAPH<int,int> markovResultToGraph( int SIZE ){

	GRAPH<int,int> M;
	char matrixFile[1024] = "C://GraphUnion//markovResult.txt";
	node n;
	FILE *fptr;
	for( int i = 0; i < SIZE; i++ ){
		n = M.new_node();
	}
	array<node> index( SIZE );
	int count = 0;
	forall_nodes( n , M ){
		index[ count ] = n;
		count++;
	}

	double input = 0;
	char temp[ 1024 ];
	if ( fptr = fopen( matrixFile, "r" ) )
	{
		int dim1 = 0,dim2 = 0;
		while( ! feof( fptr) ){
			fscanf( fptr, "%lf", &input );
			sprintf( temp, "%lf", input );
			if( input > 0 ){
				if( input < 1 )
					cout << " " << input << " ";
				M.new_edge( index[ dim2 ] , index[ dim1 ], 1 );
			}
			if( dim1 == SIZE - 1 ){

				fscanf( fptr, "%f", &input );
				dim1 = 0;
				dim2++;
				//printf( "\n" );
			}
			if( dim2 == SIZE )
				break;

			dim1++;
		}
		fclose( fptr );
	}
	else{
		cout << " \n***	FILE COULD NOT BE OPENED ***\n";
	}

	return M;
} 