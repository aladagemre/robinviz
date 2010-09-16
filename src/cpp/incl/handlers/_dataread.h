// #include "declerations.h"


// ALL tab formatted inputs have the same order for annotation. We limit id, gene names with 128 chars
struct annotation{
// 	#ID = ID from Platform data table
	char id[128];
// 	#Gene title = Entrez Gene name
	char nameEntrez[1024];
// 	#Gene symbol = Entrez Gene symbol
	char symbolEntrez[128];
// 	#Gene ID = Entrez Gene identifier
	char geneIdEntrez[128];
// 	#UniGene title = Entrez UniGene name
	char unigeneNameEntrez[128];
// 	#UniGene symbol = Entrez UniGene symbol
	char unigeneSymbolEntrez[128];
// 	#UniGene ID = Entrez UniGene identifier
	char unigeneIdEntrez[128];
// 	#Nucleotide Title = Entrez Nucleotide title
	char nucleotideIdEntrez[1024];
// 	#GI = GenBank identifier
	char idGenebank[128];
// 	#GenBank Accession = GenBank accession
	char accessionGenebank[128];
// 	#Platform_CLONEID = CLONE_ID from Platform data table
	char cloneId[128];
// 	#Platform_ORF = ORF from Platform data table
	char orf[128];
// 	#Platform_SPOTID = SPOT_ID from Platform data table
	char spotId[128];
// 	#Chromosome location = Entrez gene chromosome and location
	char chromosomeLocation[128];
// 	#Chromosome annotation = Entrez gene chromosome annotation
	char chromosomeAnnotation[128];
// 	#GO:Function = Gene Ontology Function term
	char functermGO[1024];
// 	#GO:Process = Gene Ontology Process term
	char proctermGO[1024];
// 	#GO:Component = Gene Ontology Component term
	char comptermGO[1024];
// 	#GO:Function = Gene Ontology Function identifier
	char funcGO[1024];
// 	#GO:Process = Gene Ontology Process identifier
	char procGO[1024];
// 	#GO:Component = Gene Ontology Component identifier
	char compGO[1024];
};

struct store512{
	char store[512];
};

struct store1024{
	char store[1024];
};

struct store128{
	char store[128];
};

typedef struct store128 S128;
typedef struct store512 S512;
typedef struct store1024 S1024;

matrix dataRead( char filename[256] ){
	leda::matrix INPUT;
	int dimension1, dimension2;
	char filename2[256];
	sprintf( filename2, "%s%s","", filename );
	INPUT = getMatrixFromFile( filename2, 1, dimension1, dimension2 ); 
	return INPUT;
}

matrix dataRead( char filename[256], array<GENES> &geneArray,  array<CONDS> &condArray  ){
	leda::matrix INPUT;
	int dimension1, dimension2;
	char filename2[256];
	sprintf( filename2, "%s%s","", filename );
	INPUT = getMatrixFromFile( filename2, dimension1, dimension2, geneArray, condArray ); 
	return INPUT;
}

