
---


# Overview #
We present our PPI network visualization system
**RobinViz (Reliability Oriented Bioinformatic Networks Visualization)**
which is designed to visually aid the prediction and
verification processes of such networks.
Embedding both the reliability (confirmation) values associated
with the interactions and the verification
data pertaining to them
within a visualization model is a novel feature of the system.
**RobinViz** is a free, open-source software protected
under GPL. It is written in C++ and Python, and consists of
almost 30,000 lines of code, excluding the employed libraries.

### Contacts ###
  * [Ahmet Emre Aladağ](http://www.emrealadag.com), Computer Engineering, Kadir Has University, Istanbul, Turkey
  * [Cesim Erten](http://hacivat.khas.edu.tr/~cesim), Computer Engineering, Kadir Has University, Istanbul, Turkey
  * [Melih Sözdinler](http://melihsozdinler.blogspot.com), Computer Engineering, Bogaziçi University, Istanbul, Turkey

### How to cite ###

Ahmet Emre Aladağ, Cesim Erten, Melih Sözdinler,"Robinviz: Reliability Oriented Bioinformatic Networks Visualization",  Bioinformatics(2011) 27(11):1583-1584, http://bioinformatics.oxfordjournals.org/content/27/11/1583



---

## Requirements ##
We recommend using the Linux version of RobinViz as it is more stable than the Windows version.

Executable binaries require [Python 2.7](http://www.python.org/download/) and [PyQt 4.7+](http://www.riverbankcomputing.co.uk/software/pyqt/download). Please [note that](FAQ#I_have_a_64_bit_machine,_what_should_I_do?.md) even if you have 64-bit machine you will need to install the 32-bit version of Python 2.7, as PyQt4 is not compatible bit the 64-bit Python yet. **Please do not install Python 3.x versions!**

Compilation from source code requires [LEDA Professional Library 5.1](http://www.algorithmic-solutions.com). Codes are licensed under GPL v3 License but the binaries are under Academic Free license due to LEDA license restrictions.


---


# Screencasts #
## Running with Preconfigured Settings (quick run) ##
<a href='http://www.youtube.com/watch?feature=player_embedded&v=tAan64nal8o' target='_blank'><img src='http://img.youtube.com/vi/tAan64nal8o/0.jpg' width='425' height=344 /></a>
## A sample Co-Ontology run ##
<a href='http://www.youtube.com/watch?feature=player_embedded&v=lvV0UDdVwMA' target='_blank'><img src='http://img.youtube.com/vi/lvV0UDdVwMA/0.jpg' width='425' height=344 /></a>


---

# Showcase #

## Wizards ##

| ![![](http://robinviz.googlecode.com/svn/manual/showcase/wizard_color.png)](http://robinviz.googlecode.com/svn/manual/showcase/wizard_color_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/wizard_geoselect.png)](http://robinviz.googlecode.com/svn/manual/showcase/wizard_geoselect_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/wizard_goselect.png)](http://robinviz.googlecode.com/svn/manual/showcase/wizard_goselect_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/wizard_ppi.png)](http://robinviz.googlecode.com/svn/manual/showcase/wizard_ppi_l.png) |
|:---------------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------|

## Concepts ##

| Co-Expression |
|:--------------|

| ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_whole.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_whole_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_ppi.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_ppi_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_enrich.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_enrich_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_other.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_exp_other_l.png) |
|:---------------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|

| Co-Ontology |
|:------------|

| ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_whole.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_whole_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_ppi.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_ppi_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_go.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_go_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_online.png)](http://robinviz.googlecode.com/svn/manual/showcase/co_onto_online_l.png) |
|:-----------------------------------------------------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------------------------------------------------------|

## Visualization Hints and Layouts ##

| ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_color.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_color_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_edgeweights.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_edgeweights_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_nodesize.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_nodesize_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_tooptips.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_tooptips_l.png) |
|:-----------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------------|:-----------------------------------------------------------------------------------------------------------------------------------------------------|

| ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout1.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout1_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout2.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout2_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout3.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout3_l.png) | ![![](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout4.png)](http://robinviz.googlecode.com/svn/manual/showcase/hint_layout4_l.png) |
|:---------------------------------------------------------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------|


---

# Stats #


| &lt;wiki:gadget url="http://www.ohloh.net/p/481836/widgets/project\_languages.xml" width="420" height="220" border="1"/&gt; | &lt;wiki:gadget url="http://www.ohloh.net/p/481836/widgets/project\_basic\_stats.xml" height="220" border="1"/&gt; |
|:----------------------------------------------------------------------------------------------------------------------------|:-------------------------------------------------------------------------------------------------------------------|


---
