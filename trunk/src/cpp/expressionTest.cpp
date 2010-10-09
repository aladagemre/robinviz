 /*******************************************************************************************************/
/*	Copyright (C) <2010>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>   	 	       */
/*											 	       */
/*    This program is free software: you can redistribute it and/or modify		 	       */
/*    it under the terms of the GNU General Public License as published by		 	       */
/*    the Free Software Foundation, either version 3 of the License, or			 	       */
/*    (at your option) any later version.						 	       */
/*											 	       */
/*    This program is distributed in the hope that it will be useful,			 	       */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of			 	       */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			 	       */
/*    GNU General Public License for more details.					 	       */
/*											 	       */
/*    You should have received a copy of the GNU General Public License			 	       */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.		 	       */
/*											 	       */
/*    Copyright (C) <2010>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 	       */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.         	       */
/*    This is free software, and you are welcome to redistribute it                      	       */
/*    under certain conditions; type `show c' for details.			         	       */
/*******************************************************************************************************/
/********************************************************************************************************
*                                                                               			*
*	The project team                                                        			*
*                                                                               			*
*  Assistant Professor Cesim Erten(Kadir Has University)                        			*
*  Ahmet Emre Aladağ(Kadir Has University)                                      			*
*  Melih Sözdinler(Boğaziçi University)                                         			*
*                                                                               			*
*	Contacts for Implementation Issues                                      			*
*                                                                               			*
*  Melih Sözdinler, melihsozdinler{at}gmail{dot}com, melih.sozdinler{at}boun{dot}edu{dot}tr 		*
*  Ahmet Emre Aladağ, aladagemre{at}gmail{dot}com, emre.aladag{at}stu{dot}khas{dot}edu{dot}tr      	*
*                                                                               			*
********************************************************************************************************/

#include "incl/handlers.h"
#include "incl/handlers/_dataread.h"

struct store{
    char store[256];
};

typedef struct store STR;

// If you use main1 you need data/expressions/forParsing.txt that have the filename
// GSE code and GPL annotation at one line for countable number of time series data
// Put this executable to root folder before /data or /trunk to obtain new matrices
// entrez protein files and condition files. To convert biogrid id run the file at
// /trunk/src/python/robinviz/databases/run.sh
int main1(){

        list_item it;
        FILE *fptr,*fptr2;
        list<two_tuple<STR,STR> > files;
        char read[256],command[256],filename[256],filename2[256],filename3[256],copy,filename4[256],filename5[256];

        fptr = fopen( "data/expressions/forParsing.txt", "r" );

        while( !feof( fptr ) ){
            two_tuple<STR,STR> tmp;
            fscanf( fptr, "%s", read );
            sprintf( tmp.first().store, "%s", read );
            fscanf( fptr, "%s", read );
            fscanf( fptr, "%s", read );
            sprintf( tmp.second().store, "%s", read );
            files.append( tmp );
        }
        fclose( fptr );

        forall_items( it, files ){
            sprintf( filename, "data/expressions/gse/%s.gz", files[ it ].first().store );
            sprintf( filename3, "%s.gz", files[ it ].first().store );
            sprintf( command, "python data/expressions/ungz.py data/expressions/gse/%s.gz", files[ it ].first().store );
            system( command );
            sprintf( filename, "data/expressions/gse/%s", files[ it ].first().store );
            cout << "--------------\n";
            sprintf( filename2, "data/expressions/%s.annot.gz", files[ it ].second().store );
            if( (fptr = fopen( filename2, "r" )) == NULL ){
                cout << "+++++++++++++++\n";
                sprintf( command, "python data/expressions/ungz.py ftp://ftp.ncbi.nih.gov/pub/geo/DATA/annotation/platforms/%s.annot.gz", files[ it ].second().store );
                system( command );
                fclose( fptr );
                cout << "Copy begins\n";
                sprintf( filename4, "%s.annot", files[ it ].second().store );
                sprintf( filename5, "data/expressions/%s.annot", files[ it ].second().store );
                fptr = fopen( filename4, "r" );
                fptr2 = fopen( filename5, "w" );
                while( !feof( fptr ) ){
                    fscanf( fptr, "%c", &copy );
                    fprintf( fptr2, "%c", copy );
                }
                fclose( fptr );
                fclose( fptr2 );
                cout << "Copy is done\n";
            }
            else{
                cout << "--------------\n";
                sprintf( command, "python data/expressions/ungz.py %s", filename2 );
                system( command );
            }
            sprintf( filename2, "data/expressions/%s.annot", files[ it ].second().store );
            cout << "\n READING :" << filename << " - " << filename2 << endl;
            cout << "\n______________________________\n";

            array<GENES> geneArray;
            array<CONDS> condArray;
            matrix INPUT;

            dataReadGEO( INPUT, filename, filename2, filename3, geneArray, condArray, 1 );
            cout << "\n END READING :";
            cout << filename << " - " << filename2 << endl;
        }

        //

	return 0;
}

