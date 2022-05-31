#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LENGTE 2048


static int flag = 0;
int sockfd = 0;
char naam[32];

void str_over() {
  printf("%s ", "> ");
  fflush(stdout);
}

void str_trim(char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { 
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void catch_exit(int sig) {
    flag = 1;
}

void bericht_handler() { 
char bericht[LENGTE] = {};
char buffer[LENGTE + 32] = {};

  while(1) {
  	str_over();
    fgets(bericht, LENGTE, stdin);
    str_trim(bericht, LENGTE);

    if (strcmp(bericht, "exit") == 0) {
			break;
    } else {
      sprintf(buffer,"%s: %s\n",naam, bericht);
      send(sockfd, buffer, strlen(buffer), 0);
    }

		bzero(bericht, LENGTE);
    bzero(buffer, LENGTE + 32);
  }
  catch_exit(2);
}

    void inkomend_bericht() {
	char bericht[LENGTE] = {};
    while (1) {
		int ontvang = recv(sockfd, bericht, LENGTE, 0);
    if (recv > 0) 
	{
      printf("%s", bericht);
      str_over();
    } 
	else if (recv == 0) 
	{
			break;
    }
		memset(bericht, 0, sizeof(bericht));
  }
}

int main(int argc, char **argv){
	if(argc != 2)
	{
		printf("gebruikte poort: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);

	signal(SIGINT, catch_exit);

	printf("Geef een naam in: ");
    fgets(naam, 32, stdin);
    str_trim(naam, strlen(naam));

	struct sockaddr_in server_addr;
//instelling voor de sockets
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);


//probbert te verbinden met de server
  int error = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (error == -1) {
		printf("Verbindings probleem\n");
		return EXIT_FAILURE;
	}

//functie om de naam te sturen
	send(sockfd, naam, 32, 0);

	printf("Welkom\n");

	pthread_t send_msg_thread;
    if(pthread_create(&send_msg_thread, NULL, (void *) bericht_handler, NULL) != 0){
		
		printf("ERROR: pthread\n");
    return EXIT_FAILURE;
	}

	pthread_t recv_msg_thread;
  if(pthread_create(&recv_msg_thread, NULL, (void *) inkomend_bericht, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}

	while (1){
		if(flag){
			printf("\nBye\n");
			break;
    }
	}

	close(sockfd);

	return EXIT_SUCCESS;
}