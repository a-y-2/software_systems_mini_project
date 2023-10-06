#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "../include/constants.h"

void connection_handler(int sockFD)
{
    char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading from / writting to the server
    ssize_t readBytes, writeBytes;            // Number of bytes read from / written to the socket

    char tempBuffer[1000];

    do
    {
        bzero(readBuffer, sizeof(readBuffer)); // Empty the read buffer
        bzero(tempBuffer, sizeof(tempBuffer));
        readBytes = read(sockFD, readBuffer, sizeof(readBuffer));//read data from sockFD and store it in readBuffer
        if (readBytes <= 0)
            perror("Error while reading from client socket!");
//         else if (strchr(readBuffer, '^') != NULL)//The strchr() function is used to search for the '^' character in the readBuffer. 
//         {
//             // Skip read from client
//             strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 1);
//             printf("%s", tempBuffer);
//             writeBytes = write(sockFD, "^", strlen("^"));
//             if (writeBytes == -1)
//             {
//                 perror("Error while writing to client socket!");
//                 break;
//             }
//         }
//         else if (strchr(readBuffer, '$') != NULL)
//         {
//             // Server sent an error message and is now closing it's end of the connection
//             strncpy(tempBuffer, readBuffer, strlen(readBuffer) - 2);
//             printf("%s", tempBuffer);
//             printf("\n\nClosing the connection to the server now!\n\n");
//             exit(EXIT_SUCCESS);
//         }
//         else
//         {

//             bzero(writeBuffer, sizeof(writeBuffer)); // Empty the write buffer
//             // password input
//             if (strchr(readBuffer, '>') != NULL)
//                 strcpy(writeBuffer, getpass(readBuffer));//The getpass() function is a C library function that is used to securely read a 
//                                                         //password or passphrase from the user without displaying the characters on the screen. 
//             else
//             {
//                 // data input
                printf("%s", readBuffer);
                scanf("%s", writeBuffer); // Take user input!
//             }

            writeBytes = write(sockFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing to client socket!");
                printf("Closing the connection to the server now!\n");
                //break;
            }
        }while(readBytes > 0);

    close(sockFD);

   }


int main(){

    int client_socket, connectStatus;
    struct sockaddr_in serverAddress;
    struct sockaddr server;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Error while creating server socket!");
        exit(0);
    }

    serverAddress.sin_family = AF_INET;                     // IPv4
    serverAddress.sin_port = htons(PORT);                   // Server will listen to port 8080
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // server's IP address

    //The client then connects to the server using the connect() function, specifying the server's address. 
    //This establishes a connection between the client and server.

    connectStatus = connect(client_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (connectStatus == -1)
    {
        perror("Error while connecting to server!");
        close(client_socket);
        exit(0);
    }

    printf("\n\n*******Welcome to ACADEMIA : for hassle free course registration !! ******\n");

    char ch;

    printf("\n\nEnter the login account type\n1. Administrator\n2. Student \n3. Faculty\n\nEnter your choice(1,2 or 3) : ");
    scanf("%c", &ch);
    if (ch != '1' && ch != '2' && ch != '3')
    {
        printf("Not a vaild input\n");
        exit(0);
    }

    write(client_socket, &ch, 1);
    printf("\n----------------------------------------------------------\n");
    connection_handler(client_socket);
    close(client_socket);

    return 0;
}