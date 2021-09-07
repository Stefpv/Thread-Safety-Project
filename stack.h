#include "const.h"
#include <pthread.h>
#ifndef STACK_H
#define STACK_H
//stack struct
struct Stack {
  int maxsize;
  int top;
  char array[ARRAY_SIZE][MAX_NAME_LENGTH];
  pthread_mutex_t mutex;
};
//stack functions
struct Stack *createStack();
void destroyStack(struct Stack *stack);
int isEmpty(struct Stack *stack);
int isFull(struct Stack *stack);
char *peek(struct Stack *stack);
void push(struct Stack *stack, char *str);
void pop(struct Stack *stack);

#endif
