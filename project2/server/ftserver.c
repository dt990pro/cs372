#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <dirent.h>
#include <fcntl.h>

/////////////////////////////////////////////////////////////////////////////////
void valid_command(int argc, char *argv[], int *portnum){
	if (argc != 2){
		fprintf(stderr, "invalid command line\n");
		exit(1);
	}

	if (atoi(argv[1]) == 0){
		fprintf(stderr, "%s is not a valid portnum\n", argv[2]);
		exit(1);
	}

	// store port number
	*portnum = atoi(argv[1]);
}

/////////////////////////////////////////////////////////////////////////////////
void init_files (char **files){
    int i;
    for (i = 0; i < 64; i++){
        files[i] = (char *) malloc(64 * sizeof(char));
        memset(files[i], '\0', 64);
    }
}

/////////////////////////////////////////////////////////////////////////////////
void getconnection (char datahostname[], int sockfd, int portnum){
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // get server
    server = gethostbyname(datahostname);
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
        fprintf(stderr, "ERROR, connection to %s fail\n", datahostname);
        exit(1);
    }
}

/////////////////////////////////////////////////////////////////////////////////
void mapfilename(char **files){
    int i = 0;
    DIR *current_dir;
    struct dirent *dir;
    current_dir = opendir(".");
    i = 0;
    if (current_dir){
        while (1){
            dir = readdir(current_dir);
            if (dir != NULL){
                if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) continue;
                if (dir->d_type == DT_REG){
                    strcpy(files[i], dir->d_name);
                    i++;
                }
                else if(dir->d_type == 4){
                    strcpy(files[i], "./");
                    strcat(files[i], dir->d_name);
                    i++;
                }
            }
            else {
                strcat(files[i], "--end");
                break;
            }
        }
        closedir(current_dir);
    }
}

