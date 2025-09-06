#ifndef VECTOR_H_
#define VECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
  char **data; // Vector of string pointers
  size_t size; // Current number of elements
  size_t capacity; // Allocated capacity
} Vector;

typedef struct {
  int *data;
  size_t size;
  size_t capacity;
} VectorInt;

// Create a new vector
Vector *create_vector(void) {
  Vector *vector = (Vector *)malloc(sizeof(Vector));
  if (!vector) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  vector->data = NULL;
  vector->size = 0;
  vector->capacity = 0;
  return vector;
}

// Add an element to the vector
void push_back_string(Vector* vector, const char* value) {
  if (!vector || !value) return;

  if (vector->capacity == 0) {
    vector->data = (char**)malloc(sizeof(char*));
    if (!vector->data) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
    }
    vector->capacity = 1; 
  } else if (vector->size >= vector->capacity) {
      size_t new_capacity = vector->capacity * 2;
      char **tmp = (char**)realloc(vector->data, new_capacity * sizeof(char*));
      if (!tmp) {
        perror("realloc failed");
        exit(EXIT_FAILURE);
      }
      vector->data = tmp;
      vector->capacity = new_capacity; 
  }

  vector->data[vector->size] = strdup(value);
  if (!vector->data[vector->size]) {
    perror("strdup failed");
    exit(EXIT_FAILURE);
  }

  vector->size++;
}

// Remove the last element from the vector
void pop_back(Vector* vector) {
  if (!vector || vector->size == 0) return;
  free(vector->data[vector->size - 1]);
  vector->size--;
}

// Access an element at a specific index
char* vector_at(Vector* vector, size_t index) {
  if (!vector) {
    fprintf(stderr, "Vector is NULL\n");
    exit(EXIT_FAILURE);
  }
  if (index >= vector->size) {
    fprintf(stderr, "Index out of bounds\n");
    exit(EXIT_FAILURE);
  }
  return vector->data[index];
}

// Get the current size of the vector
size_t vector_size(Vector* vector) {
  if (!vector) return 0;
  return vector->size;
}

// Check if the vector is empty
bool is_empty(Vector* vector) {
  if (!vector) return true;
  return vector->size == 0;
}

// Free the vector and all strings it contains
void free_vector(Vector* vector) {
  if (!vector) return;
  for (size_t i = 0; i < vector->size; i++) {
    free(vector->data[i]);
  }
  free(vector->data);
  free(vector);
}

#endif // VECTOR_H_
