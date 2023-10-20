# software_systems_mini_project
# COURSE REGISTRATION PORTAL

Key features include:
Storage: All details are stored in files.
Login System: Administrator , students and faculty must pass through a login system to access their accounts.
Account holders have to pass through a login system to enter their accounts, and all these User accounts will be managed by the Administrator.
User Types: Three login types are available: faculty, student , and administrator.
* Once the Admin connects to the server, He/She gets a login and password prompt.
After successful login, He/She gets menu:
Do you want to:
- Add Student
- Add Faculty
- Activate/Deactivate Student
- Update Student/Faculty details
- Exit
* Once the Student connects to the server, He/She gets a login and password prompt.
After successful login, He/She gets menu for example:
Do you want to:
- Enroll to new Courses
- Unenroll from already enrolled Courses
- View enrolled Courses
- Password Change
- Exit
* Once the Faculty connects to the server, He/She gets a login and password prompt.
After successful login, He/She gets menu for example:
Do you want to:
- Add new Course
- Remove offered Course
- View enrollments in Courses
- Password Change
- Exit
(Assumption: Course will have a limited number of seats.) 
Socket Programming: The server maintains a database and serves multiple clients concurrently, allowing clients to connect and access their specific account details.
System Calls: System calls are used instead of library functions for process management, file management, file locking, etc.
