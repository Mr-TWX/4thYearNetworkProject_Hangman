/* Hangclient.c - Client for hangman server.  */

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

 # define LINESIZE 80
 # define HANGMAN_UDP_PORT 1067 // port number used by the server

 int main (int argc, char * argv [])
 {
 	struct sockaddr_in server; /* Server's address assembled here */
 	struct hostent * host_info;
 	int sock, count;
 	char i_line[LINESIZE];
 	char o_line[LINESIZE];
 	char * server_name;

 	/* Get server name from the command line.  If none, use 'localhost' */

 	server_name = (argc > 1)?  argv [1]: "localhost";

 	/* Create the socket */
 	sock = socket (AF_INET, SOCK_DGRAM, 0);
 	if (sock <0) {
 		perror ("Creating datagram socket");
 		exit (1);
 	}

 	host_info = gethostbyname(server_name);
 	if (host_info == NULL) {
 		fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 		exit (2);
 	}

 	/* Set up the server's socket address, then connect */

 	server.sin_family = host_info->h_addrtype;
 	memcpy ((char *) & server.sin_addr, host_info->h_addr, host_info->h_length);
 	server.sin_port = htons (HANGMAN_UDP_PORT);

// connect UDP socket, not real connect but saves from using recvfrom and sent to
 	if (connect (sock, (struct sockaddr *) & server, sizeof server) < 0) {
 		perror ("connecting to server");
 		exit (3);
 	}
 	
 	//ask and get username from user
 	printf("Please insert username\n");
 	count = read(0, o_line, LINESIZE); // read from stdin
 	printf("count = %d\n", count);
 	
 	char username [count-1];
 	sprintf(username, "%s", o_line);
 	username[count-1] = '\0';
 	//username[count] = 'R';
 	
 	sprintf(o_line, "%s_R", username);
 	//printf("Username %s\n", username);
 	//printf("Sending %s\n", o_line);

 	write(sock, o_line, count+2);

 	/*OK connected to the server.  
 Take a line from the server and show it, take a line and send the user input to the server. 
 Repeat until the server terminates the connection. */

 	printf ("Connected to server: %s\n", server_name);
 	while ((count = read (sock, i_line, LINESIZE)) > 0) {
 		write (1, i_line, count);
 		count = read (0, o_line, LINESIZE);//0 = STDIN
 		write (sock, o_line, count);
 	}
 }