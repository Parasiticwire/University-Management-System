#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<map>
using namespace std;

//==========Student Class==========
class Student {
private:
    string name;
    string studentID;
    string email;
    vector<string> enrolledCourses;
public:
    Student() : name(""), studentID(""), email("") {}
    Student(string n, string id, string e) : name(n), studentID(id), email(e) {}

    string getName() const { return name; }
    string getID() const { return studentID; }
    string getEmail() const { return email; }

    void setName(string n) { name = n; }
    void setEmail(string e) { email = e; }

    void enrollCourse(const string& course) {
        if (course.empty()) return;
        if (find(enrolledCourses.begin(), enrolledCourses.end(), course) == enrolledCourses.end())
            enrolledCourses.push_back(course);
    }

    void dropCourse(const string& course) {
        auto it = find(enrolledCourses.begin(), enrolledCourses.end(), course);
        if (it != enrolledCourses.end()) enrolledCourses.erase(it);
    }

    void viewCourses() const {
        cout << "Courses enrolled by " << name << ": ";
        for (const auto& course : enrolledCourses) cout << course << " ";
        cout << endl;
    }

    void displayInfo() const {
        cout << "Name: " << name << ", ID: " << studentID << ", Email: " << email << endl;
        viewCourses();
    }

    void saveToFile(ofstream& out) const {
        out << name << "," << studentID << "," << email;
        for (const auto& course : enrolledCourses) out << "," << course;
        out << endl;
    }

    static Student loadFromString(const string& line) {
        stringstream ss(line);
        string name, id, email, token;
        getline(ss, name, ',');
        getline(ss, id, ',');
        getline(ss, email, ',');
        Student s(name, id, email);
        while (getline(ss, token, ',')) s.enrollCourse(token);
        return s;
    }
};

//==========Teacher Class==========
class Teacher {
private:
    string name;
    string teacherID;
    string email;
    vector<string> coursesTaught;
public:
    Teacher() : name(""), teacherID(""), email("") {}
    Teacher(string n, string id, string e) : name(n), teacherID(id), email(e) {}

    string getName() const { return name; }
    string getID() const { return teacherID; }
    string getEmail() const { return email; }

    void assignCourse(const string& course) {
        if (!course.empty() && find(coursesTaught.begin(), coursesTaught.end(), course) == coursesTaught.end())
            coursesTaught.push_back(course);
    }

    void viewCourses() const {
        cout << "Courses taught by " << name << ": ";
        for (const auto& course : coursesTaught) cout << course << " ";
        cout << endl;
    }

    void displayInfo() const {
        cout << "Name: " << name << ", ID: " << teacherID << ", Email: " << email << endl;
        viewCourses();
    }

    void saveToFile(ofstream& out) const {
        out << name << "," << teacherID << "," << email;
        for (const auto& course : coursesTaught) out << "," << course;
        out << endl;
    }

    static Teacher loadFromString(const string& line) {
        stringstream ss(line);
        string name, id, email, token;
        getline(ss, name, ',');
        getline(ss, id, ',');
        getline(ss, email, ',');
        Teacher t(name, id, email);
        while (getline(ss, token, ',')) t.assignCourse(token);
        return t;
    }
};

//==========Course Class==========
class Course {
private:
    string courseName;
    string courseCode;
    Teacher teacher;
    int maxCapacity;
    vector<Student> enrolledStudents;
public:
    Course() : courseName(""), courseCode(""), teacher(), maxCapacity(0) {}
    Course(string name, string code, const Teacher& t, int cap)
        : courseName(name), courseCode(code), teacher(t), maxCapacity(cap) {
    }

    string getCourseCode() const { return courseCode; }

    bool addStudent(const Student& student) {
        if (enrolledStudents.size() >= static_cast<size_t>(maxCapacity)) return false;
        for (const auto& s : enrolledStudents)
            if (s.getID() == student.getID()) return false;
        enrolledStudents.push_back(student);
        return true;
    }

    void displayInfo() const {
        cout << "Course: " << courseName << " (" << courseCode << "), Teacher: " << teacher.getName() << endl;
        cout << "Students: ";
        for (const auto& s : enrolledStudents)
            cout << s.getName() << " ";
        cout << endl;
    }

    void saveToFile(ofstream& out) const {
        out << courseName << "," << courseCode << "," << teacher.getID() << "," << maxCapacity;
        for (const auto& s : enrolledStudents) out << "," << s.getID();
        out << endl;
    }

    static Course loadFromString(const string& line, const map<string,
        Teacher>& teacherMap, const map<string, Student>& studentMap) {
        stringstream ss(line);
        string name, code, teacherID, capStr, token;
        getline(ss, name, ',');
        getline(ss, code, ',');
        getline(ss, teacherID, ',');
        getline(ss, capStr, ',');
        int capacity = stoi(capStr);

        Teacher teacher = teacherMap.at(teacherID);
        Course c(name, code, teacher, capacity);
        while (getline(ss, token, ',')) {
            if (studentMap.count(token)) c.addStudent(studentMap.at(token));
        }
        return c;
    }
};

//==========Utility Functions==========
void saveAllData(const vector<Student>& students, const vector<Teacher>& teachers, const vector<Course>& courses) {
    ofstream sOut("students.txt"), tOut("teachers.txt"), cOut("courses.txt");
    for (const auto& s : students) s.saveToFile(sOut);
    for (const auto& t : teachers) t.saveToFile(tOut);
    for (const auto& c : courses) c.saveToFile(cOut);
}

vector<Student> loadStudents() {
    vector<Student> v;
    ifstream in("students.txt");
    string line;
    while (getline(in, line)) v.push_back(Student::loadFromString(line));
    return v;
}

