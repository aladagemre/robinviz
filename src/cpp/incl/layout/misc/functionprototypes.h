/*************************************************************************************
	Copyright (C) <2008>  <Arda Cakiroglu Ömer Karataþ Melih Sözdinler>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    <matrixToGraph.h>  Copyright (C) <2008>  <Arda Cakiroglu  Ömer Karataþ Melih Sözdinler>
    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.
**************************************************************************************/

#define hfunctionprototypes

#ifndef hifdefs
#include "ifdefs.h"
#endif



#ifndef hheaders
#include "headers.h"
#endif


/*
	IO Functions
*/
void readFromGraphBase(leda::string filename,GRAPH<int,int>& G,list<node>& A,list<node>& B);
void writeRow(ofstream &report,int ,int ,int ,int ,int ,array<integer>,leda::string extra,array<int>);


/*
	Converters
*/
matrix g2m(GRAPH<int,int>& G, list<node> A, list<node> B, bool);
void m2g(GRAPH<int,int> &G, list<node> &A, list<node> &B, matrix &m, bool);


/*
	Drawings
*/
void drawBipartite (GRAPH<int,int> &G, list<node> A, list<node> B);


/*

*/
template <class numberType>
numberType findMin(list<numberType> &numbers);
template <class numberType>
numberType findMax(list<numberType> &numbers);

/*
	Algorithms
*/
void wolf(GRAPH<int,int> &G, list<node> &A, list<node> B);
	integer weight(GRAPH<int,int> & , node alt1, node alt2, node ust, list<node> alt);
	void merge(GRAPH<int,int> &G, list<node> &ptRow, list<node> &L, list<node> &R, node owner, list<node> &alt);
	void wolfPhase2(GRAPH<int,int> &G, list<node> &ptRow, node owner, list<node> alt);
	integer crossingNumber(GRAPH<int,int> &G, list<node> &A, list<node> &B);
	integer crossingNumber(matrix &m);
//void barycenter(GRAPH<int,int> &G,list<node> &A,list<node> &B);
//void median(GRAPH<int,int> &G,list<node> &A,list<node> B);
//void gre(GRAPH<int,int> &G, list<node> &A, list<node> B);
//	void buildCrossTable(GRAPH<int,int> &G, list<node> &A, list<node> &B, node_matrix<integer> &nmdCrossTable);
//
//
//
///*
//	Random generators
//*/
//void randomGraph2(GRAPH<int,int> &G,list<node> &A,list<node> &B,int aCount,int bCount,double edgeDensity,int sumOfWeight,int weightBalance);
//void assignWeight(matrix &m,int sumOfWeight,int weightBalance);
//int calculateSumOfWeight(int a,int b,int cn,int averageWeight);

/*list<int> generateNumbers(structGeneralDistribution dist, int numberOfNumbers, gsl_rng *r, bool freeR=false);
list<int> generateNumbers(structGamma dist, int numberOfNumbers, gsl_rng *r, bool freeR=false)
list<int> generateNumbers(structBeta dist, int numberOfNumbers, gsl_rng *r, bool freeR=false)
list<int> generateNumbers(structStudentt dist, int numberOfNumbers, gsl_rng *r, bool freeR=false)*/
