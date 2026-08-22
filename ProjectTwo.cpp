//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Joseph Charitable Jr.
// Description : ABCU Advising Assistance Program
//============================================================================

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Structure used to store information for one course
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Node used by the binary search tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(const Course& aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Binary search tree used to store courses
class BinarySearchTree {

private:
    Node* root;

    // Recursively insert a course into the correct position
    Node* addNode(Node* node, const Course& course) {

        if (node == nullptr) {
            return new Node(course);
        }

        if (course.courseNumber < node->course.courseNumber) {
            node->left = addNode(node->left, course);
        }
        else {
            node->right = addNode(node->right, course);
        }

        return node;
    }

    // Recursively print courses in alphanumeric order
    void inOrder(Node* node) const {

        if (node != nullptr) {

            // Visit the left subtree first
            inOrder(node->left);

            // Print the current course
            cout << node->course.courseNumber
                << ", "
                << node->course.courseTitle
                << endl;

            // Visit the right subtree
            inOrder(node->right);
        }
    }

    // Recursively delete all nodes in the tree
    void deleteTree(Node* node) {

        if (node != nullptr) {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
        }
    }

public:
    BinarySearchTree() {
        root = nullptr;
    }

    // Destructor frees memory when the tree is destroyed
    ~BinarySearchTree() {
        deleteTree(root);
        root = nullptr;
    }

    // Insert a course into the tree
    void Insert(const Course& course) {
        root = addNode(root, course);
    }
    // Search for a course by course number
    Course Search(const string& courseNumber) const {

        Node* current = root;

        while (current != nullptr) {

            if (courseNumber == current->course.courseNumber) {
                return current->course;
            }

            if (courseNumber < current->course.courseNumber) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        // Return an empty course if no match is found
        Course emptyCourse;
        return emptyCourse;
    }

    // Print all courses in alphanumeric order
    void PrintCourseList() const {
        inOrder(root);
    }

    // Remove all courses currently stored in the tree
    void Clear() {
        deleteTree(root);
        root = nullptr;
    }
};

// Remove extra spaces from the beginning and end of a string
string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");

    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(" \t\r\n");

    return text.substr(first, last - first + 1);
}

// Convert a string to uppercase
string toUpper(string text) {
    for (char& character : text) {
        character = toupper(character);
    }

    return text;
}

// Split a line of course data using commas
vector<string> splitLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }

    return tokens;
}

// Load course data from the input file
bool loadCourses(const string& fileName, BinarySearchTree& courses) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }

    courses.Clear();

    string line;
    vector<Course> courseList;

    while (getline(file, line)) {

        line = trim(line);

        // Skip blank lines
        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        // Each course must have at least a course number and title
        if (tokens.size() < 2) {
            cout << "Error: Invalid course data format." << endl;
            return false;
        }

        Course course;

        course.courseNumber = toUpper(tokens[0]);
        course.courseTitle = tokens[1];

        // Store any prerequisites
        for (size_t i = 2; i < tokens.size(); i++) {
            if (!tokens[i].empty()) {
                course.prerequisites.push_back(toUpper(tokens[i]));
            }
        }

        courseList.push_back(course);
    }

    // Validate that every prerequisite exists in the course list
    for (const Course& course : courseList) {

        for (const string& prerequisite : course.prerequisites) {

            bool prerequisiteFound = false;

            for (const Course& possibleCourse : courseList) {

                if (possibleCourse.courseNumber == prerequisite) {
                    prerequisiteFound = true;
                    break;
                }
            }

            if (!prerequisiteFound) {
                cout << "Error: Prerequisite "
                    << prerequisite
                    << " does not exist." << endl;

                return false;
            }
        }
    }

    // Insert each validated course into the binary search tree
    for (const Course& course : courseList) {
        courses.Insert(course);
    }

    cout << "Course data loaded successfully." << endl;

    return true;
}

// Display information for one course
void printCourse(const BinarySearchTree& courses, string courseNumber) {

    // Allow the user to enter lowercase or uppercase course numbers
    courseNumber = toUpper(trim(courseNumber));

    Course course = courses.Search(courseNumber);

    // Check whether the course was found
    if (course.courseNumber.empty()) {
        cout << "Course " << courseNumber << " not found." << endl;
        return;
    }

    cout << course.courseNumber << ", " << course.courseTitle << endl;

    // Display prerequisites and their course titles
    cout << "Prerequisites: ";

    if (course.prerequisites.empty()) {
        cout << "None";
    }
    else {
        for (size_t i = 0; i < course.prerequisites.size(); i++) {
            Course prerequisite = courses.Search(course.prerequisites[i]);

            cout << prerequisite.courseNumber << ", "
                << prerequisite.courseTitle;

            if (i < course.prerequisites.size() - 1) {
                cout << "; ";
            }
        }
    }

    cout << endl;
}

int main() {

    BinarySearchTree courses;
    bool dataLoaded = false;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {

        cout << endl;
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a menu number." << endl;
            continue;
        }

        cin.ignore(10000, '\n');

        switch (choice) {

        case 1: {
            string fileName;

            cout << "Enter the file name: ";
            getline(cin, fileName);

            if (loadCourses(fileName, courses)) {
                dataLoaded = true;
            }

            break;
        }

        case 2:
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                courses.PrintCourseList();
            }
            break;

        case 3: {
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            }
            else {
                string courseNumber;

                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);

                printCourse(courses, courseNumber);
            }

            break;
        }

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }

    }

    return 0;
}