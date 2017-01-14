#include <sys/select.h>
#include <netinet/in.h>
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
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int fd_tcp, fd_udp, connfd, maxfd;
	struct sockaddr_in srv_addr;
	struct sockaddr_in cli_addr;
	char buf_snd[255], buf_rec[255];
	time_t time_buf;
	pid_t pid, pid2;
	fd_set readfds;
	
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
			/* Zmiana domyślnych praw tworzenia plików */
			umask(0);	
			
			/* Zamykanie standardowych deskryptorów plików */
			close(0);
			close(1);
			close(2);

			/* Tworzenie gniazda dla UDP */
			if((fd_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
				perror("Wystąpił błąd podczas tworzenia gniazda dla protokołu UDP");
				exit(1);
			}

			/* Tworzenie gniazda dla TCP */
			if((fd_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				perror("Wystąpił błąd podczas tworzenia gniazda dla protokołu TCP");
				exit(1);
			}

			/* Ustawianie adresu */
			bzero(&srv_addr, addrlen);
			srv_addr.sin_family = AF_INET;
			srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			srv_addr.sin_port = htons(8796);
	
			/* Poczepianie gniazd pod port */
			if(bind(fd_udp, (struct sockaddr *)&srv_addr, addrlen) < 0) {
				perror("Wystąpił błąd podczas podczepiania gniazda do portu");
				exit(1);
			}
			
			if(bind(fd_tcp, (struct sockaddr *)&srv_addr, addrlen) < 0) {
				perror("Wystąpił błąd podczas podczepiania gniazda do portu");
				exit(1);
			}

			maxfd = (fd_udp > fd_tcp ? fd_udp+1 : fd_tcp+1);

			/* Nasłuchiwanie wiadomości */
			if(listen(fd_tcp, 0) < 0) {
				perror("Wystąpił błąd podczas nasłuchiwania");
				exit(1);
			}
			
			/* Główna pętla */
			while(1) {
				/* Czyszczenie buforów */
				bzero(&buf_snd, sizeof(char)*255);
				bzero(&buf_rec, sizeof(char)*255);
				bzero(&time_buf, sizeof(time_t));

				/* Ustawianie zastawu deskryptorów */
				FD_ZERO(&readfds);
				FD_SET(fd_udp, &readfds);
				FD_SET(fd_tcp, &readfds);
				
				/* Monitorowanie deskryptorów */
				if(select(maxfd, &readfds, NULL, NULL, NULL) < 0) {
					perror("Wystąpił błąd podczas sprawdzania deskryptorów do odczytu");
					exit(1);
				}

				/* UDP: Odbieranie wiadomości */
				if(FD_ISSET(fd_udp, &readfds)) {
					if(recvfrom(fd_udp, buf_rec, sizeof(char)*255, 0, (struct sockaddr *)&cli_addr, &addrlen) < 0) {
						perror("Wystąpił błąd podczas odbierania wiadomości");
						exit(1);
					}
				}
				
				if(FD_ISSET(fd_tcp, &readfds)) {
					if((connfd = accept(fd_tcp, (struct sockaddr *)&srv_addr, &addrlen)) < 0) {
						perror("Wystąpił błąd podczas akceptowania połączenia");
						exit(1);
					}

					if(recv(connfd, buf_rec, sizeof(char)*255, 0) < 0) {
						perror("Wystąpił błąd podczas odbierania wiadomości");
						exit(1);
					}					
				}

				/* Pobieranie czasu serwera lub komunikatu o błędnym zapytaniu */
				if(strcmp(buf_rec, "czas") == 0) {
					if((time_buf = time(NULL)) < 0) {
						perror("Wystąpił błąd podczas pobieranie czasu");
						exit(1);
					}
					strcpy(buf_snd, ctime(&time_buf));
				} 
				else {
					strcpy(buf_snd, "błąd zapytania\n");
				}
		
				/* UDP: Wysyłanie wiadomości zwrotnej */
				if(FD_ISSET(fd_udp, &readfds)) {
					if(sendto(fd_udp, buf_snd, sizeof(char)*(strlen(buf_snd)+1), 0,
							 (struct sockaddr *)&cli_addr, addrlen) < 0) {
						perror("Wystąpił błąd podczas wysłania wiadomości zwrotnej");
						exit(1);
					}
				}

				if(FD_ISSET(fd_tcp, &readfds)) {
					if(send(connfd, buf_snd, sizeof(char)*255, 0) < 0) {
						perror("Wystąpił błąd podczas wywsyłania wiadomości");
						exit(1);
					}
				}
			}

		}
	}

	return 0;
}
