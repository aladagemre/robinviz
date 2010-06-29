g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/circular.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o circular.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/layered.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o layered.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/ppi_layered.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o ppi_layered.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/force.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o force.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/star.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o star.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/session.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o session.exe
g++ src/cpp/robinviz.cpp -o robinviz.exe 
