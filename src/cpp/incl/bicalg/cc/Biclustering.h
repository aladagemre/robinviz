/* ---------------------------------------------------
 * --- File     : Biclustering.h                   ---
 * --- Author   : Ahmet ARDAL                      ---
 * --- Contact  : ardalahmet{at}hotmail{dot}com    ---
 * ---------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


/* -------------------------------------------------
 * --- Symbolic Constants --------------------------
 * -------------------------------------------------
 */
//#define LINE_BUF_LEN    2048
#define LINE_BUF_LEN    8192
//#define LINE_BUF_LEN    16384

/* -------------------------------------------------
 * --- Macro Definitions ---------------------------
 * -------------------------------------------------
 */
#define MatrixSetItem(mat, nCols, rowIdx, colIdx, val)      (mat)[(rowIdx) * (nCols) + (colIdx)] = (val)
#define MatrixGetItem(mat, nCols, rowIdx, colIdx)           (mat)[(rowIdx) * (nCols) + (colIdx)]

/* -------------------------------------------------
 * --- Global Variables ----------------------------
 * -------------------------------------------------
 */
static int numberOfColumns = 0;
static int numberOfRows = 0;
static short *matrix = NULL;
int maxScore = 1200;
int minHeight = 5;
int minWidth = 10;
static int batchThreshold = 100;
static bool   *remainingRs  = NULL;
static bool   *remainingCs  = NULL;
static double *rowMeans     = NULL;
static double *columnMeans  = NULL;
static double *rowScores    = NULL;
static double *columnScores = NULL;
static double mean = 0;
static int smWidth = 0;
static int smHeight = 0;
static double HScore = 0;

/* -------------------------------------------------
 * --- Static Function Declarations ----------------
 * -------------------------------------------------
 */
static int  strIndexOf(const char *str, const char *keys, int startIdx);
static void calculateScoring(void);
static void displayBicluster(void);

#ifndef __cplusplus
typedef enum Bool_
{
    FALSE = 0,
    TRUE  = 1
} _Bool;

# define bool    _Bool
# define false   FALSE
# define true    TRUE
#endif  // __cplusplus

static int strIndexOf(const char *str, const char *keys, int startIdx);
static void calculateScoring(void);
static void displayBicluster(void);
bool BicInitialize(const char *filePath, int nRowsSkip, int nColsSkip);
void BicGenerateBicluster(void);
void BicDestroy(void);

/* -------------------------------------------------
 * --- Static Function Definitions -----------------
 * -------------------------------------------------
 */
static int strIndexOf(const char *str, const char *keys, int startIdx)
{
    char *pc;

    if ((pc = strpbrk((str + startIdx), keys)) == NULL)
    {
        return -1;
    }
    else
    {
        return (pc - str);
    }
}

static void calculateScoring(void)
{
    int i, j;
    
    mean = 0;

    for (j = 0; j < numberOfColumns; ++j)
    {
        if (remainingCs[j])
        {
            columnMeans[j] = 0;
        }
    }

    for (i = 0; i < numberOfRows; ++i)
    {
        if (remainingRs[i])
        {
            rowMeans[i] = 0;
            for (j = 0; j < numberOfColumns; ++j)
            {
                if (remainingCs[j])
                {
                    /*rowMeans[i]    += matrix[i][j];
                    columnMeans[j] += matrix[i][j];*/
                    rowMeans[i]    += MatrixGetItem(matrix, numberOfColumns, i, j);
                    columnMeans[j] += MatrixGetItem(matrix, numberOfColumns, i, j);
                }
            }
            mean += rowMeans[i];
            rowMeans[i] /= smWidth;
        }
    }

    for (j = 0; j < numberOfColumns; ++j)
    {
        if (remainingCs[j])
        {
            columnMeans[j] /= smHeight;
        }
    }

    mean /= smWidth * smHeight;
    HScore = 0;

    for (j = 0; j < numberOfColumns; ++j)
    {
        if (remainingCs[j])
        {
            columnScores[j] = 0;
        }
    }

    for (i = 0; i < numberOfRows; ++i)
    {
        if (remainingRs[i])
        {
            rowScores[i] = 0;
            for (j = 0; j < numberOfColumns; ++j)
            {
                if (remainingCs[j])
                {
                    /*double r = matrix[i][j] - rowMeans[i] - columnMeans[j] + mean;*/
                    double r = MatrixGetItem(matrix, numberOfColumns, i, j) - rowMeans[i] - columnMeans[j] + mean;
                    r = r * r;
                    rowScores[i]    += r;
                    columnScores[j] += r;
                }
            }
            HScore += rowScores[i];
            rowScores[i] /= smWidth;   
        }
    }

    HScore /= smWidth * smHeight;

    for (j = 0; j < numberOfColumns; ++j)
    {
        if (remainingCs[j])
        {
            columnScores[j] /= smHeight;
        }
    }
}   // calculateScoring()

