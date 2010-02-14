#include <iostream>
#include <string.h>

//#define LINUX
#define WINDOWS

int char_arr_to_int(char c[]) {
    int x = 0, i = 0;
    float m = .1;
    for(i = strlen(c)-1; i >= 0; i--) {
        if(c[i] <= '9' && c[i] >= '0') {
            x += (c[i]-'0') * (m *= 10);
        }
    }

    // negative?
    return c[0] == '-' ? -x : x;
}

using namespace std;

int main(int argc, char** argv) {
	if( argc == 4 ){
        if( argc > 1 && argc < 5 && strcmp( argv[3], "save" ) == 0 ){
                char filename[256],line[1000];
                sprintf( filename, "%s", argv[1] );
                int numberOfBiclusters = char_arr_to_int( argv[2] );
                FILE * fptr = fopen( filename, "w" );
                FILE *fptr2;
                fprintf( fptr, "%d\n", numberOfBiclusters );
                for( int i = 0; i < numberOfBiclusters; i++ ){
#ifdef LINUX
                    sprintf( filename, "outputs/graphs/graph%d.gml", i );
#else
                    sprintf( filename, "outputs//graphs//graph%d.gml", i );
#endif
                    if( ( fptr2 = fopen( filename, "r") )!= NULL ){
                        //cout << i << endl;
                        fprintf( fptr, "%d\t", i );
                        int lines = 0;
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                lines++;
                        }
                        rewind( fptr2 );
                        fprintf( fptr, "%d\n", lines );
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                fputs (line, fptr);
                        }                        
                        fclose( fptr2 );
						fprintf( fptr, "\n" );
                    }
                    else{
                        fprintf( fptr, "%d\t", i );
                        fprintf( fptr, "%d\n", 0 );
                    }
                }
#ifdef LINUX
                    sprintf( filename, "outputs/graphs/maingraph.gml" );
#else
                    sprintf( filename, "outputs//graphs//maingraph.gml" );
#endif
                    if( ( fptr2 = fopen( filename, "r") ) != NULL ){
                        fprintf( fptr, "%s\t", filename );
                        int lines = 0;
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                lines++;
                        }
                        rewind( fptr2 );
                        fprintf( fptr, "%d\n", lines );
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                fputs (line, fptr);
                        }
                        fclose( fptr2 );
						fprintf( fptr, "\n" );
                    }
                    else{
                        fprintf( fptr, "%d\n", 0 );
                    }

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

                for( int i = 0; i < numberOfBiclusters; i++ ){
#ifdef LINUX
                    sprintf( filename, "outputs/enrich/resultbic%d.html", i );
#else
                    sprintf( filename, "outputs//enrich//resultbic%d.html", i );
#endif
                    if( ( fptr2 = fopen( filename, "r") ) != NULL ){
                        fprintf( fptr, "%d\t", i );
                        int lines = 0;
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                lines++;
                        }
                        rewind( fptr2 );
                        fprintf( fptr, "%d\n", lines );
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                fputs (line, fptr);
                        }
                        fclose( fptr2 );
						fprintf( fptr, "\n" );
                    }
                    else{
                        fprintf( fptr, "%d\t", i );
                        fprintf( fptr, "%d\n", 0 );
                    }
                }
#ifdef LINUX
                    sprintf( filename, "outputs/enrich/result.html" );
#else
                    sprintf( filename, "outputs//enrich//result.html" );
