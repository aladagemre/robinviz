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


#define IsArc_(x,y)                 (mCross[x][y] < mCross[y][x])
#define Weight_(x,y)                (mCross[y][x] - mCross[x][y])

enum { NONE, DOING, DONE };


    void WDFS( node inRoot , bool &thruRemoved , node_array<int> &theVisited , node_array<int> &theParent, int &thruPenaltyArcs);
    void WDFSOrder ( node inRoot , node_array<int> &theVisited , int &inPosition );
    void index();
    integer crossCount ( node u , node v );
    void BuildCrossTable ();
    void initGlobals();
    void RunPMAlgorithm();
    ////void barycenter_calc_for_median( matrix& m,int a, int b );
    void Partition_Preprocess( matrix& m, matrix& median_index, matrix& partition );

    /*** GLOBALS of PM ***/
    ofstream afile ;
    ofstream pmfile ;
    ifstream pmin ;
    list<node> AG , BG , Apm ;
    GRAPH<int,int> Gpm ;

    node_array<int> indexApm(Gpm);
    node_array<int> indexBpm(Gpm);
    node_array<int> indexApm2(Gpm);

    array <node> arrayApm( AG.size() ) , arrayApm2 ( AG.size() ) ;
    
    int theRuns ;
    
    node_matrix<integer> mCross (Gpm) ;
    /*** END OF GLOBALS / pm ***/

    /***********    PM    functions        ****************************************/
    integer minimum ( integer a , integer b){
        if ( a < b ) return a ;
        else return b ;
    }

   

    
    void printOrder( node_array<int> index ){
        node mahmut ; // AG listesini iterate edip indexlerini bas�o , indexApm ye bag�ml� yani bast�g� say�lar
        forall(mahmut,AG){
           afile << index[mahmut] << " " ;
        }    
        afile << "\n\n" ;
        
        
    }

    void orderAG( list<node> &newA ){
        node n ;
        int i = 0 ;
        array <int> order( AG.size() ) ;
        
        forall(n,AG){
           order[i] = indexApm2[n]  ;
           i++;
        }
        //order.print();

        newA.clear();
        for( int i = 0 ; i < order.size() ; i++ )
            newA.append ( arrayApm[ order[i] ] ) ;            
    }
    
    

    

    void writeTolog (){
        
        list<edge> out ;     
        afile << AG.size() << " " << BG.size() << "\n";

        node n ;
        forall( n , AG ){
            out = Gpm.out_edges(n) ;
            
            edge e ;
            forall ( e , out ){
                    afile << indexApm[ Gpm.source(e)] << " " << AG.size() + indexBpm[ Gpm.target(e)] << "\n" ;
            }

        }
    }    

    void WDFS( node inRoot , bool &thruRemoved , node_array<int> &theVisited , node_array<int> &theParent, int &thruPenaltyArcs){

        integer  theCycleLength, theMinWeight ;
        node theAdj , theStart , theEnd ;
        int j , i ;
        theVisited[ inRoot ] = DOING ;

#ifdef DEBUG
        afile << "\tvisiting :: " ;
        afile << indexApm[inRoot] << " \n";

        afile << "\tcalled from  :: " ;
        afile <<  theParent[inRoot]<< " \n\n" ;
#endif

        for( i = 0 ; i < AG.size() ; i++ ){
            theAdj = arrayApm[i];
            if ( IsArc_(inRoot,theAdj) ){
                thruPenaltyArcs++;
            
                if (theVisited[theAdj] == DOING) {
                    
                    theStart = inRoot ;
                    theEnd = theAdj ;
                    theMinWeight = Weight_( theStart , theEnd );
                    
                    for( j = indexApm[inRoot] ; j != i  ; j = theParent[  arrayApm[ j ]  ]  ){

                        if ( ! IsArc_( arrayApm [ theParent[  arrayApm[ j ]  ] ] , arrayApm[j] ) )
                            break ;
                        if ( Weight_( arrayApm [ theParent[  arrayApm[ j ]  ] ] , arrayApm[ j ] ) < theMinWeight ){
                            theMinWeight = Weight_( arrayApm [ theParent[ arrayApm[ j ] ] ] , arrayApm[j] );
                            theStart = arrayApm[ theParent [ arrayApm[j] ] ];
                            theEnd = arrayApm[j] ;
                        }
                    }

                    if ( j == i ) {
                        integer theCrossSE(mCross[theStart][theEnd]);//////
                        integer theCrossES(mCross[theEnd][theStart]);//////

                        thruRemoved = true ;

                        theCycleLength = 1 ;

                        for ( j = indexApm[inRoot] ; j != i ;  j = theParent[  arrayApm[ j ]  ]   ){

                            theCycleLength++;
                            mCross[ arrayApm[j] ] [ arrayApm[ theParent[  arrayApm[ j ] ] ] ]  += theCrossSE ;
                            mCross[ arrayApm[ theParent[  arrayApm[ j ] ] ] ] [ arrayApm[j] ]  += theCrossES ;

#ifdef DEBUG
                        if(  Weight_(  arrayApm[ theParent[  arrayApm[ j ] ] ] ,  arrayApm[ j ] ) == 0  )
                            afile << "---- Removed arc :: " << theParent[  arrayApm[ j ] ] << " , " << j << "of weight " << theMinWeight <<"\n\n" ;
#endif    
                        }
                        mCross[theAdj][inRoot] += theCrossSE ;
                        mCross[inRoot][theAdj] += theCrossES ;
#ifdef DEBUG
                    if (theStart==inRoot && theEnd==theAdj && Weight_(inRoot,theAdj) == 0 )
                          afile << "---Removed arc (" << indexApm[inRoot] <<" , " << indexApm[theAdj] << ") of weight " << theMinWeight << "\n" ;
                        
                    afile << "\t---Cycle length is ::" << theCycleLength << "\n";
                    printNodeMatrix(mCross , AG ) ;
#endif
                    }
                }
                else if ( theVisited[theAdj] == NONE ){
                    theParent[theAdj] = indexApm[inRoot] ;
                    WDFS(theAdj,thruRemoved,theVisited,theParent,thruPenaltyArcs);
                }
            }
    }
        
#ifdef parent
        afile << "in wdfs :: " ;
        printNodeArray( theVisited , "the visited:: ") ;

        afile << "in wdfs :: " ;
        printNodeArray( theParent , "the parent :: ") ;
#endif

        theVisited[inRoot] = DONE ;
    }

    void WDFSOrder ( node inRoot , node_array<int> &theVisited , int &inPosition ) {
            int theAdj ;

            theVisited[inRoot] = DONE;
#ifdef DEBUG
            afile << "\tinposition :: " << inPosition << "\n";    
#endif

#ifdef parent
            afile << "in order :: " ;
            printNodeArray(theVisited , "theVisited : ");
#endif
            for( theAdj = 0 ; theAdj < AG.size() ; theAdj++ ){
                if( theVisited[ arrayApm[theAdj] ] == NONE && IsArc_(inRoot,arrayApm[theAdj]) )
                    WDFSOrder(arrayApm[theAdj] , theVisited , inPosition );
            }
            
            arrayApm2[ indexApm[inRoot] ] = arrayApm [ AG.size() -( inPosition ) - 1  ] ;
            // if ( inPosition < AG.size()-1 )
            inPosition++ ;
    }

    void index2(){
        
        int i ;
        
        for ( i = 0  ; i < arrayApm2.size() ; i++ ){
             indexApm2 [ arrayApm2[ i ] ] = i ;
        }
    
    }

    void index(){
        node n ;
        int i = 0 ;
        
        forall(n,AG){
            indexApm[n] = i ;
            i++;
        }
        i = 0 ;
        forall(n,AG){
            indexApm2[n] = i ;
            i++;
        }    
        i = 0 ;
        forall(n,BG){
            indexBpm[n] = i ;
            i++;
        }

        i = 0;
        forall(n,AG){
            arrayApm[i] = n ;
            i++;
        }

        i = 0 ;
        forall_nodes(n,Gpm){
            Gpm[n]=i ;
            i++;
        }
    }

	
    void destroyCrossTable(){
		node u,v ;    
		forall(u,AG)
			forall(v,AG)
				mCross(u,v) = 0 ;
    }
    void initGlobals(){

        indexApm2.init(Gpm);
        indexApm.init(Gpm);
        indexBpm.init(Gpm);
        //mCross.init(Gpm);
        arrayApm.resize(AG.size());
        arrayApm2.resize(AG.size());

    }
    void pm() {
    
            integer originalCross , nowCross ;
            initGlobals();
            
            int thePenaltyArcs;
            bool theRemoved ;
            node_array<int> theVisited(Gpm) , theParent(Gpm) ;
            int  theSortCounter = 0  ;
            //float theStartTime = 0 ; //////
            index();
            theRuns = 0 ;
#ifdef time         
            time_t t0 = time (0);
            // afile <<  "start time ::" << t0 << "\n";
#endif
#ifdef write
            writeToFileForPM(Gpm,AG,BG);    
#endif
//Repeat : //////
            afile << "********************************************\n" ;
            afile << "REPEAT " << theRuns << "\n";
            afile << "********************************************\n\n" ;

            thePenaltyArcs     = 0;
            //BuildCrossTable (Gpm,AG,mCross,indexBpm);

#ifdef DEBUG
           
            afile << "indexApm at the start ::" ; // 0 1 2 3 4 5 die s�ral� oarak bas�cak initial cond.
            printOrder(indexApm);
                       
            printNodeMatrix( mCross , AG ); // initial cross table
            afile << "\nFirst Visit ::: \n" ;
#endif
            theRemoved = false ;
            
            node n ;
            forall(n,AG){
                theVisited[n] = NONE ;
                theParent[n] = -1 ;
            }
            
            
            for(int i = 0 ; i < arrayApm.size() ; i++ ){
                node n = arrayApm[i] ;
                if ( theVisited[ n ] == NONE )
                    WDFS( n , theRemoved ,theVisited , theParent , thePenaltyArcs );
            }
                       
            while ( theRemoved == true ) {
#ifdef DEBUG
                afile << "\n\nNew Visit ::: \n" ;
#endif    
                theRemoved = false ;
                
                /* clear theVisited */
                forall(n,AG){
                    theVisited[n] = NONE ;
                    theParent[n] = -1 ;
                }

                for(int i = 0 ; i < arrayApm.size() ; i++ ){
                    node n = arrayApm[i] ;
                    if ( theVisited[ n ] == NONE )
                        WDFS( n , theRemoved , theVisited , theParent , thePenaltyArcs );
                }
            
            }
            theSortCounter = 0 ;

#ifdef DEBUG
            afile << "\n\nLast Visit ::: \n" ;
#endif            
            forall(n,AG){ // memset replacement
                theVisited[n] = NONE ;
            }

            for(int i = 0 ; i < arrayApm.size() ; i++ ){
                node n = arrayApm[i] ;
                if ( theVisited[n] == NONE )
                    WDFSOrder( n , theVisited , theSortCounter );
            }

            
#ifdef   parent    
             
            printNodeArray(theVisited , "\ntheVisited:: " );
            printNodeArray(theParent , "theParent :: " );
    

#endif            
        
#ifdef time             
            time_t t1 = time (0);
            
            //afile << "end time ::" << t1 << "\n";
#endif
                        
            list<node> AAA ;
            index2();
            orderAG( AAA ) ;
            AG = AAA ;
			/*GRAPH<int,int> GGG;
			GGG.clear();
			mCross.init(GGG);*/

#ifdef DEBUG
             afile << "\nCross count with ( AAA ) :: " << nowCross << "\n\n" ;
        
#endif                    
            // BuildCrossTable();
            //printNodeMatrix( mCross , AG );
}
