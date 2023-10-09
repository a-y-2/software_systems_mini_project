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
#include<crypt.h>



//global var declaration


bool admin_operation_handler(int connFD);
bool add_student(int connFD);
bool add_faculty(int connFD);
bool update_student(int connFD);

bool admin_operation_handler(int connFD)
{
    ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
    char readBuffer[1000], writeBuffer[1000]; 

    strcat(writeBuffer, "\n\n------------------------Admin Menu-------------------------\n");
            strcat(writeBuffer, "\n1. Add Student\n2. Add faculty\n3. Activate faculty\n4. Deactivate faculty\n5. Update Student Details\n6. Update Faculty Details\n7. Logout\n");
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

            case 2:
                add_faculty(connFD);
                break;
            // case 3:
            //     modify_customer_info(connFD);
            //     break;

            // case 4:
            //     get_customer_details(connFD, -1);
            //     break;

            case 5:
                update_student(connFD);
                break;

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

  
//ADD STUDENT
bool add_student(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Student new_stu, prev_stu;
  
    int fd = open("STUDENT_RECORD.txt", O_RDONLY);
    if (fd == -1 && errno == ENOENT)  //The code first checks if customerFileDescriptor is equal to -1 and errno is equal to ENOENT (which stands for "No such file or directory"). 
                                        //This condition checks if the file does not exist.
    {
        // Student file was never created
        new_stu.id = 0;
    }
    else if (fd == -1)
    {
        perror("Error while opening student file");
        return -2;
    }
    else
    {
        int offset = lseek(fd, -sizeof(struct Student), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last student record!");
            return -2;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};
        int lockingStatus = fcntl(fd, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Student record!");
            return -2;
        }

        readBytes = read(fd, &prev_stu, sizeof(struct Student));
        if (readBytes == -1)
        {
            perror("Error while reading Student record from file!");
            return -2;
        }

        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);

        close(fd);

        new_stu.id = prev_stu.id + 1;
    }


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

    writeBytes = write(connFD, "\n student's gender?\n 1. Male(M)\n 2. Female(F)\n 3. Others(O)\n Enter the choice(M/F/O) : ", strlen("\nWhat is the Faculty's gender?\n 1. Male(M)\n 2. Female(F)\n 3. Others(O)\n Enter the choice(M/F/O) : "));
    if (writeBytes == -1)
    {
        perror("Error writing message to client!");
        return -2;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading Student gender response from client!");
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


    int fd1  = open("STUDENT_RECORD.doc",O_CREAT | O_APPEND | O_WRONLY,0777);

    if (fd1 == -1)
    {
        perror("Error while opening student file");
        return -2;
    }
    
    else{
        

        struct flock lock ;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
        lock.l_start = (new_stu.id-1)*sizeof(new_stu);//Set the starting position for the lock. 
        lock.l_len = sizeof(new_stu);//lock covers the entire faculty record
        lock.l_pid = getpid();   
    
        //set lock on file    
        fcntl(fd1, F_SETLKW, &lock);
        //new_stu.id++;

       //entering critical section to add faculty 
        lseek(fd1,(new_stu.id-1)*sizeof(new_stu),SEEK_SET);
        write(fd1,&new_stu,sizeof(new_stu));

        lock.l_type = F_UNLCK;
        fcntl(fd1,F_SETLK,&lock);
        printf("\n student details added");
        close(fd1);
    }
}


//ADD FACULTY
bool add_faculty(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Faculty new_fac, prev_fac;
  
    int fd = open("FACULTY_RECORD.txt", O_RDONLY);
    if (fd == -1 && errno == ENOENT)  //The code first checks if customerFileDescriptor is equal to -1 and errno is equal to ENOENT (which stands for "No such file or directory"). 
                                        //This condition checks if the file does not exist.
    {
        // Faculty file was never created
        new_fac.id = 0;
    }
    else if (fd == -1)
    {
        perror("Error while opening faculty file");
        return -2;
    }
    else
    {
        int offset = lseek(fd, -sizeof(struct Faculty), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last faculty record!");
            return -2;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};
        int lockingStatus = fcntl(fd, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Faculty record!");
            return -2;
        }

        readBytes = read(fd, &prev_fac, sizeof(struct Faculty));
        if (readBytes == -1)
        {
            perror("Error while reading Faculty record from file!");
            return -2;
        }

        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);

        close(fd);

        new_fac.id = prev_fac.id + 1;
    }


    sprintf(writeBuffer, "%s%s", "\nEnter the details of the faculty : \n\n", "faculty name - ");
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
        perror("Error reading faculty name response from client!");
        
        return -2;
    }

    strcpy(new_fac.name, readBuffer);

    writeBytes = write(connFD, "\n faculty's gender?\n 1. Male(M)\n 2. Female(F)\n 3. Others(O)\n Enter the choice(M/F/O) : ", strlen("\nWhat is the Faculty's gender?\n 1. Male(M)\n 2. Female(F)\n 3. Others(O)\n Enter the choice(M/F/O) : "));
    if (writeBytes == -1)
    {
        perror("Error writing message to client!");
        return -2;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading Faculty gender response from client!");
        return -2;
    }

    if (readBuffer[0] == 'M' || readBuffer[0] == 'F' || readBuffer[0] == 'O' || readBuffer[0] == 'm' || readBuffer[0] == 'f' || readBuffer[0] == 'o')
        new_fac.gender = toupper(readBuffer[0]);
    else
    {
        writeBytes = write(connFD, "\nIt seems you've entered a wrong gender choice!\n^", strlen("\nIt seems you've entered a wrong gender choice!\n^"));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return -2;
    }

    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, "\nfaculty's age ? : ");
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
        perror("Error reading faculty age response from client!");
        return -2;
    }

    int facultyAge = atoi(readBuffer);
     new_fac.age = facultyAge;

    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, "\ncourse taught by the faculty ? : ");
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
        perror("Error reading Faculty course response from client!");
        return -2;
    }

    strcpy(new_fac.course, readBuffer);




    int fd1  = open("FACULTY_RECORD.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    if (fd1 == -1)
    {
        perror("Error while opening faculty file");
        return -2;
    }
    
    else{
        

        struct flock lock ;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
        lock.l_start = (new_fac.id-1)*sizeof(new_fac);//Set the starting position for the lock. 
        lock.l_len = sizeof(new_fac);//lock covers the entire faculty record
        lock.l_pid = getpid();   
    
        //set lock on file    
        fcntl(fd1, F_SETLKW, &lock);
        //new_fac.id++;

       //entering critical section to add faculty 
        lseek(fd1,(new_fac.id-1)*sizeof(new_fac),SEEK_SET);
        write(fd1,&new_fac,sizeof(new_fac));

        lock.l_type = F_UNLCK;
        fcntl(fd1,F_SETLK,&lock);
        printf("\n faculty details added");
        close(fd1);
    }
}

