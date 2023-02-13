/*
 * IERG3310 Project
 * Student Coding Part :3
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define LISTEN_PORT 3310
#define ROBOT_VERSION "3.0"

int main(){

	/* Step 2 - Set up a student (client) and connect to robot (server) */
	int StudentSocket;
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = inet_addr("172.16.143.2"); //robot ip
	client.sin_port = htons(LISTEN_PORT);
	
	/* 
	   int socket(int domain, int type, int protocol); - create an endpoint for communication
	   int connect(int socket, const struct sockaddr *address, socklen_t address_len); 
	   Reference 1: https://man7.org/linux/man-pages/index.html (linux man page)
	   Reference 2: https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
	*/
	StudentSocket = socket(AF_INET, SOCK_STREAM, 0); //End point at client
	if (StudentSocket < 0) { //Error Checking (Modification from robot.cc)
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error at creating socket...\n");
		return 1;
	}
	/* Connection */
	if (connect(StudentSocket, (struct sockaddr *)&client, sizeof(client)) == -1) //Fail to connect
	{
		printf(" %s %d \n",strerror(errno), errno);
		printf(" Fail to connect the robot\n");
		return -1; //End Program
	} else 
	{
		printf("Connection Success\n"); //Connect to robot
		//ssize_t send(int socket, const void *buffer, size_t length, int flags);
		send(StudentSocket, "1155158477", 10, 0); 
		printf("SID sending...\n");
	}
	
	
	//Step 3 - Refernece: robot.cc recv() 
	/* 
	   Setup s_2Listener, bind with correct IP and port number
	   Wait the connection request form robot
	   Accept it
	*/
	
	char s_2portnum[6];
	memset(s_2portnum, 0, 6);
	int totalByteReceived = 0;
	int byteToReceive = 5;
	int iResult=0;
		do {
		iResult = recv(StudentSocket, s_2portnum+totalByteReceived, 6-totalByteReceived, 0);
		if ( iResult > 0 ){
			printf("Bytes received: %d\n", iResult);
			totalByteReceived += iResult;
			if (totalByteReceived >= byteToReceive)
				break;
		}
		else if ( iResult == 0 ){
			printf(" %s %d \n",strerror(errno), errno);
			printf("Connection closed\n");
			return 1;
		}
		else{
			printf(" %s %d \n",strerror(errno), errno);			
			printf("recv failed\n");
			return 1;
		}
	} while( iResult > 0 );
	printf("s_2 port number received: %s\n", s_2portnum);
	
	/* change char[] to int */
	int s_2port = 0, convert_2portnumberbase = 10000;
	for (int i = 0; i < byteToReceive; i++) 
	{
		//printf("Converting %d", s_2_portnumber[i]); Debug
		/* s_2_portnumber[i] in ASCII Code */
		s_2port += (s_2portnum[i] - 48) * convert_2portnumberbase;
		convert_2portnumberbase /= 10;
	}
	
	//printf("Converted s_2 port number to int as: %d\n", s_2port); Debug Line
	
	int s_2Listener, s_2; 
	struct sockaddr_in client_2;
	client_2.sin_family = AF_INET;
	client_2.sin_addr.s_addr = inet_addr("0.0.0.0");
	client_2.sin_port = htons(s_2port);
	int size_client_2 = sizeof(client_2);
	
	s_2Listener = socket(AF_INET, SOCK_STREAM, 0);
	if (s_2Listener < 0) { //socket error checking
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error at creating socket...\n");
		return 1;
	}
	 
	
	if (bind(s_2Listener, (struct sockaddr*)&client_2, size_client_2) < 0) { 
	//bind() error checking
		printf(" %s %d \n",strerror(errno), errno);
		printf("bind() failed.\n");
		close(s_2Listener);
		return 1;
	} 
	if (listen(s_2Listener, 1 ) < 0) { 
	// allow max = 1 connections in pending state 
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error listening on socket.\n");
		close(s_2Listener);
		return 1;
	}
	
	printf("TCP Socket s_2 ready\n");
	
	printf("\nReady to accept connection on port %d\n", s_2port);
	printf("Waiting for connection...\n");
	//Accept the connection from ROBOT here
	s_2 = accept(s_2Listener, (struct sockaddr*)&client_2, (socklen_t *)&size_client_2); 
	if (s_2 < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("accept failed\n");
		close(s_2Listener);
		return 1;
	} else 
	{
		printf("s_2 Connect Sucessful!\n"); //nice :)
	}
	
	
	/* Step 7 - Get buffer size (receive) then print it out */
	// Reference: https://gist.github.com/roxtar/e42b67745bbbfb598834 
	// 	      https://stackoverflow.com/questions/57455692/linux-unix-sockets-vs-tcp-sockets-send-buffer-size
	
	int n;
	unsigned int m = sizeof(n);
	getsockopt(s_2,SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);
	
	int nRecvBuf = n;	
	//Testing Factor for Step 8: 
	// Divide 3 // Orginal // Multiple 2 / 10 / 25 / 60
	setsockopt(s_2,SOL_SOCKET,SO_RCVBUF, (const char*)&nRecvBuf, sizeof(int));
	
	printf("[STUDENT] Receive buffer size: %d\n", nRecvBuf);
	
	/* Step 7 - Pack the infomation into bsxxxxx string and send to robot */
	
	char buffersizestring[30] = "bs", buffersize[15];
	sprintf(buffersize, "%d", nRecvBuf);
	strcat(buffersizestring, buffersize);
	printf("[STUDENT] Buffersize sending: %s\n", buffersizestring);
	sleep(2);
	send(StudentSocket, buffersizestring, sizeof(buffersizestring)/sizeof(char), 0);
	printf("[STUDENT] Buffersize finish sending\n");
	close(StudentSocket);

	/* Step 7 - Receive message (s2) from ROBOT, count then print */
	char s_2messagebuffer[1024];
	memset(s_2messagebuffer, 0, 1024);
	totalByteReceived = 0;
	iResult=0;
	byteToReceive = 1024 - 1;
	int messageNo = 0;
	do {
		iResult = recv(s_2, s_2messagebuffer, byteToReceive, 0);
		
		if ( iResult > 0 ){
			//printf("Bytes received: %d\n", iResult);
			totalByteReceived += iResult;
			messageNo++;
		}
		else if ( iResult == 0 ){
			printf(" %s %d \n",strerror(errno), errno);
			printf("[STUDENT] Message Total: %d Total Bytes received: %d\n", messageNo, totalByteReceived);
			printf("[STUDENT] Step 7 finished\n");
			printf("Connection closed\n");
			close(s_2Listener); 
			return 1;
		}
		else{
			printf(" %s %d \n",strerror(errno), errno);			
			printf("recv failed\n");
			return 1;
		}
		
	} while( iResult > 0 );
	
	
	return 0;
	
}
