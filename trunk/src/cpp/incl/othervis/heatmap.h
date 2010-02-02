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

/**
* Function that takes a matrix of input or bicluster with respect to its
* genes and conditions. Also, it takes the folder we want to put the output.
* <array> version
* @param M(matrix); it is n by m matrix
* @param genes(array of GENES struct data type) the labels of dim1
* @param conds(array of CONDS struct data type) the labels of dim2
* @param filename(char *) output folder
**/
void drawHeatmap( matrix M, array<GENES> &genes, array<CONDS> &conds, char filename[ 64 ] ){
	FILE *fptr,*fptr2;
	char c;
	fptr = fopen( filename, "w" );
#ifdef LINUX
	fptr2 = fopen( "src/cpp/incl/othervis/start.txt", "r" );
#else
	fptr2 = fopen( "src//cpp//incl//othervis//start.txt", "r" );
#endif
	while( !feof( fptr2 ) ){
	      fscanf( fptr2, "%c", &c );
	      fprintf( fptr, "%c", c );
	}
	fclose( fptr2 );
// 	cout << "DONE\n";
	fprintf( fptr, "\n" );
	for( int i = 0; i < M.dim2(); i++ ){
	      fprintf( fptr, "data.addColumn('number', '%s');\n", conds[ i ].COND );
	}
	fprintf( fptr, "data.addRows(%d);\n", genes.size() );
// 	cout << "DONE\n";
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = -1; j < M.dim2(); j++ ){
			if( j == -1 ){
				fprintf( fptr, "\ndata.setCell(%d, %d, '%s');\n", i, j+1, genes[ i ].GENE );
			}
			else{
				fprintf( fptr, "data.setCell(%d, %d, %lf); ", i, j+1, M( i, j ) );
			}
		}
	}
#ifdef LINUX
	fptr2 = fopen( "src/cpp/incl/othervis/end.txt", "r" );
#else
	fptr2 = fopen( "src//cpp//incl//othervis//end.txt", "r" );
#endif
	while( !feof( fptr2 ) ){
	      fscanf( fptr2, "%c", &c );
	      fprintf( fptr, "%c", c );
	}
// 	cout << "DONE\n";
	fclose( fptr2 );
	fclose( fptr );
}


/**
* Function that takes a matrix of input or bicluster with respect to its
* genes and conditions. Also, it takes the folder we want to put the output.
* <list> version
* @param M(matrix); it is n by m matrix
* @param genes(list of GENES struct data type) the labels of dim1
* @param conds(list of CONDS struct data type) the labels of dim2
* @param filename(char *) output folder
**/
void drawHeatmap( matrix M, list<GENES> &genes, list<CONDS> &conds, char filename[ 64 ] ){
	FILE *fptr,*fptr2;
	char c;
	fptr = fopen( filename, "w" );
#ifdef LINUX
	fptr2 = fopen( "src/cpp/incl/othervis/start.txt", "r" );
#else
	fptr2 = fopen( "src//cpp//incl//othervis//start.txt", "r" );
#endif
	while( !feof( fptr2 ) ){
	      fscanf( fptr2, "%c", &c );
	      fprintf( fptr, "%c", c );
	}
	fclose( fptr2 );
// 	cout << "DONE\n";
	fprintf( fptr, "\n" );
	for( int i = 0; i < M.dim2(); i++ ){
// 	      cout << conds[ conds.get_item( i ) ].COND << endl;
	      fprintf( fptr, "data.addColumn('number', '%s');\n", conds[ conds.get_item( i )].COND );
	}
	fprintf( fptr, "data.addRows(%d);\n", genes.size() );
// 	cout << "DONE\n";
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = -1; j < M.dim2(); j++ ){
			if( j == -1 ){
				fprintf( fptr, "\ndata.setCell(%d, %d, '%s');\n", i, j+1, genes[ genes.get_item( i )].GENE );
			}
			else{
				fprintf( fptr, "data.setCell(%d, %d, %lf); ", i, j+1, M( i, j ) );
			}
		}
	}
// 	cout << "DONE\n";
#ifdef LINUX
	fptr2 = fopen( "src/cpp/incl/othervis/end.txt", "r" );
#else
	fptr2 = fopen( "src//cpp//incl//othervis//end.txt", "r" );
#endif
	while( !feof( fptr2 ) ){
	      fscanf( fptr2, "%c", &c );
	      fprintf( fptr, "%c", c );
	}
// 	cout << "DONE\n";
	fclose( fptr2 );
	fclose( fptr );
}

/**
* Function that takes a matrix of input or bicluster with respect to its
* genes and conditions. Also, it takes the folder we want to put the output.
* <list> version
* @param M(matrix); it is n by m matrix
* @param genes(list of GENES struct data type) the labels of dim1
* @param conds(list of CONDS struct data type) the labels of dim2
* @param filename(char *) output folder
**/
void produceHeatmapInput( matrix M, list<GENES> &genes, list<CONDS> &conds, char filename[ 64 ] ){
	FILE *fptr,*fptr2;
	char c;
	fptr = fopen( filename, "w" );
	
// 	cout << "DONE\n";
	fprintf( fptr, "%d\t%d\n", M.dim1(), M.dim2() );
	fprintf( fptr, "%s\t", "heatmap" );
	for( int i = 0; i < M.dim2(); i++ ){
		if( i != M.dim2() - 1 )
			fprintf( fptr, "%s\t", conds[ conds.get_item( i )].COND );
		if( i == M.dim2() - 1 )
			fprintf( fptr, "%s\n", conds[ conds.get_item( i )].COND );
	}
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = -1; j < M.dim2(); j++ ){
			if( j == -1 ){
				fprintf( fptr, "%s\t", genes[ genes.get_item( i )].GENE );
			}
			else{
				fprintf( fptr, "%lf\t",M( i, j ) );
			}
		}
		fprintf( fptr, "\n" );
	}
	fclose( fptr );
}