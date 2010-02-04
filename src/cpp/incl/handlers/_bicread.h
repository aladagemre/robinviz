// #include "declerations.h"

void bicRead( matrix &INPUT, list<list<GENES> > &biclusters, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, int minBicSize, int maxBicSize, int biclustering, int dimension1, int dimension2  ){
	cout << "/**************************************************/" << endl;
	cout << "\t" << " Bicluster Examination Part" << endl;
	cout << "/**************************************************/" << endl;
	biclusterHandling( INPUT, biclusters, categ, matrixBiclusters, H_values, Hmax, minBicSize, maxBicSize, biclustering, dimension1, dimension2  );
} 
