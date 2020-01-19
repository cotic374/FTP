#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>



#define LISTENQ 5
#define BUFFSIZE 1024

/*TBD
void upload();
void mainmenu(){
	int option;
	scanf("%d",&option);
	switch(option){

		case 1//przeslij plik
			upload();
			break;
		case 2//pobierz plik
			download();
			break;
		case 3//zmien nazwe
			rename();
			break;
		case 4//wyszukaj plik
			findfile();
			break;
		case 5//wyswietl pliki
			displayfiles();
			break;
		case 6//zmien biezacy katalog
			changedirectory();
			break;
		case 7 //wyjdz
			exit(1)
}
*/
/*----zaladowanie wyjscia terminalu do bufora----*/
void socket_read_bytes(int socket_fd, const char* buf, size_t size) {
  size_t pos = 0;

  while (pos < size) {
    ssize_t n = send(socket_fd, buf + pos, size - pos,0);
    if (n == -1) {
      puts("send error"); break;
    }
    if (n == 0) {
      break;
    }

    pos += n;
  }
}
void socket_write_bytes(int socket_fd, const char* buf, size_t size) {
  size_t pos = 0;

  while (pos < size) {
    ssize_t n = send(socket_fd, buf + pos, size - pos,0);
    if (n <= 0) {
      puts("send error"); break;
    }

    pos += n;
  }
}

char* read_command_output(const char* command) {
    const int CHUNK = 1024;

    FILE* pipe = popen(command, "r");
    if (pipe == NULL) { puts("popen failed"); exit(1); }

    int capacity = CHUNK;
    int position = 0;
    char* buffer = malloc(capacity);
    if (buffer == NULL) { puts("malloc failed"); exit(1); }

    for (;;) {
        if (position + CHUNK > capacity) {
            capacity *= 2;
            buffer = realloc(buffer, capacity);
            if (buffer == NULL) { puts("realloc failed"); exit(1); }
        }
        int bytes_read = fread(buffer + position, 1, CHUNK, pipe);
        position += bytes_read;
        if (bytes_read < CHUNK) {
            break;
        }
    }

    buffer = realloc(buffer, position + 1);
    if (buffer == NULL) { puts("realloc failed"); exit(1); }
    buffer[position] = '\0';

    pclose(pipe);
    return buffer;
}


int main(int argc, char* argv[])
{
    int listenfd, connfd;
    char buf[BUFFSIZE], command[5], filename[20];
    struct sockaddr_in serverAddr, cliAddr;
    int k, i, size, len, sndbuf;
    socklen_t clilen;
    char* huj;
    pid_t pid;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        printf("Socket creation failed");
        exit(1);
    }
	serverAddr.sin_family = AF_INET;

    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(7891);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sndbuf = 1;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &sndbuf, sizeof(sndbuf)) < 0){
                printf("setsockopt error");
        }
    /*---- Bind the address struct to the socket ----*/
    k = bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (k == -1) {
        printf("Binding error");
        exit(1);
    }
    /*---- Listen on the socket, with 5 max connection requests queued ----*/
    if (listen(listenfd, LISTENQ) == -1) {
        printf("Listening error\n");
        exit(1);
    }
    /*---- Accept call creates a new socket for the incoming connection ----*/

    /*---- Send message to the socket of the incoming connection ----*/
    /*----Koniec inicjalizacji, poczatek obslugi komend ----*/
    while (1) {
        clilen = sizeof(cliAddr);
        if ((connfd = accept(listenfd, (struct sockaddr *) &cliAddr, &clilen)) < 0) {
            printf("Accept error");
            exit(1);
        }
        if ((pid = fork()) == -1) {
            close(connfd);
            continue;
        }
        else if (pid > 0) {
            close(connfd);
            continue;
        }
        else if (pid == 0) {
	    close(listenfd);
            /*----pobranie komendy----*/
            recv(connfd, buf, sizeof(buf), 0);
printf(buf);
printf("1");
            sprintf(command, "%s", buf);
printf(command);
sleep(2);
	    huj=read_command_output(command);
	    sprintf(buf, "%s", huj);
printf(buf);
            send(connfd, buf, BUFFSIZE,0);
        }
    }
    return 0;
}