// If you use main1 you need to run the process mentioned in main1 comments.
// Then for each file in run.sh you must have gene matrix and cond files.
// gene file is produced by python code and you need to run sh run.sh
// then for each three files you get one matrix file in the root folder.
// Remember you can run this main function from root folder before trunk and
// data.
int main(){

    FILE *fptr,*matrix,*gene,*cond,*save;
    char read0[512],read1[512],read2[512],read3[512],readF[512];
    double value;
    int dim1,dim2;
    char label[512],label2[512];
    fptr = fopen( "trunk/src/python/robinviz/databases/run.sh", "r" );
    while( !feof( fptr ) ){
        fscanf( fptr, "%s%s%s%s", read0, read1, read2, read3 );
        sprintf( readF,"trunk/src/python/robinviz/databases/%s.matrix", read2 );
        if( (matrix = fopen( readF, "r" ) ) != NULL ){
            cout << "1.";
            sprintf( readF,"trunk/src/python/robinviz/databases/%s.gene", read2 );
            if( (gene = fopen( readF, "r" ) ) != NULL ){
                cout << "2.";
                sprintf( readF,"trunk/src/python/robinviz/databases/%s.cond", read2 );
                if( ( cond = fopen( readF, "r" ) ) != NULL ){
                    cout << read1 << " - " << read2 << " - " << read3 << endl;
                    leda::string f( read2 );
                    f = f.replace_all( ".txt.gz", ".txt" );
                    char temp[512];
                    sprintf( temp, "%s", f.c_str() );
                    cout << f << " - " << temp << endl;
                    save = fopen( temp, "w" );
                    fscanf( matrix, "%d%d", &dim1, &dim2 );
                    cout << dim1 << " - " << dim2 << endl;
                    fprintf( save, "%d\t%d\n", dim1, dim2 );
                    fprintf( save, "probset\t" );
                    for( int i = 0; i < dim2; i++ ){
                        fgets( label, 512, cond );
                        int j = 0;
                        while( label[ j ] != '\0' ){
                            if( label[ j ] == ' ' )
                                label[ j ] = '_';
                            j++;
                        }
                        label[ strlen( label ) - 2 ] = '\t';
                        label[ strlen( label ) - 1 ] = '\0';
                        fprintf( save, "%s", label );
                    }
                    fprintf( save, "\n" );
                    for( int i = 0; i < dim1; i++ ){
                        cout << "|";
                        fscanf( gene, "%s", label );
                        int count = 0;
                        for(int j = 0; j < strlen( label ); j++ ){
                            if( label[ j ] == '/' )
                                break;
                            count++;
                        }
                        strncpy (label2,label,count);
                        label2[count]='\0';
                        fprintf( save, "%s\t", label2 );
                        for( int j = 0; j < dim2; j++ ){                            
                            fscanf( matrix, "%lf", &value );
                            if( j == dim2 - 1 )
                                fprintf( save, "%lf", value );
                            else
                                fprintf( save, "%lf\t", value );
                        }
                        fprintf( save, "\n" );
                    }
                    cout << endl;
                }
            }
        }
    }
    fclose( fptr );
    fclose( gene );
    fclose( cond );
    fclose( save );
    fclose( matrix );
    return 0;
}
