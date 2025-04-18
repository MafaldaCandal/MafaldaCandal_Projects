#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

typedef struct ListNode* List;

// structure representing a node in a linked list of passed courses
struct ListNode{
  char* courseName;  // name of the passed course
  List next;  // pointer to the next node in the list
};

// structure representing a student with basic details and a list of passed courses
typedef struct studentInner{
  char name[11];  // first name
  char surname[11];  // surname
  int id;  // unique student id
  int year;  // year of enrollment
  List passedCourses;  // linked list of passed courses
  int countPassed;  // number of passed courses
} studentInner;

// adds a course to the beginning of the linked list
List appendList(List head, char* course){
  List newNode = (List) malloc(sizeof(struct ListNode));
  newNode->courseName = course;
  newNode->next = head;
  return newNode;
}

// creates a new student with the given data and returns it
// if name or surname exceed 10 characters, an error is printed and NULL is returned
student newStudent(char* name, char* surname, int id, int year) {
  if (strlen(name) > 10 || strlen(surname) > 10){
    printf("Name or surname too long, the student was not added.\n");
    return NULL;
  }
  // memory allocation for the new student
  student newStudent = (student)malloc(sizeof(studentInner));

  int i = 0;
  // name is copied into the struct
  while (name[i] != '\0'){
    newStudent->name[i] = name[i];
    i++;
  }
  newStudent->name[i] = '\0';

  i = 0;
  // surname is copied into the struct
  while (surname[i] != '\0'){
    newStudent->surname[i] = surname[i];
    i++;
  }
  newStudent->surname[i] = '\0';

  newStudent->id = id;
  newStudent->year = year;
  newStudent->countPassed = 0;
  newStudent->passedCourses = NULL;

  return newStudent;
}

// returns the name of the student s. If s is NULL it returns "NONE"
char* getStudentName(student s) {
  if (s == NULL) return "NONE";
  else return s->name;
}

// returns the surname of the student s. If s is NULL it returns "NONE"
char* getStudentSurname(student s) {
  if (s == NULL) return "NONE";
  else return s->surname;
}

// returns the id of the student s. If s is NULL it returns -1
int getStudentId(student s) {
  if (s == NULL) return -1;
  else return s->id;
}

// returns the year in which the student s enrolled. If s is NULL it returns -1
int getStudentYear(student s) {
  if (s == NULL) return -1;
  else return s->year;
}

// returns an array of the courses that the student s passed in
// the order in which they were passed. If s is NULL it returns NULL
char** getPassedCourses(student s) {
  if (s == NULL || s->countPassed == 0) return NULL;
  // memory allocation for the array
  char** courseArray = (char**)malloc(s->countPassed * sizeof(char*));

  List temp = s->passedCourses;
  // adds the courses to the list in the correct order
  for (int i = s->countPassed - 1; i >= 0; i--){
    courseArray[i] = temp->courseName;
    temp = temp->next;
  }

  return courseArray;
}

// returns the number of the courses that the student s passed. If s is NULL it returns -1
int getNumberOfPassedCourses(student s) {
  if (s == NULL) return -1;
  else return s->countPassed;
}

// adds courseName to the courses passed by the student s
void addLastPassedCourse(student s, char* courseName) {
  if (s == NULL) return;
  s->passedCourses = appendList(s->passedCourses, courseName);
  s->countPassed++;
}

// frees the space used to store s. Has no effects if s is NULL
void freeStudent(student s) {
  if (s == NULL) return;

  List temp;
  while (s->passedCourses){
    temp = s->passedCourses;
    s->passedCourses = s->passedCourses->next;
    free(temp);  // frees each node in the linked list
  }
  free(s); // frees the studentInner struct
}