#endif
                    if( ( fptr2 = fopen( filename, "r") ) != NULL ){
                        fprintf( fptr, "%s\t", filename );
                        int lines = 0;
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                lines++;
                        }
                        rewind( fptr2 );
                        fprintf( fptr, "%d\n", lines );
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                fputs (line, fptr);
                        }
                        fclose( fptr2 );
						fprintf( fptr, "\n" );
                    }
                    else{
						fprintf( fptr, "%s\t", filename );
                        fprintf( fptr, "%d\n", 0 );
                    }

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

                    for( int i = 0; i < numberOfBiclusters; i++ ){
    #ifdef LINUX
                        sprintf( filename, "outputs/heatmap/out%d.txt", i );
    #else
                        sprintf( filename, "outputs//heatmap//out%d.txt", i );
    #endif
                        if( ( fptr2 = fopen( filename, "r") ) != NULL ){
                            fprintf( fptr, "%d\t", i );
                            int lines = 0;
                            while( !feof( fptr2 ) ){
                                    fgets( line, 1000, fptr2 );
                                    lines++;
                            }
                            rewind( fptr2 );
                            fprintf( fptr, "%d\n", lines );
                            while( !feof( fptr2 ) ){
                                    fgets( line, 1000, fptr2 );
                                    fputs (line, fptr);
                            }
                            fclose( fptr2 );
							fprintf( fptr, "\n" );
                        }
                        else{
                            fprintf( fptr, "%d\t", i );
                            fprintf( fptr, "%d\n", 0 );
                        }
                    }

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

                    for( int i = 0; i < numberOfBiclusters; i++ ){
    #ifdef LINUX
                        sprintf( filename, "outputs/go/gobicluster%d.txt", i );
    #else
                        sprintf( filename, "outputs//go//gobicluster%d.txt", i );
    #endif
                        if( ( fptr2 = fopen( filename, "r") ) != NULL ){
                            fprintf( fptr, "%d\t", i );
                            int lines = 0;
                            while( !feof( fptr2 ) ){
                                    fgets( line, 1000, fptr2 );
                                    lines++;
                            }
                            rewind( fptr2 );
                            fprintf( fptr, "%d\n", lines );
                            while( !feof( fptr2 ) ){
                                    fgets( line, 1000, fptr2 );
                                    fputs (line, fptr);
                            }
                            fclose( fptr2 );
							fprintf( fptr, "\n" );
                        }
                        else{
                            fprintf( fptr, "%d\t", i );
                            fprintf( fptr, "%d\n", 0 );
                        }
                    }

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

#ifdef LINUX
                    sprintf( filename, "outputs/colors_func.txt" );
#else
                    sprintf( filename, "outputs//colors_func.txt" );
