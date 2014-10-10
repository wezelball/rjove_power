/* 
 * 140008_server.c - A simple TCP echo server 
 * usage: 140008_server <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include "robotMap.h"

/* Includes for RPi only*/
#ifdef RPI
	#include <wiringPi.h>
	#include <softPwm.h>
#endif

#define BUFSIZE 1024
#define MSGSIZE 32

// we could use TRUE and FALSE
typedef enum { false = 0, true = !false } bool;

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
}

/* Abstracted I/O functions*/
void bitWrite(int pin, int value)	{
	#ifdef RPI
		digitalWrite(pin, value);
	#else
		printf("Digital Write: pin: %d, value: %d\n", pin, value);
	#endif
} 

int bitRead(int pin)	{
	#ifdef RPI
		return (digitalRead(pin));
	#else
		printf("Digital Read: pin: %d, value: unknown\n", pin);
		return(0);
	#endif
}

void PWMWrite(int pin, int value)	{
	#ifdef RPI
		softPwmWrite(pin, value);
	#else
		printf("PWM Write: pin: %d, value: %d\n", pin, value);
	#endif
}

int main(int argc, char **argv) {
	int parentfd; /* parent socket */
	int childfd; /* child socket */
	int portno; /* port to listen on */
	int clientlen; /* byte size of client's address */
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	struct hostent *hostp; /* client host info */
	char buf[BUFSIZE]; /* message buffer */
	char input[BUFSIZE]; /*modifiable copy of buf*/
	char* param[3]; /*holds seperated values*/
	char *hostaddrp; /* dotted decimal host addr string */
	int optval; /* flag value for setsockopt */
	int n; /* message byte size */
	int command; /*command type for agv*/
	int comaddr; /*addr for command to go*/
	int comval; /*value of command*/
	char reply[MSGSIZE]; /*send reply to command*/
	bool I_AM_PI; /* true if raspberry pi */
	
	/* Determine if I am a Raspberry Pi*/
	#ifdef NOPI
		I_AM_PI = false;
	#endif

	/* 
	 * If this is a RPi, set up the gpio pins
	 * This should be abstracted later to a 
	 * generic writePin readPin command 
	 */
	#ifdef RPI
		wiringPiSetup();
		//pinMode(int pin, int output);
		pinMode(D_FR_LT, OUTPUT);
		pinMode(D_FR_RT, OUTPUT);
		pinMode(D_RR_LT, OUTPUT);
		pinMode(D_RR_RT, OUTPUT);
		pinMode(CNVYR_DR, OUTPUT);
		pinMode(MAG_SR_FT, OUTPUT);
		pinMode(MAG_SR_RR, OUTPUT);
		pinMode(MAG_SR_LT, OUTPUT);
		pinMode(MAG_SR_RT, OUTPUT);
		pinMode(BATT_CURR, OUTPUT);
		pinMode(BATT_VOLT, OUTPUT);
		pinMode(MAG_SLND, OUTPUT);
		//softPwmCreate(int pin, int initialValue, int pwmRange); 
		softPwmCreate (15, 0, 100);
	#endif

	/* 
	 * check command line arguments 
	 */
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
	}
	portno = atoi(argv[1]);

	/* 
	 * socket: create the parent socket 
	 */
	parentfd = socket(AF_INET, SOCK_STREAM, 0);
	if (parentfd < 0) 
		error("ERROR opening socket");

	/*
	 * build the server's Internet address
	 */
	bzero((char *) &serveraddr, sizeof(serveraddr));

	/* this is an Internet address */
	serveraddr.sin_family = AF_INET;

	/* let the system figure out our IP address */
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* this is the port we will listen on */
	serveraddr.sin_port = htons((unsigned short)portno);

	/* 
	 * bind: associate the parent socket with a port 
	 */
	if (bind(parentfd, (struct sockaddr *) &serveraddr, 
		sizeof(serveraddr)) < 0) 
		error("ERROR on binding");

	/* setsockopt: Handy debugging trick that lets 
	 * us rerun the server immediately after we kill it; 
	 * otherwise we have to wait about 20 secs. 
	 * Eliminates "ERROR on binding: Address already in use" error.
	 * 
	 * dcohen:
	 * This problem has not been fully solved.  When server is
	 * shut down, still need to wait TIME_WAIT seconds before
	 * trying to reconnect, or same error will occur
	 */
	optval = 1;
	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, 
			 (const void *)&optval , sizeof(int));

	/* 
	 * listen: make this socket ready to accept connection requests 
	 */
	if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */ 
		error("ERROR on listen");

	/* 
	 * main loop: wait for a connection request, echo input line, 
	 * then close connection.
	 */
	clientlen = sizeof(clientaddr);

	while (1) {
		/* 
		 * accept: wait for a connection request 
		 */
		childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
		if (childfd < 0) 
			error("ERROR on accept");
    
		/* 
		 * gethostbyaddr: determine who sent the message 
		 */
		hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
				sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		if (hostp == NULL)
			error("ERROR on gethostbyaddr");
		hostaddrp = inet_ntoa(clientaddr.sin_addr);
		if (hostaddrp == NULL)
			error("ERROR on inet_ntoa\n");
		printf("server established connection with %s (%s)\n", hostp->h_name, hostaddrp);
    
		/* 
		 * read: read input string from the client
		 */
		bzero(buf, BUFSIZE);
		bzero(input, BUFSIZE);
		n = read(childfd, buf, BUFSIZE);
		if (n < 0) 
			error("ERROR reading from socket");
		printf("server received %d bytes: %s", n, buf);
		//for the future, strcmp(string, string) is useful for comparison
		//between two strings. will return 0 if equal.
		//while strtoken(char *str, const char *delim) is probably the
		//most useful for parsing a string
		param[0] = strtok(buf, " ");
		param[1] = strtok(NULL, " ");
		param[2] = strtok(NULL, "\0");

		// The command is always the first parameter
		command = atoi(param[0]);
		
		/*
		 * This code is here to insure that we react properly to the
		 * number of bytes sent. If a variable assignment is made
		 * using a nul pointer, the program will segfault
		 */
		 // do we have a 2nd parameter (address)
		if (param[1] != NULL)	{	
			comaddr = atoi(param[1]);
		}
		// do we have a 3rd parameter (value)
		if (param[2] != NULL)	{
			comval = atoi(param[2]);
		}
		
		// Branch according to command number
		switch (command) {
		case 0: /* RPi pin test - */
			strcpy(reply, "Command not supported\n");
			
			break;
		case 1: /* version */
			strcpy(reply, "Flexicart S/N: 00 \nVersion 0.0.0\n");
			break;
		case 2:
			// Make sure user doesn't enter stupid values, but need
			// to determine what values are considered stupid
			if (comaddr < 0 || comaddr > 5)	{
				perror("invalid address, dumbass!");
				strcpy(reply, "false\n");
				break;
			}
			if ((comval != 0) && (comval != 1))	{
				perror("invalid value, idiothead!");
				strcpy(reply, "false\n");
				break;
			}
			sprintf(reply, "%d", bitRead(comaddr));
			break;
		case 3:
			printf("DigitalWrite\n");
			bitWrite(comaddr, comval);
			strcpy(reply, "true\n");
			break;
		case 4:
			printf("AnalogRead not supported\n");
			strcpy(reply, "false\n");
			break;
		case 5:
			printf("AnalogWrite not supported\n");
			strcpy(reply, "false\n");
			break;
		case 6:
			/* This is the abstracted command */
			PWMWrite(comaddr, comval);
			strcpy(reply, "true\n");
			break;
		case 7:
			printf("Beat\n");
			break;
		case 99:	// quit
			return(0);
			break;
		default:
			printf("Input in int-int-int format - wtf is wrong with you?\n");
		}

		/* 
		 * write: reply the message to the client 
		 */
		n = write(childfd, reply, strlen(reply));
		if (n < 0) 
			error("ERROR writing to socket");

		close(childfd);
	}	
}
