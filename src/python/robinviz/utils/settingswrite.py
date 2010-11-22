# -*- coding: utf-8 -*-

exp= """Confirmation:
    CoExpression:
        Input:
            dataName_bic: %(dataName_bic)s
            dataName2_bic: %(dataName2_bic)s
            ppifilename_bic: %(ppifilename_bic)s
            catfile_bic: %(catfile_bic)s
            gofile_bic: %(gofile_bic)s
        Drawing:
            minBicSize: %(minBicSize)s
            maxBicSize: %(maxBicSize)s
            width: %(width)s
            space: %(space)s
            increment: %(increment)s
            removeRat: %(removeRat)s
            nodeSize: %(nodeSize)s
            edgeBendImp: %(edgeBendImp)s
            colorScale: %(colorScale)s
            edgThicknessTher: %(edgThicknessTher)s
        Biclustering:
            biclustering: %(biclustering)s
            bimax_low_dim1: %(bimax_low_dim1)s
            bimax_low_dim2: %(bimax_low_dim2)s
            ther_disc: %(ther_disc)s
            bic_num_bimax: %(bic_num_bimax)s
            bic_num_cc: %(bic_num_cc)s
            maxScore_: %(maxScore_)s
            minHeight_: %(minHeight_)s
            minWidth_: %(minWidth_)s
            maxSizeSubMatrix_exp1_g: %(maxSizeSubMatrix_exp1_g)s
            maxSizeSubMatrix_exp1_c: %(maxSizeSubMatrix_exp1_c)s
            minSizeSubMatrix_exp1_g: %(minSizeSubMatrix_exp1_g)s
            minSizeSubMatrix_exp1_c: %(minSizeSubMatrix_exp1_c)s
            repeat: %(repeat)s
            hvaluemin: %(hvaluemin)s
            increment_exp1_g: %(increment_exp1_g)s
            increment_exp1_c: %(increment_exp1_c)s
        Algorithms:
            ownFlag: %(ownFlag)s
            lebFlag: %(lebFlag)s
            ccFlag: %(ccFlag)s
            msbeFlag: %(msbeFlag)s
            bimaxFlag: %(bimaxFlag)s
            rlebFlag: %(rlebFlag)s
            brandFlag: %(brandFlag)s
            brandFlag2: %(brandFlag2)s
            ourMethodFlag: %(ourMethodFlag)s
            ledaPostFlag: %(ledaPostFlag)s
            readOption: %(readOption)s
            ffd_layout: %(ffd_layout)s
            layered_layout: %(layered_layout)s
            circular_layout: %(circular_layout)s
            circular_ffd_layout: %(circular_ffd_layout)s
            go_info: %(go_info)s
            edgesBetween: %(edgesBetween)s
            sharedGenes: %(sharedGenes)s
            hvalueWeighting: %(hvalueWeighting)s
            enrichmentWeighting_o: %(enrichmentWeighting_o)s
            enrichmentWeighting_f: %(enrichmentWeighting_f)s
            ppihitratioWeighting: %(ppihitratioWeighting)s
            is_ppi_directed: %(is_ppi_directed)s
            """

func="""
    CoFunctionality:
        Input:
            ppifilename_go: %(ppifilename_go)s
            catfile_go: %(catfile_go)s
            gofile_go: %(gofile_go)s
            dataName_go: %(dataName_go)s
        Drawing:
            removeRat: %(removeRat)s
            nodeSize: %(nodeSize)s
            edgeBendImp: %(edgeBendImp)s
            colorScale: %(colorScale)s
            edgThicknessTher: %(edgThicknessTher)s
        Algorithms:
            go_info: %(go_info)s
            edgesBetween: %(edgesBetween)s
            sharedGenes: %(sharedGenes)s
"""

	
def get_exp(values):
    union = {}
    d = values["Confirmation"]["CoExpression"]

    for key in d.keys():
	for k,v in d[key].iteritems():
	    union[k] = v
            
    return exp % union
def get_ont(values):
    union = {}
    d = values["Confirmation"]["CoFunctionality"]

    for key in d.keys():
	for k,v in d[key].iteritems():
	    union[k] = v
    return func % union

def write_values(values):
    return get_exp(values) + get_ont(values)