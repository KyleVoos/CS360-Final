#include "mftp.h"

// function prototypes
void getArgs(char *input, conInfo *info);
void estabDataCon(conInfo *info);
void clientPut(conInfo info);
void clientGet(conInfo info);
void clientShow(conInfo info);
void clientCD(conInfo info);
void clientRCD(conInfo info);
void clientLS(conInfo info);
void clientRLS(conInfo info);

int main(int argc, char *argv[]) {
    int sfd;
    struct sockaddr_in servAddr;
    struct hostent *hostEntry;
    struct in_addr **pptr;
    conInfo newCon; // struct to hold client connection info

    if (argc == 1) exit(1); // exit if hostname or address not entered by client
    client = 1;
    if (strcmp(argv[1], "-d") == 0) {
        debugging = 1;
        fprintf(stdout, "Debugging enabled.\n");
    }

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR - client socket: %d - %s\n", errno, strerror(errno));
        exit(1);
    }
    newCon.origAccFD = sfd;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT_NUMBER);
    if (argc == 2) {
        hostEntry = gethostbyname(argv[1]);
    }
    else if (argc == 3) {
        hostEntry = gethostbyname(argv[2]);
    }
    strcpy(newCon.hostname, hostEntry->h_name);
    pptr = (struct in_addr **) hostEntry->h_addr_list;
    memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));
    if (connect(sfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
        fprintf(stderr, "ERROR client connect: %d - %s\n", errno, strerror(errno));
        exit(1);
    }
    fprintf(stdout, "Connected to server %s\n", newCon.hostname);
    newCon.arg1 = "a";
    while (strcmp(newCon.arg1, "exit") != 0) {  // continue running while the client hasn't entered exit
        memset(newCon.messageBuf, 0, BUF_SIZE); // clear the client message buffer
        fprintf(stdout, "MFTP> ");
        fgets(newCon.messageBuf, BUF_SIZE - 1, stdin);
        if (isspace(newCon.messageBuf[0]) == 0) { // make sure the first character entered by the user is not whitespace
            getArgs(newCon.messageBuf, &newCon);    // get the two arguments from the input entered by the user
            fprintf(stdout, "newCon.messageBuf = %s\n", newCon.messageBuf);
            if (strcmp(newCon.arg1, "exit") != 0) {
                if (strcmp(newCon.arg1, "ls") == 0) {
                    if (newCon.arg2 != NULL) fprintf(stdout, "Please do not enter a argument when using ls.\n");
                    else clientLS(newCon);
                }
                else if (strcmp(newCon.arg1, "rls") == 0) {
                    if (newCon.arg2 != NULL) fprintf(stdout, "Please do not enter a argument when using rls.\n");
                    else clientRLS(newCon);
                }
                else if (newCon.arg2 != NULL) {
                    if (strcmp(newCon.arg1, "cd") == 0) clientCD(newCon);
                    else if (strcmp(newCon.arg1, "rcd") == 0) clientRCD(newCon);
                    else if (strcmp(newCon.arg1, "get") == 0) clientGet(newCon);
                    else if (strcmp(newCon.arg1, "put") == 0) clientPut(newCon);
                    else if (strcmp(newCon.arg1, "show") == 0) clientShow(newCon);
                } else if (strcmp(newCon.arg1, "cd") == 0 || strcmp(newCon.arg1, "rcd") == 0 ||
                           strcmp(newCon.arg1, "get") == 0 || strcmp(newCon.arg1, "put") == 0 ||
                           strcmp(newCon.arg1, "show") == 0)
                    fprintf(stdout, "Command error: expecting a parameter.\n");
                else fprintf(stdout, "Command '%s' is unknown - ignored\n", newCon.arg1);
            }
        }
    }
    if (debugging) fprintf(stdout, "Exit command encountered\n");
    sendMessage("Q\n", newCon);
    if (debugging) fprintf(stdout, "Awaiting server response\n");
    receiveMessage(&newCon);
    if (debugging) fprintf(stdout, "Client exiting normally\n");
    exit(0);
}

/*
 * gets the arguments from the input entered by the user, splits the string on " "
 */
