# -*-makefile-*-

CC=g++
SRCDIR=src/cpp
LDFLAGS =  -L$(LEDAROOT) -lX11 -lm -lL -lG -lP -lW -O1
INCL = -I$(LEDAROOT)/incl

all: robinviz co_expression co_ontologies runLayouts proteinScreen

runLayouts:
	$(CC) -o runLayouts.exe $(INCL) $(SRCDIR)/runLayouts.cpp $(LDFLAGS)
proteinScreen:
	$(CC) -o proteinScreen.exe $(INCL) $(SRCDIR)/proteinScreen.cpp $(LDFLAGS)
co_expression:
	$(CC) -o co_expression.exe $(INCL) $(SRCDIR)/co_expressionNew.cpp $(LDFLAGS)
co_ontologies:
	$(CC) -o co_ontologies.exe $(INCL) $(SRCDIR)/co_ontologies.cpp $(LDFLAGS)
robinviz:
	$(CC) -o robinviz.exe $(SRCDIR)/robinviz.cpp

cleanexe:
	rm *.exe
cleancache:
	cd src/python/robinviz
	python clean.py
	cd ../../..

clean: cleanexe cleancache