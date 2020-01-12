/****************** SERVER CODE ****************/
/*struct stat {
    dev_t     st_dev;      ID of device containing file 
    ino_t     st_ino;      inode number 
    mode_t    st_mode;     protection 
    nlink_t   st_nlink;    number of hard links 
    uid_t     st_uid;      user ID of owner 
    gid_t     st_gid;      group ID of owner 
    dev_t     st_rdev;     device ID (if special file) 
    off_t     st_size;     total size, in bytes 
    blksize_t st_blksize;  blocksize for file system I/O 
    blkcnt_t  st_blocks;   number of 512B blocks allocated 
    time_t    st_atime;    time of last access 
    time_t    st_mtime;    time of last modification 
    time_t    st_ctime;    time of last status change 
};
*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>



#define LISTENQ 5

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


void exec(int connfd, char* buffer, int buffersize, const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {

            send(connfd, buffer, strlen(buffer),0); 
    }
    pclose(pipe);
}

int main(int argc, char* argv[])
{
    int listenfd, connfd;
    char buf[100], command[5], filename[20];
    struct sockaddr_in serverAddr, cliAddr;
    int k, i, size, len, sndbuf;
    socklen_t clilen;
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
printf("1 ");
            sscanf(buf, "%s", command);
printf(command);
sleep(2);
	    exec(connfd, buf, sizeof(buf), command);
printf("4 ");
     
        }
    }
    return 0;
}
