
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
#include "Biclustering.h"

int main(int argc, char **argv){
    int  i;
    char inFilePath[FILENAME_MAX];
    int  nRowsSkip = 0;
    int  nColsSkip = 0;
    FILE *fptr;
    if( (fptr = fopen( "CCResult_r.txt", "w" ) ) == NULL ){
	  printf( " Could not open the file " );
    }
    fclose( fptr );
    // check command line arguments
    if (argc != 4)
    {
        fprintf(stderr, "Usage:\n\tapp  -r=<#_of_rows_to_skip>  -c=<#_of_columns_to_skip>  <input_file>\n");
        return EXIT_FAILURE;
    }

    // parse command line arguments
    for (i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            if (toupper(argv[i][1]) == 'R')
            {
                nRowsSkip = strtol((argv[i] + 3), NULL, 10);
            }
            else if (toupper(argv[i][1]) == 'C')
            {
                nColsSkip = strtol((argv[i] + 3), NULL, 10);
            }
            else
            {
                fprintf(stderr, "Unknown command line option: %s.\n", argv[i]);
                return EXIT_FAILURE;
            }
        }
        else
        {
            strcpy(inFilePath, argv[i]);
        }
    }

    // initialize rand() seed
    srand((unsigned int) time(NULL));

    // initialize Biclustering module
    if (BicInitialize(inFilePath, nRowsSkip, nColsSkip) == false)
    {
        fprintf(stderr, "Error: BicInitialize() failed.\n");
        return EXIT_FAILURE;
    }

    int count = 0;
    while( count != 100 )
    {
        // generate bicluster and display it
        BicGenerateBicluster();
	count++;
    }

    // free resources allocated by Biclustering module
    BicDestroy();

    puts("\nAll success...");

    /*
    tests...
    {
        const char *str = "ali ata bak";
        puts(strpbrk(str, "t"));
    }
    printf("%f\n", strtod("23.2 \t 32.6", NULL));*/
    
    return 0;
}
