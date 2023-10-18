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
bool enroll(int connFD,int id);
bool unenroll(int connFD,int id);
bool view_courses(int connFD,int id);
bool update_password(int connFD,int id);

bool student_operation_handler(int connFD)
{
    ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
    char readBuffer[1000], writeBuffer[1000]; 
    bzero(writeBuffer,sizeof(writeBuffer));
    int flag=1,id;
    while(flag==1){
        id = student_login_handler(connFD, 3);
        if(id!=0)
            flag=0;
    }
    
    if(id!=0){
        //ALLOW THE STUDENT TO DO ANYTHING ONLY IF THE STATUS IS SET TO 1
        struct Student student;

        int fd  = open("STUDENT_RECORD.txt",O_RDONLY);
        if (fd == -1)
        {
            perror("Error while opening STUDENT file");
            return -2;
        }

        int offset = lseek(fd, (id-1) * sizeof(struct Student), SEEK_SET);
        if (offset == -1)
        {
            perror("Error while seeking to required record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};

        // Lock the record to be read
        int lockingStatus = fcntl(fd, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Couldn't obtain lock on record!");
            return false;
        }

        readBytes = read(fd, &student, sizeof(struct Student));
        if (readBytes == -1)
        {
            perror("Error while reading student record from the file!");
            return false;
        }

        // Unlock the record
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);

        close(fd);
        if(student.status==1){
             while(1){
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
                    enroll(connFD,id);
                    break;

                case 2:
                    unenroll(connFD,id);
                    break;
                case 3:
                    view_courses(connFD,id);
                    break;

                case 4:
                    update_password(connFD,id);
                    break;

                
                default:
                    writeBytes = write(connFD, "\nLogging you out!$", strlen("\nLogging you out!$"));
                    return false;
                }
        }
            
        }
        else{
            strcat(writeBuffer, "\nNOT ALLOWED !!!!! YOU ARE BLOCKED : ");
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));            
            if (writeBytes == -1)
            {
                perror("Error while writing to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));//dummy read
            if (readBytes == -1)
            {
                perror("Error while reading");
                return false;
            }
        }
       
    }
    
    else
    {
        //LOGIN FAILED
        bzero(writeBuffer, sizeof(writeBuffer));
        sprintf(writeBuffer, "%s\n", "No account found!!$");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        return false;
    }
    return true;
}

//enroll
bool enroll(int connFD,int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Enrollment enrol;// structure for storing student id and course id
    struct Course course;
    //firstly display course file contents to the student so that he can choose which courses are being offered

    int fd = open("COURSE.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("Error while opening COURSE file");
        return -2;
    }
          

    struct flock lock2 = {F_RDLCK, SEEK_SET, 0, 0, getpid()};
    int lockingStatus = fcntl(fd, F_SETLKW, &lock2);
    if (lockingStatus == -1)
    {
        perror("Error obtaining read lock on COURSE record!");
        return -2;
    }

    ssize_t bytes_read;
    // Read and send the file's content to the client
    while ((bytes_read = read(fd, &course, sizeof(struct Course))) > 0) {
    bzero(writeBuffer,sizeof(writeBuffer));    
    sprintf(writeBuffer, "%d %s %d %s %d", course.courseID,course.courseName,course.max_seats,course.faculty_name,course.seats_left);
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1) {
        perror("Error writing message to client!");
        return -2;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    
}

// Dummy read on connFD
//readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    

    lock2.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock2);

    // Close the file
    close(fd);

    // bzero(writeBuffer,sizeof(writeBuffer));
    // strcpy(writeBuffer, "\nEnter your id ? : ");
    // writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    // if (writeBytes == -1)
    // {
    //     perror("Error writing message to client!");
    //     return -2;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error reading student id response from client!");
    //     return -2;
    // }

    // int studentID = atoi(readBuffer);
    // enrol.studentid = studentID;

    enrol.studentid = id;

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

    int fd2 = open("COURSE.txt", O_RDONLY);
    if (fd2 == -1)
    {
        perror("Error while opening COURSE file");
        return -2;
    }
          

    struct flock lock1 = {F_RDLCK, SEEK_SET, 0, 0, getpid()};
    int lockingStatus1 = fcntl(fd2, F_SETLKW, &lock1);
    if (lockingStatus1 == -1)
    {
        perror("Error obtaining read lock on COURSE record!");
        return -2;
    }

    ssize_t bytes_read1;
    // Read and send the file's content to the client
    while ((bytes_read1 = read(fd2, &course, sizeof(struct Course))) > 0) {
        if(courseID==course.courseID){
            course.seats_left--;
            if(course.seats_left>-1){
                strcpy(enrol.faculty_name,course.faculty_name); 
                break;
            }
                
            else{
                bzero(writeBuffer,sizeof(writeBuffer));
                strcpy(writeBuffer, "\nno seat left !!! : ");
                writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
                if (writeBytes == -1)
                {
                    perror("Error writing message to client!");
                    return -2;
                }

                bzero(readBuffer, sizeof(readBuffer));
                readBytes = read(connFD, readBuffer, sizeof(readBuffer));//dummy read
                if (readBytes == -1)
                {
                    perror("dummy read .");
                    return false;
                }
            }
            
        }
            
    }

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

