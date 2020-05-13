# CS360-Final
This was a final project for CS360 Systems Programming. The project was to create an FTP server and client program.
The server acts as a daemon that allows multiple clients to connect. For each client that attempts to connects
a new process is spawned on the server. The client is able to run multiple different commands to upload, download and view
files from the server. For any command that requires data to be sent between the client and server the client will
request that a new data connection be opened by the server by sending 'D'. The client will then wait
for the server to respond with 'A<port#>' and attempt to establish the new data connection on the port
sent from the server. If the connection is successful the client will then respond with 'A' and the necessary data will
be sent through the new data connection. For all commands such as ls and cd that require either the client or server
to call exec() a new process is forked.

## How To Run:
The server must be running first otherwise the client has nothing to connect to.
Both the client and server can be run with an optional argument -d to enable debugging output.

1) in a terminal type 'make' 
2) ./mftpserve [-d]
3) ./mftp [-d] [address]

Where [address] is the IP address or name of where the server is running.
For example both the client and server are running locally on the same machine you 
can connect with localhost or 127.0.0.1.

## Commands:
No commands are done from the server besides the initial running
since it acts as a daemon.
All of the commands work with relative and absolute paths to files/directories.
### Client Commands:
#### ls : Displays the contents of the CWD. Executes ls -al | more -20
#### rls : Displays the contents of the servers CWD. The server executes ls -al | more -20 and send the results to the client.
#### cd [dir]: Changes the CWD of the client to [dir].
#### rcd [dir]: Chnages the CWD of the server to [dir].
#### put [file]: Sends a copy of [file] to the server where it is copied to its CWD.
#### show [file]: The client can view the contents of [file] located on the server.
#### get [file]: The client gets a copy of [file] from the server.

## Sample Output
### Server Output:
kyle@LAPTOP-T16SRG68:/mnt/c/Users/Tormentt/Documents/GitHub/CS360-Final$ ./mftpserve -d  
Parent: Debug output enabled.  
Parent: socket created with descriptor 3  
Parent: socket bound to port 49999  
Parent: accepted client connection with descriptor 4  
Parent: spawned child 212, waiting for new connection  
Establishing data connection  
Child 216: data socket created with descriptor 6  
Child 216: data socket bound to port 53319  
Child 216: listening on data socket  
Child 216: Accepted connection on the data socket with descriptor 7  
Child 216: read 4096 bytes  
Child 216: write 4096 bytes  
Child 216: read 4096 bytes  
Child 216: write 4096 bytes  
Child 216: read 4096 bytes  
Child 216: write 4096 bytes  
Child 216: read 4096 bytes  
Child 216: write 4096 bytes  
Child 216: read 2419 bytes  
Child 216: write 2419 bytes  
Child 216: Quitting  
Child 216: Sending positive acknowledgement  
Child 216: exiting normally.  
Parent: accepted client connection with descriptor 5  
Parent: spawned child 212, waiting for new connection  
Child 262: startedChild 262: Connection accepted from host localhost  
Child 262: Client IP address -> 127.0.0.1  
Child 262: Quitting  
Child 262: Sending positive acknowledgement  
Child 262: exiting normally.  
Parent: accepted client connection with descriptor 6  
Parent: spawned child 212, waiting for new connection  
Child 268: startedChild 268: Connection accepted from host localhost  
Child 268: Client IP address -> 127.0.0.1  
Establishing data connection  
Child 268: data socket created with descriptor 7  
Child 268: data socket bound to port 53427  
Child 268: listening on data socket  
Child 268: Accepted connection on the data socket with descriptor 8  
Child 268: forking ls process  
Child 268: forked ls process  
Child 268: ls command complete  
Establishing data connection  
Child 268: data socket created with descriptor 8  
Child 268: data socket bound to port 53430  
Child 268: listening on data socket  
Child 268: Accepted connection on the data socket with descriptor 9  
Child 268: Reading file DELL.csv  
Child 268: transmitting file DELL.csv to client  
Child 268: numRead = 4096  
Child 268: numWrite = 4096  
Child 268: numRead = 4096  
Child 268: numWrite = 4096  
Child 268: numRead = 4096  
Child 268: numWrite = 4096  
Child 268: numRead = 4096  
Child 268: numWrite = 4096  
Child 268: numRead = 2419  
Child 268: numWrite = 2419  
Changed current directory to /mnt/c/Users/Tormentt/Documents/GitHub  
Establishing data connection  
Child 268: data socket created with descriptor 9  
Child 268: data socket bound to port 53432  
Child 268: listening on data socket  
Child 268: Accepted connection on the data socket with descriptor 10  
Child 268: forking ls process  
Child 268: forked ls process  
Child 268: ls command complete  
Changed current directory to /mnt/c/Users/Tormentt/Documents/GitHub/BackItUp  
Establishing data connection  
Child 268: data socket created with descriptor 10  
Child 268: data socket bound to port 53434  
Child 268: listening on data socket  
Child 268: Accepted connection on the data socket with descriptor 11  
Child 268: forking ls process  
Child 268: forked ls process  
Child 268: ls command complete  
Establishing data connection  
Child 268: data socket created with descriptor 11  
Child 268: data socket bound to port 53436  
Child 268: listening on data socket  
Child 268: Accepted connection on the data socket with descriptor 12  
Child 268: Reading file Makefile  
Child 268: transmitting file Makefile to client  
Child 268: numRead = 83  
Child 268: numWrite = 83  
Changed current directory to /mnt/c/Users/Tormentt/Documents/GitHub/BackItUp/TEST  
Establishing data connection  
Child 268: data socket created with descriptor 12  
Child 268: data socket bound to port 53443  
Child 268: listening on data socket  
Child 268: Accepted connection on the data socket with descriptor 13  
Child 268: forking ls process  
Child 268: forked ls process  
Child 268: ls command complete  
Establishing data connection  
Child 268: data socket created with descriptor 13  
Child 268: data socket bound to port 53446  
Child 268: listening on data socket  
Child 268: Accepted connection on the data socket with descriptor 14  
Child 268: read 154 bytes  
Child 268: write 154 bytes  
Child 268: Quitting  
Child 268: Sending positive acknowledgement  
Child 268: exiting normally.  

