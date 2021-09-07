#include "stack.h"
#include "const.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// maybe add mutexes to critical  sections here

struct Stack *createStack(){
  struct Stack *ptr = (struct Stack*)malloc(sizeof(struct Stack));
  if(ptr == NULL){
    printf("createStack() failed.\n");
  }
  else{
    ptr->maxsize = ARRAY_SIZE;
    ptr->top = -1;
    pthread_mutex_init(&ptr->mutex, NULL);
    //fill array with \0
    for(int i = 0; i < ARRAY_SIZE; i++){
      memset(ptr->array[i], '\0', MAX_NAME_LENGTH);
    }
  }
  return ptr;
}

void destroyStack(struct Stack *ptr){
  for(int i = 0; i < ARRAY_SIZE; i++){
    memset(ptr->array[i], '\0', MAX_NAME_LENGTH);
  }
  pthread_mutex_destroy(&ptr->mutex);
  free(ptr);
  printf("stack memory has been freed!\n");
}

int isEmpty(struct Stack *ptr){
  return ptr->top == -1;
}

int isFull(struct Stack *ptr){
  return ptr->top == ptr->maxsize-1;
}

char *peek(struct Stack *ptr){
  char *str = NULL;
  //check im empty
  if(isEmpty(ptr)){
    printf("Stack is empty!\n");
  }
  else{
    //pthread_mutex_lock(&ptr->mutex);
    str = ptr->array[ptr->top];
    //pthread_mutex_unlock(&ptr->mutex);
  }
  return str;
}

void push(struct Stack *ptr, char *str){
  //check if full
  if(isFull(ptr)){
    printf("Stack is full!\n");
  }
  else{
    //pthread_mutex_lock(&ptr->mutex);
    ptr->top = ptr->top + 1;
    strcpy(ptr->array[ptr->top], str);
    //pthread_mutex_unlock(&ptr->mutex);
  }
}

void pop(struct Stack *ptr){
  //check if empty
  if(isEmpty(ptr)){
    printf("Stack is empty!\n");
  }
  else{
    //pthread_mutex_lock(&ptr->mutex);
    memset(ptr->array[ptr->top], '\0', MAX_NAME_LENGTH);
    ptr->top = ptr->top - 1;
    //pthread_mutex_unlock(&ptr->mutex);
  }
}

//the  mutexes that I added here, don't seem to have an effect in
// the program's behavior


// int main(){
//   struct Stack *stack = NULL;
//   stack = createStack();
//   char *names[] = {"a","b","c","d","e","f","g","h","i","j"};
//   //fill stack
//   for(int i = 0; i < ARRAY_SIZE; i++){
//     push(stack, names[i]);
//     printf("%s\n",peek(stack));
//   }
//   push(stack, "lalala");
//   printf("full: %d\n",isFull(stack));
//   printf("empty: %d\n",isEmpty(stack));
//   printf("\n");
//
//   //empty stack
//   for (int i = 0; i < ARRAY_SIZE; i++){
//     printf("%s\n",peek(stack));
//     pop(stack);
//   }
//   pop(stack);
//   printf("full: %d\n",isFull(stack));
//   printf("empty: %d\n",isEmpty(stack));
//   destroyStack(stack);
//
//   return 0;
// }
