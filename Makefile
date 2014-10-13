INCLUDES = -I/usr/local/include/libhackrf/
LIBS = -lhackrf -L/usr/local/lib
CC = gcc

all:	rj_hackrf

rj_hackrf:
	$(CC) rj_hackrf.c -o rj_hackrf $(INCLUDES) $(LIBS)
	
clean:
	rm -rf *.o rj_hackrf *~
