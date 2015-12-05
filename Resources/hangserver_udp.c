 /* Network server for hangman game */
 /* File: hangserver.c */

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
# include "words"
};
# define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
//# define MAXLEN 80 /* Maximum size in the world of Any string */
# define HANGMAN_UDP_PORT 1067

int main ()
 {
 	int sock, fd;
 	socklen_t client_len;
 	struct sockaddr_in server, client;

 	srand ((int) time ((long *) 0)); /* randomize the seed */

 	sock = socket (AF_INET, SOCK_DGRAM, 0);//0 or IPPROTO_TCP
 	if (sock <0) { //This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating datagram socket");
 		exit (1);
 	}

 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = htonl(INADDR_ANY);
 	server.sin_port = htons(HANGMAN_UDP_PORT);

 	if (bind(sock, (struct sockaddr *) &server, sizeof(server)) <0) {
 		perror ("binding socket");
	 	exit (2);
 	}

 	// using UDP no need for listen
 	//listen (sock, 5);

 	// initialise game list
 	GameList* gList = GameList_create();
 	GameNode* gNode;
 	char buffer[MAXLEN];
 	int count;

 	printf("Ready to Receive v2\n");
 	while (1) {

 		client_len = sizeof (client);
 		count = recvfrom(sock, buffer, MAXLEN, 0, (struct sockaddr*) &client, &client_len);
 		if (count == 0) {
 			perror ("lost connection");
 			continue;
 		}
 		buffer[count] = '\0';
 		printf("(Received %s)\n", buffer);

 		printf("out of IF\n");

 		// detect valid formatted string from client
 		if(strstr(buffer, "_") != NULL)
 		{
 			char* username = strtok(buffer, "_");
 			char* message = strtok(NULL, "_");
 			printf("In IF with username %s and message %s \n", username, message);
 			//look if client already connected
 			gNode = GameList_searchGameNodeByUsername(gList, username);
 			if(gNode == NULL)
 			{
 				// client not in list, create new and add it to list
 				add_user(gList, username, sock, &client, client_len);
 				//gNode = GameNode_create(GameList_count(gList), username, )

 			} else {
 				//play_hangman(gNode, message, client_len, client_len);
 				printf("client: %s already connected, sent: %s \n", username, message);
 				play_hangman(gNode, message, sock, &client, client_len);
 			}
 		}
 		//play_hangman (fd, fd);
 		close (fd);
 	}
 }

 /* ---------------- Add_User () -----------------------*/
add_user(GameList* gList, char* username, int sfd, struct sockaddr_in* client, socklen_t client_len)
{
	// print state
	printf("Adding new client %s\n", username);

	// create a new GameNode and fill details
	//char* whole_word, part_word[MAXLEN], outbuf[MAXLEN];
	char* outbuf[MAXLEN], part_word[MAXLEN];
	int i, word_length;
	char hostname[MAXLEN];

	//whole_word = word[rand() % NUM_OF_WORDS];
	//word_length = strlen(whole_word);

	//create new GameNode and add it to the list
	GameNode* gNode = GameNode_create(GameList_count(gList), username, word[rand() % NUM_OF_WORDS], maxlives);
	word_length = strlen(gNode->correctWord);
	printf("Word chosen is %s\n", gNode->correctWord);

	/* no letters are guessed initially */
	for (i = 0; i < word_length; i++)
	{
 		gNode->part_word[i] = '-';
 		printf("part_word at %d is %c \n", i, gNode->part_word[i]);
	}
	gNode->part_word[i] = '\0';

	//gNode->part_word = part_word;
	printf("par word %s\n", gNode->part_word);
	//gNode->state = I;

	// add node to list
	GameList_push(gList, gNode);

	// gNode set, send message back to client
	gethostname(hostname, MAXLEN);
	sprintf(outbuf, "Playing hangman on host: %s \n%s %d \n", hostname, gNode->part_word, gNode->numOfLives);
	//sprintf(outbuf, "Playing hangman:\n%s %d \n\0", gNode->part_word, gNode->numOfLives);
	

	printf("Sending %s\n", outbuf);

	int count, buflen;
	buflen = strlen(outbuf);
	printf("buflen %d\n", buflen);

	count = sendto(sfd, outbuf, buflen, 0, (struct sockaddr*) client, client_len);
	if(count == -1 )
		perror("sendto fail");
	printf("count %d\n", count);
}	

 /* ---------------- Play_hangman () ---------------------*/

 //play_hangman (int in, int out)
 play_hangman (GameNode* gNode, char* guess, int sfd, struct sockaddr_in* client, socklen_t client_len)
 {
 	char* outbuf [MAXLEN];

 	//int lives = maxlives;
 	//int game_state = INCOMPLETE;//I = Incomplete
 	int i, good_guess, word_length;
 	//char hostname[MAXLEN];

 	/* Ggame is not over yet */
 	if (gNode->state == INCOMPLETE)
 	{
 		/*
		while (read (in, guess, MAXLEN) <0) {
 			if (errno != EINTR)
 				exit (4);
 			printf ("re-read the startin \n");
 			} // Re-start read () if interrupted by signal 
 		*/
 		//update lives and 
 		word_length = strlen(gNode->correctWord);
 		printf("In PLAY HANGMAN with correct word %s\n", gNode->correctWord);

 		good_guess = 0;
 		for (i = 0; i <word_length; i++) {
 			if (guess[0] == gNode->correctWord[i]) {
 				good_guess = 1;
 				gNode->part_word[i] = gNode->correctWord[i];
 			}
 		}
 		printf("In PLAY HANGMAN with part word now: %s\n", gNode->part_word);
 		
 		// decrement live
 		if (! good_guess) 
 			gNode->numOfLives--;

 		if (strcmp (gNode->correctWord, gNode->part_word) == 0)
 		{
 			gNode->state = WON; /* W ==> User Won */
 			sprintf(outbuf, "W_You Won, thank you for playing \n");
 		} 
 		else if (gNode->numOfLives == 0) 
 		{
 			gNode->state = LOST; /* L ==> User Lost */
 			sprintf(outbuf, "L_You Lost, try again \nCorrect word: %s\n", gNode->correctWord);
 			//strcpy (part_word, whole_word); /* User Show the word */
 		}
 		else
 		{
 			sprintf (outbuf, "I_%s %d \n", gNode->part_word, gNode->numOfLives);
 		}

 		printf("Sending %s\n", outbuf);

 		int count, buflen;
		buflen = strlen(outbuf);
		printf("buflen %d\n", buflen);
 		
 		count = sendto(sfd, outbuf, buflen, 0, (struct sockaddr*) client, client_len);
		if(count == -1 )
			perror("sendto fail");
		printf("count %d\n", count);
 	}
 }
