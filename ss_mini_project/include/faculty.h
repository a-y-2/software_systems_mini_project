//function declaration

bool faculty_operation_handler(int connFD);
bool add_course(int connFD,int id);
bool remove_course(int connFD,int id);
bool view_enrollments(int connFD,int id);
bool change_password(int connFD,int id);

bool faculty_operation_handler(int connFD)
{
    ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
    char readBuffer[1000], writeBuffer[1000]; 
    bzero(writeBuffer,sizeof(writeBuffer));
    int flag=1,id;
    while(flag==1){
        id = faculty_login_handler(connFD, 2);
        if(id!=0)
            flag=0;
    }
    
    if(id!=0){
        while(1){
            strcat(writeBuffer, "\n\n------------------------Faculty Menu-------------------------\n");
            strcat(writeBuffer, "\n1. Add new course\n2. Remove offered course\n3. View enrollments in courses\n4. Change password\n5. exit\n");
            strcat(writeBuffer, "\n------------------------------------------------------------\nEnter your choice : ");


            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            
            if (writeBytes == -1)
            {
                perror("Error while writing FACULTY_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));//client has seen the above menu and sent the choice
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for FACULTY_MENU");
                return false;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                add_course(connFD,id);
                break;

            case 2:
                remove_course(connFD,id);
                break;
            case 3:
                view_enrollments(connFD,id);
                break;

            case 4:
                change_password(connFD,id);
                break;

            default:
                writeBytes = write(connFD, "\nLogging you out!$", strlen("\nLogging you out!$"));
                return false;
            }
        }
            
    }
    
        
    
    else
    {
        //LOGIN FAILED
        bzero(writeBuffer, sizeof(writeBuffer));
        sprintf(writeBuffer, "%s\n", "login failed$");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        return false;
    }
    return true;
}

//=================================================ADD NEW COURSE===============================================
bool add_course(int connFD,int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course course;

    while(1){
        sprintf(writeBuffer, "%s%s", "\nEnter the details : \n\n", "course name - ");
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
            perror("Error reading course name response from client!");
            
            return -2;
        }

        strcpy(course.courseName, readBuffer);

        bzero(writeBuffer,sizeof(writeBuffer));
        strcpy(writeBuffer, "\nEnter course id ? : ");

        writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error writing message to client!");
            return -2;
        }

        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading course id response from client!");
            
            return -2;
        }

        int c_id = atoi(readBuffer);
        course.courseID = c_id;

        struct Faculty faculty;

        int fd1  = open("FACULTY_RECORD.txt",O_RDONLY);

        if (fd1 == -1)
        {
            perror("Error while opening faculty file");
            return -2;
        }
        
        else{
        

        // struct flock lock ;
        // lock.l_type = F_RDLCK;
        // lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
        // lock.l_start = (id-1)*sizeof(faculty);//Set the starting position for the lock. 
        // lock.l_len = sizeof(faculty);//lock covers the entire faculty record
        // lock.l_pid = getpid();   
    
        // //set lock on file    
        // fcntl(fd1, F_SETLKW, &lock);
        //new_stu.id++;

       //entering critical section to add faculty 
            lseek(fd1,(id-1)*sizeof(struct Faculty),SEEK_SET);
            readBytes = read(fd1, &faculty, sizeof(struct Faculty));
            if (readBytes == -1)
            {
                perror("Error while reading faculty record from file!");
                return -2;
            }

            strcpy(course.faculty_name,faculty.name);

    //     lock.l_type = F_UNLCK;
    //     fcntl(fd1,F_SETLK,&lock);
    //     printf("\n student details added");
    //     close(fd1);
    // }
    
        // bzero(writeBuffer,sizeof(writeBuffer));
        // strcpy(writeBuffer, "\nEnter your name ? : ");
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
        //     perror("Error reading faculty name response from client!");
        //     return -2;
        // }

        // strcpy(course.faculty_name,readBuffer);

            bzero(writeBuffer,sizeof(writeBuffer));
            strcpy(writeBuffer, "\nEnter maximum no. of seats ? : ");
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
                perror("Error reading max_seat response from client!");
                return -2;
            }

            int seat = atoi(readBuffer);
            course.max_seats = seat;
            course.seats_left = seat;

            int fd1  = open("COURSE.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

            if (fd1 == -1)
            {
                perror("Error while opening COURSE file");
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

                
                write(fd1,&course,sizeof(course));

                lock.l_type = F_UNLCK;
                fcntl(fd1,F_SETLK,&lock);
                printf("\n details added");
                close(fd1);

                }

            bzero(writeBuffer,sizeof(writeBuffer));
            strcpy(writeBuffer, "\nDO YOU WANT TO ADD MORE COURSES ( NO or YES) ? : ");

            writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error writing message to client!");
                return -2;
            }

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error reading course id response from client!");
                
                return -2;
            }

            // int choice = atoi(readBuffer);
            // if(choice==0)
            if((strcmp(readBuffer,"No")==0)||(strcmp(readBuffer,"NO")==0)||(strcmp(readBuffer,"no")==0))
                break;
    } }     
    return true;
}

