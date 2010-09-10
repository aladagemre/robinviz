g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/circularLayout.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o circularLayout.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/forceDirectedLayoutUW.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o forceDirectedLayoutUW.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/forceDirectedLayoutW.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o forceDirectedLayoutW.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/starLayout.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o starLayout.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/layeredLayout.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o layeredLayout.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/proteinScreen.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o proteinScreen.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/layered.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o layered.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_expression.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o co_expression.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_functional.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o co_functional.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/session.cpp -lX11 -lm -lL -lG -lP -lW -lD3 -lGeoW -O1 -o session.exe
g++ src/cpp/robinviz.cpp -o robinviz.exe