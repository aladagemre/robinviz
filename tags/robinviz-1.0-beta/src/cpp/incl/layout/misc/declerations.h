/*****************************************************************************************/
/*	Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>   	 */
/*											 */
/*    This program is free software: you can redistribute it and/or modify		 */
/*    it under the terms of the GNU General Public License as published by		 */
/*    the Free Software Foundation, either version 3 of the License, or			 */
/*    (at your option) any later version.						 */
/*											 */
/*    This program is distributed in the hope that it will be useful,			 */
/*    but WITHOUT ANY WARRANTY; without even the implied warranty of			 */
/*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the			 */
/*    GNU General Public License for more details.					 */
/*											 */
/*    You should have received a copy of the GNU General Public License			 */
/*    along with this program.  If not, see <http://www.gnu.org/licenses/>.		 */
/*											 */
/*    Copyright (C) <2009>  <Melih Sozdinler,Ahmet Emre Aladağ, Cesim Erten>		 */
/*    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.         */
/*    This is free software, and you are welcome to redistribute it                      */
/*    under certain conditions; type `show c' for details.			         */
/*****************************************************************************************/
/*************************************************************************************

declerations.h is for the main.cpp. The main contribuiton of this
header is to import all needed header files.

Created

01 January 2009, by Melih

*************************************************************************************/
#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_gen.h>
#include <LEDA/graph/edge_array.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/graph/graph_misc.h>
#include <LEDA/graph/graph_gen.h>

// #include <LEDA/numbers/integer_matrix.h>
#include <LEDA/numbers/integer_matrix.h>
#include <LEDA/numbers/matrix.h>

#include <LEDA/core/random_source.h>
#include <LEDA/core/d_int_set.h> 
#include <LEDA/core/b_stack.h>
#include <LEDA/core/tuple.h>
#include <LEDA/core/list.h>
#include <LEDA/core/string.h>
#include <LEDA/core/queue.h>

#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/color.h> 
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/d3_window.h>
#include <LEDA/graphics/file_panel.h>
#include <LEDA/graphics/panel.h>
#include <LEDA/graphics/color.h> 

#include <LEDA/geo/point.h> 
#include <LEDA/geo/d3_delaunay.h>

#include <LEDA/system/stream.h>

#include <math.h>
#include <fstream>
#include <iostream>
#include <string.h>

#define LINUX
//  #define WINDOWS

// #define DEBUG_ROBINVIZ

using namespace leda;

using leda::node_array;
using leda::GRAPH;
using leda::random_source;
using leda::graph;
using leda::edge_array;
using leda::edge;
using leda::node;
using leda::GraphWin;
using leda::window;
using leda::list;
using std::cout;
using std::cin;
using leda::integer_matrix;
// using leda::node_matrix;
using leda::matrix;
using leda::vector;
using leda::gw_label_type ;
using leda::array;
using leda::list_item ;
using std::ios;
using std::ofstream;
using std::fstream;
using std::istream;
using leda::integer;
using leda::point;
using std::endl;
using leda::color;
using leda::b_stack;
using leda::two_tuple;
using leda::three_tuple;
using leda::gw_label_type;
using leda::gw_font_type;
using leda::gw_edge_shape;
using leda::gw_node_shape;
using leda::d_int_set;
using namespace leda;
using namespace std;