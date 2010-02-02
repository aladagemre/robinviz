// #include "declerations.h"


void interactionRead( node_array<GENENAMES> &temp, array<GENENAMES> &GenesNode, GRAPH<int,int> &INTERACTIONS, GRAPH<int,int> &TEMPINT, leda::string &ppiFile ){

	cout << "/**************************************************/" << endl;
	cout << "\t" << " Create Protein-protein Interaction Graph" << endl;
	cout << "/**************************************************/" << endl;
	interactionHandling( temp, GenesNode, INTERACTIONS, TEMPINT, ppiFile );
	cout << "* " << INTERACTIONS.number_of_edges() << " " << INTERACTIONS.number_of_nodes() << endl; 
}