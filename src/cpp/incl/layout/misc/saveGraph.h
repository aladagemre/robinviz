

/*		savegraph.h,header file to			*/
/*		save the necessary matrix to		*/
/*		specified file                		*/
/*		Last modifid at 12th May 2008		*/
/*		by Melih Sozdinler					*/

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
#include "declerations.h"

void saveAsAdjacencyMatrix( matrix &M, list<node> A, list<node> B, char filename[] ){

	FILE *fptr;
	fptr = fopen( filename, "w" );

	M.print();
	for( int i = 0; i < M.dim1(); i++ ){ 
		for( int j = 0; j < M.dim2(); j++ ){
			fprintf( fptr, "%f\t", (double)M( i, j ) );
		}
		fprintf( fptr, "\n" );
	}
	fclose( fptr );
}