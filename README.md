# Overview
We present our PPI network visualization system
*RobinViz (Reliability Oriented Bioinformatic Networks Visualization)*
which is designed to visually aid the prediction and
verification processes of such networks.
Embedding both the reliability (confirmation) values associated
with the interactions and the verification
data pertaining to them
within a visualization model is a novel feature of the system.
*RobinViz* is a free, open-source software protected
under GPL. It is written in C++ and Python, and consists of
almost 30,000 lines of code, excluding the employed libraries.

## Contacts

* [Ahmet Emre Aladağ][emre-aladag], Computer Engineering, Kadir Has University, Istanbul, Turkey
* [Cesim Erten][cesim-erten], Computer Engineering, Kadir Has University, Istanbul, Turkey
* [Melih Sözdinler][melih-sozdinler], Computer Engineering, Bogaziçi University, Istanbul, Turkey

## How to cite

Ahmet Emre Aladağ, Cesim Erten, Melih Sözdinler,"Robinviz: Reliability Oriented Bioinformatic Networks Visualization",  Bioinformatics(2011) 27(11):1583-1584, http://bioinformatics.oxfordjournals.org/content/27/11/1583


# Requirements

We recommend using the Linux version of RobinViz as it is more stable than the Windows version.

Executable binaries require Python 2.7 and PyQt 4.7+. Please note that even if you have 64-bit machine you will need to install the 32-bit version of Python 2.7, as PyQt4 is not compatible bit the 64-bit Python yet. *Please do not install Python 3.x versions!*

Compilation from source code requires LEDA Professional Library 5.1. Codes are licensed under GPL v3 License but the binaries are under Academic Free license due to LEDA license restrictions.

-------------------------

Screenshots
=============
See [Screenshots page][screenshots] for screenshots.

Screencasts
============

Watch "Preconfigured Settings (quick run)" on Youtube: http://www.youtube.com/watch?v=tAan64nal8o

Watch "A sample Co-Ontology run" on Youtube: http://www.youtube.com/watch?v=lvV0UDdVwMA


[emre-aladag]: http://www.emrealadag.com
[cesim-erten]: http://hacivat.khas.edu.tr/~cesim
[melih-sozdinler]: http://melihsozdinler.blogspot.com
[screenshots]: SCREENSHOTS.md

