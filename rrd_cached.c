// #############################################################################
// #                    MQTT RRDtool Sensor Database Client                    #
// #############################################################################
// # rrd_cached.c - Data cache and RRD database connector                      #
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
#include <stdlib.h>
#include <time.h>
#include <rrd.h>
#include "rrd_cached.h"


// Initialize RRD data cache
int rrd_cached_init( struct rrd_cached_config_t **config, const char* dbfile, unsigned int update_delay, unsigned int update_interval )
 {
  *config = NULL;
  if(dbfile==NULL) return 1;
  // Allocate memory
  *config = malloc(sizeof(struct rrd_cached_config_t));
  if(*config==NULL) return 1;
  // Initialize structure
  strncpy((*config)->dbfile, dbfile, DBFILE_LEN);
  (*config)->dbfile[DBFILE_LEN-1] = '\0';
  (*config)->update_delay = update_delay;
  (*config)->update_interval = update_interval;
  (*config)->need_update = 0;
  (*config)->needupdate_timestamp = time(NULL);
  (*config)->lastupdate_timestamp = time(NULL);
  (*config)->ds_item_count = 0;
  (*config)->ds_item = NULL;
  return 0;
 }


// Register new data source
int rrd_cached_register_ds( struct rrd_cached_config_t **config, const char* ds, unsigned int timevalid )
 {
  struct rrd_cached_ds_t *ds_new;
  if(ds==NULL) return 1;
  // Create new item
  ds_new = malloc(sizeof(struct rrd_cached_ds_t));
  if(ds_new==NULL) return 1;
  strncpy(ds_new->ds_name, ds, DSNAME_LEN);
  ds_new->ds_name[DSNAME_LEN-1] = '\0';
  *ds_new->ds_value='\0';
  ds_new->timevalid = timevalid;
  ds_new->lastupdate = 0;
  ds_new->next = NULL;
  // Add to linked list
  if((*config)->ds_item==NULL)
   {
    // New item
    (*config)->ds_item = ds_new;
   } else {
    // Append
    ds_new->next = (*config)->ds_item;
    (*config)->ds_item = ds_new;
   }
  ++(*config)->ds_item_count;
  return 0;
 }


// Push value to DS
int rrd_cached_push_value( struct rrd_cached_config_t *config, const char* ds, const char* value )
 {
  struct rrd_cached_ds_t *ds_find;
  if(ds==NULL||value==NULL) return 1;
  // Find DS
  ds_find = config->ds_item;
  while(ds_find!=NULL)
   {
    if(strcmp(ds_find->ds_name, ds)==0)
     {
      // Update value
      strncpy(ds_find->ds_value, value, DSVALUE_LEN);
      ds_find->ds_value[DSVALUE_LEN-1] = '\0';
      ds_find->lastupdate = time(NULL);
      if(config->need_update==0)
       {
        config->need_update = 1;
        config->needupdate_timestamp = time(NULL);
       }
      return 0;
     }
    ds_find = ds_find->next;
   }
  return 1;
 }


// Check if database needs update
int rrd_cached_db_check_update( struct rrd_cached_config_t *config )
 {
  struct rrd_cached_ds_t *ds_list;
  time_t now = time(NULL);
  char *ds_names=NULL;
  char *ds_values=NULL;
  int rtn;
  if(config->need_update!=0)
   {
    // Apply delay and minimum inverval
    if(((now-config->needupdate_timestamp)<config->update_delay)||((now-config->lastupdate_timestamp)<config->update_interval)) return 0;
    // Allocate memory
    ds_names = malloc(DSNAME_LEN*(config->ds_item_count+1));     // [values]\0
    ds_values = malloc(DSVALUE_LEN*(2+config->ds_item_count+1)); // N:[values]\0
    // Create list
    *ds_names = '\0';
    sprintf(ds_values, "N");
    ds_list = config->ds_item;
    while(ds_list!=NULL)
     {
      if(now-ds_list->lastupdate<=ds_list->timevalid)
       {
        if(*ds_names!='\0') strcat(ds_names, ":");
        strncat(ds_names, ds_list->ds_name, DSNAME_LEN);
        strcat(ds_values, ":");
        strncat(ds_values, ds_list->ds_value, DSVALUE_LEN);
       }
      ds_list = ds_list->next;
     }
    // Now update DB
    rtn = rrd_cached_db_update_values(config, ds_names, ds_values);
    // Free memory
    free(ds_names);
    free(ds_values);
    config->need_update = 0;
    config->lastupdate_timestamp = time(NULL);
    if(rtn!=0) return 1;
   }
  return 0;
 }


// Update values (internal function)
int rrd_cached_db_update_values( struct rrd_cached_config_t *config, char *names, char *values )
 {
  char *argv[5];
  int rtn;
  if(config->dbfile==NULL) return 1;
  // Create arguments, same as "rrdtool update [dbfile.rrd] --template [names] [values]"
  argv[0]="update";
  argv[1]=config->dbfile;
  argv[2]="--template";
  argv[3]=names;
  argv[4]=values;
  // Update database
  rtn = rrd_update(5, argv);
  if (rtn == -1)
   { 
    printf("Error: Updating DB failed (%s)\n", rrd_get_error()); 
    rrd_clear_error();
    return 1;
   }
  return 0;
 }


// Free memory
int rrd_cached_destroy( struct rrd_cached_config_t **config )
 {
  struct rrd_cached_ds_t *ds_remove;
  // Remove DS
  while((*config)->ds_item!=NULL)
   {
    ds_remove = (*config)->ds_item;
    (*config)->ds_item = (*config)->ds_item->next;
    free(ds_remove);
    --(*config)->ds_item_count;
   }
  // Free config
  if(*config!=NULL) free(*config);
  *config = NULL;
  return 0;
 }
