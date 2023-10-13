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
bool view_courses(int connFD);

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
            case 3:
                view_courses(connFD);
                break;

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
    struct Teach teach;
    //firstly display teach file contents to the student so that he can choose which courses are being offered

    int fd = open("TEACH.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("Error while opening TEACH file");
        return -2;
    }
          

    struct flock lock = {F_RDLCK, SEEK_SET, 0, 0, getpid()};
    int lockingStatus = fcntl(fd, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error obtaining read lock on TEACH record!");
        return -2;
    }

    ssize_t bytes_read;
    // Read and send the file's content to the client
    while ((bytes_read = read(fd, &teach, sizeof(struct Teach))) > 0) {
    bzero(writeBuffer,sizeof(writeBuffer));
    
    sprintf(writeBuffer, "%d %d %s %s", teach.faculty_id,teach.course_id,teach.fac_name,teach.course_name);
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1) {
        perror("Error writing message to client!");
        return -2;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    
}

// Dummy read on connFD
//readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    // Close the file
    close(fd);

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
    enrol.courseid = courseID;

    int fd1  = open("ENROLLMENTS.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    if (fd1 == -1)
    {
        perror("Error while opening enrol file");
        return -2;
    }
    
    else{
        

        struct flock lock ;
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
        lock.l_start = 0;//Set the starting position for the lock. 
        lock.l_len = 0;//lock covers the entire record
        lock.l_pid = getpid();   
    
        //set lock on file    
        fcntl(fd1, F_SETLKW, &lock);
        //new_stu.id++;

          
        write(fd1,&enrol,sizeof(enrol));

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

    struct Enrollment new_enrol,temp_enrol;
  
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
    new_enrol.studentid = studentID;

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
    new_enrol.courseid = courseID;

    int fd1  = open("ENROLLMENTS.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    if (fd1 == -1)
    {
        perror("Error while opening enrol file");
        return -2;
    }

    struct flock lock ;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
    lock.l_start = 0;//Set the starting position for the lock. 
    lock.l_len = 0;//lock covers the entire faculty record
    lock.l_pid = getpid();   

    //set lock on file    
    fcntl(fd1, F_SETLKW, &lock);

    // int fd2  = open("temp.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    int bytesRead;
    int bytesToRead = sizeof(struct Enrollment);
    int bytesToWrite = 0;

    // Read and write records, excluding the one to be deleted
    while ((bytesRead = read(fd1, &temp_enrol, bytesToRead)) > 0) {
        if ((temp_enrol.studentid == new_enrol.studentid)&&(temp_enrol.courseid == new_enrol.courseid)) {
            // Skip this record
            lseek(fd1, -bytesToRead, SEEK_CUR);
        } else {
            // Write the record back to the file
            write(fd1, &temp_enrol, bytesToRead);
            bytesToWrite += bytesRead;
        }
    }

    // Truncate the file to the new size
    ftruncate(fd1, bytesToWrite);

    lock.l_type = F_UNLCK;
    fcntl(fd1,F_SETLK,&lock);
    printf("\n unenrolled successfully!!");

    close(fd1);

   

    return true;
}

//=======================================VIEW ENROLLED COURSES==============================================================
bool view_courses(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Enrollment new_enrol,temp_enrol;// structure for storing faculty id and course id
  
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
    new_enrol.studentid = studentID;

    int fd1  = open("ENROLLMENTS.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    if (fd1 == -1)
    {
        perror("Error while opening teach file");
        return -2;
    }

    struct flock lock ;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
    lock.l_start = 0;//Set the starting position for the lock. 
    lock.l_len = 0;//lock covers the entire record
    lock.l_pid = getpid();   

    //set lock on file    
    fcntl(fd1, F_SETLKW, &lock);

    // int fd2  = open("temp.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    int bytesRead;
    int bytesToRead = sizeof(struct Enrollment);
    int bytesToWrite = 0;

    // Read and write records, excluding the one to be deleted
    while ((bytesRead = read(fd1, &temp_enrol, bytesToRead)) > 0) {
        if (temp_enrol.studentid == new_enrol.studentid) {
            bzero(writeBuffer,sizeof(writeBuffer));    
            sprintf(writeBuffer, "%d %d ", temp_enrol.studentid,temp_enrol.courseid);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1) {
                perror("Error writing message to client!");
                return -2;
            }
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd1,F_SETLK,&lock);
    printf("\n displayed successfully!!");
    close(fd1);
    return true;
}
