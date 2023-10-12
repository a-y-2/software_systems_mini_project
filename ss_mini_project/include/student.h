// #include <stdbool.h>
// #include "./struc.h"
// #include<sys/types.h>
// #include<fcntl.h>
// #include<sys/stat.h>
// #include<stdio.h>
// #include<errno.h>
// #include<time.h>
// #include<unistd.h>
// #include<ctype.h>
// #include<crypt.h>

//#include "./common.h"

//function declaration
bool student_operation_handler(int connFD);
bool enroll(int connFD);
bool unenroll(int connFD);

bool student_operation_handler(int connFD)
{
    ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
    char readBuffer[1000], writeBuffer[1000]; 

    strcat(writeBuffer, "\n\n------------------------Student Menu-------------------------\n");
            strcat(writeBuffer, "\n1. enroll to new courses\n2. unenroll from a course\n3. view enrolled courses\n4. password change\n5. exit\n");
            strcat(writeBuffer, "\n------------------------------------------------------------\nEnter your choice : ");


            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            
            if (writeBytes == -1)
            {
                perror("Error while writing STUDENT_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));//client has seen the above menu and sent the choice
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for STUDENT_MENU");
                return false;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                enroll(connFD);
                break;

            case 2:
                unenroll(connFD);
                break;
            // case 3:
            //     view_courses(connFD);
            //     break;

            // case 4:
            //     update_password(connFD);
            //     break;

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

//enroll
bool enroll(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Enrollment enrol;// structure for storing student id and course id
  
    bzero(writeBuffer,sizeof(writeBuffer));
    strcpy(writeBuffer, "\nEnter your id ? : ");
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
        perror("Error reading student id response from client!");
        return -2;
    }

    int studentID = atoi(readBuffer);
    enrol.studentid = studentID;

    bzero(writeBuffer,sizeof(writeBuffer));
    strcpy(writeBuffer, "\nEnter COURSE id ? : ");
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
        perror("Error reading course id response from client!");
        return -2;
    }

    int courseID = atoi(readBuffer);
    new_teach.course_id = courseID;

    int fd1  = open("TEACH.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    if (fd1 == -1)
    {
        perror("Error while opening teach file");
        return -2;
    }
    
    else{
        

        struct flock lock ;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
        lock.l_start = 0;//Set the starting position for the lock. 
        lock.l_len = 0;//lock covers the entire faculty record
        lock.l_pid = getpid();   
    
        //set lock on file    
        fcntl(fd1, F_SETLKW, &lock);
        //new_stu.id++;

          
        write(fd1,&new_teach,sizeof(new_teach));

        lock.l_type = F_UNLCK;
        fcntl(fd1,F_SETLK,&lock);
        printf("\n details added");
        close(fd1);
    }

   

    return true;
}

bool unenroll(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Enrollment enrol;

    int enrol_id;

    off_t offset;//off_t is a data type often used in C and C++ programming for representing file offsets
    int lockingStatus;

    writeBytes = write(connFD, "\n enter your enrollment ID : ", strlen("\n enter your enrollment ID : "));
    if (writeBytes == -1)
    {
        perror("Error while writing message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading enrollment ID from client!");
        return false;
    }

    enrol_id = atoi(readBuffer);
    // printf("%s", readBuffer);

    int enrolFileDescriptor = open("ENROLLMENTS.csv", O_RDONLY);
    if (enrolFileDescriptor == -1)
    {
        // enrol File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***error in opening file***\n");
        //strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing error message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    offset = lseek(enrolFileDescriptor, (enrol_id-1) * sizeof(struct Enrollment), SEEK_SET);
    if (errno == EINVAL)
    {
        // record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No enrollment found for the given ID***\n");
        //strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required enrollment record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Enrollment), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(enrolFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on enrollment record!");
        return false;
    }

    readBytes = read(enrolFileDescriptor, &enrol, sizeof(struct Enrollment));
    if (readBytes == -1)
    {
        perror("Error while reading enrollment record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(enrolFileDescriptor, F_SETLK, &lock);

    close(enrolFileDescriptor);
    if (readBytes == 0)
    {
        // record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, "\n***No enrollment could be found for the given ID***\n");
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    
    enrol.status = 0;

    int fd1  = open("ENROLLMENTS.csv",O_CREAT | O_APPEND | O_WRONLY,0777);

    if (fd1 == -1)
    {
        perror("Error while opening enrollments file");
        return -2;
    }
    
    else{
        

        struct flock lock ;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
        lock.l_start = (enrol.id-1)*sizeof(enrol);//Set the starting position for the lock. 
        lock.l_len = sizeof(enrol);//lock covers the entire record
        lock.l_pid = getpid();   
    
        //set lock on file    
        fcntl(fd1, F_SETLKW, &lock);
        //new_fac.id++;

       //entering critical section to add faculty 
        lseek(fd1,(enrol.id-1)*sizeof(enrol),SEEK_SET);
        write(fd1,&enrol,sizeof(enrol));

        lock.l_type = F_UNLCK;
        fcntl(fd1,F_SETLK,&lock);
        printf("\n unenrolled successfully");
        close(fd1);
    }

    return true;


}