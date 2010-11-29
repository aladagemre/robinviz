

#include "layout/misc/declerations.h"

#define ERROR 1

// REAL max H-value should not be larger than 1000 times bigger than the mean of an input matrix
void minHvalueErrorChecker( matrix &INPUT, double hvaluemin ){
	double sum = 0;
	for( int i = 0; i < INPUT.dim1(); i++ ){
                for( int j = 0; j < INPUT.dim2(); j++ ){
                    if( INPUT( i,j ) < 0 )
                        sum += (double)(INPUT( i,j )*-1.0);
                    else
                        sum += (double)(INPUT( i,j ));
		}
	}
        sum /= (double)( INPUT.dim1() * INPUT.dim2() );
        if( sum * 1000.0 < hvaluemin ){
		FILE *erptr;
#ifdef LINUX
		erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
                fprintf( erptr, "Error 201: Error in minHvalueErrorChecker of ehandlers.h: You are runnning REAL with large max Hvalue parameter, data mean is %lf\n", sum);
		fclose( erptr );
		cout << "\nError 201: Error in minHvalueErrorChecker of ehandlers.h: You are runnning REAL with large max Hvalue parameter\n";
		exit(1);
	}
}

void mainOntologyFlagCheckerEXP( void ){
            FILE *erptr;
#ifdef LINUX
            erptr = fopen( "outputs/error.txt", "w" );
#else
            erptr = fopen( "outputs//error.txt", "w" );
#endif
            fprintf( erptr, "Error 101: Error in Main function of co_expression.cpp: You did not specify ontology flag\n" );
            fclose( erptr );
            cout << "\nError 101: Error in Main function of co_expression.cpp: You did not specify ontology flag\n";
            exit(1);
}

void mainOntologyFlagCheckerONT( void ){
            FILE *erptr;
#ifdef LINUX
            erptr = fopen( "outputs/error.txt", "w" );
#else
            erptr = fopen( "outputs//error.txt", "w" );
#endif
            fprintf( erptr, "Error 101: Error in Main function of co_ontologies.cpp: You did not specify ontology flag\n" );
            fclose( erptr );
            cout << "\nError 101: Error in Main function of co_ontologies.cpp: You did not specify ontology flag\n";
            exit(1);
}

void settingsYamlChecker( void ){
            FILE *erptr;
#ifdef LINUX
            erptr = fopen( "outputs/error.txt", "w" );
#else
            erptr = fopen( "outputs//error.txt", "w" );
#endif
            fprintf( erptr, "Error 102: Error in Main function of your chosen setting: The settings.yaml file is missing, make sure you run Robinviz from GUI\n" );
            fclose( erptr );
            cout << "\nError 102: Error in Main function of your chosen setting: The settings.yaml file is missing, make sure you run Robinviz from GUI\n";
            exit(1);
}

void geneFunctionFileChecker( void ){
	FILE *erptr;
#ifdef LINUX
	erptr = fopen( "outputs/error.txt", "w" );
#else
	erptr = fopen( "outputs//error.txt", "w" );
#endif
	fprintf( erptr, "Error 103: Error in Main function of your chosen setting: Your genefunctions.txt is missing\n" );
	fclose( erptr );
	cout << "\nError 103: Error in Main function of your chosen setting: Your genefunctions.txt is missing\n";
	exit(1);
}

void functionsFileChecker( void ){
	FILE *erptr;
#ifdef LINUX
	erptr = fopen( "outputs/error.txt", "w" );
#else
	erptr = fopen( "outputs//error.txt", "w" );
#endif
	fprintf( erptr, "Error 104: Error in Main function of your chosen setting: Your functions.txt is missing\n" );
	fclose( erptr );
	cout << "\nError 104: Error in Main function of your chosen setting: Your functions.txt is missing\n";
	exit(1);
}

void categoryFileChecker( void ){
	FILE *erptr;
#ifdef LINUX
	erptr = fopen( "outputs/error.txt", "w" );
#else
	erptr = fopen( "outputs//error.txt", "w" );
#endif
	fprintf( erptr, "Error 105: Error in Main function of your chosen setting: Check that your color handling in that run is working properly\n" );
	fclose( erptr );
	cout << "\nError 105: Error in Main function of your chosen setting: Check that your color handling in that run is working properly\n";
	exit(1);
}

void dimensionChecker( char algName[64] ){
	FILE *erptr;
#ifdef LINUX
	erptr = fopen( "outputs/error.txt", "w" );
#else
	erptr = fopen( "outputs//error.txt", "w" );
#endif
	fprintf( erptr, "Error 106: Check the desired dimensions of %s algorithm\n", algName );
	fclose( erptr );
	cout << "\nError 106: Check the desired dimensions of" << algName <<  " algorithm\n";
	exit(1);
}

