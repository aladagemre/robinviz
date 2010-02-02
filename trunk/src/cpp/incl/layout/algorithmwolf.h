/*************************************************************************************
    Copyright (C) <2008>  <Arda Cakiroglu Ömer Karataş Melih Sözdinler>

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

    <matrixToGraph.h>  Copyright (C) <2008>  <Arda Cakiroglu  Ömer Karataş Melih Sözdinler>
    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.
**************************************************************************************/

#define halgorithmwolf

#include "misc/declerations.h"

void wolf(GRAPH<int,int> &G, list<node> &A, list<node> B);
integer weight(GRAPH<int,int> &G, node alt1, node alt2, node ust, list<node> alt);
int getCrossings(GRAPH<int,int> &G, list<node> &A, list<node> &B);
void merge(GRAPH<int,int> &G, list<node> &ptRow, list<node> &L, list<node> &R, node owner, list<node> &alt);
void wolfPhase2(GRAPH<int,int> &G, list<node> &ptRow, node owner ,list<node> alt);
integer crossingNumber(matrix &m);
integer crossingNumber(GRAPH<int,int> &G, list<node> &A, list<node> &B);
void m2g(GRAPH<int,int> &G, list<node> &A, list<node> &B, matrix &m, bool weighted);
matrix g2m(GRAPH<int,int>& G, list<node> A, list<node> B, bool weighted);

int wolfDebug=0;
int partitionCount;
int justRunPhase1=false;

void wolf(GRAPH<int,int> &G, list<node> &A, list<node> B){
    int i,j,k,neighbourWeight;
    int leftsum=0,rightsum=0;
   
    node n=nil;
    node ntemp;
    int min;
    list_item it,minit;
    edge e=nil;
  
    array<node> anB(B.size()+1);
    array<node> partitionOwners(B.size()+1);
   
    array<list<node> > partitionMembers(B.size()+1);
   
    list<node> newA;
    list<node> temp,temp2;
    list<node> newAA;

    list<edge> elist,elist2;
   
    node_array<int> naiB(G);
    node_array<int> naiA(G);
   
    if ( wolfDebug == 1 ) cout << "\n\nWOLF PH1 STARTS\n---------------------\n";

    i=0;
    forall(n,A){
        naiA[n] = i++;
    }
    if ( wolfDebug == 1 ){
    cout << "A:\n"; i=0; forall(n,A) cout << i << "\t" << n << "\n";
    cout << "naiA:\n"; forall(n,A) cout << "naiA[" << n << "]\t" << naiA[n] << "\n";
    }

    i=1;
    forall(n,B){
        naiB[n] = i;
        anB[i] = n;
        i++;
    }
    if ( wolfDebug == 1 ) {
    cout << "B:\n"; i=0; forall(n,B) cout << i << "\t" << n << "\n";
    cout << "naiB:\n"; forall(n,B) cout << "naiB[" << n << "]\t" << naiB[n] << "\n";
    cout << "anB:\n"; for ( i=0 ; i<B.size()+1 ; i++ ) cout << "anB[" << i << "]\t" << anB[i] << "\n";
    }
    partitionOwners = anB;


    forall(n,A){
        elist = G.adj_edges(n);
        elist2.clear();
        while (elist.size()>0){
            min=B.size()*2;
            forall_items(it,elist){
                e = elist[it];
                ntemp = G.target(e);

                if ( naiB[ntemp] < min ){
                    min = naiB[ntemp];
                    minit = it;
                }
            }
            e = elist[minit];
            elist.del_item(minit);
            elist2.append(e);
        }
        elist = elist2;


        leftsum=0;
        rightsum=0;
        forall(e,elist){
            rightsum += G[e];
        }


        k=0;
        list_item it;
        forall_items(it,elist){
            e = elist[it];
           
            k=naiB[G.target(e)]-1;

            rightsum -= G[e];
            if ( leftsum >= rightsum ){
                break;
            }

            leftsum += G[e];
            if ( elist.succ(it) && naiB[G.target(e)] + 1 == naiB[G.target(elist[elist.succ(it)])] ){
                neighbourWeight = G[elist[elist.succ(it)]];
            }else{
                neighbourWeight = 0;
            }
            if ( leftsum >= rightsum - neighbourWeight ){
                k++;
                break;
            }
        }

        partitionMembers[k].append(n);

    }

   
    if ( wolfDebug == 1 ){
    cout << "Partition Members\n";
    }
    for ( i=0 ; i<partitionMembers.size() ; i++ ){
        temp = partitionMembers[i];
        if ( wolfDebug == 1 ){
        cout << i << "\t=> "; temp.print();cout << "\n";
        }
    }

    if ( wolfDebug == 1 ){
    j=0; forall(n,A) cout << j++ << " => " << n << "\t";
    }
   

    if ( wolfDebug == 1 ) cout << "0. Part -> ";
    forall(n,partitionMembers[0]){
        //G[n] = naiA[n];
        if ( wolfDebug == 1 ) cout << n << " ";
        newA.append(n);
    }

   
    partitionCount=0;
    for ( i=1 ; i<partitionMembers.size() ; i++ ){
        temp = partitionMembers[i];
       
        if ( !temp.empty() ){
            partitionCount++;
        }
        if ( justRunPhase1 == 1 ){
        }else{
            if ( wolfDebug == 1 ) {
            cout << "("<< i <<") Before: "; temp.print(); cout << "\n";
            }
           
            wolfPhase2( G , temp , partitionOwners[i], B );
            if ( wolfDebug == 1 ) {
            cout << "("<< i <<") After: "; temp.print(); cout << "\n";
            }

            forall(n,temp){
                //G[n] = naiA[n];
                cout << "";
                newA.append(n);
            }
        }
    }
   
    A= newA;
}