static void displayBicluster(void)
{
    int i, j, r_count = 0, c_count = 0;
    FILE *fptr;
    if( (fptr = fopen( "outputs/biclusters/CCResult.txt", "a" ) ) == NULL ){
	  printf( " Could not open the file " );
    }
    else{
	printf("smHeight: %d, smWidth: %d, HScore: %d\n", smHeight, smWidth, (int) HScore);
	puts("-----------------------------------------");
	puts("Rows:");

	for (i = 0; i < numberOfRows; ++i)
	{
	    if (remainingRs[i])
	    {
		//recordFile.print(i + " ");
		r_count++;
	    }
	}
	for (j = 0; j < numberOfColumns; ++j)
	{
	    if (remainingCs[j])
	    {
		//recordFile.print(j + " ");
		c_count++;
	    }
	}


	fprintf( fptr, "%d\t%d\n", r_count, c_count );
	c_count = 0; r_count = 0;
	for (i = 0; i < numberOfRows; ++i)
	{
	    if (remainingRs[i])
	    {
		//recordFile.print(i + " ");
		printf("%d, ", i);
		fprintf( fptr, "gene%d\t", i );
	    }
	}
	fprintf( fptr, "\n" );

	puts("\n-----------------------------------------");
	puts("Columns:");

	for (j = 0; j < numberOfColumns; ++j)
	{
	    if (remainingCs[j])
	    {
		//recordFile.print(j + " ");
		printf("%d, ", j);
		fprintf( fptr, "cond%d\t", j );
	    }
	}
	fprintf( fptr, "\n" );
	puts("\n-----------------------------------------");
    }
    fclose( fptr );
}   // displayBicluster()



/* -------------------------------------------------
 * --- Public Function Definitions -----------------
 * -------------------------------------------------
 */
bool BicInitialize(const char *filePath, int nRowsSkip, int nColsSkip)
{
    int i, j;
    int n = 0;
    FILE *f;
    char line[LINE_BUF_LEN];
    char *pc;
    const char *strDelim = " \t";

    if ((f = fopen(filePath, "r")) == NULL)
    {
#ifndef NDEBUG
        fprintf(stderr, "cannot open file %s.(%s:%d)\n", filePath, __FILE__, __LINE__);
#endif
        return false;
    }

    // count columns
    fgets(line, LINE_BUF_LEN, f);
    pc = strtok(line, strDelim);
    while (pc != NULL)
    {
        ++n;
        pc = strtok(NULL, strDelim);
    }

    numberOfColumns = n - nColsSkip;

    for (i = 1; i < nRowsSkip; ++i)
    {
        fgets(line, LINE_BUF_LEN, f);
    }

    // count rows
    numberOfRows = 0; 
    while (fgets(line, LINE_BUF_LEN, f) != NULL)
    {
        ++numberOfRows;
    }  
    
    //br.close();
    // reset file pointer
    rewind(f);

#ifndef NDEBUG
    printf("There are %d rows and %d columns.\n", numberOfRows, numberOfColumns);
#endif

    // create matrix
    matrix = (short *) malloc(sizeof(short) * numberOfRows * numberOfColumns);

    // skip rows
    for (i = 0; i < nRowsSkip; ++i)
    {
        fgets(line, LINE_BUF_LEN, f);
    }

    for (i = 0; i < numberOfRows; ++i)
    {
        int tab = 0;

        fgets(line, LINE_BUF_LEN, f);
        //String line = br.readLine();
        //String w = null;

        // skip columns
        for (j = 0; j < nColsSkip; ++j)
        {
            //tab = line.indexOf('\t', tab) + 1;
            tab = strIndexOf(line, strDelim, tab) + 1;
        }

        // read remaining columns
        for (j = 0; j < numberOfColumns; ++j)
        {
            //int nextTab = line.indexOf('\t', tab);
            int nextTab = strIndexOf(line, strDelim, tab);

            if (nextTab == (tab + 1))
            {
                //matrix[i][j] = (short) ((rand() % 1600) - 800);
                MatrixSetItem(matrix, numberOfColumns, i, j, (short) ((rand() % 1600) - 800));
            }
            else
            {
                double d = strtod((line + tab), NULL);
                //matrix[i][j] = (short) (d * 100.0);
                MatrixSetItem(matrix, numberOfColumns, i, j, (short) (d * 100.0));
                /*if (nextTab == -1)
                {
                    w = line.substring(tab);
                }
                else
                {
                    w = line.substring(tab, nextTab);
                }
                try
                {
                    double d = Double.parseDouble(w);
                    matrix[i][j] = (short)(d * 100.0);
                }
                catch(NumberFormatException e)
                {
                    matrix[i][j] = (short) ((rand() % 1600) - 800);
                }*/
            }
            tab = nextTab + 1;
        }
    }

    fclose(f);

    remainingRs  = (bool *)   malloc(sizeof(bool)   * numberOfRows);
    remainingCs  = (bool *)   malloc(sizeof(bool)   * numberOfColumns);
    rowMeans     = (double *) malloc(sizeof(double) * numberOfRows);
    columnMeans  = (double *) malloc(sizeof(double) * numberOfColumns);
    rowScores    = (double *) malloc(sizeof(double) * numberOfRows);
    columnScores = (double *) malloc(sizeof(double) * numberOfColumns);

    // check whether any allocation failed
    if (!remainingRs || !remainingCs || !rowMeans || !columnMeans || !rowScores || !columnScores)
    {
        free(remainingRs);
        free(remainingCs);
        free(rowMeans);
        free(columnMeans);
        free(rowScores);
        free(columnScores);

#ifndef NDEBUG
        fprintf(stderr, "memory allocation failed.(%s:%d)\n", __FILE__, __LINE__);
#endif
        return false;
    }

    return true;
}   // BicInitialize()

