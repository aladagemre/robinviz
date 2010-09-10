// #include "declerations.h"

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

// GEO file parser, according to time series format of GEO
/*!Series_title   "Cigarette Smoking Induces Overexpression of a Fat
Depleting Gene AZGP1 in the Human Airway Epithelium"
!Series_geo_accession   "GSE10135"
!Series_status  "Public on Jan 06 2009"
!Series_submission_date "Jan 10 2008"
!Series_last_update_date        "Mar 24 2009"
!Series_pubmed_id       "19188554"
!Series_web_link        "http://www.ncbi.nlm.nih.gov/pubmed/19188554?ordinalpos=1&itool=EntrezSytem2.PEntrez.Pubmed.Pubmed_ResultsPanel.Pubmed_DefaultReportPanel.Pubmed_RVDocSum"
!Series_summary "Smokers weigh less and have less body fat than
non-smokers, and increased body fat and weight gain are observed
following smoking cessation. To assess a possible molecular mechanism
underlying the inverse association between smoking and body weight, we
hypothesized that smoking may induce the expression of a fat depleting
gene in the airway epithelium, the cell population that takes the
brunt of the stress of cigarette smoke. As a candidate gene we
evaluated the expression of alpha2-zinc-glycoprotein1 (AZGP1), a
soluble protein that stimulates lipolysis, induces a reduction in body
fat in mice and is associated with the cachexia related to cancer, and
is known to be expressed in secretory cells of lung epithelium. To
assess if smoking upregulates AZGP1 expression, microarray analysis
with TaqMan confirmation was used to evaluate large airway epithelial
samples obtained by fiberoptic bronchoscopy from 37 normal smokers and
55 normal nonsmokers. Both microarray and TaqMan analysis demonstrated
that AZGP1 mRNA levels were higher in the large airway epithelium of
normal smokers compared to normal nonsmokers (p<0.05, all
comparisons). Western analysis of airway biopsies of smokers compared
with nonsmokers demonstrated upregulation of AZGP1 at the protein
level, and immunohistochemical analysis demonstrated upregulation of
AZGP1 in secretory as well as neuroendocrine cells of smokers. In the
context that AZGP1 is involved in lipolysis and fat loss, its
overexpression in the airway epithelium of chronic smokers may
represent one mechanism for the weight difference in smokers vs
nonsmokers."
!Series_summary "Keywords: Comparison of gene expression in large
airway epithleium of normal non-smokers and normal smokers"
!Series_overall_design  "Airway epithelial cells obtained by
bronchoscopy with brushing from healthy smokers and healthy
non-smokers"
!Series_type    "Expression profiling by array"
!Series_contributor     "Holly,,Vanni"
!Series_contributor     "Angeliki,,Kazeros"
!Series_contributor     "Ben-Gary,,Harvey"
!Series_contributor     "Barbara,,Ferris"
!Series_contributor     "Bishnu,P,De"
!Series_contributor     "Brendan,J,Carolan"
!Series_contributor     "Ralf-H,,HÃ¼bner"
!Series_contributor     "Ronald,G,Crystal"
!Series_sample_id       "GSM114035 GSM114037 GSM114039 GSM114041 GSM114043
GSM114045 GSM114047 GSM114049 GSM114051 GSM114053 GSM114055 GSM114057
GSM114059 GSM114061 GSM114063 GSM114065 GSM114067 GSM114069 GSM114071
GSM114073 GSM114075 GSM114077 GSM114078 GSM114079 GSM114080 GSM114081
GSM114082 GSM114083 GSM114084 GSM114085 GSM114086 GSM114087 GSM114088
GSM252799 GSM252800 GSM252801 GSM252802 GSM252803 GSM252804 GSM252805
GSM252806 GSM252807 GSM252808 GSM252810 GSM252811 GSM252812 GSM252813
GSM252814 GSM252815 GSM252816 GSM252817 GSM252818 GSM252819 GSM252820
GSM252822 GSM252823 GSM252824 GSM252825 GSM252826 GSM252827 GSM256181
GSM256182 GSM256183 GSM256184 GSM256185 GSM256186 GSM256187 GSM256188
GSM256189 GSM256190 GSM256191 GSM256192 GSM256193 GSM256194 GSM256195
GSM256196 GSM256197 GSM256198 GSM256199 GSM256200 GSM256201 GSM256202
GSM256203 GSM256204 GSM256205 GSM256206 GSM256207 GSM256208 GSM256209
GSM256210 GSM256211 GSM256212 "
!Series_contact_name    "Yael,,Strulovici-Barel"
!Series_contact_email   "yas2003@med.cornell.edu"
!Series_contact_phone   "646-962-5560"
!Series_contact_laboratory      "Crystal"
!Series_contact_department      "Department of Genetic Medicine"
!Series_contact_institute       "Weill Cornell Medical College"
!Series_contact_address "1300 York Avenue"
!Series_contact_city    "New York "
!Series_contact_state   "NY"
!Series_contact_zip/postal_code "10021"
!Series_contact_country "USA"
!Series_supplementary_file      "ftp://ftp.ncbi.nih.gov/pub/geo/DATA/supplementary/series/GSE10135/GSE10135_RAW.tar"
!Series_platform_id     "GPL80"
!Series_platform_id     "GPL96"
!Series_platform_id     "GPL570"
!Series_platform_taxid  "9606"
!Series_sample_taxid    "9606"

!Sample_title   "large airways, smoker 1, MAS5" "large airways, smoker
2, MAS5"        "large airways, smoker 3, MAS5" "large airways, smoker 4,
MAS5"   "large airways, smoker 5, MAS5" "large airways, smoker 6,
MAS5"   "large airways, non-smoker 1, MAS5"     "large airways, non-smoker
2, MAS5"        "large airways, non-smoker 3, MAS5"     "large airways,
non-smoker 4, MAS5"     "large airways, non-smoker 5, MAS5"
!Sample_geo_accession   "GSM114078"     "GSM114079"     "GSM114080"     "GSM114081"     "GSM114082"     "GSM114083"     "GSM114084"     "GSM114085"     "GSM114086"     "GSM114087"     "GSM114088"
!Sample_status  "Public on Nov 21 2006" "Public on Nov 21 2006" "Public
on Nov 21 2006" "Public on Nov 21 2006" "Public on Nov 21
2006"   "Public on Nov 21 2006" "Public on Nov 21 2006" "Public on Nov
21 2006"        "Public on Nov 21 2006" "Public on Nov 21 2006" "Public on
Nov 21 2006"
!Sample_submission_date "Jun 12 2006"   "Jun 12 2006"   "Jun 12 2006"   "Jun
12 2006"        "Jun 12 2006"   "Jun 12 2006"   "Jun 12 2006"   "Jun 12 2006"   "Jun
12 2006"        "Jun 12 2006"   "Jun 12 2006"
!Sample_last_update_date        "Nov 21 2006"   "Nov 21 2006"   "Nov 21
2006"   "Nov 21 2006"   "Nov 21 2006"   "Nov 21 2006"   "Nov 21 2006"   "Nov 21
2006"   "Nov 21 2006"   "Nov 21 2006"   "Nov 21 2006"
!Sample_type    "RNA"   "RNA"   "RNA"   "RNA"   "RNA"   "RNA"   "RNA"   "RNA"   "RNA"   "RNA"   "RNA"
!Sample_channel_count   "1"     "1"     "1"     "1"     "1"     "1"     "1"     "1"     "1"     "1"     "1"
!Sample_source_name_ch1 "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"      "airway epithelial cells obtained by
bronchoscopy and brushing"
!Sample_organism_ch1    "Homo sapiens"  "Homo sapiens"  "Homo
sapiens"        "Homo sapiens"  "Homo sapiens"  "Homo sapiens"  "Homo
sapiens"        "Homo sapiens"  "Homo sapiens"  "Homo sapiens"  "Homo sapiens"
!Sample_characteristics_ch1     "Age: 33"       "Age: 37"       "Age: 48"       "Age:
38"     "Age: 42"       "Age: 36"       "Age: 29"       "Age: 34"       "Age: 31"       "Age: 29"       "Age:
38"
!Sample_characteristics_ch1     "Sex: F"        "Sex: F"        "Sex: M"        "Sex: M"        "Sex:
M"      "Sex: M"        "Sex: F"        "Sex: M"        "Sex: M"        "Sex: F"        "Sex: M"
!Sample_characteristics_ch1     "Ethnic group: white"   "Ethnic group:
black"  "Ethnic group: white"   "Ethnic group: white"   "Ethnic group:
black"  "Ethnic group: hispanic"        "Ethnic group: hispanic"        "Ethnic
group: black"   "Ethnic group: white"   "Ethnic group: black"   "Ethnic
group: white"
!Sample_characteristics_ch1     "Smoking Status: smoker, 20
pack-years"     "Smoking Status: smoker, 23 pack-years" "Smoking Status:
smoker, 30 pack-years"  "Smoking Status: smoker, 20
pack-years"     "Smoking Status: smoker, 24 pack-years" "Smoking Status:
smoker, 25 pack-years"  "Smoking Status: non-smoker"    "Smoking Status:
non-smoker"     "Smoking Status: non-smoker"    "Smoking Status:
non-smoker"     "Smoking Status: non-smoker"
!Sample_molecule_ch1    "total RNA"     "total RNA"     "total RNA"     "total
RNA"    "total RNA"     "total RNA"     "total RNA"     "total RNA"     "total
RNA"    "total RNA"     "total RNA"
!Sample_extract_protocol_ch1    "Trizol extraction and RNAeasy clean-up
of total RNA was performed according to the manufacturer's
instructions."  "Trizol extraction and RNAeasy clean-up of total RNA
was performed according to the manufacturer's instructions."    "Trizol
extraction and RNAeasy clean-up of total RNA was performed according
to the manufacturer's instructions."    "Trizol extraction and RNAeasy
clean-up of total RNA was performed according to the manufacturer's
instructions."  "Trizol extraction and RNAeasy clean-up of total RNA
was performed according to the manufacturer's instructions."    "Trizol
extraction and RNAeasy clean-up of total RNA was performed according
to the manufacturer's instructions."    "Trizol extraction and RNAeasy
clean-up of total RNA was performed according to the manufacturer's
instructions."  "Trizol extraction and RNAeasy clean-up of total RNA
was performed according to the manufacturer's instructions."    "Trizol
extraction and RNAeasy clean-up of total RNA was performed according
to the manufacturer's instructions."    "Trizol extraction and RNAeasy
clean-up of total RNA was performed according to the manufacturer's
instructions."  "Trizol extraction and RNAeasy clean-up of total RNA
was performed according to the manufacturer's instructions."
!Sample_label_ch1       "biotin"        "biotin"        "biotin"        "biotin"        "biotin"        "biotin"        "biotin"        "biotin"        "biotin"        "biotin"        "biotin"
!Sample_label_protocol_ch1      "Biotinylated cRNA were prepared according
to the standard Affymetrix protocol from 6 microg total
RNA."   "Biotinylated cRNA were prepared according to the standard
Affymetrix protocol from 6 microg total RNA."   "Biotinylated cRNA were
prepared according to the standard Affymetrix protocol from 6 microg
total RNA."     "Biotinylated cRNA were prepared according to the standard
Affymetrix protocol from 6 microg total RNA."   "Biotinylated cRNA were
prepared according to the standard Affymetrix protocol from 6 microg
total RNA."     "Biotinylated cRNA were prepared according to the standard
Affymetrix protocol from 6 microg total RNA."   "Biotinylated cRNA were
prepared according to the standard Affymetrix protocol from 6 microg
total RNA."     "Biotinylated cRNA were prepared according to the standard
Affymetrix protocol from 6 microg total RNA."   "Biotinylated cRNA were
prepared according to the standard Affymetrix protocol from 6 microg
total RNA."     "Biotinylated cRNA were prepared according to the standard
Affymetrix protocol from 6 microg total RNA."   "Biotinylated cRNA were
prepared according to the standard Affymetrix protocol from 6 microg
total RNA."
!Sample_taxid_ch1       "9606"  "9606"  "9606"  "9606"  "9606"  "9606"  "9606"  "9606"  "9606"  "9606"  "9606"
!Sample_hyb_protocol    "Following fragmentation, 15 microg of cRNA were
hybridized for 16 hr at 45C on GeneChip HG-U133A. GeneChips were
washed and stained in the Affymetrix Fluidics Station 450."     "Following
fragmentation, 15 microg of cRNA were hybridized for 16 hr at 45C on
GeneChip HG-U133A. GeneChips were washed and stained in the Affymetrix
Fluidics Station 450."  "Following fragmentation, 15 microg of cRNA
were hybridized for 16 hr at 45C on GeneChip HG-U133A. GeneChips were
washed and stained in the Affymetrix Fluidics Station 450."     "Following
fragmentation, 15 microg of cRNA were hybridized for 16 hr at 45C on
GeneChip HG-U133A. GeneChips were washed and stained in the Affymetrix
Fluidics Station 450."  "Following fragmentation, 15 microg of cRNA
were hybridized for 16 hr at 45C on GeneChip HG-U133A. GeneChips were
washed and stained in the Affymetrix Fluidics Station 450."     "Following
fragmentation, 15 microg of cRNA were hybridized for 16 hr at 45C on
GeneChip HG-U133A. GeneChips were washed and stained in the Affymetrix
Fluidics Station 450."  "Following fragmentation, 15 microg of cRNA
were hybridized for 16 hr at 45C on GeneChip HG-U133A. GeneChips were
washed and stained in the Affymetrix Fluidics Station 450."     "Following
fragmentation, 15 microg of cRNA were hybridized for 16 hr at 45C on
GeneChip HG-U133A. GeneChips were washed and stained in the Affymetrix
Fluidics Station 450."  "Following fragmentation, 15 microg of cRNA
were hybridized for 16 hr at 45C on GeneChip HG-U133A. GeneChips were
washed and stained in the Affymetrix Fluidics Station 450."     "Following
fragmentation, 15 microg of cRNA were hybridized for 16 hr at 45C on
GeneChip HG-U133A. GeneChips were washed and stained in the Affymetrix
Fluidics Station 450."  "Following fragmentation, 15 microg of cRNA
were hybridized for 16 hr at 45C on GeneChip HG-U133A. GeneChips were
washed and stained in the Affymetrix Fluidics Station 450."
!Sample_scan_protocol   "GeneChips were scanned using the GeneChip
Scanner 3000 7G"        "GeneChips were scanned using the GeneChip Scanner
3000 7G"        "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000
7G"     "GeneChips were scanned using the GeneChip Scanner 3000 7G"
!Sample_description     "large airways, smoker 1, MAS5" "large airways,
smoker 2, MAS5" "large airways, smoker 3, MAS5" "large airways, smoker
4, MAS5"        "large airways, smoker 5, MAS5" "large airways, smoker 6,
MAS5"   "large airways, non-smoker 1, MAS5"     "large airways, non-smoker
2, MAS5"        "large airways, non-smoker 3, MAS5"     "large airways,
non-smoker 4, MAS5"     "large airways, non-smoker 5, MAS5"
!Sample_data_processing "The data were analyzed with Microarray Suite
version 5.0 (MAS 5.0) using Affymetrix default analysis settings and
global scaling as normalization method."        "The data were analyzed with
Microarray Suite version 5.0 (MAS 5.0) using Affymetrix default
analysis settings and global scaling as normalization method."  "The
data were analyzed with Microarray Suite version 5.0 (MAS 5.0) using
Affymetrix default analysis settings and global scaling as
normalization method."  "The data were analyzed with Microarray Suite
version 5.0 (MAS 5.0) using Affymetrix default analysis settings and
global scaling as normalization method."        "The data were analyzed with
Microarray Suite version 5.0 (MAS 5.0) using Affymetrix default
analysis settings and global scaling as normalization method."  "The
data were analyzed with Microarray Suite version 5.0 (MAS 5.0) using
Affymetrix default analysis settings and global scaling as
normalization method."  "The data were analyzed with Microarray Suite
version 5.0 (MAS 5.0) using Affymetrix default analysis settings and
global scaling as normalization method."        "The data were analyzed with
Microarray Suite version 5.0 (MAS 5.0) using Affymetrix default
analysis settings and global scaling as normalization method."  "The
data were analyzed with Microarray Suite version 5.0 (MAS 5.0) using
Affymetrix default analysis settings and global scaling as
normalization method."  "The data were analyzed with Microarray Suite
version 5.0 (MAS 5.0) using Affymetrix default analysis settings and
global scaling as normalization method."        "The data were analyzed with
Microarray Suite version 5.0 (MAS 5.0) using Affymetrix default
analysis settings and global scaling as normalization method."
!Sample_platform_id     "GPL96" "GPL96" "GPL96" "GPL96" "GPL96" "GPL96" "GPL96" "GPL96" "GPL96" "GPL96" "GPL96"
!Sample_contact_name    "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"        "Yael,,Strulovici-Barel"
!Sample_contact_email   "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"       "yas2003@med.cornell.edu"
!Sample_contact_phone   "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"  "646-962-5560"
!Sample_contact_laboratory      "Crystal"       "Crystal"       "Crystal"       "Crystal"       "Crystal"       "Crystal"       "Crystal"       "Crystal"       "Crystal"       "Crystal"       "Crystal"
!Sample_contact_department      "Department of Genetic
Medicine"       "Department of Genetic Medicine"        "Department of Genetic
Medicine"       "Department of Genetic Medicine"        "Department of Genetic
Medicine"       "Department of Genetic Medicine"        "Department of Genetic
Medicine"       "Department of Genetic Medicine"        "Department of Genetic
Medicine"       "Department of Genetic Medicine"        "Department of Genetic
Medicine"
!Sample_contact_institute       "Weill Cornell Medical College" "Weill
Cornell Medical College"        "Weill Cornell Medical College" "Weill
Cornell Medical College"        "Weill Cornell Medical College" "Weill
Cornell Medical College"        "Weill Cornell Medical College" "Weill
Cornell Medical College"        "Weill Cornell Medical College" "Weill
Cornell Medical College"        "Weill Cornell Medical College"
!Sample_contact_address "1300 York Avenue"      "1300 York Avenue"      "1300
York Avenue"    "1300 York Avenue"      "1300 York Avenue"      "1300 York
Avenue" "1300 York Avenue"      "1300 York Avenue"      "1300 York Avenue"      "1300
York Avenue"    "1300 York Avenue"
!Sample_contact_city    "New York "     "New York "     "New York "     "New York
"       "New York "     "New York "     "New York "     "New York "     "New York "     "New
York "  "New York "
!Sample_contact_state   "NY"    "NY"    "NY"    "NY"    "NY"    "NY"    "NY"    "NY"    "NY"    "NY"    "NY"
!Sample_contact_zip/postal_code "10021" "10021" "10021" "10021" "10021" "10021" "10021" "10021" "10021" "10021" "10021"
!Sample_contact_country "USA"   "USA"   "USA"   "USA"   "USA"   "USA"   "USA"   "USA"   "USA"   "USA"   "USA"
!Sample_supplementary_file      "NONE"  "NONE"  "NONE"  "NONE"  "NONE"  "NONE"  "NONE"  "NONE"  "NONE"  "NONE"  "NONE"
!Sample_data_row_count  "22283" "22283" "22283" "22283" "22283" "22283" "22283" "22283" "22283" "22283" "22283"
!series_matrix_table_begin*/
matrix dataReadGEO( char filename[256], array<GENES> &geneArray,  array<CONDS> &condArray  ){
	leda::matrix INPUT;
	int dimension1, dimension2;
	char filename2[256];
	sprintf( filename2, "%s", filename );
	FILE *fptr = fopen( filename2, "r" );
	char readData1[1024],readData2[1024],title[1024],platformId[32],platformTaxId[32],seriesType[512],seriesSampleId[1024];

	// Reading Series Header
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
	list<S128> sampleTitle, sampleOrganism;
	list<S1024> sampleGeo, sampleDescription;
	while( !feof( fptr ) ){
		fscanf( fptr, "%s", readData1 );
		if( strcmp( "!Sample_title", readData1 ) == 0 ){
			sprintf( readData2, "%s", readData1 );
			int count = 0;
			while( strcmp( "!Sample_geo_accession", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
				S128 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleTitle.append( tmp );
				count++;
			}
			while( strcmp( "!Sample_status", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
				S1024 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleGeo.append( tmp );
			}
			while( strcmp( "!Sample_organsim_ch1", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
			}
			int count2 = 0;
			while( count2 != count ){
				fscanf( fptr, "%s", readData2 );
				S128 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleOrganism.append( tmp );
				count2++;
			}
			while( strcmp( "!Sample_description", readData2 ) != 0 ){
				fscanf( fptr, "%s", readData2 );
			}
			count2 = 0;
			while( count2 != count ){
				fscanf( fptr, "%s", readData2 );
				S1024 tmp;
				sprintf( tmp.store, "%s", readData2 );
				sampleDescription.append( tmp );
				count2++;
			}
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