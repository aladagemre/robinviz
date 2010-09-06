g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/circularLayout.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o circularLayout.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/layered.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o layered.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_expression.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o co_expression.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_functional.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o co_functional.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/forceDirectedLayout.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o forceDirectedLayout.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/starLayout.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o starLayout.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/layeredLayout.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -o layeredLayout.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/session.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o session.exe
g++ src/cpp/robinviz.cpp -o robinviz.exe 
