#include "mftp.h"

// function prototypes
void listenForMessage(conInfo info);
void newDataCon(conInfo *info);
void serverPut(conInfo *info);
void serverGet(conInfo *info);
void serverLS(conInfo *info);
void serverCD(conInfo info);

int main(int argc, char *argv[]) {
    struct sockaddr_in servAddr, clientAddr;
    struct hostent* hostEntry;
    struct in_addr **pptr;
    int socfd, lfd;
    int length = sizeof(struct sockaddr_in);
    pid_t pid;

    client = 0; // set client flag to 0, indicates this is the server

    if (argc == 2) {
        if (strcmp(argv[1], "-d") == 0) {
            debugging = 1;
            fprintf(stdout, "Parent: Debug output enabled.\n");
        }
    }
    if ((socfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR - socket: %d - %s\n", errno, strerror(errno));
    }
    if (debugging) fprintf(stdout, "Parent: socket created with descriptor %d\n", socfd);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT_NUMBER);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        fprintf(stderr, "ERROR - bind: %d - %s\n", errno, strerror(errno));
        exit(1);
    }
    if (debugging) fprintf(stdout, "Parent: socket bound to port %d\n", PORT_NUMBER);
    listen(socfd, 4);   // allow up to 4 simultaneous connections to the server

    while (1) {         // keep the server running until forcibly terminated
        if ((lfd = accept(socfd, (struct sockaddr *) &clientAddr, &length)) == -1) {
            fprintf(stderr, "ERROR - server accept: %d - %s\n", errno, strerror(errno));
        }
        conInfo newClientCon;   // struct to hold the information about the new connetion
        newClientCon.origAccFD = lfd;
        if (debugging) fprintf(stdout, "Parent: accepted client connection with descriptor %d\n", lfd);
        if ((pid = fork()) < 0) {   // fork a new child process to handle the new connection
            fprintf(stderr, "ERROR - fork(): %d: %s\n", errno, strerror(errno));
        }

        if (pid > 0) {  // have the parent process wait until the child terminates
            if (debugging) fprintf(stdout, "Parent: spawned child %d, waiting for new connection\n", getpid());
            waitpid(-1, NULL, getpid());
        }
        else {          // child process handles requests from the client
            fprintf(stdout, "Child %d: started", getpid());
            hostEntry = gethostbyaddr(&(clientAddr.sin_addr), sizeof(struct in_addr), AF_INET); // get client info
            pptr = (struct in_addr **) hostEntry->h_addr_list;
            memcpy(&clientAddr.sin_addr, *pptr, sizeof(struct in_addr));
            if (hostEntry->h_name == NULL) fprintf(stdout, "Child %d: Translation of client hostname failed -> %s\n", getpid(), strerror(h_errno));
            else fprintf(stdout, "Child %d: Connection accepted from host %s\n", getpid(), hostEntry->h_name);
            fprintf(stdout, "Child %d: Client IP address -> %s\n", getpid(), inet_ntoa(clientAddr.sin_addr));
            listenForMessage(newClientCon); // continue listening for messages from client until 'Q' or original connection terminates
            fprintf(stdout, "Child %d: exiting normally.\n", getpid());
            close(newClientCon.origAccFD);
            exit(0);    // if 'Q\n' was received from the client the server terminates the process normally
        }
    }
}

