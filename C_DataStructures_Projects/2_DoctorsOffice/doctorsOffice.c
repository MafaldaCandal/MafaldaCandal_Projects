#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Patient structure
struct Patient {
  char name[201];
  int age;
  int priority;
  struct Patient* next;  // Pointer to the next patient
};

// Define the WaitingList structure, which holds the head of the list and the count of patients
struct WaitingList {
  struct Patient* head;  // Pointer to the first patient
  int count;             // Count of patients
};

// Function to collect the patient's details (name, age, and priority)
void GetPatientDetails(char* name, int* age, int* priority) {
  printf("What is the name of the patient?\n");
  scanf(" %200[^\n]", name);

  printf("What is the age of the patient?\n");
  scanf(" %d", age);

  printf("What is the priority of the patient?\n");
  scanf(" %d", priority);
}

// Function to insert a new patient based on priority.
void Admit(struct WaitingList* li, char* name, int age, int priority) {
  struct Patient* newPatient = (struct Patient*)malloc(sizeof(struct Patient));
  if (newPatient == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }

  strcpy(newPatient->name, name);
  newPatient->age = age;
  newPatient->priority = priority;
  newPatient->next = NULL;

  // If the queue is empty, set the new patient as the head
  if (li->head == NULL) {
    li->head = newPatient;
    li->count++;
    return;
  }

  // If the new patient has the highest priority, insert at the head
  if (li->head->priority < priority) {
    newPatient->next = li->head;
    li->head = newPatient;
  } else {
    // Traverse to find the correct position to insert based on priority
    struct Patient* current = li->head;
    while (current->next != NULL && current->next->priority >= priority) {
      current = current->next;
    }

    // Insert the patient in the correct position
    newPatient->next = current->next;
    current->next = newPatient;
  }

  li->count++;
}

// Function to remove the patient with the highest priority.
void GiveTreatment(struct WaitingList* li) {
  struct Patient* DischargedPatient = li->head;  // DischargedPatient holds the current head (the patient being treated)
  li->head = li->head->next;                     // Move the head to the next patient in the queue
  free(DischargedPatient);                       // Free the memory of the treated patient
  li->count--;                                   // Decrease the patient count
}

// Function to print the patient waiting list.
void CheckWaitingList(struct Patient* head) {
  struct Patient* current = head;
  while (current != NULL) {
    printf("%s (age: %d, priority: %d)\n", current->name, current->age, current->priority);
    current = current->next;
  }
}

int main(void) {
  struct WaitingList li = {NULL, 0};  // Initialize the waiting list with head = NULL and count = 0
  char command;

  while (1) {
    printf("Currently there are %d patients in the queue.\n", li.count);

    printf("What do you want to do? [N]ew patient, ");
    if (li.count > 0) {
      printf("[T]reat patient, [L]ist, ");
    }
    printf("[Q]uit:\n");
    scanf(" %c", &command);

    if (command == 'N') {
      char name[201];
      int age, priority;
      GetPatientDetails(name, &age, &priority);
      Admit(&li, name, age, priority);
    } else if (command == 'T') {
      if (li.head == NULL) {
        printf("No patients to treat.\n");
      } else {
        printf("Treating the patient %s (age: %d, priority: %d).\n", li.head->name, li.head->age, li.head->priority);
        GiveTreatment(&li);
      }
    } else if (command == 'L') {
      CheckWaitingList(li.head);
    } else if (command == 'Q') {
      // Free all remaining patients before quitting
      while (li.head != NULL) {
        GiveTreatment(&li);  // Free all remaining patients
      }
      printf("Goodbye!\n");
      exit(0);
    }
  }
}