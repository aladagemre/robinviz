g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/circular.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o circular.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/layered.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o layered.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_expression.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o co_expression.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_functional.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o co_functional.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/force.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o force.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/star.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o star.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/selfLayered.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o self.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/session.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o session.exe
g++ src/cpp/robinviz.cpp -o robinviz.exe 
