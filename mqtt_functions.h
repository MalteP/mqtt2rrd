// #############################################################################
// #                    MQTT RRDtool Sensor Database Client                    #
// #############################################################################
// # mqtt_functions.h - Callback and helper functions                          #
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

#ifndef MQTT_FUNCTIONS_H
#define MQTT_FUNCTIONS_H

 #include "mqtt2rrd.h"

 void connect_callback( struct mosquitto *mosq, void *obj, int result );
 void message_callback( struct mosquitto *mosq, void *obj, const struct mosquitto_message *message );
 int register_all_ds( struct ws_config *wscfg );
 int find_ds( struct ws_config *wscfg, char *topic, const char **ds );

#endif