//===========================================REMOVE COURSE========================================================
bool remove_course(int connFD,int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course new_course,temp_course;

    // while(1){

    // }
  
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

    int c_id = atoi(readBuffer);
    new_course.courseID = c_id;
    printf("%d", new_course.courseID);

    struct Faculty faculty;

    int fd1  = open("FACULTY_RECORD.txt",O_RDONLY);

    if (fd1 == -1)
    {
        perror("Error while opening faculty file");
        return -2;
    }
    
    else{
        lseek(fd1,(id-1)*sizeof(struct Faculty),SEEK_SET);
        readBytes = read(fd1, &faculty, sizeof(struct Faculty));
        if (readBytes == -1)
        {
            perror("Error while reading faculty record from file!");
            return -2;
        }

        strcpy(new_course.faculty_name,faculty.name);
        printf("%s", new_course.faculty_name);
    }

    // bzero(writeBuffer,sizeof(writeBuffer));
    // strcpy(writeBuffer, "\nEnter faculty name ? : ");
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
    //     perror("Error reading course id response from client!");
    //     return -2;
    // }

    // strcpy(new_course.courseName,readBuffer);

    //int fd  = open("COURSE.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    int fd  = open("COURSE.txt",O_RDONLY);


    if (fd == -1)
    {
        perror("Error while opening course file");
        return -2;
    }

    struct flock lock ;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
    lock.l_start = 0;//Set the starting position for the lock. 
    lock.l_len = 0;//lock covers the entire faculty record
    lock.l_pid = getpid();
   

    //set lock on file    
    fcntl(fd, F_SETLKW, &lock);

    //Create a temporary file for writing
    int tempFile = creat("tempfile.txt", 0644);
    if (tempFile == -1) {
        perror("Error creating the temporary file");
        //close(originalFile);
        exit(1);
    }

    // Read records from the original file and write them to the temporary file
    ssize_t bytesRead;
    //bytesRead = read(fd, &temp_course, sizeof(struct Course));
    
    while (((bytesRead = read(fd, &temp_course, sizeof(struct Course))) > 0)) {
        if (!((temp_course.courseID == new_course.courseID) &&
            (strcmp(temp_course.faculty_name, new_course.faculty_name) == 0))){
            
            write(tempFile, &temp_course, sizeof(struct Course));
        }
        
    }

    close(fd);
    close(tempFile);
    int value  = rename("tempfile.txt","COURSE.txt");

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLK,&lock);
    printf("\n removed successfully!!");

    return true;
}

bool view_enrollments(int connFD,int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Enrollment new_enrol,temp_enrol;// structure for storing faculty id and course id
  
    bzero(writeBuffer,sizeof(writeBuffer));

    struct Faculty faculty;
    
    int fd2 = open("FACULTY_RECORD.txt", O_RDONLY);
    if (fd2 == -1)
    {
        perror("Error while opening COURSE file");
        return -2;
    }
          

    struct flock lock = {F_RDLCK, SEEK_SET, 0, 0, getpid()};
    int lockingStatus = fcntl(fd2, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error obtaining read lock on faculty record!");
        return -2;
    }

    ssize_t bytes_read;
    // Read and send the file's content to the client
    while ((bytes_read = read(fd2, &faculty, sizeof(struct Faculty))) > 0) {
        if(id==faculty.id){
            strcpy(new_enrol.faculty_name,faculty.name); 
            break;
        }
            
    }
    int fd1  = open("ENROLLMENTS.txt",O_RDONLY);

    if (fd1 == -1)
    {
        perror("Error while opening teach file");
        return -2;
    }

    struct flock lock1 ;
    lock1.l_type = F_RDLCK;
    lock1.l_whence = SEEK_SET;//start position of the lock (lock.l_start) is relative to the beginning of the file.
    lock1.l_start = 0;//Set the starting position for the lock. 
    lock1.l_len = 0;//lock covers the entire record
    lock1.l_pid = getpid();   

    //set lock on file    
    fcntl(fd1, F_SETLKW, &lock1);

    // int fd2  = open("temp.txt",O_CREAT | O_APPEND | O_WRONLY,0777);

    int bytesRead;
    int bytesToRead = sizeof(struct Enrollment);
    int bytesToWrite = 0;

    // Read and write records, excluding the one to be deleted
    while ((bytesRead = read(fd1, &temp_enrol, bytesToRead)) > 0) {
        //printf("%d \n",temp_enrol.studentid);
        if (strcmp(temp_enrol.faculty_name,new_enrol.faculty_name)) {
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

    lock1.l_type = F_UNLCK;
    fcntl(fd1,F_SETLK,&lock1);
    printf("\n displayed successfully!!");
    close(fd1);

    return true;
}

bool change_password(int connFD , int id){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Faculty faculty;
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

    int fd  = open("FACULTY_RECORD.txt",O_RDONLY);
    if (fd == -1)
    {
        perror("Error while opening FACULTY file");
        return -2;
    }

    int offset = lseek(fd, (id-1) * sizeof(struct Faculty), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};

    // Lock the record to be read
    int lockingStatus = fcntl(fd, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on record!");
        return false;
    }

    readBytes = read(fd, &faculty, sizeof(struct Faculty));
    if (readBytes == -1)
    {
        perror("Error while reading faculty record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);

    close(fd);

    strcpy(faculty.password,pwd);

    int studentFileDescriptor = open("FACULTY_RECORD.txt", O_WRONLY);
    if (studentFileDescriptor == -1)
    {
        perror("Error while opening faculty file");
        return false;
    }
    offset = lseek(studentFileDescriptor, (id-1) * sizeof(struct Faculty), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required record!");
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

    writeBytes = write(studentFileDescriptor, &faculty, sizeof(struct Faculty));
    if (writeBytes == -1)
    {
        perror("Error while writing update faculty info into file");
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