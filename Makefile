all: mftp mftpserve

mftp: mftp.c mftp.h
	gcc -o mftp mftp.c mftp.h

mftpserve:
	gcc -o mftpserve mftpserve.c mftp.h

clean:
	rm mftp mftpserve
