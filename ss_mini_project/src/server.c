#include <stdio.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/ip.h> 
#include <string.h>
#include <stdlib.h>
#include "../include/common.h"

// includes
#include "../include/constants.h"
//#include "../include/admin.h"
#include "../include/faculty.h"
#include "../include/student.h"


// Handles the communication with the client
void connection_handler(int connfd)
{
    printf("Client has connected to the server!\n");

    char readBuffer[1000], writeBuffer[1000], choice;
    ssize_t readBytes, writeBytes;
    int userChoice;

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connfd, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
        perror("Error while reading from client");
    else if (readBytes == 0)
        printf("No data was sent by the client\n");
    else
    {
        userChoice = atoi(readBuffer);

        switch (userChoice)
        {
        // case 1:
        //     // Admin

        //     admin_operation_handler(connfd);
        //     break;
        case 2:
            // student

            student_operation_handler(connfd);
            break;
        case 3:
            // faculty

            faculty_operation_handler(connfd);
            break;
        default:
            exit(EXIT_FAILURE);
        }
    }

    printf("Terminating connection to client!\n");
}

int main(){

    int socketFileDescriptor, socketBindStatus, socketListenStatus, connfd;
    struct sockaddr_in serverAddress, clientAddress;

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);  //create new socket , returns fd on success else -1
                                                             //AF_INET is passed as the first argument to the socket function to create a socket that uses IPv4

    if (socketFileDescriptor == -1)
    {
        perror("Error while creating server socket!");
        exit(0);
    }

    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_port = htons(PORT);              // Server will listen to port 8080
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Binds the socket to all interfaces

    //bind socket to address
    socketBindStatus = bind(socketFileDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (socketBindStatus == -1)
    {
        perror("Error while binding to server socket!");
        exit(0);
    }

    printf("Server Listening on port %d \n", PORT);

    //listen() system call allows a stream socket to accept incoming connections from other sockets
    socketListenStatus = listen(socketFileDescriptor, 10);
    if (socketListenStatus == -1)
    {
        perror("Error while listening for connections on the server socket!");
        close(socketFileDescriptor);
        exit(0);
    } 
    puts("Waiting for connections ...");
    int addrlen;
    while (1)
    {
        addrlen = (int)sizeof(clientAddress);

        //accept an incoming connection request from a client socket after the server socket has been set up to listen for connections 
        //The accept() function typically creates a new socket to handle the actual communication with the client. 
        //Once the connection has been accepted, the server can use the new socket (often referred to as the "client socket" or 
        //"accepted socket") to send and receive data to and from the client. This is where the actual data exchange between the server and client occurs
        
        connfd = accept(socketFileDescriptor, (struct sockaddr *)&clientAddress, &addrlen);
        if (connfd == -1)
        {
            perror("Error while connecting to client!");
            close(socketFileDescriptor);
            exit(EXIT_FAILURE);
        }

        printf("New connection , socket fd is %d\n", connfd);

        if (!fork())
        {
            // Child will enter this branch
            connection_handler(connfd);
            close(connfd);
            exit(0);
        }
    }

    close(socketFileDescriptor);
    

    return 0;
}