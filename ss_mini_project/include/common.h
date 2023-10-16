#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <crypt.h>
#include <ctype.h>
#include "./struc.h"
#include "./admin_credentials.h"
#include "./constants.h"

//-------------------definition of function-----------------------------------
bool Login_Check(int Socket_Descriptor, int Role);

//-----------------------login approval--------------------------------------
//Login Function(Role : 1=Admin, 2=Faculty, 3=Student)
bool login_handler(int Socket_Descriptor, int Role){
	ssize_t Write_Status, Bytes_Read;
	char Write_Buffer[1000], Read_Buffer[1000], Password[1000];
	int Id, UserId_Matched = 0, Password_Matched = 0;
	
	while(UserId_Matched == 0){
		bzero(Write_Buffer, sizeof(Write_Buffer));
		strcat(Write_Buffer, "Enter UserID : ");
		Write_Status = write(Socket_Descriptor, &Write_Buffer, sizeof(Write_Buffer));
		//Check_Write_Status(Write_Status);
		
		bzero(Read_Buffer, sizeof(Read_Buffer)); 
		Bytes_Read = read(Socket_Descriptor, &Read_Buffer, 1000);  //Collect ID from Client  (use of strlen() doesn't work here)
		Id = atoi(Read_Buffer);
		
		if(Role == 1){   //Admin Login
			if(Id == ADMIN_LOGIN_ID){
				UserId_Matched = 1;
				while(Password_Matched == 0){
					bzero(Write_Buffer, sizeof(Write_Buffer));
					strcat(Write_Buffer, "Enter Password : ");
					Write_Status = write(Socket_Descriptor, &Write_Buffer, sizeof(Write_Buffer));
					//Check_Write_Status(Write_Status);
					
					bzero(Read_Buffer, sizeof(Read_Buffer)); 
					Bytes_Read = read(Socket_Descriptor, &Read_Buffer, 1000);  //Collect Password from Client
					strcpy(Password, Read_Buffer);
					printf("Password obtained : %s", Password);
					if(strcmp(ADMIN_PASSWORD, Password) == 0){
                        Password_Matched = 1;
                        return true;
                    }
						
					else
						Password_Matched = 0;
				}
			}
			else{
				bzero(Write_Buffer, sizeof(Write_Buffer));
				strcat(Write_Buffer, "\nEntered UserId is wrong! ");
			}
		}
		/*
		else if(){
		
		}
		
		else{
		
		}
		*/
	}
}

int faculty_login_handler(int Socket_Descriptor, int Role){
	ssize_t Write_Status, Bytes_Read;
	char Write_Buffer[1000], Read_Buffer[1000], Password[1000];
	int Id;
	
	
	bzero(Write_Buffer, sizeof(Write_Buffer));
	strcat(Write_Buffer, "Enter UserID : ");
	write(Socket_Descriptor, &Write_Buffer, sizeof(Write_Buffer));
			
	bzero(Read_Buffer, sizeof(Read_Buffer)); 
	Bytes_Read = read(Socket_Descriptor, &Read_Buffer, 1000);  //Collect ID from Client  (use of strlen() doesn't work here)
	Id = atoi(Read_Buffer);

	bzero(Write_Buffer, sizeof(Write_Buffer));
	strcat(Write_Buffer, "Enter Password : ");
	Write_Status = write(Socket_Descriptor, &Write_Buffer, sizeof(Write_Buffer));
	//Check_Write_Status(Write_Status);
	
	bzero(Read_Buffer, sizeof(Read_Buffer)); 
	Bytes_Read = read(Socket_Descriptor, &Read_Buffer, 1000);  //Collect Password from Client
	strcpy(Password, Read_Buffer);

	struct Faculty faculty;

	int fd1 = open("FACULTY_RECORD.txt", O_RDONLY);
	if (fd1 == -1)
    {
        perror("Error while opening faculty file");
        return -2;
    }

	int bytesRead;
	int bytesToRead = sizeof(struct Faculty);
	int bytesToWrite = 0;

	// Read and write records, excluding the one to be deleted
	while ((bytesRead = read(fd1, &faculty, bytesToRead)) > 0) {
		if ((faculty.id == Id)&&(strcmp(Password,faculty.password)==0)) {
			return Id;
		} else {
			continue;
		}
	}
	return 0;	
}

