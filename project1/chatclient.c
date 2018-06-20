#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

void getconnection (char *v[], int sockfd, int portnum){
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // get server
    server = gethostbyname(v[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portnum);

    // if connect successfully
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "ERROR, connection fail\n");
        exit(1);
    }
}


int main (int argc, char *argv[]){
	int sockfd, portnum;
	char temp[501];
   	char buffer[501];

	// if command valid
	{
		if (argc != 3){
			fprintf(stderr, "invalid command line\n");
			exit(1);
		}

		if (atoi(argv[2]) == 0){
			fprintf(stderr, "%s is not a valid portnum\n", argv[2]);
			exit(1);
		}
	}

	// store portnum
	portnum = atoi(argv[2]);

	// open socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
      	fprintf(stderr, "ERROR, fail to open socket\n");
      	exit(1);
   	}

   	// connect
    getconnection(argv, sockfd, portnum);

   	// get message and send to server
   	int status = 0;
    int i = 0, j, c;
    char hostb[12];

    while (1){
        // if open again
        if (strcmp(buffer, "\\quit") == 0){
            // open socket
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                fprintf(stderr, "ERROR, fail to open socket\n");
                exit(1);
            }

            // connect
            getconnection(argv, sockfd, portnum);
        }

        // if in one word
        j = 1;
        while (j){
            printf("Your \'handle\': ");
            bzero(hostb, 12);
            fgets(hostb, 11, stdin);
            for (; i < strlen(hostb); i++){
                if (hostb[i] == ' '){
                    printf("In one word\n");
                    j = 1;
                    break;
                }
                j = 0;
            }
            i = 0;
        }

        setbuf(stdin,NULL);
        hostb[strcspn(hostb, "\n")] = '\0';
        //printf("%s--\n", hostb);

        while (1){
            printf("%s: ", hostb);
            memset(buffer, '\0', 501);
            memset(temp, '\0', 501);
            fgets(buffer, 500, stdin);
            strcat(temp, hostb);
            strcat(temp, ": ");
            strcat(temp, buffer);

            // send
            status = write(sockfd, temp, strlen(temp));
            if (status < 0){
                fprintf(stderr, "ERROR, fail to write\n");
                exit(1);
            }

            // if host b quit
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strcmp(buffer, "\\quit") == 0){
                status = shutdown(sockfd, SHUT_RDWR);
                if (status < 0){
                    fprintf(stderr, "ERROR, fail to quit\n");
                    exit(1);
                }
                else {
                    printf("you quit\n");
                    //break;
                    return;
                }
            }

            // read
            bzero(buffer, 501);
            status = read(sockfd, buffer, 500);
            if (status < 0){
                fprintf(stderr, "ERROR, fail to read\n");
                exit(1);
            }

            // if server quit
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strcmp(buffer, "\\quit") == 0){
                status = shutdown(sockfd, SHUT_RDWR);
                if (status < 0){
                    fprintf(stderr, "ERROR, fail to quit\n");
                    exit(1);
                }
                else {
                    printf("server quit\n");
                    //break;
                    return;
                }
            }
            else printf("%s\n\n", buffer);
        }
    }

	return 0;
}