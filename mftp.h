#ifndef FINAL_MFTP_H
#define FINAL_MFTP_H

// all the library header files used, saves having to include them all in the server and client
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <ctype.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#define PORT_NUMBER 49999   // port number for the original connections for the server and client
#define BUF_SIZE 4096       // size of the buffer used by the server and client

static int debugging = 0;   // flag to enable debugging output, have the second argument be '-d' on client or server to enable
static int client = 0;      // flag thats used by sendMessage

typedef struct _conInfo {   // structure that holds the relevent information needed by the client and server
    int dataConEst;         // used by the server as a flag to determine if a data connection has been established or not
    int origAccFD;          // fd used by the client and server to send data to each other
    int newSocFD;
    int newAccFD;
    uint16_t newPort;       // holds the new port number received from the server when the client is establishing a new data connection
    char messageBuf[BUF_SIZE];  // holds messages received, mainly used the server
    char *arg1;             // first argument entered by the user on the client
    char *arg2;             // second argument entered by the server on the client
    char hostname[100];     // holds the hostname entered by the user on the client
} conInfo;

char *fileName(char *path);
int checkPerm(char *path, conInfo info);
void sendMessage(char *message, conInfo info);
void receiveMessage(conInfo *info);

// gets the filename from a path
char *fileName(char *path) {
    char *token, *str1;
    char *file, *file1;
    int backS = 0;

    for (int i = 0; i < strlen(path); i++) {    // check if the path contains '/', if not the path is the filename
        if (path[i] == '/') backS = 1;
    }
    str1 = strdup(path);    // duplicate the path to str1 so the original path is not changed, needed for printing debugging messages
    if (backS) {            // if the path does contain '/', split the string on '/' until NULL is returned
        token = strtok(str1, "/");
        while (token != NULL) {
            file1 = token;
            token = strtok(NULL, "/");  // continues splitting the string left over after the split
        }
        file = file1;   // will hold the previous non NULL string found when NULL is returned
    }
    else file = path;

    free(str1);
    return strdup(file);
}

// used to check whether a file is a directory or not and if read permissions are available
int checkPerm(char *path, conInfo info) {
    char buf[BUF_SIZE];
    struct stat s;

    stat(path, &s); // call stat so the structure holds the information about the file

    if (S_ISDIR(s.st_mode)) {   // make sure the file is not a directory
        if (!client) {          // if its the server send the appropriate error message to the client
            buf[0] = 'E';
        strcat(buf, "Path is a directory.\n");
        sendMessage(buf, info);
        }
        return -2;
    }
    if (access(path, R_OK) != 0) {  // check whether there are read permissions for the file
        if (!client) {              // if its the server send the appropriate error message to the client
            buf[0] = 'E';
            strcat(buf, strerror(errno));
            strcat(buf, "\n");
            sendMessage(buf, info);
        }
        return -1;
    }
    else return 0;  // return 0 is file is not a directory and there are read permissions
}

// function to send a message to the client/server
void sendMessage(char *message, conInfo info) {

    if (write(info.origAccFD, message, strlen(message)) == -1) {    // write data into the connection, print error message and exit otherwise
        fprintf(stderr, "Error: Process %d - Unable to send data through connection.\n", getpid());
        close(info.origAccFD);
        exit(1);
    }
}

// function to continue reading the message sent until '\n' is found
void receiveMessage(conInfo *info) {
    int numRead;
    char temp[BUF_SIZE] = {0};

    for (int i = 0; ; i++) {    // essentially eqivalent a while (1) loop, but gives easy access to an index
        if ((numRead = read(info->origAccFD, info->messageBuf + i, 1)) == 0) {  // read one byte at a time and add it to the end of the buffer
            fprintf(stdout, "Error: Control socket closed unexpectedly.\n");
            close(info->origAccFD);
            exit(1);
        }
        if (info->messageBuf[i] == '\n') {  // when '\n' is found replace it with a null character and break out of the for loop
            info->messageBuf[i] = '\0';
//            fprintf(stdout, "read new line char detected\n");
            break;
        }
//        fprintf(stdout, "Client messageBuf = %s\n", info->messageBuf);
    }
    if (debugging) fprintf(stdout, "Received server response '%s'\n", info->messageBuf);
    if (info->messageBuf[0] == 'E') {   // used by the client to print out the error messages received from the server
        strcpy(temp, &info->messageBuf[1]);
        fprintf(stdout, "Error response from server: %s\n", temp);
    }
}

#endif //FINAL_MFTP_H
