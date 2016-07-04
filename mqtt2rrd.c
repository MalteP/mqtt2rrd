// #############################################################################
// #                    MQTT RRDtool Sensor Database Client                    #
// #############################################################################
// # mqtt2rrd.c - Main program                                                 #
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
#include <libconfig.h>
#include <mosquitto.h>
#include "log_functions.h"
#include "mqtt_functions.h"
#include "mqtt2rrd.h"

static int running = 1;


// *** Signal handler ***
void handle_signal( int s )
 {
  running = 0;
 }


// *** Main program ***
int main( int argc, char** argv )
 {
  static config_t cfg;
  struct mosquitto *mosq;
  struct ws_config wscfg;
  int rc = EXIT_SUCCESS;
  char* configfile = "mqtt2rrd.cfg";

  // If parameter given, use it as config file
  if(argc>1)
   {
    configfile=argv[1];
   }

  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  // Init config and log
  memset(&wscfg, 0, sizeof(struct ws_config));
  config_init(&cfg);
  log_init(NULL);

  // Read config
  if(!config_read_file(&cfg, configfile))
   {
    log_printf("Error: Config error in line %d: %s\n", config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return(EXIT_FAILURE);
   }
  if(!config_lookup_string(&cfg, "logfile", &(wscfg.logfile)))              wscfg.logfile   = DEFAULT_LOGFILE;
  if(!config_lookup_string(&cfg, "mqtt_host", &(wscfg.mqtt_host)))          wscfg.mqtt_host = DEFAULT_MQTT_HOST;
  if(!config_lookup_int(&cfg, "mqtt_port", &(wscfg.mqtt_port)))             wscfg.mqtt_port = DEFAULT_MQTT_PORT;
  if(!config_lookup_string(&cfg, "mqtt_user", &(wscfg.mqtt_user)))          wscfg.mqtt_user = DEFAULT_MQTT_USER;
  if(!config_lookup_string(&cfg, "mqtt_pass", &(wscfg.mqtt_pass)))          wscfg.mqtt_pass = DEFAULT_MQTT_PASS;
  if(!config_lookup_string(&cfg, "client_id", &(wscfg.client_id)))          wscfg.client_id = DEFAULT_CLIENT_ID;
  if(!config_lookup_string(&cfg, "topic", &(wscfg.topic)))                  wscfg.topic     = DEFAULT_TOPIC;
  if(!config_lookup_string(&cfg, "database", &(wscfg.database)))            wscfg.database  = DEFAULT_DATABASE;
  if(!config_lookup_int(&cfg, "update_delay", &(wscfg.update_delay)))       wscfg.update_delay = DEFAULT_UPDATEDELAY;
  if(!config_lookup_int(&cfg, "update_interval", &(wscfg.update_interval))) wscfg.update_interval = DEFAULT_UPDATEINTERVAL;

  // Init log with file from config
  log_init((char*)wscfg.logfile);

  // Register topics
  wscfg.topics = config_lookup(&cfg, "topics");
  rrd_cached_init(&wscfg.rrdconfig, wscfg.database, wscfg.update_delay, wscfg.update_interval);
  register_all_ds(&wscfg);

  // Init MQTT Client
  mosq = mosquitto_new(wscfg.client_id, true, &wscfg);
  if(mosq)
   {
    mosquitto_connect_callback_set(mosq, connect_callback);
    mosquitto_message_callback_set(mosq, message_callback);
    mosquitto_username_pw_set(mosq, wscfg.mqtt_user, wscfg.mqtt_pass);

    // Connect to server
    rc = mosquitto_connect(mosq, wscfg.mqtt_host, wscfg.mqtt_port, 60);

    // Main loop
    while(running)
     {
      rc = mosquitto_loop(mosq, -1, 1);
      if(running && rc)
       {
        sleep(100);
        mosquitto_reconnect(mosq);
       }
      // DB update necessary?
      rrd_cached_db_check_update(wscfg.rrdconfig);
     }

    // Free memory
    mosquitto_destroy(mosq);
    rrd_cached_destroy(&wscfg.rrdconfig);
    config_destroy(&cfg);
   }

  return rc;
 }
