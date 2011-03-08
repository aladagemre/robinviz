
#include "misc/declerations.h"

void gwSave( array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, bool flag  ){
                GraphWin gw( INTERACTIONS );
		node n;
                forall_nodes( n, INTERACTIONS ){
                    gw.set_label( n, GenesNode[ INTERACTIONS[ n ]  ].GENE );
                }
#ifdef LINUX
		gw.save_gml("outputs/graphs/ppigraph.gml"); 
#else
		gw.save_gml("outputs//graphs/ppigraph.gml"); 
#endif
                
 }

void gwSave( array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS ){
		node n;
		edge e;
		FILE *fptr;
#ifdef LINUX
		fptr = fopen( "outputs/graphs/ppigraph.txt", "w" ); 
#else
		fptr = fopen( "outputs//graphs/ppigraph.txt", "w" ); 
#endif
                forall_nodes( n, INTERACTIONS ){
			if( INTERACTIONS.degree( n ) > 0 ){
				fprintf( fptr, "%s\t%d\t", GenesNode[ INTERACTIONS[ n ] ].GENE, INTERACTIONS.degree( n ) );
				forall_out_edges( e, n ){
					fprintf( fptr, "%s\t", GenesNode[ INTERACTIONS[ INTERACTIONS.target( e ) ] ].GENE );
				}
				forall_in_edges( e, n ){
					fprintf( fptr, "%s\t", GenesNode[ INTERACTIONS[ INTERACTIONS.source( e ) ] ].GENE );
				}
				int count = 0;
				forall_out_edges( e, n ){
					if( count != INTERACTIONS.outdeg( n ) - 1 ){
						fprintf( fptr, "%d\t", INTERACTIONS[ e ] );
					}
					else{
						if( INTERACTIONS.indeg( n ) == 0 )
							fprintf( fptr, "%d\n", INTERACTIONS[ e ] );
						else
							fprintf( fptr, "%d\t", INTERACTIONS[ e ] );
					}
					count++;
				}
				count = 0;
				forall_in_edges( e, n ){
					if( count != INTERACTIONS.indeg( n ) - 1 ){
						fprintf( fptr, "%d\t", INTERACTIONS[ e ] );
					}
					else{
						fprintf( fptr, "%d\n", INTERACTIONS[ e ] );
					}
					count++;
				}
			}
                }  
		fclose( fptr );
 }

void gwSave( array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, array<list<char> > &Categories ){
                node n;
                edge e;
                list_item it;
                FILE *fptr;
                FILE *yFptr;
#ifdef LINUX
                fptr = fopen( "outputs/graphs/ppigraph.txt", "w" );
#else
                fptr = fopen( "outputs//graphs/ppigraph.txt", "w" );
#endif

#ifdef __linux__
                yFptr = fopen( "sources/usr_sources/visualization_data/genenames.txt", "r" );
#else
                yFptr = fopen( "sources//usr_sources//visualization_data//genenames.txt", "r" );
#endif

                forall_nodes( n, INTERACTIONS ){
                        fprintf( yFptr, "%s\n", GenesNode[ INTERACTIONS[ n ] ].GENE  );
                        if( INTERACTIONS.degree( n ) > 0 ){
                                char temp_str2[256] = "";
                                strcat( temp_str2, GenesNode[ INTERACTIONS[ n ] ].GENE  );
                                strcat( temp_str2, "_" );
                                forall_items( it, Categories[ INTERACTIONS[ n ] ] ){
                                        char number[4];
                                        sprintf( number, "%c:", Categories[ INTERACTIONS[ n ] ][ it ] );
                                        strcat( temp_str2, number );
                                }
//                                fprintf( fptr, "%s\t", temp_str2);
                                fprintf( fptr, "%s\t%d\t", temp_str2, INTERACTIONS.degree( n ) );
                                forall_out_edges( e, n ){
                                        char temp_str[256] = "";
                                        strcat( temp_str, GenesNode[ INTERACTIONS[ INTERACTIONS.target( e ) ] ].GENE  );
                                        strcat( temp_str, "_" );
                                        forall_items( it, Categories[ INTERACTIONS[ INTERACTIONS.target( e ) ] ] ){
                                                char number[4];
                                                sprintf( number, "%c:", Categories[ INTERACTIONS[ INTERACTIONS.target( e ) ] ][ it ] );
                                                strcat( temp_str, number );
                                        }
                                        fprintf( fptr, "%s\t", temp_str);
                                }
                                forall_in_edges( e, n ){
                                        char temp_str[256] = "";
                                        strcat( temp_str, GenesNode[ INTERACTIONS[ INTERACTIONS.source( e ) ] ].GENE );
                                        strcat( temp_str, "_" );
                                        forall_items( it, Categories[ INTERACTIONS[ INTERACTIONS.source( e ) ] ] ){
                                                char number[4];
                                                sprintf( number, "%c:", Categories[ INTERACTIONS[ INTERACTIONS.source( e ) ] ][ it ] );
                                                strcat( temp_str, number );
                                        }
                                        fprintf( fptr, "%s\t", temp_str);
                                }
                                int count = 0;
                                forall_out_edges( e, n ){
                                        if( count != INTERACTIONS.outdeg( n ) - 1 ){
                                                fprintf( fptr, "%d\t", INTERACTIONS[ e ] );
                                        }
                                        else{
                                                if( INTERACTIONS.indeg( n ) == 0 )
                                                        fprintf( fptr, "%d\n", INTERACTIONS[ e ] );
                                                else
                                                        fprintf( fptr, "%d\t", INTERACTIONS[ e ] );
                                        }
                                        count++;
                                }
                                count = 0;
                                forall_in_edges( e, n ){
                                        if( count != INTERACTIONS.indeg( n ) - 1 ){
                                                fprintf( fptr, "%d\t", INTERACTIONS[ e ] );
                                        }
                                        else{
                                                fprintf( fptr, "%d\n", INTERACTIONS[ e ] );
                                        }
                                        count++;
                                }
                        }
                }
                fclose( fptr );
                fclose( yFptr );
 }
