#include <stdlib.h>

int main(){
#ifdef __linux__
    system( "python src/python/robinviz/RobinViz.py" );
#else
    system( "src\\python\\robinviz\\RobinViz.py" );
#endif
    return 0;
}