#endif
                    if( ( fptr2 = fopen( filename, "r") ) != NULL ){
                        fprintf( fptr, "%s\t", filename );
                        int lines = 0;
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                lines++;
                        }
                        rewind( fptr2 );
                        fprintf( fptr, "%d\n", lines );
                        while( !feof( fptr2 ) ){
                                fgets( line, 1000, fptr2 );
                                fputs (line, fptr);
                        }
                        fclose( fptr2 );
                    }
                    else{
						fprintf( fptr, "%s\t", filename );
                        fprintf( fptr, "%d\n", 0 );
                    }

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////
                    fclose( fptr );

		}
	}
    else{
        if( argc > 1 && argc < 4 && strcmp( argv[2], "load" ) == 0 ){
			    //cout << "\n******************************************************\n";
                char filename[256],line[1000];
                sprintf( filename, "%s", argv[1] );
				cout << filename << endl;
                int numberOfBiclusters;
                FILE * fptr = fopen( filename, "r" );
                FILE *fptr2;
                fscanf( fptr, "%d", &numberOfBiclusters );
				//cout << "\n******************************************************\n";
				                      
                int lines, j;
                for( int i = 0; i < numberOfBiclusters; i++ ){
                    fscanf( fptr, "%d", &i );  
					//cout << i << "\t";
                    fscanf( fptr, "%d", &lines );
					//cout << lines << "\n";
					j = 0;
#ifdef LINUX
                    sprintf( filename, "outputs/graphs/graph%d.gml", i );
#else
                    sprintf( filename, "outputs//graphs//graph%d.gml", i );
#endif
                    fptr2 = fopen( filename, "w");
                        //cout << i << endl;
                        while( j <= lines ){
                                fgets( line, 1000, fptr );
								//cout << line << "\n";
                                fputs (line, fptr2 );
                                j++;
                        }
                        fclose( fptr2 );
                }
#ifdef LINUX
                    sprintf( filename, "outputs/graphs/maingraph.gml" );
#else
                    sprintf( filename, "outputs//graphs//maingraph.gml" );
#endif
                    fptr2 = fopen( filename, "w");
                        fscanf( fptr, "%s", filename );		
						j = 0;
                        //cout << i << endl;
                        fscanf( fptr, "%d", &lines );
                        while( j <= lines ){
                                fgets( line, 1000, fptr );
                                fputs (line, fptr2 );
                                j++;
                        }
                        fclose( fptr2 );

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

					//cout << "\n******************************************************\n";
                    for( int i = 0; i < numberOfBiclusters; i++ ){
                        fscanf( fptr, "%d", &i );
#ifdef LINUX
                        sprintf( filename, "outputs/enrich/resultbic%d.html", i );
#else
                        sprintf( filename, "outputs//enrich//resultbic%d.html", i );
#endif
                        fptr2 = fopen( filename, "w");
                            //cout << i << endl;
							j = 0;
                            fscanf( fptr, "%d", &lines );
                            while( j <= lines ){
                                    fgets( line, 1000, fptr );
                                    fputs (line, fptr2 );
                                    j++;
                            }
                            fclose( fptr2 );
                    }
#ifdef LINUX
                    sprintf( filename, "outputs/enrich/result.html" );
#else
                    sprintf( filename, "outputs//enrich//result.html" );
#endif
                    fptr2 = fopen( filename, "w");
                        fscanf( fptr, "%s", filename );
						j = 0;
                        fscanf( fptr, "%d", &lines );
                        while( j <= lines ){
                                fgets( line, 1000, fptr );
								if( j != 0 ){
									//cout << line << endl;
									fputs (line, fptr2 );
								}
								j++;
                        }
                        fclose( fptr2 );

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

					//cout << "\n******************************************************\n";
                    for( int i = 0; i < numberOfBiclusters; i++ ){
                        fscanf( fptr, "%d", &i );
#ifdef LINUX
                        sprintf( filename, "outputs/heatmap/out%d.txt", i );
#else
                        sprintf( filename, "outputs//heatmap//out%d.txt", i );
#endif
                        fptr2 = fopen( filename, "w");
                            //cout << i << endl;
							j = 0;
                            fscanf( fptr, "%d", &lines );
                            while( j <= lines ){
                                    fgets( line, 1000, fptr );
                                    if( j != 0 ){
										//cout << line << endl;
										fputs (line, fptr2 );
									}
									j++;
                            }
                            fclose( fptr2 );
                    }

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

                    for( int i = 0; i < numberOfBiclusters; i++ ){
                        fscanf( fptr, "%d", &i );
						j = 0;
						fscanf( fptr, "%d", &lines );
						if( lines != 0 ){
#ifdef LINUX
							sprintf( filename, "outputs/go/gobicluster%d.html", i );
#else
							sprintf( filename, "outputs//go//gobicluster%d.html", i );
#endif
							fptr2 = fopen( filename, "w");
								//cout << i << endl;
								while( j <= lines ){
									fgets( line, 1000, fptr );
									if( j != 0 ){
										//cout << line << endl;
										fputs (line, fptr2 );
									}
									j++;
								}
								fclose( fptr2 );
						}
                    }

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////

					//cout << "\n******************************************************\n";
#ifdef LINUX
                    sprintf( filename, "outputs/colors_func.txt" );
#else
                    sprintf( filename, "outputs//colors_func.txt" );
#endif
                    fptr2 = fopen( filename, "w");						
						//cout << filename << endl;
                        fscanf( fptr, "%s", filename );
						//cout << filename << endl;
						j = 0;
                        fscanf( fptr, "%d", &lines );
                        while( j <= lines ){
                            fgets( line, 1000, fptr );								
							if( j != 0 ){
								cout << line << endl;
                                fputs (line, fptr2 );
							}
                            j++;
                        }
                        fclose( fptr2 );

                    ///////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////
                    fclose( fptr );
        }
        else{
            std::cout << "\nInvalid Arguments\n";
        }
    }
	return 0;
}
