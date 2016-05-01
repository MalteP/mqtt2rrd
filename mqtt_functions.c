// #############################################################################
// #                    MQTT RRDtool Sensor Database Client                    #
// #############################################################################
// # mqtt_functions.c - Callback and helper functions                          #
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
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <libconfig.h>
#include <mosquitto.h>
#include "rrd_cached.h"
#include "mqtt_functions.h"


// MQTT client connected
void connect_callback( struct mosquitto *mosq, void *obj, int result )
 {
  printf("MQTT connect: result=%d\n", result);
 }


// MQTT message received
void message_callback( struct mosquitto *mosq, void *obj, const struct mosquitto_message *message )
 {
  struct ws_config *wscfg = (struct ws_config *) obj;
  const char *ds;
  printf("MQTT message: topic=%s, payload=%s, len=%d\n", message->topic, (char*)message->payload, message->payloadlen);
  // Find DS for matching topic in configuration
  if(find_ds(wscfg, message->topic, &ds)==0)
   {
    // Push DS data
    rrd_cached_push_value(wscfg->rrdconfig, ds, (char*)message->payload);
   } else {
    printf("Warning: No DS found for topic=%s.\n", message->topic);
   }
 }


// Find RRD DS in configuration and register in rrd_cached list
int register_all_ds( struct ws_config *wscfg )
 {
  int i;
  int count;
  config_setting_t *element;
  const char *element_d;
  if(wscfg->topics==NULL) return 1;
  count = config_setting_length(wscfg->topics);
  for(i=0; i<count; i++)
   {
    element = config_setting_get_elem(wscfg->topics, i);
    if(!config_setting_lookup_string(element, "ds", &element_d)) continue;
    rrd_cached_register_ds( &wscfg->rrdconfig, element_d, 600 );
   }
  return 0;
 }


// Find DS for given topic
int find_ds( struct ws_config *wscfg, char *topic, const char **ds )
 {
  int i;
  int count;
  config_setting_t *element;
  const char *element_t;
  const char *element_d;
  if(wscfg->topics==NULL) return 1;
  count = config_setting_length(wscfg->topics);
  for(i=0; i<count; i++)
   {
    element = config_setting_get_elem(wscfg->topics, i);
    if(!config_setting_lookup_string(element, "topic", &element_t)||!config_setting_lookup_string(element, "ds", &element_d)) continue;
    if(strcmp(topic, element_t)!=0) continue;
    *ds = element_d;
    return 0;
   }
  return 1;
 }

