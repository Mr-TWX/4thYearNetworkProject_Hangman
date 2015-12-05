/* Hangclient.c - Client for hangman server.  */
/* File: hangclient_udp.c */

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

 int main (int argc, char * argv []) {
 	struct sockaddr_in server; /* Server's address assembled here */
 	struct hostent* host_info;
 	int sock, count;
 	char i_line[LINESIZE];	// buffer for input strings
 	char o_line[LINESIZE];	// buffer for output strings
 	char * server_name;		// string for server name

 	/* Get server name from the command line.  If none, use 'localhost' */
 	server_name = (argc > 1)?  argv [1]: "localhost";

 	/* Create the UDP socket */
 	sock = socket (AF_INET, SOCK_DGRAM, 0);
 	if (sock <0) {
 		perror ("Creating datagram socket");
 		exit (1);
 	}

 	// get addreww information of the server
 	host_info = gethostbyname(server_name);
 	if (host_info == NULL) {
 		fprintf (stderr, "%s: unknown host:%s \n", argv [0], server_name);
 		exit (2);
 	}

 	/* Set up the server's socket address, then connect */
 	server.sin_family = host_info->h_addrtype;
 	memcpy ((char *) &server.sin_addr, host_info->h_addr, host_info->h_length);
 	server.sin_port = htons (HANGMAN_UDP_PORT);

	// connect UDP socket, not real connect but saves from using recvfrom and sendto
 	if (connect (sock, (struct sockaddr *) & server, sizeof server) < 0) {
 		perror ("connecting to server");
 		exit (3);
 	}
 	
 	// Ask and get username from user
 	printf("Please insert username\n");
 	count = read(0, i_line, LINESIZE); // read from stdin
 	//printf("count = %d\n", count);
 	
 	// Store username for future needs
 	char username [count-1];	// create username container
 	sprintf(username, "%s", i_line);	// extract username from input buffer
 	username[count-1] = '\0';	// be sure not to take '\n' character;
 	
 	// Format output message to server and send it
 	sprintf(o_line, "%s_R", username);	// format output buffer in <username>_<message>
 	//printf("Username %s\n", username);
 	//printf("Sending %s\n", o_line);
 	write(sock, o_line, count+2); // send "connection" request to server

 	/*	OK connected to the server.  
 		Take a line from the server and print it, 
 		take and send the user input to the server. 
 		Repeat until the server sends GameOver. */
 	printf ("Waiting for server: %s\n", server_name);

 	char gameState = 'I'; // start with an Incomplete game
 	while (gameState == 'I') 
 	{
 		// Receive message from server
 		count = read (sock, i_line, LINESIZE);
 		if(count < 0) {
 			perror ("read failed");
 			continue;
 		}
 		i_line[count] = '\0';

 		/*	Detect formatted line from server for game status.
			Line format is "<GameState>_<ServerMessage>"
 		*/
 		if(strstr(i_line, "_") != NULL) {
			// Line received matches the right format.
			// Extract game state and server message
			char* gStateptr = strtok(i_line, "_");
			char* servMessage = strtok(NULL, "_");
			gameState = gStateptr[0];	// update gameState

			printf("%s", servMessage);	// show server message to user
		}
		else {
			// line received is the greeting one at the beginning of the game
 			write (1, i_line, count);
		}
 		
 		// game still running, take and send message to server
 		if (gameState == 'I') {
 			char userInput[LINESIZE]; // buffer for user input
 			count = read (0, userInput, LINESIZE);//read from STDIN
 			userInput[count] = '\0';

 			// format and send message for server
 			// line format is "<username>_<message>"
 			sprintf(o_line, "%s_%s", username, userInput);
 			count = strlen(o_line); //send the right amount
 			write (sock, o_line, count);
 		}
 	}
 }