void dataReadGEOAnnotation( char filename[256], array<GENES> &geneArray ){
	FILE *fptr;
// 	cout << " READING\n";
	if( (fptr = fopen( filename, "r" )) != NULL ){
		char lineBuffer[10000],line[256];
		while( !feof( fptr ) ){
			fscanf( fptr, "%s", line );
// 			cout << line << endl;
			if( strcmp( line, "!platform_table_begin" ) == 0 ){
				break;
			}
		}
// 		cout << " READING\n";
		list<two_tuple<store512,store512> > annotationList;
		fgets( lineBuffer, 10000, fptr );
// 		cout << lineBuffer << endl;
		while( !feof( fptr ) ){
			fgets( lineBuffer, 10000, fptr );
			if( strcmp( "!platform_table_end", lineBuffer ) == 0 )
				break;
// 			cout << lineBuffer << endl;
// 		// 	#ID = ID from Platform data table
// 			char id[128];
// 		// 	#Gene title = Entrez Gene name
// 			char nameEntrez[1024];
// 		// 	#Gene symbol = Entrez Gene symbol
// 			char symbolEntrez[128];
// 		// 	#Gene ID = Entrez Gene identifier
// 			char geneIdEntrez[128];
// 		// 	#UniGene title = Entrez UniGene name
// 			char unigeneNameEntrez[128];
// 		// 	#UniGene symbol = Entrez UniGene symbol
// 			char unigeneSymbolEntrez[128];
// 		// 	#UniGene ID = Entrez UniGene identifier
// 			char unigeneIdEntrez[128];
// 		// 	#Nucleotide Title = Entrez Nucleotide title
// 			char nucleotideIdEntrez[1024];
// 		// 	#GI = GenBank identifier
// 			char idGenebank[128];
// 		// 	#GenBank Accession = GenBank accession
// 			char accessionGenebank[128];
// 		// 	#Platform_CLONEID = CLONE_ID from Platform data table
// 			char cloneId[128];
// 		// 	#Platform_ORF = ORF from Platform data table
// 			char orf[128];
// 		// 	#Platform_SPOTID = SPOT_ID from Platform data table
// 			char spotId[128];
// 		// 	#Chromosome location = Entrez gene chromosome and location
// 			char chromosomeLocation[128];
// 		// 	#Chromosome annotation = Entrez gene chromosome annotation
// 			char chromosomeAnnotation[128];
// 		// 	#GO:Function = Gene Ontology Function term
// 			char functermGO[1024];
// 		// 	#GO:Process = Gene Ontology Process term
// 			char proctermGO[1024];
// 		// 	#GO:Component = Gene Ontology Component term
// 			char comptermGO[1024];
// 		// 	#GO:Function = Gene Ontology Function identifier
// 			char funcGO[1024];
// 		// 	#GO:Process = Gene Ontology Process identifier
// 			char procGO[1024];
// 		// 	#GO:Component = Gene Ontology Component identifier
// 			char compGO[1024];
			char * pch;
			two_tuple<store512,store512> annotation;
			pch = strtok( lineBuffer, "\t" );
			sprintf( annotation.first().store, "\"%s\"", pch );
// 			cout << annotation.first().store << "\t";
			int tabCount = 1;
			while ( pch != NULL ){
				pch = strtok( NULL, "\t" );
				if( tabCount == 3 ){
					if( pch == NULL ){
						sprintf( annotation.second().store, "%s", "NULL" );
					}
					else{
						sprintf( annotation.second().store, "%s", pch );
					}
					break;
				}
				tabCount++;
			}
// 			cout << annotation.second().store << endl;
			annotationList.append( annotation );
		}
		fclose( fptr );
// 		for( int j = 0; j < annotationList.size(); j++ ){
// 			cout << annotationList[ annotationList.get_item( j ) ].first().store << "\t" << annotationList[ annotationList.get_item( j ) ].second().store << endl;
// 		}
		for( int i = 0; i < geneArray.size(); i++ ){
			cout << geneArray[ i ].GENE << endl;
			for( int j = i; j < annotationList.size(); j++ ){
				if( strcmp( geneArray[ i ].GENE , annotationList[ annotationList.get_item( j ) ].first().store ) == 0 ){
					sprintf( geneArray[ i ].GENE, "%s", annotationList[ annotationList.get_item( j ) ].second().store );
					break;
				}
			}
		}
	}
	else{
		FILE *erptr;
#ifdef LINUX
		erptr = fopen( "outputs/error.txt", "w" );
#else
		erptr = fopen( "outputs//error.txt", "w" );
#endif
		fprintf( erptr, "GEO File read error(annotation file)\n" );
		fclose( erptr );
		cout << "\nGEO File read error(annotation file)\n";
		exit(1);
	}
}

