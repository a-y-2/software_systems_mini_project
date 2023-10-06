#include <stdbool.h>
#include "./struc.h"
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<stdio.h>
#include<errno.h>
#include<time.h>
#include<unistd.h>
#include<ctype.h>

//global var declaration


bool admin_operation_handler(int connFD);
bool add_student(int connFD);

bool admin_operation_handler(int connFD)
{
    ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
    char readBuffer[1000], writeBuffer[1000]; 

    strcat(writeBuffer, "\n\n------------------------Admin Menu-------------------------\n");
            strcat(writeBuffer, "\n1. Add Student\n2. Add faculty\n3. Activate student\n4. Deactivate student\n5. Update Student Details\n6. Update Faculty Details\n7. Logout\n");
            strcat(writeBuffer, "\n------------------------------------------------------------\nEnter your choice : ");


            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            
            if (writeBytes == -1)
            {
                perror("Error while writing ADMIN_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));//client has seen the above menu and sent the choice
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                add_student(connFD);
                break;

            // case 2:
            //     delete_account(connFD);
            //     break;
            // case 3:
            //     modify_customer_info(connFD);
            //     break;

            // case 4:
            //     get_customer_details(connFD, -1);
            //     break;

            // case 5:
            //     get_account_details(connFD, NULL);
            //     break;

            // case 6:

            //     writeBytes = write(connFD, "\nLogging you out!$", strlen("\nLogging you out!$"));
            //     return false;
            default:
                writeBytes = write(connFD, "\nLogging you out!$", strlen("\nLogging you out!$"));
                return false;
            }
        
    
    // else
    // {
    //     // ADMIN LOGIN FAILED
    //     bzero(writeBuffer, sizeof(writeBuffer));
    //     sprintf(writeBuffer, "%s\n", "No account found!!$");
    //     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    //     return false;
    // }
    return true;
}

bool add_student(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Student new_stu;
    int id=0;


    sprintf(writeBuffer, "%s%s", "\nEnter the details of the student : \n\n", "student name - ");
    //After the sprintf() call, writeBuffer will contain the formatted message

    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing message to client!");
        return -2;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading student name response from client!");
        
        return -2;
    }

    strcpy(new_stu.name, readBuffer);

    writeBytes = write(connFD, "\n student's gender?\n 1. Male(M)\n 2. Female(F)\n 3. Others(O)\n Enter the choice(M/F/O) : ", strlen("\nWhat is the customer's gender?\n 1. Male(M)\n 2. Female(F)\n 3. Others(O)\n Enter the choice(M/F/O) : "));
    if (writeBytes == -1)
    {
        perror("Error writing message to client!");
        return -2;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer gender response from client!");
        return -2;
    }

    if (readBuffer[0] == 'M' || readBuffer[0] == 'F' || readBuffer[0] == 'O' || readBuffer[0] == 'm' || readBuffer[0] == 'f' || readBuffer[0] == 'o')
        new_stu.gender = toupper(readBuffer[0]);
    else
    {
        writeBytes = write(connFD, "\nIt seems you've entered a wrong gender choice!\n^", strlen("\nIt seems you've entered a wrong gender choice!\n^"));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return -2;
    }

    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, "\nstudent's age ? : ");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing message to client!");
        return -2;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading student age response from client!");
        return -2;
    }

    int studentAge = atoi(readBuffer);
     new_stu.age = studentAge;

    int fd  = open("STUDENT_RECORD.txt",O_CREAT|O_RDWR,0777);

    if (fd == -1)
    {
        perror("Error while opening student file");
        return -2;
    }
    
    else{
        new_stu.id=id;

        struct flock lock ;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
        lock.l_start = (new_stu.id-1)*sizeof(new_stu);//Set the starting position for the lock. 
        lock.l_len = sizeof(new_stu);//lock covers the entire student record
        lock.l_pid = getpid();   
    
        //set lock on file    
        fcntl(fd, F_SETLKW, &lock);
        new_stu.id++;

       //entering critical section to add student 
        lseek(fd,(new_stu.id-1)*sizeof(new_stu),SEEK_SET);
        write(fd,&new_stu,sizeof(new_stu));

        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLK,&lock);
        printf("\n student details added");
    
    }
}