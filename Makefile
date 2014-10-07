all: 140008_client 140008_server

140008_client: 140008_client.o
	gcc -g 140008_client.c -o 140008_client

140008_server: 140008_server.o
	gcc -g 140008_server.c -o 140008_server
	
clean: 
	rm -rf *.o 140008_client 140008_server DEADJOE *~
