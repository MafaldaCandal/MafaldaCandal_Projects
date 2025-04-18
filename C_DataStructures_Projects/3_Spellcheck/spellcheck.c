// Spellcheck Lab assignment

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// maximum length for a word
#define LENGTH 45

// I prefered using Dict over Trie to facilitate comprehension of what the Trie is being used to mimic: a dictionary.
typedef struct trieNode* Dict;

struct trieNode {
  bool endNode;
  Dict children[26];
};

// A function to make the dictionary (trie). No input, outputs an empty dictionary.
Dict newEmptyDict(void) {
  Dict d = malloc(1 * sizeof(struct trieNode));
  if (d == NULL) {                                  // Check if malloc failed.
    fprintf(stderr, "Memory allocation failed\n");  // Print an error message.
    exit(EXIT_FAILURE);                             // Exit the program with a failure status.
  }
  d->endNode = false;
  for (int i = 0; i < 26; i++) {
    d->children[i] = NULL;
  }
  return d;
}

// A helper function mapping a to 0, b to 1, etc.
int c2n(char c) {
  c = tolower(c);
  return (c - 'a');
}

// A function to check if the word is in the dictioanry (trie). Inputs a word and the dictionary and outputs a boolean
// true if the word is in the dictionary and false otherwise.
bool check(char* word, Dict d) {
  Dict current = d;

  for (int i = 0; word[i] != '\0'; i++) {
    int index = c2n(word[i]);
    if (current->children[index] == NULL) {
      return false;
    }
    current = current->children[index];
  }
  return current->endNode;
}

// A function to add a word to the dictionary (trie).
void addWord(char* word, Dict d) {
  Dict current = d;  // Start from the root of the trie.

  for (int i = 0; i < (int)strlen(word); i++) {          // Iterate through each character in the word.
    if (current->children[c2n(word[i])] == NULL) {       // If the child node does not exist, create it.
      current->children[c2n(word[i])] = newEmptyDict();  // Allocate a new empty dictionary for the character.
    }
    current = current->children[c2n(word[i])];  // Move to the child node.
  }

  if (current->endNode)
    return;  // If this node is already marked as the end of a word, do nothing.
  else
    current->endNode = true;  // Otherwise, mark the current node as the end of the word.
}

// A function that frees the trie starting from the leaves.
void freeDict(Dict d) {
  for (int i = 0; i < 26; i++) {
    if (d->children[i] != NULL) {
      freeDict(d->children[i]);
    }
  }
  free(d);
}

// A function that removes non-alphabetic characters and convert to lower case.
void trimWord(char* word) {
  int k = 0;
  for (int i = 0; i < (int)strlen(word); i++) {
    if (isalpha(word[i])) {
      word[k] = tolower(word[i]);
      k++;
    }
  }
  word[k] = '\0';
}

// The main function.
int main(void) {
  char word[LENGTH + 1] = "";

  // step 1: read in the dictionary
  Dict dictionary = newEmptyDict();
  while (scanf("%45s", word) && word[0] != '%') {
    trimWord(word);
    addWord(word, dictionary);
  }

  // step 2: read in text
  int counter = 0;  // number of unknown words
  int index = 0;
  int c = EOF;
  while ((c = getchar()) && c != EOF) {
    if (isalpha(c)) {  // if c is a letter, then add to the array and move index
      word[index] = c;
      index++;
    } else if (index > 0) {  // if c isn't a consecutive non-letter, enter the loop
      word[index] = '\0';    // Without this termination, functions like strlen or trimWord would not know where the string ends
      trimWord(word);
      if (!check(word, dictionary)) {
        counter++;
        printf("%s\n", word);
      }
      index = 0;
    }
  }

  // step 3: print number of unknown words
  printf("%d\n", counter);

  freeDict(dictionary);
  return 0;
}