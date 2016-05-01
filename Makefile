CFLAGS = -g -Wall
LDFLAGS = -lconfig -lmosquitto -lrrd
CC = gcc

default: mqtt2rrd.c
	$(CC) $(CFLAGS) rrd_cached.c mqtt_functions.c mqtt2rrd.c -o mqtt2rrd $(LDFLAGS)

clean: 
	rm -f mqtt2rrd
