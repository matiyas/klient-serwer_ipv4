#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

int main() {
	socklen_t addrlen;
	int sockfd;
	struct sockaddr_in srv_addr;
	struct sockaddr_in cli_addr;
	char buf[255];
	time_t time_buf;
	pid_t pid, pid2;
	
	/* Tworzenie procesu potomnego */
	if((pid = fork()) < 0) {
		perror("Wystąpił błąd podczas tworzenia procesu potomnego");
		exit(1);
	}
	
	/* Zakończ działanie procesu macierzystego */
	else if(pid > 0) {
		exit(0);
	}
	
	else {
		signal(SIGHUP, SIG_IGN);
		
		/* Tworzenie nowej sesji */
		if(setsid() < 0) {
			perror("Wystąpił błąd podczas tworzenia nowej sesji");
			exit(1);
		}

		/* Tworzenie kolejnego potomka */
		if((pid2 = fork()) < 0) {
			perror("Wystąpił błąd podczas tworzenia potomka");
			exit(1);
		}

		/* Zakończ działanie procesu macierzystego */
		else if(pid2 > 0) {
			exit(0);
		}

		else {
			/* Zmiana katalogu roboczego */
			if(chdir("/") < 0) {
				perror("Wystąpił błąd podczas zmiany katalogu roboczego");
				exit(1);
			}

			umask(0);	/* Zmiana domyślnych praw tworzenia plików */
			
			/* Zamykanie standardowych deskryptorów plików */
			close(0);
			close(1);
			close(2);

			/* Tworzenie gniazda */
			if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
				perror("Wystąpił błąd podczas tworzenia gniazda");
				exit(1);
			}

			bzero(&srv_addr, sizeof(srv_addr));
			srv_addr.sin_family = AF_INET;
			srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			srv_addr.sin_port = htons(8796);
	
			addrlen = sizeof(cli_addr);	

			/* Poczepianie gniazad pod port */
			if(bind(sockfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0) {
				perror("Wystąpił błąd podczas podczepiania gniazda do portu");
				exit(1);
			}

			/* Główna pętla */
			while(1) {
				bzero(&buf, sizeof(char)*255);
		
				/* Odbieranie wiadomości */
				if(recvfrom(sockfd, buf, sizeof(char)*255, 0, (struct sockaddr *)&cli_addr, &addrlen) < 0) {
					perror("Wystąpił błąd podczas odbierania wiadomości");
					exit(1);
				}

				/* Pobieranie czasu serwera */
				if(strcmp(buf, "czas") == 0) {
					bzero(buf, sizeof(char)*255);
					bzero(&time_buf, sizeof(time_t));
			
					if((time_buf = time(NULL)) < 0) {
						perror("Wystąpił błąd podczas pobieranie czasu");
						exit(1);
					}

					strcpy(buf, ctime(&time_buf));
				} 

				/* Kopiowanie do bufora komunikatu o błędnym zapytaniu */
				else {
					bzero(buf, sizeof(char)*255);
					strcpy(buf, "błąd zapytania");
				}
		
				/* Wysyłanie wiadomości zwrotnej */
				if(sendto(sockfd, buf, sizeof(char)*(strlen(buf)+1), 0, (struct sockaddr *)&cli_addr, addrlen) < 0) {
					perror("Wystąpił błąd podczas wysłania wiadomości zwrotnej");
					exit(1);
				}
			}

		}
	}

	return 0;
}
