 /* Network server for hangman game */
 /* File: hangserver.c */

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <stdio.h>
 #include <syslog.h>
 #include <signal.h>
 #include <errno.h>

 extern time_t time ();

 int maxlives = 12;
 char *word [] = {
 # include "words"
 };
 # define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
 # define MAXLEN 80 /* Maximum size in the world of Any string */
 # define HANGMAN_TCP_PORT 1066

 main ()
 {
 	int sock, fd, client_len;
 	//master file descriptor list
 	fd_set master;
 	// temp file descriptor list for select ()
 	fd_set read_fds

 	// maximum file desciptor number
 	int fdmax; 

 	// listening socket descriptor
 	int listener;

 	// newly accepted socket descriptor
 	int newfd;

 	// buffer for client data
 	char buffer[1024];
 	int nbytes;

 	// for setsockopt() SO-REUSEADDR, below
 	int yes = 1;
 	int addrlen;
 	int i,j;

 	// clear the master and temp sets
 	FD-ZERO(&master);
 	FD_ZERO(&read_fds);

 	
 	struct sockaddr_in server, client;

 	srand ((int) time ((long *) 0)); /* randomize the seed */
	
 	/*
 	creating the socket 
 	sock = socket (AF_INET, SOCK_STREAM, 0);//0 or IPPROTO_TCP
 	if (sock <0) { //This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}
	*/

	//get the listener
	if((listener = socker (AF_INET, SOCK_STREAM, 0))) == -1)
 	{
 		perror("Server-socket() error!")
 		exit(1)
 	}
 	printf("Server-socket() is OK...........\n");

 	// BIND
 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = htonl(INADDR_ANY);
 	server.sin_port = htons(HANGMAN_TCP_PORT);
 	memset(&(serveraddr.sin_zero), '\0', 8);

 	if (bind(listener, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) ==-1) 
 	{
 		perror ("binding socket error!");
	 	exit (2);
 	}
 	printf("Server-bind() is OK....");
	
	// LISTEN 	
 	//listen (sock, 5);
 	if (listen(listener, 10) == -1)
 	{
 		perror("Server-listen() error!");
 		exit(1);
 	}
 	printf("Server-listen() is OK....\n");

 	// adding the listener to the master set
 	FD-SET(listener, &master);

 	// keep track of the biggest file descriptor
 	fdmax = listener;

 	

 	while (1) {
 		client_len = sizeof (client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) <0) {
 			perror ("accepting connection");
 			exit (3);
 		}
 		play_hangman (fd, fd);
 		// cant just close? 

 		close (fd);
 	}
 }

 /* ---------------- Play_hangman () ---------------------*/

 play_hangman (int in, int out)
 {
 	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

 	int lives = maxlives;
 	int game_state = 'I';//I = Incomplete
 	int i, good_guess, word_length;
 	char hostname[MAXLEN];

 	gethostname (hostname, MAXLEN);
 	sprintf(outbuf, "Playing hangman on host% s: \n \n", hostname);
 	write(out, outbuf, strlen (outbuf));

 	/* Pick a word at random from the list */
 	whole_word = word[rand() % NUM_OF_WORDS];
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	// tell me what is going on
 	printf("Word chosen is %s\n", whole_word);

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';
 	
	part_word[i] = '\0';

 	sprintf (outbuf, "%s %d \n", part_word, lives);
 	write (out, outbuf, strlen(outbuf));

 	while (game_state == 'I')
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, MAXLEN) <0) {
 			if (errno != EINTR)
 				exit (4);
 			printf ("re-read the startin \n");
 			} /* Re-start read () if interrupted by signal */
 	good_guess = 0;
 	for (i = 0; i <word_length; i++) {
 		if (guess [0] == whole_word [i]) {
 		good_guess = 1;
 		part_word [i] = whole_word [i];
 		}
 	}
 	if (! good_guess) lives--;
 	if (strcmp (whole_word, part_word) == 0)
 		game_state = 'W'; /* W ==> User Won */
 	else if (lives == 0) {
 		game_state = 'L'; /* L ==> User Lost */
 		strcpy (part_word, whole_word); /* User Show the word */
 	}
 	sprintf (outbuf, "%s %d \n", part_word, lives);
 	write (out, outbuf, strlen (outbuf));
 	}
 }