//---------------------------UPDATE STUDENT ---------------------------------------------------
bool update_student(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Student student;

    int studentID;

    off_t offset;//off_t is a data type often used in C and C++ programming for representing file offsets
    int lockingStatus;

    writeBytes = write(connFD, "\nEnter the ID of the student whose information you want to edit : ", strlen("\nEnter the ID of the student whose information you want to edit : "));
    if (writeBytes == -1)
    {
        perror("Error while writing message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading student ID from client!");
        return false;
    }

    studentID = atoi(readBuffer);
    // printf("%s", readBuffer);

    int studentFileDescriptor = open("STUDENT_RECORD.txt", O_RDONLY);
    if (studentFileDescriptor == -1 || ((strcmp(readBuffer, "0") != 0) && studentID == 0))
    {
        // student File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No student found for the given ID***\n");
        //strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    offset = lseek(studentFileDescriptor, (studentID-1) * sizeof(struct Student), SEEK_SET);
    if (errno == EINVAL)
    {
        // student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No student be found for the given ID***\n");
        //strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on student record!");
        return false;
    }

    readBytes = read(studentFileDescriptor, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error while reading customer record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(studentFileDescriptor, F_SETLK, &lock);

    close(studentFileDescriptor);
    if (readBytes == 0)
    {
        // student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No student could be found for the given ID***\n");
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    writeBytes = write(connFD, "Which information would you like to modify?\n1. Name 2. Age 3. Gender 4. Login ID 5. Password \nPress any other key and enter to cancel : ", strlen("Which information would you like to modify?\n1. Name 2. Age 3. Gender 4. Login ID 5. Password \nPress any other key and enter to cancel : "));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_STUDENT_MENU message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting student modification menu choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\nInvalid input!\nYou'll now be redirected to the main menu!\n^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        // UPDATE NAME
        writeBytes = write(connFD, "\nWhat's the updated value for name ? : ", strlen("What's the updated value for name ? : "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_STUDENT_NEW_NAME message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new name from client!");
            return false;
        }
        strcpy(student.name, readBuffer);
        break;
    case 2:
        // update age
        writeBytes = write(connFD, "\nWhat's the updated value for age ? ", strlen("What's the updated value for age ? "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_STUDENT_NEW_AGE message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new age from client!");
            return false;
        }
        int updatedAge = atoi(readBuffer);
        if (updatedAge < 18 || updatedAge > 100)
        {
            // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, "Invalid input!\nYou'll now be redirected to the main menu!\n^");
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
                return false;
            }
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
            return false;
        }
        student.age = updatedAge;
        break;
    case 3:
        // update gender
        writeBytes = write(connFD, "\nWhat's the updated value for gender ? ", strlen("What's the updated value for gender ? "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));

        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new gender from client!");
            return false;
        }
        if (readBuffer[0] != 'M' && readBuffer[0] != 'F' && readBuffer[0] != 'O' && readBuffer[0] != 'm' && readBuffer[0] != 'f' && readBuffer[0] != 'o')
        {
            perror("Error while getting response for student's new gender from client!");
            bzero(writeBuffer, sizeof(writeBuffer));
            sprintf(writeBuffer, "%s^", "\nEnter a valid character for gender.\n");

            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            bzero(writeBuffer, sizeof(writeBuffer));
            return false;
        }
        student.gender = toupper(readBuffer[0]);
        break;

    case 4:
        // UPDATE login id
        writeBytes = write(connFD, "\nWhat's the updated value for LoginId ? : ", strlen("\nWhat's the updated value for LoginId ? : "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_STUDENT_NEW_LOGIN message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new login_id from client!");
            return false;
        }
        strcat(readBuffer, "-");
        // strcat(readBuffer, customer.id);

        sprintf(readBuffer, "%s%d", readBuffer, student.id);//readBuffer is used both as the destination and as part of the source for the formatted string. This means that it appends the formatted value of customer.id to the end of the existing readBuffer.

        strcpy(student.login, readBuffer);
        bzero(writeBuffer, sizeof(writeBuffer));
        sprintf(writeBuffer, "%s %s^", "\nThe new customer login  id is : ", readBuffer);

        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));

        break;
    // case 5:
    //     // UPDATE password
    //     writeBytes = write(connFD, "\nWhat's the updated value for Password ?: ", strlen("\nWhat's the updated value for Password ?: "));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ADMIN_MOD_STUDENT_PASSWORD message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    //     if (readBytes == -1)
    //     {
    //         perror("Error while getting response for student's new password from client!");
    //         return false;
    //     }

    //     strcpy(student.password, crypt(readBuffer, SALT_BAE));
    //     //crypt is a cryptographic function that takes two arguments: the string to be hashed (readBuffer in this case) and a salt value
    //     // (SALT_BAE in this case). The salt value is used to enhance security by adding randomness to the hash.
    //     //The result of the crypt function is a hashed version of readBuffer, and it's being copied into student.password using strcpy

    //     break;
    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n\nExiting..\nYou'll now be redirected to the main menu!");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    studentFileDescriptor = open("STUDENT_RECORD.txt", O_WRONLY);
    if (studentFileDescriptor == -1)
    {
        perror("Error while opening student file");
        return false;
    }
    offset = lseek(studentFileDescriptor, (studentID-1) * sizeof(struct Student), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on student record!");
        return false;
    }

    writeBytes = write(studentFileDescriptor, &student, sizeof(struct Student));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(studentFileDescriptor, F_SETLKW, &lock);

    close(studentFileDescriptor);

    writeBytes = write(connFD, "\n\nThe required modification was successfully made!\nYou'll now be redirected to the main menu!^", strlen("\n\nThe required modification was successfully made!\nYou'll now be redirected to the main menu!^"));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_STUDENT_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}
//_______________________________________________________________________________________________


//---------------------------UPDATE FACULTY ---------------------------------------------------
bool update_faculty(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Faculty faculty;

    int facultyID;

    off_t offset;//off_t is a data type often used in C and C++ programming for representing file offsets
    int lockingStatus;

    writeBytes = write(connFD, "\nEnter the ID of the faculty whose information you want to edit : ", strlen("\nEnter the ID of the faculty whose information you want to edit : "));
    if (writeBytes == -1)
    {
        perror("Error while writing message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading faculty ID from client!");
        return false;
    }

    facultyID = atoi(readBuffer);
    // printf("%s", readBuffer);

    int facultyFileDescriptor = open("FACULTY_RECORD.txt", O_RDONLY);
    if (facultyFileDescriptor == -1 || ((strcmp(readBuffer, "0") != 0) && facultyID == 0))
    {
        // faculty File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No faculty found for the given ID***\n");
        //strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    offset = lseek(facultyFileDescriptor, (facultyID-1) * sizeof(struct Faculty), SEEK_SET);
    if (errno == EINVAL)
    {
        // faculty record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No faculty found for the given ID***\n");
        //strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required faculty record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on student record!");
        return false;
    }

    readBytes = read(studentFileDescriptor, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error while reading customer record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(studentFileDescriptor, F_SETLK, &lock);

    close(studentFileDescriptor);
    if (readBytes == 0)
    {
        // student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No student could be found for the given ID***\n");
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    writeBytes = write(connFD, "Which information would you like to modify?\n1. Name 2. Age 3. Gender 4. Login ID 5. Password \nPress any other key and enter to cancel : ", strlen("Which information would you like to modify?\n1. Name 2. Age 3. Gender 4. Login ID 5. Password \nPress any other key and enter to cancel : "));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_STUDENT_MENU message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting student modification menu choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\nInvalid input!\nYou'll now be redirected to the main menu!\n^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        // UPDATE NAME
        writeBytes = write(connFD, "\nWhat's the updated value for name ? : ", strlen("What's the updated value for name ? : "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_STUDENT_NEW_NAME message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new name from client!");
            return false;
        }
        strcpy(student.name, readBuffer);
        break;
    case 2:
        // update age
        writeBytes = write(connFD, "\nWhat's the updated value for age ? ", strlen("What's the updated value for age ? "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_STUDENT_NEW_AGE message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new age from client!");
            return false;
        }
        int updatedAge = atoi(readBuffer);
        if (updatedAge < 18 || updatedAge > 100)
        {
            // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, "Invalid input!\nYou'll now be redirected to the main menu!\n^");
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
                return false;
            }
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
            return false;
        }
        student.age = updatedAge;
        break;
    case 3:
        // update gender
        writeBytes = write(connFD, "\nWhat's the updated value for gender ? ", strlen("What's the updated value for gender ? "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));

        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new gender from client!");
            return false;
        }
        if (readBuffer[0] != 'M' && readBuffer[0] != 'F' && readBuffer[0] != 'O' && readBuffer[0] != 'm' && readBuffer[0] != 'f' && readBuffer[0] != 'o')
        {
            perror("Error while getting response for student's new gender from client!");
            bzero(writeBuffer, sizeof(writeBuffer));
            sprintf(writeBuffer, "%s^", "\nEnter a valid character for gender.\n");

            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            bzero(writeBuffer, sizeof(writeBuffer));
            return false;
        }
        student.gender = toupper(readBuffer[0]);
        break;

    case 4:
        // UPDATE login id
        writeBytes = write(connFD, "\nWhat's the updated value for LoginId ? : ", strlen("\nWhat's the updated value for LoginId ? : "));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_STUDENT_NEW_LOGIN message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for student's new login_id from client!");
            return false;
        }
        strcat(readBuffer, "-");
        // strcat(readBuffer, customer.id);

        sprintf(readBuffer, "%s%d", readBuffer, student.id);//readBuffer is used both as the destination and as part of the source for the formatted string. This means that it appends the formatted value of customer.id to the end of the existing readBuffer.

        strcpy(student.login, readBuffer);
        bzero(writeBuffer, sizeof(writeBuffer));
        sprintf(writeBuffer, "%s %s^", "\nThe new customer login  id is : ", readBuffer);

        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        bzero(writeBuffer, sizeof(writeBuffer));

        break;
    // case 5:
    //     // UPDATE password
    //     writeBytes = write(connFD, "\nWhat's the updated value for Password ?: ", strlen("\nWhat's the updated value for Password ?: "));
    //     if (writeBytes == -1)
    //     {
    //         perror("Error while writing ADMIN_MOD_STUDENT_PASSWORD message to client!");
    //         return false;
    //     }
    //     readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    //     if (readBytes == -1)
    //     {
    //         perror("Error while getting response for student's new password from client!");
    //         return false;
    //     }

    //     strcpy(student.password, crypt(readBuffer, SALT_BAE));
    //     //crypt is a cryptographic function that takes two arguments: the string to be hashed (readBuffer in this case) and a salt value
    //     // (SALT_BAE in this case). The salt value is used to enhance security by adding randomness to the hash.
    //     //The result of the crypt function is a hashed version of readBuffer, and it's being copied into student.password using strcpy

    //     break;
    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n\nExiting..\nYou'll now be redirected to the main menu!");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    studentFileDescriptor = open("STUDENT_RECORD.txt", O_WRONLY);
    if (studentFileDescriptor == -1)
    {
        perror("Error while opening student file");
        return false;
    }
    offset = lseek(studentFileDescriptor, (studentID-1) * sizeof(struct Student), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on student record!");
        return false;
    }

    writeBytes = write(studentFileDescriptor, &student, sizeof(struct Student));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(studentFileDescriptor, F_SETLKW, &lock);

    close(studentFileDescriptor);

    writeBytes = write(connFD, "\n\nThe required modification was successfully made!\nYou'll now be redirected to the main menu!^", strlen("\n\nThe required modification was successfully made!\nYou'll now be redirected to the main menu!^"));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_STUDENT_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}
//_______________________________________________________________________________________________
