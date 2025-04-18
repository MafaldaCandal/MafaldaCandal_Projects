#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "studentManager.h"

// structure representing a student manager
typedef struct studManInner{
  student* students;  // collection of students
  int studentCount;  // number of students in the manager
  int startingID;  // starting id for the first student
  int size;  // size of the students array
} studManInner;


// creates a new empty student manager and returns it
studman newStudMan(int startingID){
  studman newManager = (studman)malloc(sizeof(studManInner));

  newManager->students = (student*)malloc(10 * sizeof(student));
  newManager->studentCount = 0;
  newManager->startingID = startingID;
  newManager->size = 10;  // initial capacity is 10 students

  return newManager;
}

// doubles the size of the student array
void doubleArray(studman sm){
  sm->size *= 2;
  sm->students = (student*)realloc(sm->students, sm->size * sizeof(student));
}

// adds a new student with the given data to sm and returnes the assigned studentID
int addStudent(studman sm, char * name, char * surname, int year){
  // if the array is full, double its size
  if (sm->studentCount == sm->size) doubleArray(sm);

  // create a new student and assign a unique id to them
  student newS = newStudent(name, surname, sm->studentCount + sm->startingID, year);

  sm->students[sm->studentCount] = newS;
  sm->studentCount++;

  return getStudentId(newS);
}

// returns the number of students in sm
int getStudentsNum(studman sm){
  if (sm == NULL) return -1;
  else return sm->studentCount;
}

// returns the index of a student in the array based on their studentID. If the studentID
// is invalid or out of range it returns -1
int studentIndex(studman sm, int studentID){
  if (sm != NULL){
    int index = studentID - sm->startingID;
    if (index >= 0 && index < sm->studentCount) return index;
    else return -1;  // invalid index
  }
  else return -1;  // sm is NULL
}

// returns the name of the student in sm with ID studentID. If there is no such student returns "NONE"
char * getName(studman sm, int studentID){
  int index = studentIndex(sm, studentID);

  if (index == -1) return "NONE";
  else return getStudentName(sm->students[index]);
}

// returns the surname of the student in sm with ID studentID. If there is no such student returns "NONE"
char * getSurname(studman sm, int studentID){
  int index = studentIndex(sm, studentID);

  if (index == -1) return "NONE";
  else return getStudentSurname(sm->students[index]);
}

// returns the year of enrollment of the student in sm with ID studentID. If there is no such student returns -1
int getYear(studman sm, int studentID){
  int index = studentIndex(sm, studentID);

  if (index == -1) return -1;
  else return getStudentYear(sm->students[index]);
}

// adds the last passed course to the student with ID studentID
// if there is no such a student it does not do anything
void addCourseToStudent(studman sm, int studentID, char * courseName){
  int index = studentIndex(sm, studentID);

  if (index != -1) addLastPassedCourse(sm->students[index], courseName);
}

// returns an array of the courses that the student with studentID passed in the order in which they were passed
// if there is no student with the given studentID it returns NULL
char** getStudentPassedCourses(studman sm, int studentID){
  int index = studentIndex(sm, studentID);

  if (index == -1) return NULL;
  return getPassedCourses(sm->students[index]);
}

// returns the number of courses passed by the student with ID studentID
// if there is no such a student returns -1
int getNumberStudentPassedCourses(studman sm, int studentID){
  int index = studentIndex(sm, studentID);

  if (index == -1) return -1;
  else return getNumberOfPassedCourses(sm->students[index]);
}

// returns the studentID of the student with the given surname
// if there is no such a student returns -1
// if there are more than one students with the same surname returns -2
int getStudentsBySurname(studman sm, char * surname){
  int i = 0;
  int count = 0;
  int matchID;

  if (sm == NULL) return -1;
  while (count < 2 && i < sm->studentCount){
    if (strcmp(getStudentSurname(sm->students[i]), surname) == 0){
      matchID = sm->startingID + i;  // store the matching studentID
      count++;
    }
    i++;
  }

  if (count == 0) return -1;
  else if (count > 1) return -2;
  else return matchID;
}

// frees the space used to store sm. Has no effects if sm is NULL
void freeStudman(studman sm){
  if (sm == NULL) return;

  for (int i = 0; i < sm->studentCount; i++){
    student s = sm->students[i];
    if (s != NULL) freeStudent(s);
  }
  free(sm->students);
  free(sm);
}