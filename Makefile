LIBS = -L/usr/local/src/wiringPi/wiringPi -lwiringPi


pi: 140008_client 140008_server_pi

x86: 140008_client 140008_server_x86

140008_client: 140008_client.o
	gcc -g 140008_client.c -o 140008_client

140008_server_pi: 140008_server.o
	gcc -g 140008_server.c -o 140008_server $(LIBS)
	
140008_server_x86: 140008_server.o
	gcc -g 140008_server.c -o 140008_server	
	
clean: 
	rm -rf *.o 140008_client 140008_server DEADJOE *~