### Client Output:
kyle@LAPTOP-T16SRG68:/mnt/c/Users/Tormentt/Documents/GitHub/CS360-Final$ ./mftp -d 127.0.0.1  
Debugging enabled.  
Connected to server 127.0.0.1  
MFTP> cd TEST  
Command string = 'cd' with parameter = 'TEST'  
newCon.messageBuf = cd  
Changed local directory to /mnt/c/Users/Tormentt/Documents/GitHub/CS360-Final/TEST  
MFTP> rls  
Command string = 'rls'  
newCon.messageBuf = rls  
Sent D command to server  
Awaiting server response  
Received server response 'A53427'  
Obtained port number 53427 from server  
Created data socket with descriptor 4  
Data Socket Address/Port => 127.0.0.1:53427  
Attempting to establish Data Connection...  
Data connection to server established  
Awaiting server response  
Received server response 'A'  
Displaying data from server & forking to 'more'...  
Waiting for child process 267 to complete execustion of more  
total 5032  
drwxrwxrwx 1 kyle kyle    4096 May 13 12:52 .  
drwxrwxrwx 1 kyle kyle    4096 May 11 12:14 ..  
drwxrwxrwx 1 kyle kyle    4096 May 13 12:42 .git  
-rwxrwxrwx 1 kyle kyle      66 Mar 19 16:07 .gitattributes  
drwxrwxrwx 1 kyle kyle    4096 May 13 12:42 .idea  
-rwxrwxrwx 1 kyle kyle   18803 May 13 12:28 DELL.csv  
-rwxrwxrwx 1 kyle kyle     154 May 13 12:50 Makefile  
-rwxrwxrwx 1 kyle kyle     585 May 13 12:36 README.md  
drwxrwxrwx 1 kyle kyle    4096 May 13 12:52 TEST  
-rwxrwxrwx 1 kyle kyle   27016 May 13 12:50 mftp  
-rwxrwxrwx 1 kyle kyle   18112 May 13 12:23 mftp.c  
-rwxrwxrwx 1 kyle kyle    5507 Apr 28  2019 mftp.h  
-rwxrwxrwx 1 kyle kyle   22880 May 13 12:50 mftpserve  
-rwxrwxrwx 1 kyle kyle   13057 Apr 28  2019 mftpserve.c  
-rwxrwxrwx 1 kyle kyle 5029183 May 13 12:28 mobydick.txt  
Data display & more command completed.  
MFTP> get DELL.csv  
Command string = 'get' with parameter = 'DELL.csv'  
newCon.messageBuf = get  
Getting DELL.csv from server and storing to DELL.csv  
Sent D command to server  
Awaiting server response  
Received server response 'A53430'  
Obtained port number 53430 from server  
Created data socket with descriptor 5  
Data Socket Address/Port => 127.0.0.1:53430  
Attempting to establish Data Connection...  
Data connection to server established  
Awaiting server response  
Received server response 'A'  
Read 4096 bytes from server, writing to local file  
Read 4096 bytes from server, writing to local file  
Read 4096 bytes from server, writing to local file  
Read 4096 bytes from server, writing to local file  
Read 2419 bytes from server, writing to local file  
MFTP> ls  
Command string = 'ls'  
newCon.messageBuf = ls  
Client parent waiting on child process 267 to run ls locally  
Client child process 267 execing ls | more  
Child process 271 starting more  
Client child process 272 starting ls  
total 20  
drwxrwxrwx 1 kyle kyle  4096 May 13 12:52 .  
drwxrwxrwx 1 kyle kyle  4096 May 13 12:52 ..  
-rwxrwxrwx 1 kyle kyle 18803 May 13 12:52 DELL.csv  
Client parent continuing  
MFTP> rcd ..  
Command string = 'rcd' with parameter = '..'  
newCon.messageBuf = rcd  
Changed remote directory to ..  
Awaiting server response  
Received server response 'A'  
MFTP> rls  
Command string = 'rls'  
newCon.messageBuf = rls  
Sent D command to server  
Awaiting server response  
Received server response 'A53432'  
Obtained port number 53432 from server  
Created data socket with descriptor 4  
Data Socket Address/Port => 127.0.0.1:53432  
Attempting to establish Data Connection...  
Data connection to server established  
Awaiting server response  
Received server response 'A'  
Displaying data from server & forking to 'more'...  
Waiting for child process 267 to complete execustion of more  
total 0  
drwxrwxrwx 1 kyle kyle 4096 May 11 12:14 .  
drwxrwxrwx 1 kyle kyle 4096 Apr 21 14:27 ..  
drwxrwxrwx 1 kyle kyle 4096 Jan 13 12:17 .vs  
drwxrwxrwx 1 kyle kyle 4096 Nov 20  2018 A7  
drwxrwxrwx 1 kyle kyle 4096 Dec  1  2018 Assignment7  
drwxrwxrwx 1 kyle kyle 4096 May 13 12:00 BackItUp  
drwxrwxrwx 1 kyle kyle 4096 May 13 12:18 Basic-Shell  
drwxrwxrwx 1 kyle kyle 4096 Nov 10  2018 CS320A7  
drwxrwxrwx 1 kyle kyle 4096 Mar 19 15:52 CS351-A6  
drwxrwxrwx 1 kyle kyle 4096 Mar 19 15:58 CS360-A1  
drwxrwxrwx 1 kyle kyle 4096 May 13 12:52 CS360-Final  
drwxrwxrwx 1 kyle kyle 4096 Mar 29 14:46 DigitalForensics-Lab3  
drwxrwxrwx 1 kyle kyle 4096 May 11 15:21 Dorm-Chat  
drwxrwxrwx 1 kyle kyle 4096 May 11 12:23 KyleVoos.github.io  
drwxrwxrwx 1 kyle kyle 4096 Jan 31 09:04 Mini_Unix_Util  
drwxrwxrwx 1 kyle kyle 4096 Apr  6 18:31 Project-3-Producer-Consumer  
drwxrwxrwx 1 kyle kyle 4096 Apr 22 10:59 SQL-Injection  
drwxrwxrwx 1 kyle kyle 4096 Dec 13  2018 SRS-Project  
drwxrwxrwx 1 kyle kyle 4096 Nov 10  2018 __MACOSX  
drwxrwxrwx 1 kyle kyle 4096 Nov 10  2018 app  
drwxrwxrwx 1 kyle kyle 4096 Dec  6  2018 cougarshop  
drwxrwxrwx 1 kyle kyle 4096 Nov 11  2018 friendsApp  
drwxrwxrwx 1 kyle kyle 4096 Oct 18  2018 primefunctions  
drwxrwxrwx 1 kyle kyle 4096 Apr  8 19:17 qc-capstone  
drwxrwxrwx 1 kyle kyle 4096 Mar 19 15:41 qc-capstone.git  
drwxrwxrwx 1 kyle kyle 4096 Apr  8 20:04 qc-capstone1  
drwxrwxrwx 1 kyle kyle 4096 Nov 10  2018 simple-todos  
drwxrwxrwx 1 kyle kyle 4096 Sep 13  2018 tryGit  
Data display & more command completed.  
MFTP> rcd BackItUp  
Command string = 'rcd' with parameter = 'BackItUp'  
newCon.messageBuf = rcd  
Changed remote directory to BackItUp  
Awaiting server response  
Received server response 'A'  
MFTP> rls  
Command string = 'rls'  
newCon.messageBuf = rls  
Sent D command to server  
Awaiting server response  
Received server response 'A53434'  
Obtained port number 53434 from server  
Created data socket with descriptor 4  
Data Socket Address/Port => 127.0.0.1:53434  
Attempting to establish Data Connection...  
Data connection to server established  
Awaiting server response  
Received server response 'A'  
Displaying data from server & forking to 'more'...  
Waiting for child process 267 to complete execustion of more  
total 64  
drwxrwxrwx 1 kyle kyle  4096 May 13 12:00 .  
drwxrwxrwx 1 kyle kyle  4096 May 11 12:14 ..  
drwxrwxrwx 1 kyle kyle  4096 May 13 12:04 .git  
-rwxrwxrwx 1 kyle kyle    66 Apr  8 14:09 .gitattributes  
-rwxrwxrwx 1 kyle kyle   482 Apr  8 14:14 .gitignore  
drwxrwxrwx 1 kyle kyle  4096 May 13 12:04 .idea  
-rwxrwxrwx 1 kyle kyle 18184 May  3 16:46 BackItUp  
-rwxrwxrwx 1 kyle kyle   117 Apr  8 13:14 CMakeLists.txt  
-rwxrwxrwx 1 kyle kyle    83 May  3 16:46 Makefile  
-rwxrwxrwx 1 kyle kyle  5112 May 13 12:00 README.md  
drwxrwxrwx 1 kyle kyle  4096 May 13 11:51 TEST  
-rwxrwxrwx 1 kyle kyle 18032 Apr 11 14:44 backup  
drwxrwxrwx 1 kyle kyle  4096 Apr 29 19:27 cmake-build-debug  
-rwxrwxrwx 1 kyle kyle 14755 May  3 16:44 main.c  
Data display & more command completed.  
MFTP> show Makefile  
Command string = 'show' with parameter = 'Makefile'  
newCon.messageBuf = show  
Showing file Makefile  
Sent D command to server  
Awaiting server response  
Received server response 'A53436'  
Obtained port number 53436 from server  
Created data socket with descriptor 4  
Data Socket Address/Port => 127.0.0.1:53436  
Attempting to establish Data Connection...  
Data connection to server established  
Awaiting server response  
Received server response 'A'  
Displaying data from server & forking to 'more'  
Waiting for child process 267 to complete execution of more  
all: main.c  
        gcc -o BackItUp main.c -pthread -Wall -Werror  
  
