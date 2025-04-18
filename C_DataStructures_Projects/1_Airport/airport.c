#include <string.h>
#include "libs/LibQueue.h"
#include "libs/LibStack.h"

const int maxHangar = 5;
const int maxRunway = 7;

// declaration of helper functions. Is this good practice or redundant?
void EmptyRunway(int* numRunway);
void EmptyHangar(int* numHangar, int* numRunway);
void EndOfDay(int* numRunway, int* numHangar);

// declaration of the queue and stack
Stack hangar;  // LIFO data-structure to simulate the hangar
Queue runway;  // FIFO data-structire to simulate the waiting runway

// Simulates the departure of all the planes in the waiting runway. Takes the number of planes on the waiting runway as input. Has no output.
// This is used when 1) the waiting runway is full, 2) when the hangar needs to be emptied,
// 3) at the end of the day as clean up.
void EmptyRunway(int* numRunway) {
  if (*numRunway != 0) {
    printf("Ready for takeoff!\n");
  }
  while (*numRunway != 0) {
    printf("%d\n", dequeue(runway));
    (*numRunway)--;  // Decrement numRunway after dequeuing
  }
}

// Simulates the moving of all the planes in the hangar to the waiting runway. Takes the number of planes on the hangar and
// on the waiting runway as input. Has no output. This is used when 1) the hangar is full, 2) at the end of the day as clean up.
void EmptyHangar(int* numHangar, int* numRunway) {
  while (*numHangar != 0) {
    if (*numRunway == maxRunway) {
      EmptyRunway(numRunway);  // Clear runway if full
    }
    enqueue(pop(hangar), runway);
    (*numHangar)--;
    (*numRunway)++;
  }
}

// Simulates the end of the day clean up (ensure that both the hangar and the waiting runway are empty).
// Takes the number of planes on the hangar and on the waiting runway as input. Has no output.
void EndOfDay(int* numRunway, int* numHangar) {
  EmptyRunway(numRunway);
  EmptyHangar(numHangar, numRunway);
  EmptyRunway(numRunway);
  freeStack(hangar);
  freeQueue(runway);
}

int main(void) {
  int n = 0;  // number of planes to read
  hangar = newStack();
  runway = newQueue();
  int numHangar = 0;  // number of planes at the hangar
  int numRunway = 0;  // number of planes at the waiting runway

  scanf("%d", &n);

  // repeat this loop for each plane arriving
  for (int i = 0; i < n; i++) {
    int planeID = 0;
    char needsRepairs[4];

    scanf("%d %s", &planeID, needsRepairs);

    if (strcmp(needsRepairs, "yes") == 0) {  // if the plane needs repairing, it goes to the hangar
      push(planeID, hangar);
      numHangar++;

      if (numHangar == maxHangar) {  // if the hangar is full, empty it.
        EmptyRunway(&numRunway);
        EmptyHangar(&numHangar, &numRunway);
      }
    }

    else {  // if the plane does not need repairing, it goes to the waiting runway
      enqueue(planeID, runway);
      numRunway++;

      if (numRunway == maxRunway) {  // if the waiting runway is full, empty it.
        EmptyRunway(&numRunway);
      }
    }
  }

  EndOfDay(&numRunway, &numHangar);  // clean up, ensure there are no planes in the hangar or waiting runway and free the memory.
  return 0;
}