void listenForMessage(conInfo info) {
    int numRead;

    info.dataConEst = 0;

    while (info.messageBuf[0] != 'Q') {         // continue until 'Q' received from client
        memset(info.messageBuf, 0, BUF_SIZE);   // clear the message buffer after each message received from the client
        for (int i = 0; ; i++) {                // continue reading from the connection until '\n' is found
            if ((numRead = read(info.origAccFD, info.messageBuf + i, 1)) == 0) {    // if connection terminates unexpectedly server terminates connection
                if (debugging) fprintf(stdout, "Child %d: Control socket EOF detected, exiting\n", getpid());
                fprintf(stdout, "Child %d: Fatal error, exiting.\n", getpid());
                close(info.origAccFD);
                exit(1);
            }
//            fprintf(stdout, "Child %d: messageBuf = %s\n", getpid(), info.messageBuf);
            if (info.messageBuf[i] == '\n') {   // replace '\n' with '\0' and break out of for loop
                info.messageBuf[i] = '\0';
//                fprintf(stdout, "listenForMessage read new line char detected\n");
                break;
            }
        }
        if (info.messageBuf[0] == 'D') newDataCon(&info);
        if (info.messageBuf[0] == 'C') {
//            fprintf(stdout, "Child %d: messageBuf = %s\n", getpid(), info.messageBuf);
            serverCD(info);
        }
        if (info.messageBuf[0] == 'L') {
            if (info.dataConEst == 1) serverLS(&info);
            else {
                write(info.origAccFD, "ENo Data Connection Established.\n", 33);
            }
        }
        if (info.messageBuf[0] == 'P') {
            if (info.dataConEst == 1) serverPut(&info);
            else {
                write(info.origAccFD, "ENo Data Connection Established.\n", 33);
            }
        }
        if (info.messageBuf[0] == 'G') {
            if (info.dataConEst == 1) serverGet(&info);
            else {
                write(info.origAccFD, "ENo Data Connection Established.\n", 33);
            }
        }
    }
    fprintf(stdout, "Child %d: Quitting\n", getpid());
    if (debugging) fprintf(stdout, "Child %d: Sending positive acknowledgement\n", getpid());
    sendMessage("A\n", info);
}

