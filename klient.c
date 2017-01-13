#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in addr;
	char buf[255];

	if(argc != 2) {
		printf("Program wymaga adresu serwera jako argumentu\n");
		exit(1);
	}

	/* Tworzenie gniazda */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Wystąpił błąd podczas tworzenia gniazda");
		exit(1);
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(8796);
	
	/* Podczepianie gniazda pod adres i port */
	/*if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Wystąpił błąd podczas podczepiania gniazda pod adres i port");
		exit(1);
	}*/
	
	/* Główna pętla */
	while(1) {
		bzero(buf, sizeof(char)*255);
		scanf("%s", buf);

		/* Wysyłanie wiadomości */
		if(sendto(sockfd, buf, sizeof(char)*(strlen(buf)+1), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("Wystąpił błąd podczas wysyłania wiadomości");
			exit(1);
		}

		bzero(buf, sizeof(char)*255);

		/* Odbieranie wiadomości zwrotnej od serwera */
		if(recv(sockfd, buf, sizeof(char)*255, 0) < 0) {
			perror("Wystąpił błąd podczas odbierania wiadomości");
			exit(1);
		}

		printf("%s\n", buf);
	}

	return 0;
}
