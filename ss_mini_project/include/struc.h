struct Student
{    
    bool status;//0,1
    char name[25];
    char gender; 
    int age;
    // Login Credentials
    int id; // 0, 1, 2 ....
    char password[30]; 
};

struct Faculty
{   
    char name[25];
    // Login Credentials
    int id; // 0, 1, 2 ....
    char password[30];
};

struct Enrollment {
    int studentid;
    //char studentName[100];
    int courseid;
    //char courseName[100];
    //int status;
    char faculty_name[50];
    //char enrollmentDate[12];
};

struct Course {
    int courseID;
    char courseName[100];
    int max_seats;
    char faculty_name[100];
    int seats_left;
    
};

struct Teach {
    int faculty_id;
    int course_id;
    char fac_name[100];
    char course_name[100];
};