bool unenroll(int connFD,int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Enrollment new_enrol,temp_enrol;
  
    bzero(writeBuffer,sizeof(writeBuffer));
   
    new_enrol.studentid = id;

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

    int fd1  = open("ENROLLMENTS.txt",O_RDONLY);

    if (fd1 == -1)
    {
        perror("Error while opening enrol file");
        return -2;
    }

    struct flock lock ;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
    lock.l_start = 0;//Set the starting position for the lock. 
    lock.l_len = 0;//lock covers the entire faculty record
    lock.l_pid = getpid();   

    //set lock on file    
    fcntl(fd1, F_SETLKW, &lock);

    //Create a temporary file for writing
    int tempFile = creat("tempfile.txt", 0644);
    if (tempFile == -1) {
        perror("Error creating the temporary file");
        //close(originalFile);
        exit(1);
    }

    // Read records from the original file and write them to the temporary file
    ssize_t bytesRead;
    //bytesRead = read(fd1, &temp_enrol, sizeof(struct Enrollment));
    
    while (((bytesRead = read(fd1, &temp_enrol, sizeof(struct Enrollment))) > 0)) {
        if (!((temp_enrol.courseid == new_enrol.courseid) &&
            (temp_enrol.studentid, new_enrol.studentid))){
            
            write(tempFile, &temp_enrol, sizeof(struct Enrollment));
        }
        
    }

    close(fd1);
    close(tempFile);
    int value  = rename("tempfile.txt","ENROLLMENT.txt");

    lock.l_type = F_UNLCK;
    fcntl(fd1,F_SETLK,&lock);
    printf("\n unenrolled successfully!!");

    return true;
}

//=======================================VIEW ENROLLED COURSES==============================================================
bool view_courses(int connFD,int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Enrollment new_enrol,temp_enrol;// structure for storing faculty id and course id
  
    bzero(writeBuffer,sizeof(writeBuffer));
    // strcpy(writeBuffer, "\nEnter your id ? : ");
    // writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    // if (writeBytes == -1)
    // {
    //     perror("Error writing message to client!");
    //     return -2;
    // }

    // bzero(readBuffer, sizeof(readBuffer));
    // readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    // if (readBytes == -1)
    // {
    //     perror("Error reading student id response from client!");
    //     return -2;
    // }

    // int studentID = atoi(readBuffer);
    // new_enrol.studentid = studentID;

    new_enrol.studentid = id;

    int fd1  = open("ENROLLMENTS.txt",O_RDONLY);

    if (fd1 == -1)
    {
        perror("Error while opening teach file");
        return -2;
    }

    struct flock lock ;
    lock.l_type = F_RDLCK;
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
        //printf("%d \n",temp_enrol.studentid);
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

bool update_password(int connFD , int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Student student;
    char pwd[30];

    writeBytes = write(connFD, "\nEnter new password : ", strlen("\nEnter new password : "));
    if (writeBytes == -1)
    {
        perror("Error while writing message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading password from client!");
        return false;
    }

    strcpy(pwd,readBuffer);

    int fd  = open("STUDENT_RECORD.txt",O_RDONLY);
    if (fd == -1)
    {
        perror("Error while opening course file");
        return -2;
    }

    int offset = lseek(fd, (id-1) * sizeof(struct Student), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required student record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};

    // Lock the record to be read
    int lockingStatus = fcntl(fd, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on student record!");
        return false;
    }

    readBytes = read(fd, &student, sizeof(struct Student));
    if (readBytes == -1)
    {
        perror("Error while reading student record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    close(fd);

    strcpy(student.password,pwd);

    int studentFileDescriptor = open("STUDENT_RECORD.txt", O_WRONLY);
    if (studentFileDescriptor == -1)
    {
        perror("Error while opening student file");
        return false;
    }
    offset = lseek(studentFileDescriptor, (id-1) * sizeof(struct Student), SEEK_SET);
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