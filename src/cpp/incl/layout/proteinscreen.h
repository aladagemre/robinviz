
#include "misc/declerations.h"

void gwSave( array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS ){
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