test: main.c  
        make all  
Data display & more command completed.  
MFTP> rcd TEST  
Command string = 'rcd' with parameter = 'TEST'  
newCon.messageBuf = rcd  
Changed remote directory to TEST  
Awaiting server response  
Received server response 'A'  
MFTP> rls  
Command string = 'rls'  
newCon.messageBuf = rls  
Sent D command to server  
Awaiting server response  
Received server response 'A53443'  
Obtained port number 53443 from server  
Created data socket with descriptor 4  
Data Socket Address/Port => 127.0.0.1:53443  
Attempting to establish Data Connection...  
Data connection to server established  
Awaiting server response  
Received server response 'A'  
Displaying data from server & forking to 'more'...  
Waiting for child process 267 to complete execustion of more  
total 1280  
drwxrwxrwx 1 kyle kyle    4096 May 13 11:51 .  
drwxrwxrwx 1 kyle kyle    4096 May 13 12:00 ..  
drwxrwxrwx 1 kyle kyle    4096 May 13 11:51 .backup  
-rwxrwxrwx 1 kyle kyle   18184 May  3 16:40 BackItUp  
drwxrwxrwx 1 kyle kyle    4096 Apr 11 14:42 TEST1  
drwxrwxrwx 1 kyle kyle    4096 Apr 11 13:15 TEST2  
-rwxrwxrwx 1 kyle kyle   18032 May  3 16:40 backup  
-rwxrwxrwx 1 kyle kyle    5093 May  3 16:40 hashTable.c  
-rwxrwxrwx 1 kyle kyle 1257300 May  3 16:40 mobydick.txt  
-rwxrwxrwx 1 kyle kyle    1849 May  3 16:40 wordCount.c  
Data display & more command completed.  
MFTP> cd ..  
Command string = 'cd' with parameter = '..'  
newCon.messageBuf = cd  
Changed local directory to /mnt/c/Users/Tormentt/Documents/GitHub/CS360-Final  
MFTP> put Makefile  
Command string = 'put' with parameter = 'Makefile'  
newCon.messageBuf = put  
Putting file Makefile to Makefile  
Opened local file Makefile for reading  
Sent D command to server  
Awaiting server response  
Received server response 'A53446'  
Obtained port number 53446 from server  
Created data socket with descriptor 5  
Data Socket Address/Port => 127.0.0.1:53446  
Attempting to establish Data Connection...  
Data connection to server established  
Received server response 'A'  
Writing 154 bytes to server  
MFTP> exit  
Command string = 'exit'  
newCon.messageBuf = exit  
Exit command encountered  
Awaiting server response  
Received server response 'A'  
Client exiting normally  
kyle@LAPTOP-T16SRG68:/mnt/c/Users/Tormentt/Documents/GitHub/CS360-Final$  
