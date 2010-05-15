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


void barycenter(GRAPH<int,int> &G, list<node> &A, list<node> &B){
	node_array<int> naiA_(G),naiB_(G);
    node_array<integer> nairBaryValues(G);
    node n,tn;
    edge e;
    int i,count;
    integer sum=0,weightSum=0;

    i = 0;
    forall( n, A ){
        naiA_[n] = i++;
    }

    i=1;
    forall( n, B ){
        naiB_[n] = i++;
    }

   
    forall(n,A){
        sum=0;
        count=0;
		weightSum=0;
        forall_adj_edges(e,n){
            tn = G.target(e);
            sum += naiB_[tn]*G[e];
			weightSum += G[e];
            count++;
        }
		if ( count == 0 ){
			nairBaryValues[n] = -1 ;
		}else{
			nairBaryValues[n] = sum;
			nairBaryValues[n].operator *=(1000000);
			nairBaryValues[n].operator /=(weightSum);
		}
	}

    node minNode;
    list<node> temp = A ;
    list <node> tempResult ;
      

    for( int k=0 ; k < A.size() ; k++ ){   
        integer min = nairBaryValues[ temp.head() ] ;   
        minNode = temp.head();
        forall( n,temp ){
            if ( nairBaryValues[n] < min ){
                min = nairBaryValues[n];
                minNode = n ;
               
            }   
        }
        if( minNode != nil ){
            tempResult.append( minNode );
            temp.del_item( temp.search( minNode ) );
        }
    }   

    A = tempResult ;
}


void buildCrossTable(GRAPH<int,int>& G, list<node>& A, list<node>& B, node_matrix<integer>& nmdCrossTable){
	nmdCrossTable.init(G,0);
	edge eu,ev;
	node su,sv,tu,tv,n;
	node_array<int> naiA(G),naiB(G);
	integer temp;
	int i=0;

	forall(n,A){
		naiA[n] = i++;
	}
	i=0;
	forall(n,B){
		naiB[n] = i++;
	}

	i=0;
	forall_edges(eu,G){	
		forall_edges(ev,G){
			su = G.source(eu);
			sv = G.source(ev);
			tu = G.target(eu);
			tv = G.target(ev);
			
			if ( eu == ev )
				break;
			
			if ( naiA[su] < naiA[sv] ){							//u is before v
				temp = G[eu];
				temp *= G[ev];
				if ( naiB[tv] < naiB[tu] )						//do crossing?
					nmdCrossTable[su][sv] += temp;				//yes
				else if ( naiB[tv] > naiB[tu] )
					nmdCrossTable[sv][su] += temp;				//no


			}else if ( naiA[su] > naiA[sv] ){					//v is before u
				temp = G[eu];
				temp *= G[ev];
				if ( naiB[tv] > naiB[tu] )						//do crossing?
					nmdCrossTable[sv][su] += temp;				//yes
				else if ( naiB[tv] < naiB[tu] )
					nmdCrossTable[su][sv] += temp;				//no
			}
		}
	}
}

void gre(GRAPH<int,int> &G,list<node> &A, list<node> B, node_matrix<integer>& nmdCrossTable){
	node u,v;
	node_array<int> naiA(G);
	node_array<integer> allsumcuv(G);
	node_array<integer> allsummin(G);
	node_array<double> allrate(G);
	list<node> L,A_;
	int i;
	integer temp;
	
	i=0;
	forall(u,A){
		naiA[u] = i++;
		allsummin[u] = allsumcuv[u] = 0;
	}

	forall(u,A){
		forall(v,A){
			allsumcuv[u] += nmdCrossTable[u][v];
			allsummin[u] += nmdCrossTable[u][v]<nmdCrossTable[v][u] ? nmdCrossTable[u][v] : nmdCrossTable[v][u];
		
		}
		 if ( allsumcuv[u] == 0 ){
			allrate[u] = 0.0;
		}else if ( allsummin[u] == 0 ){
			allrate[u] = MAXDOUBLE;
		}else{
			temp = allsumcuv[u];
			temp.operator *=(1000000);
			temp.operator /=(allsummin[u]);
			
			allrate[u] = (double)(temp.to_double() / 1000000);
			//allrate[u] = temp;
		}
		 //cout << naiA[u] << "\t" << allrate[u] << "\n";
		
	}
	//cout << "\n\n\n";

	A_=L=A;
	A.clear();
	list_item itu,itv;
	while ( !L.empty() ){
		itu = L.first();
		u = L[itu];
		if ( L.size() > 1 ){
			for(itv = L.succ(itu); itv != nil; itv = L.succ(itv)){
				v = L[itv];
				if ( allrate[v] == -1 ){
					u = v;
					itu = itv;
					break;
				}
				if ( allrate[v] < allrate[u] ){
					u = v;
					itu = itv;
				}
			}
		}

		A.append(u);
		L.del_item(itu);
		//cout << "\n -- " << naiA[u] << "\t" << allrate[u] << "\n";

		forall(v,L){
			allsumcuv[v] -= nmdCrossTable[v][u];
			allsummin[v] -= nmdCrossTable[u][v]<nmdCrossTable[v][u] ? nmdCrossTable[u][v] : nmdCrossTable[v][u];
			if ( allsumcuv[v] == 0 ){
				allrate[v] = 0.0;
			}else if ( allsummin[v] == 0 ){
				allrate[v] = MAXDOUBLE;
			}else{
				temp = allsumcuv[v];
				temp.operator *=(1000000);
				temp.operator /=(allsummin[v]);
				

				allrate[v] = (double)(temp.to_double() / 1000000);
				//allrate[v] = temp;
			}
			//cout << naiA[v] << "\t" << allrate[v] << "\n";	
		}
		//cout << "\n\n";
	}
	
	

}

void median(GRAPH<int,int> &G, list<node> &A, list<node> B){
    node_array<list<node> > medianPartitions(G);

    list<node> freePartition;
    list<node> tempA;
    list<edge> elist;

    node n=nil;

    edge e1=nil;

    long sum, median_sum;
    
    forall(n,A){
        sum = 0;
        median_sum = 0;

        elist = G.out_edges( n );
        forall(e1,elist){
            sum += G[e1];
        }

        //after iteration e1 is the median
        forall(e1,elist){
            median_sum += G[e1];
            if ( median_sum < sum/2 ){
            }else{
                break;
            }
        }

        //if no node has no edge, we add it to freee partition which will not need to be sorted (using barycenter)
        if ( elist.size() == 0 ){
            freePartition.append(n);
        }else{
            medianPartitions[G.target(e1)].append(n);
        }
    }

    A.clear();
    A.conc(freePartition);

    forall(n,B){
        if ( medianPartitions[n].size() ){
            barycenter(G,medianPartitions[n],B);
            A.conc(medianPartitions[n]);
        }
    }

}


void luBound(GRAPH<int,int> &G, list<node> &A, list<node> &B, node_matrix<integer>& nmdCrossTable, integer& lower, integer& upper){
    integer uv,vu;
    node u,v ;
	lower=0;
	upper=0;

	forall(u,A){
		forall(v,A){
			if ( u==v ){
				break;
			}
			
			uv = nmdCrossTable[u][v];
			vu = nmdCrossTable[v][u];
			if ( uv > vu ){
				lower += vu;
				upper += uv;
			}else{
				lower += uv;
				upper += vu;
			}

		}
	}
    
}
