// #include "declerations.h"

void bicRead( matrix &INPUT, char defaultBicFile[256], list<list<GENES> > &biclusters, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, int minBicSize, int maxBicSize, int biclustering, int dimension1, int dimension2  ){
	cout << "/**************************************************/" << endl;
	cout << "\t" << " Bicluster Examination Part" << endl;
	cout << "/**************************************************/" << endl;
	biclusterHandling( INPUT, defaultBicFile,  biclusters, categ, matrixBiclusters, H_values, Hmax, minBicSize, maxBicSize, biclustering, dimension1, dimension2  );
}

void bicRead( matrix &INPUT, char defaultBicFile[256], list<list<GENES> > &biclusters, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, int minBicSize, int maxBicSize, int biclustering, int dimension1, int dimension2, bool hasColor  ){
        cout << "/**************************************************/" << endl;
        cout << "\t" << " Bicluster Examination Part" << endl;
        cout << "/**************************************************/" << endl;
        biclusterHandling( INPUT, defaultBicFile,  biclusters, categ, matrixBiclusters, H_values, Hmax, minBicSize, maxBicSize, biclustering, dimension1, dimension2, hasColor );
}

void goCatRead (char inputGoFile[256], char defaultGoFile[256], list<list<GENES> > &gocat, list<int> &categ, list<leda::matrix> &matrixBiclusters, list<double> &H_values, double &Hmax, array<GENENAMES> &GenesNode, bool hasColor ){
        cout << "/**************************************************/" << endl;
        cout << "\t" << " Bicluster Examination Part" << endl;
        cout << "/**************************************************/" << endl;
        goHandling( inputGoFile, defaultGoFile,  gocat, categ, matrixBiclusters, H_values, Hmax, GenesNode, hasColor );
}
