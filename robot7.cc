/*
 * IERG3310 Project
 * first version written by FENG, Shen Cody
 * Modified by Jonathan Liang @ 2016.10.25
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

	char messageBuffer[1024];
	char messageBuffer2[1024];
	char* studentIP;
	int messageBufferSize = sizeof(messageBuffer);
	int ListenSocket, s1, s2, s3;

	printf("Robot version %s (Oct. 24, 2016) started \n", ROBOT_VERSION);
	printf("You are reminded to check for the latest available version\n\n");

	// --------------------------------------------------------------------- //
	//								Step 1/2								 //
	// --------------------------------------------------------------------- //
	// Create a SOCKET for listening for incoming connection requests.
	printf("Creating TCP socket for listening and accepting connection...");
	ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error at creating socket...\n");
		return 1;
	}

	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	struct sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("0.0.0.0");
	service.sin_port = htons(LISTEN_PORT);

	if (bind( ListenSocket, (struct sockaddr *) &service, sizeof(service)) < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("bind() failed.\n");
		close(ListenSocket);
		return 1;
	} 
	if (listen( ListenSocket, 1 ) < 0) { // allow max = 1 connections in pending state 
		printf(" %s %d \n",strerror(errno), errno);
		printf("Error listening on socket.\n");
		close(ListenSocket);
		return 1;
	}
	printf("Done\n");

	printf("\nReady to accept connection on port %d\n", LISTEN_PORT);
	printf("Waiting for connection...\n");
	// When accept is called, the function is blocked and wait until a client is connected
	// A new socket is returned when accepting a connection
	
	struct sockaddr_in sin; 
	int size = sizeof(sin);
	memset(&sin, 0, size);
	
	s1 = accept( ListenSocket, (struct sockaddr*)&sin, (socklen_t *)&size );
	
	if (s1 < 0) {
		printf(" %s %d \n",strerror(errno), errno);
		printf("accept failed\n");
		close(ListenSocket);
		return 1;
	}
	
	//	The listen socket is no longer required
	//	Usually you can use a loop to accept new connections
	close(ListenSocket);
	
	printf("\nClient from %s at port %d connected\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
	studentIP = inet_ntoa(sin.sin_addr);
	
	char studentID[11];
	memset(studentID, 0, 11);
	int totalByteReceived = 0;
	int byteToReceive = 10;
	int iResult=0;

	
	//	Receive the data from the socket s1
	//	TCP socket is a stream socket which does not preserve message boundary
	//	Calling recv to receive k bytes may receive less than k bytes
	//	A loop is required
	do {
		iResult = recv(s1, studentID+totalByteReceived, 11-totalByteReceived, 0);
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
	printf("Stduent ID received: %s\n", studentID);
    
	// --------------------------------------------------------------------- //
	//								Step 3									 //
	// --------------------------------------------------------------------- //
	// Generate a random port number and ask STUDENT to listen
	srand((unsigned)time( NULL ));
	int iTCPPort2Connect = rand()%10000 + 20000;

	printf("Requesting STUDENT to accept TCP <%d>...", iTCPPort2Connect);
	// Send the port required to the STUDENT
	memset(messageBuffer, 0, messageBufferSize);
	sprintf(messageBuffer, "%d", iTCPPort2Connect);

	int byte_left = (int)strlen(messageBuffer);
	// Similar to recv, a loop is required
	while (byte_left > 0) {
		iResult = send(s1, messageBuffer-byte_left+(int)strlen(messageBuffer), byte_left,0);
		
		if (iResult < 0) {
			printf("send() failed with error\n");
			return 1;
		}

		byte_left -= iResult;
	}
	printf("Done\n"); 

	// --------------------------------------------------------------------- //
	//								Step 4	 //
	// --------------------------------------------------------------------- //
	sleep(1);
	printf("\nConnecting to the STUDENT s2 <%d> ...", iTCPPort2Connect);
	struct sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(studentIP);
	clientService.sin_port = htons(iTCPPort2Connect);

	// Connect to server.
	if(	(s2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error at creating socket s2 ... \n");
		return 1;
	}
	if ( connect( s2, (struct sockaddr *) &clientService, sizeof(clientService) ) < 0) {
		printf( "Error: connection failed.\n");
		printf(" %s %d \n",strerror(errno), errno);
		close(s2);
		return 1;
	}
	printf("Done\n");

	/* Step 7 - Receive the buffersize information of student in TCP port 3310 */
	char buffersizes2[30];
	memset(studentID, 0, 30);
	totalByteReceived = 0;
	byteToReceive = 29;
	iResult=0;

	
	//	Receive the data from the socket s1
	//	TCP socket is a stream socket which does not preserve message boundary
	//	Calling recv to receive k bytes may receive less than k bytes
	//	A loop is required
	
	do {
		iResult = recv(s1, buffersizes2+totalByteReceived, 29-totalByteReceived, 0);
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
	printf("[ROBOT] Buffer size information received: %s\n", buffersizes2);
	
	/* Step 7 - Send large number of message (as quickly as possible within 30 seconds) */
	int ctt, ptt, t;
    	bool print = true;
    	time_t sct, spt;
    	struct tm* ct; 
    	struct tm* pt;
 
    	// time in seconds
    	sct = time(NULL);
 	// to get current time
 	ct = localtime(&sct);
    	ctt = (ct->tm_hour)*3600+(ct->tm_min)*60+(ct->tm_sec); 
	while (1) {
		spt = time(NULL);
		pt = localtime(&spt);
		byte_left = (int)strlen(messageBuffer2);
		while (byte_left > 0) {
			iResult = send(s2, messageBuffer2-byte_left+(int)strlen(messageBuffer2), byte_left,0);
		
			if (iResult < 0) {
				printf("[ROBOT] Message send() failed with error\n");
				printf(" %s %d \n",strerror(errno), errno);
				break;
			}

			byte_left -= iResult;
		}
		

		ptt = (pt->tm_hour)*3600+(pt->tm_min)*60+(pt->tm_sec);
		if ((int)(ptt-ctt) % 5 == 0 && print)
		{
			printf("[ROBOT] Time: %d seconds\n", (int)(ptt-ctt));
			print = false;
		}
		if ((int)(ptt-ctt) % 5 != 0)
		 	print = true;
		 	
		if (ptt-ctt >= 30)
			break;
	}

	
	sleep(1);
	
	printf("[ROBOT] Time %d, %d, %d\n", ctt, ptt, ptt-ctt);
	printf("[ROBOT] Step 7 finished\n");
	
	close(s1);
	close(s2);
	return 0;
}
