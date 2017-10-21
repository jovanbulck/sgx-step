/*
 *  This file is part of the SGX-Step enclave execution control framework.
 *
 *  Copyright (C) 2017 Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>,
 *                     Raoul Strackx <raoul.strackx@cs.kuleuven.be>
 *
 *  SGX-Step is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SGX-Step is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SGX-Step. If not, see <http://www.gnu.org/licenses/>.
 */

#include "sys_file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int fread_value( char const *file, char const *format, void *result )
{
	FILE *fp;
	
	if ( result == NULL )
		return -1;
	
	if ( NULL == ( fp = fopen( file, "r" ) ) )
		return -2;
	
	if ( 1 != fscanf( fp, format, result ) )
		return -3;
	
	fclose( fp );
	
	return 0;
}

int fread_int( char const *file, int *result )
{
	return fread_value( file, "%i", result );
}

int fwrite_value( char const *file, int value )
{
	FILE *fp;
    int rv;
	
	if ( file == NULL )
		return -1;
	
	if ( NULL == ( fp = fopen( file, "w" ) ) )
		return -2;
	
	rv = fprintf( fp, "%i", value );
	
	fclose( fp );
	
	return rv;
}
