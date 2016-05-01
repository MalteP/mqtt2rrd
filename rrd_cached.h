// #############################################################################
// #                    MQTT RRDtool Sensor Database Client                    #
// #############################################################################
// # rrd_cached.h - Data cache and RRD database connector                      #
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

#ifndef RRD_CACHED
#define RRD_CACHED

 #define DBFILE_LEN 256
 #define DSNAME_LEN 64
 #define DSVALUE_LEN 64

 // Struct for data source linked list
 struct rrd_cached_ds_t
  {
   char ds_name[DSNAME_LEN];
   char ds_value[DSVALUE_LEN];
   unsigned int timevalid;
   time_t lastupdate;
   struct rrd_cached_ds_t *next;
  };

 // Struct for configuration
 struct rrd_cached_config_t
  {
   char dbfile[DBFILE_LEN];
   unsigned int update_delay;
   unsigned int update_interval;
   unsigned int need_update;
   time_t needupdate_timestamp;
   time_t lastupdate_timestamp;
   unsigned int ds_item_count;
   struct rrd_cached_ds_t *ds_item;
  };

 int rrd_cached_init( struct rrd_cached_config_t **config, const char* dbfile, unsigned int update_delay, unsigned int update_interval );
 int rrd_cached_register_ds( struct rrd_cached_config_t **config, const char* ds, unsigned int timevalid );
 int rrd_cached_push_value( struct rrd_cached_config_t *config, const char* ds, const char* value );
 int rrd_cached_db_check_update( struct rrd_cached_config_t *config );
 int rrd_cached_db_update_values( struct rrd_cached_config_t *config, char *names, char *values );
 int rrd_cached_destroy( struct rrd_cached_config_t **config );

#endif