void BicGenerateBicluster( int par1, int par2, int par3 )
{
    
    clock_t clkScoring, clkAll;
    double  scoringCostSecs = 0.0;
    double  allCostSecs     = 0.0;
    int i, j;
    int index = 0;

    // init clock counter for overall algorithm
    clkAll = clock();

    for (i = 0; i < numberOfRows; ++i)
    {
        remainingRs[i] = true;
    }
    for (j = 0; j < numberOfColumns; ++j)
    {
        remainingCs[j] = true;
    }

    smWidth  = numberOfColumns;
    smHeight = numberOfRows;
    
    clkScoring = clock();
    calculateScoring();
    scoringCostSecs += (double) (clock() - clkScoring) / CLOCKS_PER_SEC;

    while ((HScore > maxScore) && (index > -1))
    {
        if (smHeight > batchThreshold)
        {
           for (i = 0; i < numberOfRows; ++i)
           {
              if (remainingRs[i] && (rowScores[i] > HScore))
              {
                  remainingRs[i] = false;
                  --smHeight;
              }
           }
        }
        else
        {
            double ms  = 0;
            bool   row = true;
            
            index = -1;

            if (smHeight > minHeight)
            {
                for (i = 0; i < numberOfRows; ++i)
                {
                    if (remainingRs[i] && (rowScores[i] > ms))
                    {
                        ms = rowScores[i];
                        index = i;
                    }
                }
            }
            
            if (smWidth > minWidth)
            {
                for (i = 0; i < numberOfColumns; ++i)
                {
                    if (remainingCs[i] && (columnScores[i] > ms))
                    {
                        ms    = columnScores[i];
                        index = i;
                        row   = false;
                    }
                }
            }
            
            if (index > -1)
            {
                if (row)
                {
                    remainingRs[index] = false;
                    --smHeight;
                }
                else
                {
                    remainingCs[index] = false;
                    --smWidth;
                }
            }
        }

        clkScoring = clock();
        calculateScoring();
        scoringCostSecs += (double) (clock() - clkScoring) / CLOCKS_PER_SEC;
    }

    allCostSecs = (double) (clock() - clkAll) / CLOCKS_PER_SEC;

#ifndef NDEBUG
    printf(
        "Time spent in scoring calculation   : %.08f seconds,\n"
        "Time spent in bicluster calculation : %.08f seconds.\n",
        scoringCostSecs,
        allCostSecs
    );
#endif

    displayBicluster();

    for (i = 0; i < numberOfRows; ++i)
    {
        if (remainingRs[i])
        {
            for (j = 0; j < numberOfColumns; ++j)
            {
                if (remainingCs[j])
                {
                    /*matrix[i][j] = (short) (rand.nextInt(1600) - 800);*/
                    /*matrix[i][j] = (short) ((rand() % 1600) - 800);*/
                    MatrixSetItem(matrix, numberOfColumns, i, j, (short) ((rand() % 1600) - 800));
                }
            }
        }
    }
}   // BicGenerateBicluster()

void BicDestroy(void)
{
    free(matrix);
    free(remainingRs);
    free(remainingCs);
    free(rowMeans);
    free(columnMeans);
    free(rowScores);
    free(columnScores);
}