vector<Teacher> loadTeachers() {
    vector<Teacher> v;
    ifstream in("teachers.txt");
    string line;
    while (getline(in, line)) v.push_back(Teacher::loadFromString(line));
    return v;
}

vector<Course> loadCourses(const map<string, Student>& studentMap, const map<string, Teacher>& teacherMap) {
    vector<Course> v;
    ifstream in("courses.txt");
    string line;
    while (getline(in, line)) {
        try { v.push_back(Course::loadFromString(line, teacherMap, studentMap)); }
        catch (...) { cerr << "Error loading a course\n"; }
    }
    return v;
}

//==========Student Management==========
void addStudent(vector<Student>& students, map<string, Student>& studentMap) {
    string name, id, email;
    cin.ignore();
    cout << "Enter name: "; getline(cin, name);
    cout << "Enter ID: "; getline(cin, id);
    cout << "Enter email: "; getline(cin, email);
    Student s(name, id, email);
    students.push_back(s);
    studentMap[id] = s;
    cout << "Student added.\n";
}

void searchStudent(const vector<Student>& students) {
    string id;
    cin.ignore();
    cout << "Enter Student ID to search: "; getline(cin, id);
    for (const auto& s : students)
        if (s.getID() == id) {
            s.displayInfo();
            return;
        }
    cout << "Student not found.\n";
}

void deleteStudent(vector<Student>& students, map<string, Student>& studentMap) {
    string id;
    cin.ignore();
    cout << "Enter Student ID to delete: "; getline(cin, id);
    auto it = remove_if(students.begin(), students.end(), [&](const Student& s) { return s.getID() == id; });
    if (it != students.end()) {
        students.erase(it, students.end());
        studentMap.erase(id);
        cout << "Student deleted.\n";
    }
    else cout << "Student not found.\n";
}

void updateStudent(vector<Student>& students, map<string, Student>& studentMap) {
    string id;
    cin.ignore();
    cout << "Enter Student ID to update: "; getline(cin, id);
    for (auto& s : students) {
        if (s.getID() == id) {
            string newName, newEmail;
            cout << "Enter new name: "; getline(cin, newName);
            cout << "Enter new email: "; getline(cin, newEmail);
            s.setName(newName);
            s.setEmail(newEmail);
            studentMap[id] = s;
            cout << "Student updated.\n";
            return;
        }
    }
    cout << "Student not found.\n";
}

//==========Main==========
int main() {
    vector<Student> students = loadStudents();
    vector<Teacher> teachers = loadTeachers();

    map<string, Student> studentMap;
    for (const auto& s : students) studentMap[s.getID()] = s;

    map<string, Teacher> teacherMap;
    for (const auto& t : teachers) teacherMap[t.getID()] = t;

    vector<Course> courses = loadCourses(studentMap, teacherMap);

    int choice;
    do {
        cout << "\n==== University Management Menu ====\n";
        cout << "1. Add Student\n";
        cout << "2. Add Teacher\n";
        cout << "3. Add Course\n";
        cout << "4. Enroll Student in Course\n";
        cout << "5. View All Students\n";
        cout << "6. View All Teachers\n";
        cout << "7. View All Courses\n";
        cout << "8. Search Student\n";
        cout << "9. Delete Student\n";
        cout << "10. Update Student\n";
        cout << "0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1: addStudent(students, studentMap); break;
        case 2: {
            string name, id, email;
            cin.ignore();
            cout << "Enter name: "; getline(cin, name);
            cout << "Enter ID: "; getline(cin, id);
            cout << "Enter email: "; getline(cin, email);
            Teacher t(name, id, email);
            teachers.push_back(t);
            teacherMap[id] = t;
            cout << "Teacher added.\n";
            break;
        }
        case 3: {
            string name, code, tid;
            int cap;
            cin.ignore();
            cout << "Enter course name: "; getline(cin, name);
            cout << "Enter course code: "; getline(cin, code);
            cout << "Enter teacher ID: "; getline(cin, tid);
            cout << "Enter capacity: "; cin >> cap;
            if (teacherMap.count(tid)) {
                courses.emplace_back(name, code, teacherMap[tid], cap);
                cout << "Course added.\n";
            }
            else cout << "Teacher not found.\n";
            break;
        }
        case 4: {
            string code, sid;
            cin.ignore();
            cout << "Enter course code: "; getline(cin, code);
            cout << "Enter student ID: "; getline(cin, sid);
            auto it = find_if(courses.begin(), courses.end(), [&](Course& c) { return c.getCourseCode() == code; });
            if (it != courses.end() && studentMap.count(sid)) {
                if (it->addStudent(studentMap[sid])) {
                    studentMap[sid].enrollCourse(code);
                    for (auto& s : students)
                        if (s.getID() == sid) s = studentMap[sid];
                    cout << "Student enrolled.\n";
                }
                else cout << "Enrollment failed.\n";
            }
            else cout << "Course or student not found.\n";
            break;
        }
        case 5: for (const auto& s : students) s.displayInfo(); break;
        case 6: for (const auto& t : teachers) t.displayInfo(); break;
        case 7: for (const auto& c : courses) c.displayInfo(); break;
        case 8: searchStudent(students); break;
        case 9: deleteStudent(students, studentMap); break;
        case 10: updateStudent(students, studentMap); break;
        case 0: cout << "Exiting...\n"; break;
        default: cout << "Invalid option.\n"; break;
        }
    } while (choice != 0);

    saveAllData(students, teachers, courses);
    return 0;
}
