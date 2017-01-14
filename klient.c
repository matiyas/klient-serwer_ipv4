#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int sockfd, type, tcp = 0, udp = 0;
	struct sockaddr_in addr;
	char buf[255];

	if(argc != 3) {
		printf("Program wymaga podania protokołu komunikacyjnego oraz adresu serwera");
		exit(1);
	}

	/* Sprawdzanie opcji protokołu komunikacyjnego */
		 if(strcmp(argv[1], "-t") == 0) tcp = 1;
	else if(strcmp(argv[1], "-u") == 0) udp = 1;
	else {
		printf("Podano błędną opcję.\nPodaj -t dla TCP lub -u dla UDP\n");
		exit(1);
	}

	/* Ustawienie zależnie od protokołu semantyki komunikacji */
	if(tcp) type = SOCK_STREAM;
	if(udp) type = SOCK_DGRAM;

	/* Tworzenie gniazda */
	if((sockfd = socket(AF_INET, type, 0)) < 0) {
		perror("Wystąpił błąd podczas tworzenia gniazda");
		exit(1);
	}

	/* Ustalanie adresu serwera */
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[2]);
	addr.sin_port = htons(8796);
	
	/* Dla TCP: nawiązywanie połączenia z serwerem */
	if(tcp) {
		if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("Wystąpił błąd podczas nawiązywania połączenia z serwerem");
			exit(1);
		}
	}

	bzero(buf, sizeof(char)*255);
	printf("Treść wiadomości: ");
	scanf("%s", buf);
		
	/* TCP: Wysyłanie wiadomości */
	if(tcp)	write(sockfd, buf, sizeof(char)*(strlen(buf)+1));

	/* UDP: Wysyłanie wiadomości */
	if(udp) {
		if(sendto(sockfd, buf, sizeof(char)*(strlen(buf)+1), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("Wystąpił błąd podczas wysyłania wiadomości");
			exit(1);
		}
	}

	bzero(buf, sizeof(char)*255);

	/* TCP: Odbieranie wiadomości zwrotnej od serwera */
	if(tcp) read(sockfd, buf, sizeof(char)*255);

	/* UDP: Odbieranie wiadomości zwrotnej od serwera */
	if(udp) {
		if(recv(sockfd, buf, sizeof(char)*255, 0) < 0) {
			perror("Wystąpił błąd podczas odbierania wiadomości");
			exit(1);
		}
	}

	printf("%s", buf);
	
	close(sockfd);
	return 0;
}