// establishes a new data connection for the transfer of data
void newDataCon(conInfo *info) {
    struct sockaddr_in servAddr;
    int length = sizeof(struct sockaddr_in);
    int newPort;
    char ackPort[BUF_SIZE] = {0};

    fprintf(stdout, "Establishing data connection\n");
    if ((info->newSocFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR - socket: %d - %s\n", errno, strerror(errno));
    }
    if (debugging) fprintf(stdout, "Child %d: data socket created with descriptor %d\n", getpid(), info->newSocFD);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(0);   // get an ephemeral port number from htons
    if (bind(info->newSocFD, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {    // bind on the port received
        fprintf(stderr, "ERROR - bind: %d - %s\n", errno, strerror(errno));
        exit(1);
    }
    getsockname(info->newSocFD, (struct sockaddr *) &servAddr, &length);
    newPort = ntohs(servAddr.sin_port);                                     // get the new port number to send to client
    if (debugging) fprintf(stdout, "Child %d: data socket bound to port %d\n", getpid(), newPort);
    listen(info->newSocFD, 1);
    if (debugging) fprintf(stdout, "Child %d: listening on data socket\n", getpid());
    ackPort[0] = 'A';   // format the message with the port number to send to the client
    sprintf(ackPort + 1, "%d", newPort);
    strcat(ackPort, "\n");
    sendMessage(ackPort, *info);    // send 'A<port#>\n' to the client
    if ((info->newAccFD = accept(info->newSocFD, (struct sockaddr *) &servAddr, &length)) == -1) {
        fprintf(stderr, "ERROR - setupNewConnection accept: %d - %s\n", errno, strerror(errno));
    }
    else if (info->newAccFD > 0) {
        info->dataConEst = 1;
        fprintf(stdout, "Child %d: Accepted connection on the data socket with descriptor %d\n", getpid(), info->newAccFD);
    }
}

void serverPut(conInfo *info) {
    char buf[BUF_SIZE] = {0};
    char path[BUF_SIZE] = {0};
    char *file;
    int fd, numRead, numWrite;

    strcat(path, &info->messageBuf[1]); // copy everything after 'P' to the path
    file = fileName(path);              // get the filename from the path

    if (access(file, F_OK) != 0) {      // make sure the file does not already exist
        sendMessage("A\n", *info);
        if ((fd = open(file, O_CREAT | O_WRONLY, 0644)) > 0) {  // create the file
            while ((numRead = read(info->newAccFD, buf, BUF_SIZE)) > 0) {   // read data from connection into the buffer
                if (debugging) fprintf(stdout, "Child %d: read %d bytes\n", getpid(), numRead);
                numWrite = write(fd, buf, numRead);                         // write data in buffer to the file
                if (debugging) fprintf(stdout, "Child %d: write %d bytes\n", getpid(), numWrite);
            }
            if (numRead == -1) {
                fprintf(stdout, "Child %d: Unlinking file %s due to client termination.\n", getpid(), path);
                unlink(path);
            }
            close(fd);  // close the newly created file
        }
    }
    else {
        sendMessage("EFile already exists\n", *info);
    }
    free(file);                         // deallocate memory used by file
    close(info->newAccFD);              // close the data connection
    info->dataConEst = 0;               // set the data connection flag to 0
}

void serverGet(conInfo *info) {
    char path[BUF_SIZE];
    char buf[BUF_SIZE] = {0};
    int fd, numRead, numWrite, val;

    strcpy(path, &info->messageBuf[1]);                             // copy everything after 'G' to path

    if (access(path, F_OK) == 0) {                                  // make sure the file exists
        if ((val = checkPerm(path, *info)) == 0) {                  // see mftp.h
            sendMessage("A\n", *info);                              // send acknowledgement to the client
            fprintf(stdout, "Child %d: Reading file %s\n", getpid(), path);
            if ((fd = open(path, O_RDONLY)) > 0) {                  // open the file for reading
                fprintf(stdout, "Child %d: transmitting file %s to client\n", getpid(), path);
                while ((numRead = read(fd, buf, BUF_SIZE)) > 0) {    // read the file into the buffer
                    fprintf(stdout, "Child %d: numRead = %d\n", getpid(), numRead);
//                    if (numRead == -1) {
//                        fprintf(stdout, "Error: serverGet read: %d - %s\n", errno, strerror(errno));
//                        close(info->newAccFD);
//                        return;
//                    }
                    numWrite = write(info->newAccFD, buf, numRead); // send the data help by the buffer to the client
                    fprintf(stdout, "Child %d: numWrite = %d\n", getpid(), numWrite);
                }
                close(fd);                                          // close the file that was opened
            }
        }
    }
    else {                                                          // if file doesn't exist send error message
        sendMessage("ENo such file or directory.\n", *info);
    }
    close(info->newAccFD);                                          // close the data connection
    info->dataConEst = 0;                                           // set the data connection flag to 0
}

// function to list the contents of the servers CWD and send the output to the client
void serverLS(conInfo *info) {
    pid_t pid;
    int wstatus;

    sendMessage("A\n", *info);  // send acknowledgement to the client

    if (debugging) fprintf(stdout, "Child %d: forking ls process\n", getpid());
    if ((pid = fork()) < 0) {   // fork a new child process
        fprintf(stderr, "Error serverLS fork: %d - %s\n", errno, strerror(errno));
    }

    if (pid > 0) {              // parent process waits until the child finishes execing ls, then closes data connection and sets flag to 0
        if (debugging) fprintf(stdout, "Child %d: forked ls process\n", getpid());
        waitpid(pid, &wstatus, 0);
        close(info->newAccFD);
        info->dataConEst = 0;
        fprintf(stdout, "Child %d: ls command complete\n", getpid());
    }
    else {  // exec ls, sends the output from exec to the data connection
        dup2(info->newAccFD, STDOUT_FILENO);
        execlp("ls", "ls", "-l", "-a", (char *) NULL);
        close(info->newAccFD);
        exit(1);
    }
}

// function to change the CWD of the server
void serverCD(conInfo info) {
    char path[BUF_SIZE];
    char cwd[BUF_SIZE];

//    fprintf(stdout, "Child %d: messageBuf = %s\n", getpid(), info.messageBuf);
    strcpy(path, &info.messageBuf[1]);  // copy everything after 'C' to path
//    fprintf(stdout, "Child %d: path = %s\n", getpid(), path);

    if (chdir(path) == 0) {             // change the servers CWD to path, if successful send acknowledgement
        fprintf(stdout, "Changed current directory to %s\n", getcwd(cwd, BUF_SIZE));
        sendMessage("A\n", info);
        return;
    }
    else {                              // if chdir failed send error message to the client
        sendMessage("E", info);
        sendMessage(strerror(errno), info);
        sendMessage("\n", info);
    }
}
