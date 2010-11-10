g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/runLayouts.cpp -lX11 -lm -lL -lG -lP -lW -O1 -o runLayouts.exe 
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/proteinScreen.cpp -lX11 -lm -lL -lG -lP -lW -O1 -o proteinScreen.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_expressionNew.cpp -lX11 -lm -lL -lG -lP -lW -O1 -o co_expression.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_functionalNew.cpp -lX11 -lm -lL -lG -lP -lW -O1 -o co_functional.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/co_localizationNew.cpp -lX11 -lm -lL -lG -lP -lW -O1 -o co_localization.exe
g++ -I$LEDAROOT/incl -L$LEDAROOT src/cpp/session.cpp -lX11 -lm -lL -lG -lP -lW -O1 -o session.exe
g++ src/cpp/robinviz.cpp -o robinviz.exe