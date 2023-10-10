struct Student
{
    int id; // 0, 1, 2 ....
    bool status;//0,1
    char name[25];
    char gender; // M -> Male, F -> Female, O -> Other
    int age;
    // Login Credentials
    char login[30]; // Format : name-id (name will the first word in the structure member `name`)
    char password[30];
    // // Bank data
    // int account; // Account number of the account the student owns
    // bool isAccountholder;
};

struct Faculty
{
    int id; // 0, 1, 2 ....
    char name[25];
    char gender; // M -> Male, F -> Female, O -> Other
    int age;
    char course[25];
    // Login Credentials
    char login[30]; // Format : name-id (name will the first word in the structure member `name`)
    char password[30];
    // // Bank data
    // int account; // Account number of the account the Faculty owns
    // bool isAccountholder;
};