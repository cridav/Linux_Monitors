
CC=g++
CFLAGS=-Wall -pthread

all: monitor

monitor: l4_monitor.cc l4_monitor.h 
	$(CC) $(CFLAGS) l4_monitor.cc -o only0last1

clean:
	rm -f *.o
