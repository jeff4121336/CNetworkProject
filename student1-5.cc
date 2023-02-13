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
	client.sin_addr.s_addr = inet_addr("0.0.0.0"); //Modify for step 6 : Using IERG3800 vm secondhost : robot ip - 172.16.143.2  (Before 0.0.0.0)
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
	client_2.sin_addr.s_addr = inet_addr("127.0.0.1");
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
	
	close(s_2Listener); //s_2 setup successful, close s_2Listener
	
	/*	
	Step 4
	Use s_2 and receive a string fffff,eeeee ,that is ROBOT UDP port# and STUDENT UDP port#
	decode it and create a UDP (eeeee port), sending a random var (5 < var < 10) to port fffff
	ROBOT -> send a char string xxx… of length num * 10 to STUDENT 
	one second after receiving num and STUDENT will receive the string 
	using s3 on UDP port eeeee.
	*/
	
	char UDP_portinfo[13];
	memset(UDP_portinfo, 0, 13);
	totalByteReceived = 0, byteToReceive = 12, iResult=0;
		do { //recv two UDP port number at once
		iResult = recv(s_2, UDP_portinfo+totalByteReceived, 13-totalByteReceived, 0);
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
	
	// Debug Use - printf("UDP_portinfo received: %s\n", UDP_portinfo);
	/* 	
	Decoding - eeeee = UDP_portinfo index 0 - 4 (ROBOT)
		 - fffff = UDP_portinfo index 6 - 10 (STUDENT)
	** USING ASCII CODE for UDP_portinfo elements ** 
	*/
	
	printf("\nDecoding received UDP_portinfo...");
	int SUDP_portno = 0, RUDP_portno = 0, SUDP_base = 10000, RUDP_base = 10000;
	for (int i = 0; i < 11; i++) 
	{
		if (i <= 4) //eeeee
		{
			RUDP_portno += (UDP_portinfo[i] - '0') * RUDP_base;
			RUDP_base /= 10;
		} else if (i >= 6) //fffff
		{
			SUDP_portno += (UDP_portinfo[i] - '0') * SUDP_base;
			SUDP_base /= 10;
		}
	}
	printf("\nROBOT UDP port #: %d\nSTUDENT UDP port #: %d\n", RUDP_portno, SUDP_portno);
	
	//hard to do random (truely)
	srand(time(NULL));  // using seed here srand <time.h>
    	int randvar;
        randvar = rand();
        randvar = (randvar % 4) + 1 + 5;
	printf("randvar: %d\n", randvar);
	
	char strrandvar[2];
	memset(strrandvar, 0, sizeof(strrandvar));
	sprintf(strrandvar, "%d", randvar);
	//send this to ROBOT UDP at port fffff
	
	int s_3; // UDP socket 
	printf("\nPreparing UDP socket port...");
	s_3 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s_3 < 0) {
		printf("Error at socket()\n");
		return 1;
	}
	printf("Done\n");
	
	struct sockaddr_in RUDPAddr;
	RUDPAddr.sin_family = AF_INET;
	RUDPAddr.sin_port = htons(RUDP_portno);
	RUDPAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	struct sockaddr_in SUDPAddr;
	SUDPAddr.sin_family = AF_INET;
	SUDPAddr.sin_port = htons(SUDP_portno);
	SUDPAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(s_3, (struct sockaddr *) &SUDPAddr, sizeof(SUDPAddr));
	
	printf("Sending randvar <%s> to ROBOT UDP at port# %d...\n", strrandvar, RUDP_portno);
	
	int t = sendto(s_3, strrandvar, strlen(strrandvar), 0, (struct sockaddr*) &RUDPAddr, sizeof(RUDPAddr)); //send to ROBOT
	if (t < 0) {
		printf("Error at sending\n");
		return 1;
	} else 
	{	
		printf("Done\n");
	} 
	
	char UDPpacket[100];
	memset(UDPpacket, 0, sizeof(UDPpacket));
	recv(s_3, UDPpacket, sizeof(UDPpacket), 0);
	printf("Received packet: %s\n", UDPpacket);
	
	/*
	  step 5 - Sending the received info back to ROBOT (5 times with 1sec period)
	  Modification of code in robot.cc
	  Robot send to STUDENT ----> (changed to) STUDENT send to ROBOT
	*/
	
	for (int i = 0; i < 5; i++)
	{
		sendto(s_3, UDPpacket, strlen(UDPpacket), 0, (struct sockaddr*) &RUDPAddr, sizeof(RUDPAddr));
		sleep(1);
		printf("UDP packet %d sent\n", i + 1);
	}
	
	close(StudentSocket);
	close(s_3);
	return 0;
	
}