// GEO file parser, according to time series format of GEO
matrix dataReadGEO( char filename[256], char annotName[256], array<GENES> &geneArray, array<CONDS> &condArray, int option  ){
// 	leda::matrix INPUT;
	int dimension1, dimension2;
	char filename2[256];
	sprintf( filename2, "%s", filename );
	FILE *fptr = fopen( filename2, "r" );
	
	// Reading Series Header
	char readData1[1024],readData2[1024],title[1024],platformId[32],platformTaxId[32],seriesType[512],seriesSampleId[1024];
	while( !feof( fptr ) ){
		fscanf( fptr, "%s", readData1 );
		if( strcmp( "!Series_title", readData1 ) == 0 ){
			fscanf( fptr, "%s", title );
		}
		else{
			if( strcmp( "!Series_platform_id", readData1 ) == 0 ){
				fscanf( fptr, "%s", platformId );
			}
			else{
				if( strcmp( "!Series_platform_taxid", readData1 ) == 0 ){
					fscanf( fptr, "%s", platformTaxId );
				}
				else{
					if( strcmp( "!Series_type", readData1 ) == 0 ){
						fscanf( fptr, "%s", seriesType );
					}
					else{
						if( strcmp( "!Series_sample_id", readData1 ) == 0 ){
							fscanf( fptr, "%s", seriesSampleId );
						}
						else{
							if( strcmp( "!Series_sample_taxid", readData1 ) == 0 ){
								fscanf( fptr, "%s", readData2 );
								break;
							}
						}
					}
				}
			}
		}
	}

	// Reading Samples Header
	list<S512> sampleTitle, sampleOrganism;
	list<S1024> sampleGeo, sampleDescription;
	int samples,genes;
	while( !feof( fptr ) ){
		fscanf( fptr, "%s", readData1 );
		if( strcmp( "!Sample_title", readData1 ) == 0 ){
			sprintf( readData2, "%s", readData1 );
			int count = 0;
			while( strcmp( "!Sample_geo_accession", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
				if( strcmp( "!Sample_geo_accession", readData2 ) == 0 )
					break;
				S512 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleTitle.append( tmp );
				count++;
			}
			samples = count;
			list_item it;
// 			forall_items( it ,sampleTitle ){
// 				cout << sampleTitle[ it ].store << "\t";
// 			}
// 			cout << "\n________________________\n";
			while( strcmp( "!Sample_status", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
				if( strcmp( "!Sample_status", readData2 ) == 0 )
					break;
				S1024 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleGeo.append( tmp );
// 				cout << readData2 << "\t";
			}

// 			cout << endl;
// 			forall_items( it ,sampleGeo ){
// 				cout << sampleGeo[ it ].store << "\t";
// 			}
// 			cout << "\n________________________\n";

			while( strcmp( "!Sample_organism_ch1", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
//  				cout << readData2 << "\t";
			}
			int count2 = 0;
			while( true ){
				fscanf( fptr, "%s", readData2 );
				if( strcmp( "!Sample_characteristics_ch1", readData2 ) == 0 || strcmp( "!Sample_molecule_ch1", readData2 ) == 0 )
					break;
				S512 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleOrganism.append( tmp );
// 				cout << readData2 << "\t";
				count2++;
			}

// 			cout << endl;
// 			forall_items( it ,sampleOrganism ){
// 				cout << sampleOrganism[ it ].store << "\t";
// 			}
// 			cout << "\n________________________\n";

			while( strcmp( "!Sample_description", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
			}
			count2 = 0;
			while( count2 != count - 1){
				fscanf( fptr, "%s", readData2 );
				S1024 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleDescription.append( tmp );
				count2++;
			}
	
// 			cout << endl;
// 			forall_items( it ,sampleDescription ){
// 				cout << sampleDescription[ it ].store << "\t";
// 			}

			while( strcmp( "!series_matrix_table_begin", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
			}
			break;
		}
		else{
			FILE *erptr;
#ifdef LINUX
			erptr = fopen( "outputs/error.txt", "w" );
#else
			erptr = fopen( "outputs//error.txt", "w" );
#endif
			fprintf( erptr, "GEO File read error(series matrix) while reading Samples Header \n" );
			fclose( erptr );
			cout << "\nGEO File read error(series matrix) while reading Samples Header\n";
			exit(1);
		}
	}
	fscanf( fptr, "%s", readData2 ); // "ID_REF"
	list<list<double> > rows;
	list<S512> geneAnnot;
	double value;
	for( int i = 0; i < samples; i++ ){
		  fscanf( fptr, "%s", readData2 );
// 		  cout << readData2 << "\t";
	}
// 	cout << "\n________________________\n";
	while( true ){
// 		cout << readData1;
		fscanf( fptr, "%s", readData1 );
		if( strcmp( "!series_matrix_table_end", readData1 ) == 0 )
			break;
		S512 tmp;
		sprintf( tmp.store, "%s", readData1 );
		geneAnnot.append( tmp );
// 		cout << "\n" << readData1 << "\t";
		list<double> values;
		for( int i = 0; i < samples; i++ ){
			fscanf( fptr, "%lf", &value );
// 			cout << value << "\t";
			values.append( value );
		}
		rows.append( values );
// 		cout << endl;
	}
// 	cout << "\n________________________\n";
	genes = rows.size();

	geneArray.resize( genes );
	condArray.resize( samples );
	leda::matrix INPUT( genes, samples );
// 	cout << genes << endl << samples << endl;
	for( int j = 0; j < samples; j++ ){
		sprintf( condArray[ j ].COND, "%s", sampleTitle[ sampleTitle.get_item( j ) ].store );
	}
	for( int i = 0; i < genes; i++ ){
		sprintf( geneArray[ i ].GENE, "%s", geneAnnot[ geneAnnot.get_item( i ) ].store );
		list<double> values = rows[ rows.get_item( i ) ];
		for( int j = 0; j < samples; j++ ){
			INPUT( i, j ) = values[ values.get_item( j ) ];
		}
	}	
// 	INPUT.print();
	if( option == 1 ){
		dataReadGEOAnnotation( annotName, geneArray );
	}
	else{
		if( option == 2 ){
			;
		}
	}
	return INPUT;
}

void dataWriteBimax( matrix &M, int low_dim1, int low_dim2, double ther ){
	FILE *fptr;
#ifdef LINUX
	fptr = fopen( "src/cpp/incl/bicalg/bimax/matrix_robin.txt", "w" );
#else
	fptr = fopen( "src//cpp//incl//bicalg//bimax//matrix_robin.txt", "w" );
#endif
	fprintf( fptr, "%d %d %d %d\n", M.dim1(), M.dim2(), low_dim1, low_dim2 );
	double max = M( 0, 0 );
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( M( i, j ) > max )
			    max = M( i, j );
		}
	}
// 	cout << max << endl;
	double min = max;
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( M( i, j ) < min )
			    min = M( i, j );
		}
	}
// 	cout << min << endl;
	double difference = max - min;
// 	cout << difference << endl;
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( M( i, j ) < max - difference * ther ){
				fprintf( fptr, " %d", 0 );
			}
			else{
				fprintf( fptr, " %d", 1 );
			}
		}
		fprintf( fptr, "\n" );
	}
	fclose( fptr );
}


void dataWriteCC( matrix &M ){
	FILE *fptr;
#ifdef LINUX
	fptr = fopen( "src/cpp/incl/bicalg/cc/matrix_robin.txt", "w" );
#else
	fptr = fopen( "src//cpp//incl//bicalg//cc//matrix_robin.txt", "w" );
#endif
	for( int i = 0; i < M.dim1(); i++ ){
		for( int j = 0; j < M.dim2(); j++ ){
			if( j != M.dim2() -1 )
				fprintf( fptr, "%d ", (int)(M(i,j)*10) );
			else
				fprintf( fptr, "%d", (int)(M(i,j)*10) );
		}
		fprintf( fptr, "\n" );
	}
	fclose( fptr );
}