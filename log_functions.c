// #############################################################################
// #                    MQTT RRDtool Sensor Database Client                    #
// #############################################################################
// # log_functions.c - Log helper functions                                    #
// #############################################################################
// #                 Version: 1.0 - Compiler: GCC 5.3 (Linux)                  #
// #     (c) 2016 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de     #
// #############################################################################
// #  This program is free software; you can redistribute it and/or modify it  #
// #   under the terms of the GNU General Public License as published by the   #
// #        Free Software Foundation; either version 3 of the License,         #
// #                  or (at your option) any later version.                   #
// #                                                                           #
// #      This program is distributed in the hope that it will be useful,      #
// #      but WITHOUT ANY WARRANTY; without even the implied warranty of       #
// #           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            #
// #           See the GNU General Public License for more details.            #
// #                                                                           #
// #  You should have received a copy of the GNU General Public License along  #
// #      with this program; if not, see <http://www.gnu.org/licenses/>.       #
// #############################################################################

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "log_functions.h"

FILE *log_fp;

void log_init( char *filename )
 {
  if(filename!=NULL)
   {
    log_fp = fopen(filename, "a+");
   } else {
    log_fp = NULL;
   }
 }


void log_printf( char *fmt, ... )
 {
  va_list args;
  time_t lt;
  struct tm tm;
  FILE *fp = stderr;
  if(log_fp!=NULL) fp = log_fp;
  lt = time(NULL);
  tm = *localtime(&lt);
  fprintf(fp, "[%02d.%02d.%d %02d:%02d:%02d] ", tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec );
  va_start(args, fmt);
  vfprintf(fp, fmt, args);
  va_end(args);
 }


void log_close( void )
 {
  if(log_fp==NULL) return;
  fclose(log_fp);
  log_fp = NULL;
 }
