#!/bin/bash

title_text="MQTT2RRD Demo"
watermark_text="(c) www.maltepoeggel.de - Rendered: `date +'%d.%m.%Y %H:%M:%S'`"

declare -A timespan
timespan["1d"]="1 day"
timespan["3d"]="3 days"
timespan["1w"]="1 week"
timespan["1m"]="1 month"
timespan["3m"]="3 month"
timespan["1y"]="1 year"

graphic_path="/var/www/html"
db_file="/home/weather_station/espweather.rrd"

temperature_args="-i -E -v \"°C\""
temperature_data="DEF:var0=$db_file:temp0:AVERAGE LINE1:var0#A64600:\"Sensor 1\""

humidity_args="-i -E -v \"%rel\""
humidity_data="DEF:var0=$db_file:hum0:AVERAGE LINE1:var0#A64600:\"Sensor 1\""

pressure_args="-i -E -A -y 2:4 -X 0 -v \"hPa\""
pressure_data="DEF:var0=$db_file:pres0:AVERAGE LINE1:var0#A64600:\"Sensor 1\""

for e in "${!timespan[@]}"
do
  cmd_temp="rrdtool graph $graphic_path/temp-$e.png -e now -s end-$e $temperature_args -t \"$title_text - Temperature (${timespan[$e]})\" -W \"$watermark_text\" $temperature_data"
  eval $cmd_temp
  cmd_hum="rrdtool graph $graphic_path/hum-$e.png -e now -s end-$e $humidity_args -t \"$title_text - Relative humidity (${timespan[$e]})\" -W \"$watermark_text\" $humidity_data"
  eval $cmd_hum
  cmd_pres="rrdtool graph $graphic_path/pres-$e.png -e now -s end-$e $pressure_args -t \"$title_text - Barometric pressure (${timespan[$e]})\" -W \"$watermark_text\" $pressure_data"
  eval $cmd_pres
done
