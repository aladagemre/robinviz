
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