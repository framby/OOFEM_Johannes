/* $Header: /home/cvs/bp/oofem/oofemlib/src/strreader.h,v 1.7 2003/04/06 14:08:26 bp Exp $ */
/*

                   *****    *****   ******  ******  ***   ***                            
                 **   **  **   **  **      **      ** *** **                             
                **   **  **   **  ****    ****    **  *  **                              
               **   **  **   **  **      **      **     **                               
              **   **  **   **  **      **      **     **                                
              *****    *****   **      ******  **     **         
            
                                                                   
               OOFEM : Object Oriented Finite Element Code                 
                    
                 Copyright (C) 1993 - 2000   Borek Patzak                                       



         Czech Technical University, Faculty of Civil Engineering,
     Department of Structural Mechanics, 166 29 Prague, Czech Republic
                                                                               
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                                                                              
*/


//
// Class stringReader
//

#ifndef strreader_h

#ifndef __MAKEDEPEND
#include <stdio.h>
#include <string.h>
#endif
#include "intarray.h"
#include "flotarry.h"
#include "dictionr.h"
#include "dynalist.h"

class Range;

class StringReader
{
//
// DESCRIPTION:
//   This class implements simple string reader 
// TASKS
//
//   Read number (integer or double ) or array (integer or double)
//   depicted by string identifier.
//
// Note
//   The domain class converts all record characters to lowercase.
//   But the StringReader is case sensitive. 
//


 public:
  StringReader () {}
  ~StringReader () {}

  
  int readInteger (char* source, char* idString);
  double readDouble (char* source, char* idString);
  char * readString (char* source, char* idString, char* string, int maxchar);
/**
 Reads quoted string identified by keyword.
 The read string must be bounded by " characters. 
 @param source source record
 @param idString keyword identifiing the value
 @param string buffer to store result. Should be at least of  maxchar size.
 @param maxchar maximum character to be read.
 @return pointer to string parameter.
*/
 char*  readQuotedString (char* source, char* idString, char* string, int maxchar);
  IntArray*  ReadIntArray (char* source, char* idString);
  FloatArray* ReadFloatArray (char* source, char* idString);
  Dictionary* ReadDictionary (char* source, char* idString);
  char*  readSimpleString (char* source, char* simpleString, int maxchar, char** remain); 
  char*  readKeyAndVal    (char* source, char* key, int* val, int maxchar, char** remain);
  char*  readKeyAndVal    (char* source, char* key, double* val, int maxchar, char** remain);
  int    hasString        (char* source, char* idString);

/**
 Reads range list. The range syntax is "range_list_name { number, (start end) }",
 where range_list_name is name of list in source record. It is necessary to encose list definition into 
 {}. THe single number is shortcut for one value range. The range is pecified using two numbers 
 in parenthesis.
 @param list list to be read. Actually items are added into this list.
 @param source source string with corresponding record.
 @param idString string containing range_list_name.
 */
void   readRangeList (dynaList<Range> &list, char* source, char* idString);
 /**
  Reads single range record from input record represented by *helpSource  string.
  @param helpSource pointer to current string possition, on return helpSource points
  to next charcter after reading range record.
  @param li starting range index
  @param hi end range index
  @return on success nonzero valur returned
  */
int    readRange (char** helpSource, int& li, int& hi);

//
private:
//
// I don't expect, that You will use following functions
//

  char* getPosAfter (char*, char *) ;
  char* scanInteger (char* source, int* value);
  char* scanDouble  (char* source, double* value);
  char* skipNextWord (char* src) ;
};

#define strreader_h
#endif







