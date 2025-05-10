// Filename: 590Lab3.h
// Author: First Name Last Name
// Date: 04/23/25
// Description: Lab 3 header file.
// Intial Author: Gokul Nathan (TA)

#ifndef LAB3_H
#define LAB3_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string>

#define BUFFER_SIZE 5

/*Task 3 dynamic array definition

*/
typedef struct {
  int *data;
  int size;
  int capacity;
} DynamicArray;

typedef struct {
  int *buffer;      // Pointer to dynamically allocated buffer
  size_t head;      // Index of the next element to write
  size_t tail;      // Index of the next element to read
  size_t count;     // Current number of elements in the buffer
  size_t max_size;  // Maximum capacity of the buffer
} CircularBuffer;

// Task States
typedef enum {
  READY,
  RUNNING,
  BLOCKED
} TaskState;

// Abridged Task Control Block (TCB)
typedef struct {
  int task_id;        // Unique task identifier
  TaskState state;    // Current state of the task
  uint32_t run_time;  // Time the task should run in milliseconds
} Task;


void fibonacci(int N, unsigned long long **sequence);
int factorial(int n, unsigned long long *result);
void initArray(DynamicArray *arr, int initialCapacity);
void addElement(DynamicArray *arr, int element);
void printArray(DynamicArray *arr);
void freeArray(DynamicArray *arr);
void reverseString(char *str);

int pointer_distance(int *ptr1, int *ptr2, int *array_start, int *array_end);
void mem_copy(void *dest, const void *src, size_t n);
int array_modify(int *arr, size_t len, int (*mod_func)(int));
void print_static_array(int *arr, size_t len);
int str_to_int(const char *str);

int string_to_binary(const char *str);
void print_truth_table(const char *a, const char *b);


void initBuffer(CircularBuffer *cb, size_t size);
void resizeBuffer(CircularBuffer *cb, size_t new_size);
void writeBuffer(CircularBuffer *cb, int value);
int readBuffer(CircularBuffer *cb);
bool isFull(CircularBuffer *cb);
bool isEmpty(CircularBuffer *cb);
void pushBuffer(CircularBuffer *cb, int value);
int popBuffer(CircularBuffer *cb);
void freeBuffer(CircularBuffer *cb);

void simulateSensorData(CircularBuffer *cb, DynamicArray *processedData);


void initialize_tasks(Task *tasks, int num_tasks);
void run_scheduler(Task *tasks, int num_tasks, uint32_t *flowchart, int flowchart_size);
void print_flowchart(const uint32_t *flowchart, int flowchart_size, int num_tasks, const Task *tasks);

void printOperationComparisonTable();

#endif
