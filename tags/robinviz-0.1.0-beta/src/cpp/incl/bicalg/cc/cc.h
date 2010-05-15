
/* --------------------------------------------
 * --- File     : main.c                    ---
 * --- Author   : Ahmet ARDAL               ---
 * --- Contact  : ardalahmet@hotmail.com    ---
 * --------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "Biclustering_.h"

bool ccMain( char *inFilePath, int nRowsSkip, int nColsSkip, int bic_num, int par1, int par2, int par3 ){
    int  i;
    FILE *fptr;
    if( (fptr = fopen( "outputs/biclusters/CCResult.txt", "w" ) ) == NULL && (fptr = fopen( "outputs//biclusters//CCResult.txt", "w" ) ) == NULL  ){
	  printf( " Could not open the file " );
    }
    fclose( fptr );
    // initialize rand() seed
    srand((unsigned int) time(NULL));

    // initialize Biclustering module
    if (BicInitialize(inFilePath, nRowsSkip, nColsSkip) == false)
    {
        fprintf(stderr, "Error: BicInitialize() failed.\n");
        return false;
    }
    cout << "/**************************************************/" << endl;
    cout << "	CHENG AND CHURCH'S BICLUSTERING " << endl;
    cout << "/**************************************************/" << endl;
    int count = 0;
    while( count != bic_num )
    {
        // generate bicluster and display it
        BicGenerateBicluster(par1,par2,par3);
	count++;
	cout << "\nBicluster " << count << " is calculated for CC\n";
    }
    // free resources allocated by Biclustering module
    BicDestroy();
    puts("\nAll success...");
    
    return false;
}