integer weight(GRAPH<int,int> &G, node alt1, node alt2, node ust, list<node> alt){
    integer sum=0;
    int start=0,end=0,i;

    node n;
    list<edge> outedges = G.adj_edges(ust);
    edge e;
    node_array<int> alt_(G);

    if ( alt1 == nil ){
        return 0;
    }

    i=0;
    forall(n,alt){
        alt_[n] = i;
        if ( n == alt1 ){
            start = i;
        }
        if ( n == alt2 ){
            end = i;
        }
        i++;
    }


    forall(e,outedges){
        n = G.target(e);
        if ( alt_[n] >= start && alt_[n] <= end ){
            sum.operator +=(G[e]);
        }
    }

    return sum;
}

int getCrossings(GRAPH<int,int> &G, list<node> &A, list<node> &B){
    list<node> A_,B_;
    GRAPH<int,int> G_;
    int i;
    node n,newn,tn,corn,cortn;
    edge e;
    array<node> originalA(A.size());
    array<node> originalB(B.size());
    array<node> A__(A.size()),B__(B.size());
    node_array<int> naiB(G);
   
    i=0;
    forall(n,A){
        newn = G_.new_node();
        A_.append(newn);
        A__[i] = newn;
        originalA[i] = n;
        i++;
    }

    i=0;
    forall(n,B){
        newn = G_.new_node();
        B_.append(newn);
        B__[i] = newn;
        naiB[n] = i;
        originalB[i] = n;
        i++;
    }

    for ( i=0 ; i<originalA.size() ; i++ ){
        n = originalA[i];
        corn = A__[i];
        forall_adj_edges(e,n){
            tn = G.target(e);
            cortn = B__[naiB[tn]];
            edge e2 = G_.new_edge(corn,cortn);
            G_[e2] = G[e];
        }
    }

    integer uv = crossingNumber(G_,A_,B_);
    A_.reverse();
    integer vu = crossingNumber(G_,A_,B_);

    if ( uv < vu ){
        return -1;
    }else{
        return 1;
    }
}

void merge(GRAPH<int,int> &G, list<node> &ptRow, list<node> &L, list<node> &R, node owner, list<node> &alt){
       int i=0 , j=0 ,k=0 ;
       int rsize,lsize;
       integer ul,vl,ur,vr ;
       node ownerNext ;
       list_item it;
       ptRow.clear();

       rsize= R.size();
       lsize = L.size();

       for ( i=0 ; i < lsize + rsize ; i++ ){
            if( k < rsize && j < lsize  ){ // bu conditionlar dogru mu ? kontrol 2
                it = alt.search(owner);
                it = alt.succ(it);

                if ( it )
                    ownerNext = alt[it];
                else
                    ownerNext = nil;
                list<node> orderuv;
                orderuv.append(L[L.get_item(j)]);
                orderuv.append(R[R.get_item(k)]);
               
                /*vr = weight(G    ,ownerNext            ,alt.tail()        ,R[R.get_item(k)]    ,alt    ) ;
                vl = weight(G    ,alt.head()            ,owner            ,R[R.get_item(k)]    ,alt    ) ;
                ul = weight(G    ,alt.head()            ,owner            ,L[L.get_item(j)]    ,alt    ) ;
                ur = weight(G    ,ownerNext            ,alt.tail()        ,L[L.get_item(j)]    ,alt    ) ; */
                //cout << "\nvl*ur : " << vl*ur << "\t" << "vr*ul : " << vr*ul  << "\n"  ; 

                int ordering = getCrossings(G,orderuv,alt);
               
                if (   /*( ur*vl ) <= ( vr*ul ) */ ordering == -1  ){
                   
                    ptRow.append( L[L.get_item(j)] );   
                    j++;
                    //cout << "\ncondition 1 \n" ;
                    //ptRow.print();
                    //cout << "\n";
                }else {
                   
                    ptRow.append( R[R.get_item(k)] );
                    k++;
                    //cout << "\ncondition 2 \n" ;
                    //ptRow.print();
                    //cout << "\n";
                }

            }else if ( k >= rsize && j < lsize ){
                ptRow.append( L[L.get_item(j)] );   
                j++;
                //cout << "\ncondition 3 \n" ;
                //ptRow.print();
                //cout << "\n";

           }else if ( j >= lsize && k < rsize ){
                ptRow.append( R[R.get_item(k)] );
                k++;
                //cout << "\ncondition 4 \n" ;  
                //ptRow.print();
                //cout << "\n";
           }
           
        }
      
}