void getArgs(char *input, conInfo *info){
    char *delim = " ";
    char *arg1, *arg2;
    int i;
    int length = (int) strlen(input) - 1;

    if (input[length] == '\n') input[length] = '\0';

    for (i = 0, arg1 = input; ; i++) {
        info->arg1 = strtok(arg1, delim);
        if (info->arg1 == NULL) {
            info->arg2 = NULL;
            break;
        }
        else {
            info->arg2 = strtok(NULL, " ");
            input = strtok(NULL, " ");
            break;
        }
    }
    if (info->arg2 == NULL) {
        if (debugging) fprintf(stdout, "Command string = '%s'\n", info->arg1);
    }
    else {
        if (debugging) fprintf(stdout, "Command string = '%s' with parameter = '%s'\n",info->arg1, info->arg2);
    }
}

// establishes another connection to the server on a new port sent by the server when transfering data is required
void estabDataCon(conInfo *info) {
    struct sockaddr_in dataCon;
    struct in_addr **pptr;
    struct hostent *hostEntry;
    char c;
    int connected;
    int numTries = 2;

    memset(info->messageBuf, 0, BUF_SIZE);  // clear the client's message buffer
    sendMessage("D\n", *info); // send server 'D\n' to start the process of establishing the data connection
    if (debugging) fprintf(stdout, "Sent D command to server\n");
    if (debugging) fprintf(stdout, "Awaiting server response\n");
    receiveMessage(info);   // wait for the servers response
    if (info->messageBuf[0] == 'A') {   // make sure the repsonse begins with 'A' and not 'E'
        sscanf(info->messageBuf, "%c%d", &c, &info->newPort);   // get the port number from the acknowledgement
        if (debugging) fprintf(stdout, "Obtained port number %d from server\n", info->newPort);
        while (numTries > 0) {  // if the new connection fails the first time, try it once more
            numTries--;
            if ((info->newAccFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) { // gets the new fd for the connection from socket
                fprintf(stderr, "ERROR - client socket: %d - %s\n", errno, strerror(errno));
            }
            if (debugging) fprintf(stdout, "Created data socket with descriptor %d\n", info->newAccFD);
            memset(&dataCon, 0, sizeof(dataCon));
            dataCon.sin_family = AF_INET;
            dataCon.sin_port = htons(info->newPort); // sets the port to the new port number received from the server
            hostEntry = gethostbyname(info->hostname);
            pptr = (struct in_addr **) hostEntry->h_addr_list;
            memcpy(&dataCon.sin_addr, *pptr, sizeof(struct in_addr));
            if (debugging)
                fprintf(stdout, "Data Socket Address/Port => %s:%d\n", inet_ntoa(dataCon.sin_addr), info->newPort);
            if (debugging)fprintf(stdout, "Attempting to establish Data Connection...\n");
            if ((connected = connect(info->newAccFD, (struct sockaddr *) &dataCon, sizeof(dataCon))) == -1) {
                fprintf(stderr, "ERROR client connect: %d - %s\n", errno, strerror(errno));
//                exit(1);
            }
            if (debugging && connected == 0) fprintf(stdout, "Data connection to server established\n");
            if (connected == 0) break;
            if (connected != 0 && numTries == 1) {
                fprintf(stdout, "First attempt to establish a data connection to server failed: %s\n", strerror(errno));
            }
            else if (numTries == 0) {
                fprintf(stdout, "Data connection to server failed on second try: %s\n", strerror(errno));
            }
        }
    }
}

// function to handle transfer of file from the client to the server
void clientPut(conInfo info) {
    int fd, numRead, numWrite;
    char buf[BUF_SIZE] = {0};
    char *file = fileName(info.arg2);
    int val;

    if (debugging) fprintf(stdout, "Putting file %s to %s\n", info.arg2, file);

    if (access(info.arg2, F_OK) == 0) { // check to make sure the file exists
        if ((val = checkPerm(info.arg2, info)) == 0) {  // see checkPerm in mftp.h
            if ((fd = open(info.arg2, O_RDONLY)) < 0) { // open the file, print error and return if unable
                fprintf(stdout, "Opening file for reading: %s\n", strerror(errno));
                return;
            } else if (fd > 0) {
                if (debugging) fprintf(stdout, "Opened local file %s for reading\n", info.arg2);
                estabDataCon(&info); // establish data connection to server to transfer the file
                sendMessage("P", info); // send 'P<filename>\n' to server
                sendMessage(file, info);
                sendMessage("\n", info);
                receiveMessage(&info);  // wait for servers response

                if (info.messageBuf[0] == 'A') {    // if 'A\n' received from server write everything read from the file into the data connection fd
                    while ((numRead = read(fd, buf, BUF_SIZE)) > 0) {
                        if (debugging) fprintf(stdout, "Writing %d bytes to server\n", numRead);
                        numWrite = (int) write(info.newAccFD, buf, (size_t) numRead);
//                        fprintf(stdout, "numWrite = %d\n", numWrite);
                    }
                    close(fd);  // close the file
                }
            }
            close(info.newAccFD);   // close the data connection after transfering the file has completed
        } else if (val == -1) { // print relative error messages based on value returned from checkPerm
            fprintf(stdout, "Do not have read permissions for local file %s, command ignored\n", info.arg2);
        } else if (val == -2) {
            fprintf(stdout, "Local file %s is a directory, command ignored\n", info.arg2);
        }
    }
    else {  // print message if the file does not exist
        fprintf(stdout, "Local file %s does not exist, command ignored\n", info.arg2);
    }
}

// function the handle transfer of file from server to client
void clientGet(conInfo info) {
    int fd, numRead, numWrite;
    char buf[BUF_SIZE];
    char *file = fileName(info.arg2);   // see mftp.h, fileName gets the filename from the path entered by the user

    if (access(file, F_OK) != 0) {  // check to make sure the file does not already exist in the current directory
        if ((fd = open(file, O_RDWR | O_CREAT, 0644)) < 0) { // create the file, print error message and return if failed
            fprintf(stdout, "Creating file for writing: %s\n", strerror(errno));
            return;
        }
        if (debugging) fprintf(stdout, "Getting %s from server and storing to %s\n", info.arg2, file);
        estabDataCon(&info);    // establish data connection with server
        sendMessage("G", info); // send 'G<path>\n' to the server
        sendMessage(info.arg2, info);
        sendMessage("\n", info);
        if (debugging) fprintf(stdout, "Awaiting server response\n");
        receiveMessage(&info);  // wait to receive response from server
        if (info.messageBuf[0] == 'A') {
            while ((numRead = read(info.newAccFD, buf, BUF_SIZE)) > 0) { // continue reading until connection closes
                if (debugging) fprintf(stdout, "Read %d bytes from server, writing to local file\n", numRead);
                numWrite = write(fd, buf, numRead); // write everything received through connection to the file that was created
//                if (debugging) fprintf(stdout, "numWrite = %d\n", numWrite);
            }
            close(fd);  // close the newly created file
        }
        else if (info.messageBuf[0] == 'E') {   // if an error message from the server was received unlink the file so it gets deleted after its closed
            if (debugging) fprintf(stdout, "Unlinking file %s due to server error response.\n", file);
            unlink(file);
            close(fd);
        }
        close(info.newAccFD);
    }
    else {
        fprintf(stdout, "Local file %s already exists, command ignored\n", file);
    }
}

// function to display the file received from the server to the clients terminal 20 lines at a time
void clientShow(conInfo info) {
    pid_t pid;
    int wstatus;

    if (debugging) fprintf(stdout, "Showing file %s\n", info.arg2);
    estabDataCon(&info);    // establish data connection to server
    sendMessage("G", info); // send 'G<path>\n' to the server
    sendMessage(info.arg2, info);
    sendMessage("\n", info);
    if (debugging) fprintf(stdout, "Awaiting server response\n");
    receiveMessage(&info);  // wait to receive response from server

    if (info.messageBuf[0] == 'A') {    // if 'A' is received from server fork a new child process and exec more -20
        if (debugging) fprintf(stdout, "Displaying data from server & forking to 'more'\n");
        pid = fork();

        if (pid > 0) {
            if (debugging) fprintf(stdout, "Waiting for child process %d to complete execution of more\n", getpid());
            waitpid(pid, &wstatus, 0);
            if (debugging) fprintf(stdout, "Data display & more command completed.\n");
        }
        else {
            close(STDIN_FILENO);
            dup2(info.newAccFD, STDIN_FILENO);
            execlp("more", "more", "-20", (char *) NULL);
            close(info.newAccFD);
            exit(1);
        }
    }
    close(info.newAccFD);
}

// function to handle changing to the requested directory on the client
void clientCD(conInfo info) {
    int err;

    if ((err = chdir(info.arg2)) == -1) {   // change directory, print error message to clients terminal if it fails
        fprintf(stdout, "Error: %s\n", strerror(errno));
    }
    else if (debugging && err != -1) fprintf(stdout, "Changed local directory to %s\n", getcwd(NULL, 0));
}

// function to handle changing the servers current working directory
void clientRCD(conInfo info) {

    sendMessage("C", info); // send 'C<path>\n' to the server
    sendMessage(info.arg2, info);
    sendMessage("\n", info);
    if (debugging) fprintf(stdout, "Changed remote directory to %s\n", info.arg2);
    if (debugging) fprintf(stdout, "Awaiting server response\n");
    receiveMessage(&info);  // wait to receive reponse from the server
}

// function to list the contents of the current directory of the client
void clientLS(conInfo info) {
    int fd[2];
    pid_t pid, pid1;

    if (pipe(fd) == -1) {   // create a pipe to allow transfering data between two processes
        fprintf(stderr, "Error: %d - %s\n", errno, strerror(errno));
        exit(1);
    }
    if ((pid = fork()) == -1) { // fork a new child process
        fprintf(stderr, "Error: %d - %s\n", errno, strerror(errno));
        close(fd[0]);
        close(fd[1]);
        exit(1);
    }

    if (pid > 0) {  // have the original parent process wait for the original child process to finish
        if (debugging) fprintf(stdout, "Client parent waiting on child process %d to run ls locally\n", getpid());
        if (debugging) fprintf(stdout, "Client child process %d execing ls | more\n", getpid());
        close(fd[0]);
        close(fd[1]);
        wait(NULL);
        if (debugging) fprintf(stdout, "Client parent continuing\n");
    }
    else {  // the original child process then forks another new child process
        if ((pid1 = fork()) == -1) {
            fprintf(stderr, "Error: %d - %s\n", errno, strerror(errno));
            close(fd[0]);
            close(fd[1]);
            exit(1);
        }
        if (pid1 > 0) { // the new parent process execs more, the read end of the pipe is duped to stdin for exec to use as its input
            if (debugging) fprintf(stdout, "Child process %d starting more\n", getpid());
            close(fd[1]);
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            execlp("more", "more", "-20", (char *) NULL);
            exit(1);
        }
        else {  // the new child process execs ls, and sends the output to the write end of the pipe
            if (debugging) fprintf(stdout, "Client child process %d starting ls\n", getpid());
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            execlp("ls", "ls", "-l", "-a", (char *) NULL);
            exit(1);
        }
    }
}

// function to handle listing the contents of the CWD of the server on the clients terminal
void clientRLS(conInfo info) {
    pid_t pid;
    int wstatus;

    estabDataCon(&info);    // establish a data connection to the server
    sendMessage("L\n", info);   // send 'L\n' to the server
//    fprintf(stdout, "clientRLS newAccFD = %d\n", info.newAccFD);
    if (debugging) fprintf(stdout, "Awaiting server response\n");
    receiveMessage(&info);  // wait to receive the response from the server
    if (info.messageBuf[0] == 'A') {    // if 'A' is received from the server fork a new child process and exec more
        if (debugging) fprintf(stdout, "Displaying data from server & forking to 'more'...\n");

        if ((pid = fork()) == -1) {
            fprintf(stderr, "Error clientRls fork: %d - %s\n", errno, strerror(errno));
        }
        if (pid > 0) {  // parent process waits for the child to finish, then closes the connection
            if (debugging) fprintf(stdout, "Waiting for child process %d to complete execustion of more\n", getpid());
            waitpid(pid, &wstatus, 0);
            close(info.newAccFD);
            if (debugging) fprintf(stdout, "Data display & more command completed.\n");
        }
        else {  // child process execs more, the connection fd is used as the input for exec
            close(STDIN_FILENO);
            dup2(info.newAccFD, STDIN_FILENO);
            execlp("more", "more", "-20", (char *) NULL);
            close(info.newAccFD);
            exit(1);
        }
    }
}
