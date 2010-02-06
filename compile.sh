g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/circular.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o circular.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/tcircular.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o tcircular.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/layered.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o layered.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/force.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o force.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/star.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o star.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/orthogonal.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o orthogonal.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/simplify.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o simplify.exe 
g++ src/cpp/robinviz.cpp -o robinviz.exe 