void wolfPhase2(GRAPH<int,int> &G, list<node> &ptRow, node owner ,list<node> alt){
    int size = ptRow.size();
    int half = (int)size/2 ;
    list<node> R, L;
      
    if ( ptRow.size() == 1 || ptRow.size() == 0 ){
        return ;
    }else {
        ptRow.split(ptRow[half],L,R ); 
        wolfPhase2( G , L , owner , alt );
        wolfPhase2( G , R , owner , alt );
        merge ( G, ptRow, L , R , owner , alt) ;
    }
}

integer crossingNumber(matrix &m){
    integer crossingCount = 0,temp;
    int i,j;
    int col,row;
    col = m.dim2();
    row = m.dim1();
    //double d=0.0; //////
    double temp2;
    matrix mc(row,col);

   
    for ( i=0 ; i<row ; i++ ){
        mc(i,col-1) = 0.0;
    }

    for ( i=0 ; i<col ; i++ ){
        mc(0,i) = 0.0;
    }

    double areaA,areaB,areaC;
    for ( i=1 ; i<=row-1 ; i++ ){
        for ( j=col-2 ; j>=0 ; j-- ){
            //mc(i,j) = m(i-1,j+1) + mc(i,j+1) + mc(i-1,j);
            areaA = areaB = areaC = 0.0;
            areaA = mc(i,j+1);
            areaB = mc(i-1,j);
            areaC = mc(i-1,j+1);
            temp2=m(i-1,j+1);
            mc(i,j) = (areaB-areaC) + (areaA-areaC) + areaC +  temp2;
            temp = mc(i,j)*m(i,j);
            crossingCount.operator +=(temp);          

        }
    }

    return crossingCount;
}

integer crossingNumber(GRAPH<int,int> &G, list<node> &A, list<node> &B){
    matrix m=g2m(G,A,B,true);
    return crossingNumber(m);
}

void m2g(GRAPH<int,int> &G, list<node> &A, list<node> &B, matrix &m, bool weighted ){
    /*
     * weight e�er false verilirse, weight'� 5 olan bi edge', 5 defa teker teker ekler. Bu durumda edgein xtype integer de�eri s�f�r olarak atan�r. B�R OLARAK DE��L!!!
     * weight'in default de�eri true'dur, yani weight arguman� verilmez ise, weight true olarak set edilir. Bu durumda yarat�lan graphda, duplicate edgeler olmaz,
     *     Ayn� source ve target'a sahip ka� edge varsa, sadece 1 tane edge olarak eklenir ve o edge'in xtype'� duplication say�s�na ayarlan�r.
     * Arguman olarak verilen Graph, �st layer node listesi (A), ve alt layer node listesi(B) i�ersinde bulunan b�t�n elemanlar temizlenir.
     */
    int i,j;
    int k;
    int row=m.dim1(),col=m.dim2();
    array<node> A1(row),B1(col);
    edge e;
    node sn,tn;
    A.clear();
    B.clear();
    G.clear();
    weighted=true;

    for ( i=0 ; i<row ; i++ ){
        sn = G.new_node();
        A1[i] = sn;
        A.append(sn);
    }

    for ( j=0 ; j<col ; j++ ){
        tn = G.new_node();
        B1[j] = tn;
        B.append(tn);
    }

    if ( weighted ){
        for ( i=0 ; i<row ; i++ ){
            for ( j=0 ; j<col ; j++ ){
                if ( (int)m(i,j) != 0 ){
                    e = G.new_edge(A1[i],B1[j],(int)m(i,j));
                    G[e] = (int)m(i,j);
                }
            }
        }
    }else{
        for ( i=0 ; i<row ; i++ ){
            for ( j=0 ; j<col ; j++ ){
                for ( k=0 ; k<(int)m(i,j) ; k++ ){
                    e = G.new_edge(A1[i],B1[j]);
                    G[e] = 0;
                }
            }
        }
    }
}

matrix g2m(GRAPH<int,int>& G, list<node> A, list<node> B, bool weighted){
    int i,j;
    int row=A.size(),col=B.size();
    matrix m(row,col);
    node_array<int> A1(G),B1(G);
    weighted=true;
    edge e;
    node n;
    for ( i=0 ; i<row ; i++ ){
        for ( j=0 ; j<col ; j++ ){
            m(i,j) = (double)0;
        }
    }
    i=0;
    forall(n,A){
        A1[n] = i;
        i++;
    }

    i=0;
    forall(n,B){
        B1[n] = i;
        i++;
    }

    node sn,tn;
    forall_edges(e,G){
        sn = G.source(e);
        tn = G.target(e);

        if ( weighted ){
            m(A1[sn],B1[tn]) = G[e];
        }else{
            m(A1[sn],B1[tn]) = m(A1[sn],B1[tn]) + (double)1.0;
        }
    }

    return m;
}