/////////////////////////////////////////////////////////////////////////////////
int filevalid(char **files, char filename[]){
    int i = 0;
    for (; i < 64; i++){
        if (strcmp(files[i], filename) == 0) return 1;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////
void reset_files(char **files){
    int i;
    for (i = 0; i < 64; i++){
        memset(files[i], '\0', 64);
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]){
	int portnum, dataport, listenfd, connfd, status, sockfd;
	char hostname[64];
    char datahostname[64];
    char buff[64];
    char err_message[64] = "cmderr";
    char data_port_message[64] = "showdataport";
    char datahostname_message[64] = "datahostname";
    char **files;
    files = (char **) malloc(64 * sizeof(char *));
    char filename[64];
    char contents[1001];
    char cwd[1024];
    memset(contents, '\0', 1001);
	struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

	// check command validation
	valid_command(argc, argv, &portnum);

    // memset files
    init_files(files);
	
	// open socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) {
      	fprintf(stderr, "ERROR, fail to open socket\n");
      	exit(1);
   	}

   	gethostname(hostname, 64);
    printf("Current hostname: %s\n", hostname);

    // prepare
    serv_addr.sin_family = AF_INET;
 	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 	serv_addr.sin_port = htons(portnum);

    // bind
    status = bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (status < 0) {
        fprintf(stderr, "ERROR, fail to bind\n");
        exit(1);
    }

    // listen
    listen(listenfd, 3);
    printf("Server opens on %d\n\n", portnum);

    while(1){
        clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr *) &cli_addr, &clilen);
        memset(buff, '\0', 64);

        // receive
        while (status = recv(connfd, buff, 63, 0) > 0){
            /////////////////////////////////////////////////////////////////////////////////
            // check pw
            if (strcmp(buff, "silversun") != 0){
                printf("password err. try again\n");
                sleep(1);
                send(connfd, "pwerr", 5, 0);
            }
            else {
                send(connfd, "ok", 2, 0);
                memset(buff, '\0', 64);
                recv(connfd, buff, 63, 0);
                printf(": ");
                puts(buff);
                mapfilename(files);

                /////////////////////////////////////////////////////////////////////////////////
                // deal with cmd
                // list dir
                /////////////////////////////////////////////////////////////////////////////////
                if (strcmp(buff, "-l") == 0){

                    // get data port number
                    memset(buff, '\0', 64);
                    send(connfd, data_port_message, strlen(data_port_message), 0);
                    recv(connfd, buff, 63, 0);

                    dataport = atoi(buff);
                    printf("List dir requested on port %d\n", dataport);

                    // get data hostname
                    memset(buff, '\0', 64);
                    send(connfd, datahostname_message, strlen(datahostname_message), 0);
                    recv(connfd, buff, 63, 0);

                    memset(datahostname, '\0', 64);
                    strcat(datahostname, buff);
                    printf("Sending dir contents to %s : %d\n", datahostname, dataport);

                    // initiate data connection
                    send(connfd, "ok", 2, 0);
                    sleep(3);

                    // open socket
                    sockfd = socket(AF_INET, SOCK_STREAM, 0);
                    if (sockfd < 0) {
                        fprintf(stderr, "ERROR, fail to open socket\n");
                        exit(1);
                    }

                    // connect
                    getconnection(datahostname, sockfd, dataport);

                    // send file name
                    int i = 0;
                    for (; i < 64; i++){
                        send(sockfd, files[i], strlen(files[i]), 0);
                        if (strcmp(files[i], "--end") == 0) break;
                        else printf("%s\n", files[i]);
                        sleep(2);
                    }

                    // if change dir
                    memset(buff, '\0', 64);
                    recv(connfd, buff, 63, 0);
                    if (strcmp(buff, "chdir") == 0){
                        // get dir
                        memset(buff, '\0', 64);
                        recv(connfd, buff, 63, 0);

                        // change
                        if (chdir(buff) < 0){
                            printf("chdir err\n");
                        }
                        else {
                            getcwd(cwd, 1024);
                            printf("current dir: %s\n", cwd);
                        }
                    }

                    close(sockfd);
                }
                /////////////////////////////////////////////////////////////////////////////////
                // send the file
                /////////////////////////////////////////////////////////////////////////////////
                else if (strcmp(buff, "-g") == 0){

                    // get file name
                    memset(buff, '\0', 64);
                    send(connfd, "filename", 8, 0);
                    recv(connfd, buff, 63, 0);

                    // if file exist
                    if (filevalid(files, buff) == 0){
                        send(connfd, "fnameinvalid", 12, 0);
                    }
                    else{
                        memset(filename, '\0', 64);
                        strcat(filename, buff);

                        // get data port number
                        memset(buff, '\0', 64);
                        send(connfd, data_port_message, strlen(data_port_message), 0);
                        recv(connfd, buff, 63, 0);
                        dataport = atoi(buff);

                        printf("File \"%s\" requested on port %d\n", filename, dataport);

                        // get data hostname
                        memset(buff, '\0', 64);
                        send(connfd, datahostname_message, strlen(datahostname_message), 0);
                        recv(connfd, buff, 63, 0);

                        memset(datahostname, '\0', 64);
                        strcat(datahostname, buff);
                        printf("Sending \"%s\" to %s : %d\n", filename, datahostname, dataport);

                        // initiate data connection
                        send(connfd, "ok", 2, 0);
                        sleep(3);

                        // open socket
                        sockfd = socket(AF_INET, SOCK_STREAM, 0);
                        if (sockfd < 0) {
                            fprintf(stderr, "ERROR, fail to open socket\n");
                            exit(1);
                        }

                        // connect
                        getconnection(datahostname, sockfd, dataport);

                        // send contents
                        int fd, bytes_read, bytes_send;
                        char *ptr;
                        memset(buff, '\0', 64);
                        sprintf(buff, "./%s", filename);

                        // open file
                        if ((fd = open(buff, O_RDONLY)) == -1){
                            fprintf(stderr, "ERROR, fail to open %s\n", buff);
                            exit(1);
                        }

                        while (bytes_read = read(fd, contents, 1000)){
                            if (bytes_read < 0){
                                fprintf(stderr, "ERROR, fail to read\n");
                                exit(1);
                            }
                            else{
                                ptr = contents;
                                while (bytes_read > 0){
                                    if ((bytes_send = send(sockfd, ptr, bytes_read, 0)) < 0){
                                        fprintf(stderr, "ERROR, fail to send\n");
                                        exit(1);
                                    }
                                    bytes_read -= bytes_send;
                                    ptr += bytes_send;
                                }
                            }

                            memset(contents, '\0', 1001);
                            sleep(2);
                        }

                        // finish sending
                        send(sockfd, "\\quit", 5, 0);
                        close(sockfd);
                    }
                }

                // invalid command
                else send(connfd, err_message, strlen(err_message), 0);
                
                // reset files
                reset_files(files);
            }
        }

        if(status < 0){
            printf("Fail to receive\n");
            exit(1);
        }

        close(connfd);
    }

    close(listenfd);
}