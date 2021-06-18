#include <stdio.h>

#include <stdlib.h>

#include <errno.h>

#include <netinet/in.h>

#include <netdb.h>

#include <unistd.h>

#include <string.h>

#include <ctype.h>



#define SIZE_BUF 100



int register_login(int fd, char user[], char pass[]){

    int ret_val, isFound = 0;

	char id[SIZE_BUF], password[SIZE_BUF], message[SIZE_BUF];



    //input/sending id and password
    strcpy(id, user);
    ret_val = send(fd, id, SIZE_BUF, 0);
    printf("kesini id: %s\n", id);

    strcpy(password, pass);
    ret_val = send(fd, password, SIZE_BUF, 0);
    printf("kesini pass: %s\n", password);



    ret_val = recv(fd, message, SIZE_BUF, 0);

    puts(message);



    //check if its terminate condition

    if(!strcmp(message, "regloginsuccess\n"))

        return 1;

    else if(!strcmp(message, "userfound\n")) {

        printf("Username or ID already exist !\n\n");

        return 0;

    }

    else if(!strcmp(message, "wrongpass\n")) {

        printf("Id or Passsword doesn't match !\n\n");

        return 0;

    }



}


void sendFile(int sockfd, char filePath[]){

    int n;

    char data[SIZE_BUF] = {0};

    FILE *fp = fopen(filePath, "r");



    while(fgets(data, SIZE_BUF, fp) != NULL) {

        if (send(sockfd, data, sizeof(data), 0) == -1) {

            perror("[-]Error in sending file.");

            exit(1);

        }

        bzero(data, SIZE_BUF);

    }



    fclose(fp);

    int ret_val = send(sockfd, "done", SIZE_BUF, 0);

}



void write_file(int fd, char fileName[]){

    int n;

    char buffer[SIZE_BUF];



    FILE *fp = fopen(fileName, "w");

    fclose(fp);



    while (1) {

        n = recv(fd, buffer, SIZE_BUF, 0);

        if (n <= 0){

            break;

            return;

        }

        if(!strcmp(buffer, "done"))

            return;

        fp = fopen(fileName, "a");

        // printf("DATA --- %s\n", buffer);

        fprintf(fp, "%s", buffer);

        bzero(buffer, SIZE_BUF);

        fclose(fp);

    }

    return;

}


int main(int argc,char* argv[]) {

    struct sockaddr_in saddr;

    int fd, ret_val;

    struct hostent *local_host; /* need netdb.h for this */

    char message[SIZE_BUF],  cmd[SIZE_BUF];



    /* Step1: create a TCP socket */

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fd == -1) {

        fprintf(stderr, "socket failed [%s]\n", hstrerror(errno));

        return -1;

    }

    printf("Created a socket with fd: %d\n", fd);



    /* Let us initialize the server address structure */

    saddr.sin_family = AF_INET;

    saddr.sin_port = htons(7000);

    local_host = gethostbyname("127.0.0.1");

    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);

    /* Step2: connect to the TCP server socket */

    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));

    if (ret_val == -1) {

        fprintf(stderr, "connect failed [%s]\n", hstrerror(errno));

        close(fd);

        return -1;

    }



    //TERIMA MSG SERVE / WAIT

    ret_val = recv(fd, message, SIZE_BUF, 0);

    // puts(message);

    while(strcmp(message, "wait") == 0) {

        printf("\e[31mServer is full!\e[0m\n");

        ret_val = recv(fd, message, SIZE_BUF, 0);

    }



    int commandTrue = 0;

    while(1)

    {

        // sign up user
        if (!getuid()){
            if(argc == 5){
                if(!strcmp(argv[1], "-u") && !strcmp(argv[3], "-p")){
                    printf("masuk sini bray\n");
                    ret_val = send(fd, "login", SIZE_BUF, 0);
                    if(register_login(fd, argv[2], argv[4]))
                        commandTrue = 1;
                    }
                }
            else{
                printf("Argumen tidak sesuai, masukkan -u user dan -p pass\n");
                exit(0);
            }
        }

        if (getuid()){
                    // other command
            while(commandTrue){

                printf("\e[32mInsert Command\n>\e[0m ");

                scanf("%s", cmd);

                ret_val = send(fd, cmd, SIZE_BUF, 0);

            }
        }

        sleep(2);

        if(commandTrue) break;

    }



    close(fd);

    return 0;

}
