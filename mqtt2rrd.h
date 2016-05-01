// #############################################################################
// #                    MQTT RRDtool Sensor Database Client                    #
// #############################################################################
// # mqtt2rrd.h - Main program                                                 #
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

#ifndef MQTT2RRD_H
#define MQTT2RRD_H

 #include "rrd_cached.h"

 #define DEFAULT_MQTT_HOST      "localhost"
 #define DEFAULT_MQTT_PORT      1883
 #define DEFAULT_MQTT_USER      ""
 #define DEFAULT_MQTT_PASS      ""
 #define DEFAULT_CLIENT_ID      "mqtt2rrd"
 #define DEFAULT_TOPIC          "/sensors/#"
 #define DEFAULT_DATABASE       "espweather.rrd"
 #define DEFAULT_UPDATEDELAY    5;
 #define DEFAULT_UPDATEINTERVAL 10;

 // Structure for configuration data
 struct ws_config
  {
   const char *mqtt_host;
   int mqtt_port;
   const char *mqtt_user;
   const char *mqtt_pass;
   const char *client_id;
   const char *topic;
   const char *database;
   int update_delay;
   int update_interval;
   config_setting_t *topics;
   struct rrd_cached_config_t *rrdconfig;
  };

#endif
