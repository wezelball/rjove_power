/* 
 * tcpserver.c - A simple TCP echo server 
 * usage: tcpserver <port>
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

#define BUFSIZE 1024

#if 0
/* 
 * Structs exported from in.h
 */

/* Internet address */
struct in_addr {
	unsigned int s_addr; 
};

/* Internet style socket address */
struct sockaddr_in  {
	unsigned short int sin_family;	/* Address family */
	unsigned short int sin_port;	/* Port number */
	struct in_addr sin_addr;	/* IP address */
	unsigned char sin_zero[...];	/* Pad to size of 'struct sockaddr' */
};

/*
 * Struct exported from netdb.h
 */

/* Domain name service (DNS) host entry */
struct hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	**h_addr_list;	/* list of addresses */
}
#endif

/*
 * error - wrapper for perror
 */
void error(char *msg) {
	perror(msg);
	exit(1);
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

	/* setsockopt: Handy debugging trick that lets 
	 * us rerun the server immediately after we kill it; 
	 * otherwise we have to wait about 20 secs. 
	 * Eliminates "ERROR on binding: Address already in use" error. 
	 */
	optval = 1;
	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, 
			 (const void *)&optval , sizeof(int));

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
		param[0] = strtok(buf, "-");
		param[1] = strtok(NULL, "-");
		param[2] = strtok(NULL, "\0");
		printf("%s:%s:%s", param[0], param[1], param[2]);
		command = atoi(param[0]);
		comaddr = atoi(param[1]);
		comval = atoi(param[2]);
		switch (command) {
		case 1:
			printf("Flexicart S/N: 00 \nVersion 0.0.0\n");
			break;
		case 2:
			printf("DigitalRead\n");
			break;
		case 3:
			printf("DigitalWrite\n");
			break;
		case 4:
			printf("AnalogRead\n");
			break;
		case 5:
			printf("AnalogWrite\n");
			break;
		case 6:
			printf("pwmWrite\n");
			break;
		default:
			printf("please input in int-int-int format\n");
		}
		/* 
		 * write: echo the input string back to the client 
		 */
		n = write(childfd, buf, strlen(buf));
		if (n < 0) 
			error("ERROR writing to socket");

		close(childfd);
	}	
}
