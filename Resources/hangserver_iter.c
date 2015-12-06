 /* Network server for hangman game */
 /* File: hangserver.c */

 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <stdio.h>
 #include <syslog.h>
 #include <signal.h>
 #include <errno.h>
 #include <sys/wait.h>
 #include "Practical.h"
 #include "Hangman.h"

/* Signal Handler function that calls waitpid for all terminated children */
 	void
sig_chld(int sigNumber)
{
	pid_t	pid;
	int		stat;

	/* first argument - allows to specify the process id to wait for
	   -1 specifies to wait for the first child to terminate 
	   Second argument - termination status
	    Third argument - options argument - most commonly uses
	    is WNOHANG which tells the kernal not to block if there 
	    are no terminated children 
	*/
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		 printf("child %d terminated\n", pid); 
	}
	return;
}

 int main ()
 {
 	int sock, fd, client_len, pidCount;
 	struct sockaddr_in server, client;
 	pid_t pid;
 	pidCount = 0;

	sock = socket (AF_INET, SOCK_STREAM, 0);//0 or IPPROTO_TCP
 	if (sock <0) { //This error checking is the code Stevens wraps in his Socket Function etc
 		perror ("creating stream socket");
 		exit (1);
 	}

 	server.sin_family = AF_INET;
 	server.sin_addr.s_addr = htonl(INADDR_ANY);
 	server.sin_port = htons(HANGMAN_TCP_PORT);

 	if (bind(sock, (struct sockaddr *) & server, sizeof(server)) <0) {
 		perror ("binding socket");
	 	exit (2);
 	}

 	listen (sock, 5);

 	/* Installs signal handler by calling signal and sending in the signal it wants to catch and 
 	   the handler function it wants to use to handle it */
 	signal(SIGCHLD, sig_chld);

 	for( ; ;) {

 		client_len = sizeof (client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) < 0) {
 			if(errno == EINTR)
 			{
 				continue;	/* Forces program to go back to for loop (Handles interrupts in accept) */
 			}
 			else
 			{
 			perror ("accepting connection");
 			exit (3);
 			}
 			
 		}
		
		pid = fork();
		/*Fork Implementation*/
		/* If accept returns successfully the server calls fork which creates a new process known as the 
			child. The child process services the client while the parent process listens for more connections */
		if (pid == 0){	/* if pid is 0, fork is successful */
		  printf("pid value after fork is %d \n", pid);
		  srand ((int) time ((long *) 0));
		  close(sock);	/* Child closes listening socket */
		  play_hangman (fd, fd); /* services the client with hangman game */ 
		  exit(0);
 		}
 		else if (pid < 0)  /*if pid < 0 then an error has occurred */
 		{
 			perror( "Fork Failed" );
 		}

 		close(fd);	/* Parent closes the connected socket */
 		
		}
 
 }


