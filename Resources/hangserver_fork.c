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

 	void
sig_chld(int sigNumber)
{
	pid_t	pid;
	int		stat;

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

 	signal(SIGCHLD, sig_chld);

 	for( ; ;) {

 		client_len = sizeof (client);
 		if ((fd = accept (sock, (struct sockaddr *) &client, &client_len)) < 0) {
 			if(errno == EINTR)
 			{
 				continue;
 			}
 			else
 			{
 			perror ("accepting connection");
 			exit (3);
 			}
 			
 		}
		
		pid = fork();
		/*Fork Implementation*/
		if (pid == 0){
		  printf("pid value after fork is %d \n", pid);
		  srand ((int) time ((long *) 0));
		  close(sock);
		  play_hangman (fd, fd);
		  exit(0);
 		}
 		else if (pid < 0)
 		{
 			perror( "Fork Failed" );
 		}

 		close(fd);
 		
		}
 
 }


