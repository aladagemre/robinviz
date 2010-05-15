// #include "declerations.h"

matrix dataRead( char filename[256] ){
	leda::matrix INPUT;
	int dimension1, dimension2;
	char filename2[256];
	sprintf( filename2, "%s%s","", filename );
	INPUT = getMatrixFromFile( filename2, 1, dimension1, dimension2 ); 
	return INPUT;
}

matrix dataRead( char filename[256], array<GENES> &geneArray,  array<CONDS> &condArray  ){
	leda::matrix INPUT;
	int dimension1, dimension2;
	char filename2[256];
	sprintf( filename2, "%s%s","", filename );
	INPUT = getMatrixFromFile( filename2, dimension1, dimension2, geneArray, condArray ); 
	return INPUT;
}

void dataWriteBimax( matrix &M, int low_dim1, int low_dim2, double ther ){
	FILE *fptr;
#ifdef LINUX
	fptr = fopen( "src/cpp/incl/bicalg/bimax/matrix_robin.txt", "w" );
#else
	fptr = fopen( "src//cpp//incl//bicalg//bimax//matrix_robin.txt", "w" );
#endif
	fprintf( fptr, "%d %d %d %d\n", M.dim1(), M.dim2(), low_dim1, low_dim2 );
	double max = M( 0, 0 );
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( M( i, j ) > max )
			    max = M( i, j );
		}
	}
// 	cout << max << endl;
	double min = max;
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( M( i, j ) < min )
			    min = M( i, j );
		}
	}
// 	cout << min << endl;
	double difference = max - min;
// 	cout << difference << endl;
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( M( i, j ) < max - difference * ther ){
				fprintf( fptr, " %d", 0 );
			}
			else{
				fprintf( fptr, " %d", 1 );
			}
		}
		fprintf( fptr, "\n" );
	}
	fclose( fptr );
}


void dataWriteCC( matrix &M ){
	FILE *fptr;
#ifdef LINUX
	fptr = fopen( "src/cpp/incl/bicalg/cc/matrix_robin.txt", "w" );
#else
	fptr = fopen( "src//cpp//incl//bicalg//cc//matrix_robin.txt", "w" );
#endif
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( j != M.dim2() -1 )
				fprintf( fptr, "%d ", (int)(M(i,j)*10) );
			else
				fprintf( fptr, "%d", (int)(M(i,j)*10) );
		}
		fprintf( fptr, "\n" );
	}
	fclose( fptr );
}