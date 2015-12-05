 /* Network server for hangman game */
 /* File: hangserver_udp.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include "./common/GameList.h"

extern time_t time ();

int maxlives = 12;
char *word [] = {
#include "words"
};
#define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
#define MAXLEN 80 /* Maximum size in the world of Any string */
#define HANGMAN_UDP_PORT 1067

int main () {
 	int sock; // the UDP server socket file descriptor
 	struct sockaddr_in server, client; // server and client address structures
 	socklen_t client_len;	// length of client address structure

 	srand ((int) time ((long *) 0)); /* randomize the seed */

 	/* Initialize server socket*/
 	sock = socket (AF_INET, SOCK_DGRAM, 0);
 	if (sock <0) { 
 		perror ("creating datagram socket");
 		exit (1);
 	}

 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = htonl(INADDR_ANY);
 	server.sin_port = htons(HANGMAN_UDP_PORT);
 	/* Bind the server socket to an available address and the defined port number*/
 	if (bind(sock, (struct sockaddr *) &server, sizeof(server)) <0) {
 		perror ("binding socket");
	 	exit (2);
 	}

 	// initialise Hangman game components
 	GameList* gList = GameList_create(); // create a new empty list that holds all the gamestates
 	GameNode* gNode;	// GameNode pointer to work with one game state at the time
 	char buffer[MAXLEN]; // input buffer to hold messages sent by clients
 	int count;	// control variable to trak the amount of data received or sent

 	// All components initialised, start the infinite loop
 	printf("Ready to Receive v3\n");
 	while (1) {
 		// receive a message from any client, but store address of sender for reply
 		client_len = sizeof (client);
 		count = recvfrom(sock, buffer, MAXLEN, 0, (struct sockaddr*) &client, &client_len);
 		if (count == 0) {
 			perror ("lost connection");
 			continue;
 		}
 		buffer[count] = '\0';
 		printf("(Received %s)\n", buffer);

 		// detect valid formatted string from client
 		if(strstr(buffer, "_") != NULL) {
 			// extract username and guess from client message
 			char* username = strtok(buffer, "_"); // the username is always first
 			char* message = strtok(NULL, "_");	// message is after the '_' delimiter

 			//check if client already connected
 			gNode = GameList_searchGameNodeByUsername(gList, username);

 			if(gNode == NULL) {
 				// client not in list, create new and add it to list
 				add_user(gList, username, sock, &client, client_len);
 			} else {
 				// client already connected, handle his guess
 				play_hangman(gNode, message, sock, &client, client_len);

 				// if game session ended then remove node from list
 				if(gNode->state != INCOMPLETE) {
 					if(GameList_remove(gList, gNode)) {
 						printf("removed %s\n", username);
 					} else {
 						printf("Error! Unable to remove: %s\n", username);
 					}
 				}
 			}
 		}
	} // end of infinite loop
}

/* ---------------- Add_User () -----------------------*/
add_user(GameList* gList, char* username, int sfd, struct sockaddr_in* client, socklen_t client_len) {
	// print state
	printf("Adding new client %s\n", username);

	// create a new GameNode and fill details
	char* outbuf[MAXLEN];
	int i, word_length;
	char hostname[MAXLEN];

	//create new GameNode and add it to the list
	GameNode* gNode = GameNode_create(GameList_count(gList), username, word[rand() % NUM_OF_WORDS], maxlives);
	printf("Word chosen is %s\n", gNode->correctWord);

	/* no letters are guessed initially */
	word_length = strlen(gNode->correctWord);
	for (i = 0; i < word_length; i++) {
 		gNode->part_word[i] = '-';
	}
	gNode->part_word[i] = '\0';

	// add node to list
	GameList_push(gList, gNode);

	// format message to send back to client
	gethostname(hostname, MAXLEN);
	sprintf(outbuf, "Playing hangman on host: %s \n%s %d \n", hostname, gNode->part_word, gNode->numOfLives);
	
	printf("Sending %s\n", outbuf);

	int count, buflen; // control variables for length of output buffer and sent characters
	buflen = strlen(outbuf); // calculate how many characters to send
	// send reply to client
	count = sendto(sfd, outbuf, buflen, 0, (struct sockaddr*) client, client_len);
	if(count == -1)
		perror("sendto fail");
}	

 /* ---------------- Play_Hangman () ---------------------*/
 play_hangman (GameNode* gNode, char* guess, int sfd, struct sockaddr_in* client, socklen_t client_len) {
 	printf("Client: %s already in list, sent: %s", username, message);

 	char* outbuf [MAXLEN]; // buffer for server response to client
 	int i, good_guess, word_length; // control variables

 	/* game is not over yet */
 	if (gNode->state == INCOMPLETE) {
 		//get lenght of word associated to GameNode gNode 
 		word_length = strlen(gNode->correctWord);

 		good_guess = 0; // set it to FALSE
 		// loop through the correct word and check if client's guess appears in it
 		for (i = 0; i < word_length; i++) {
 			if (guess[0] == gNode->correctWord[i]) {
 				good_guess = 1; // found at least one match
 				gNode->part_word[i] = gNode->correctWord[i]; // reveal guessed characters
 			}
 		}
 		
 		// decrement live if incorrect guess was made
 		if (! good_guess) 
 			gNode->numOfLives--;

 		if (strcmp (gNode->correctWord, gNode->part_word) == 0) {
 			// client guessed the entire word
 			gNode->state = WON; // client Won
 			sprintf(outbuf, "W_You Won, thank you for playing \n");
 		} else if (gNode->numOfLives == 0) {
 			// client consumed all his lives
 			gNode->state = LOST; // client Lost
 			sprintf(outbuf, "L_You Lost, try again \nCorrect word: %s\n", gNode->correctWord);
 		} else {
 			// Game is stil going
 			// Format server respons with updated partial word and number of lives of the user
 			sprintf (outbuf, "I_%s %d \n", gNode->part_word, gNode->numOfLives);
 		}

 		printf("Sending %s", outbuf);

 		int count, buflen; // control variables for length of output buffer and sent characters
		buflen = strlen(outbuf); // calculate how many characters to send
 		// send reply to client
 		count = sendto(sfd, outbuf, buflen, 0, (struct sockaddr*) client, client_len);
		if(count == -1 )
			perror("sendto fail");
 	}
 }
