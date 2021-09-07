#include "stack.h"
#include "const.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


void invalidFile(char *fileName){
  fprintf(stderr,"invalid file %s\n", fileName);
}

FILE* openFile(char *fileName, char action){
  FILE *fptr = NULL;
  if(action == 'r'){
    //printf("read\n");
    if((fptr = fopen(fileName, "r")) == NULL){
      invalidFile(fileName);
      return fptr;
    }
  }
  else if(action == 'w'){
    //printf("write\n");
    if((fptr = fopen(fileName, "w+")) == NULL){
      invalidFile(fileName);
      return fptr;
    }
  }
  return fptr;
}

// //FOR TESTING
// //comment this out!!!!!!!!!!!!!!!!!!!!!!!!
// int main(int argc, char **argv){
//   char *testFile = argv[1];
//   char act = 'w';
//   FILE* fptr1 = openFile(testFile,act);
//   printf("fptr1 = %p\n",fptr1);
//   // fclose(fptr1);
//
//   char *test2 = "input/names1.txt";
//   char act2 = 'r';
//   FILE* fptr2 = openFile(test2,act2);
//   printf("fptr2 = %p\n",fptr2);
//   // fclose(fptr2);
//
//   char *test3 = "input/names2.txt";
//   char act3 = 'w';
//   FILE* fptr3 = openFile(test3,act3);
//   printf("fptr3 = %p\n",fptr3);
//
//
//   fclose(fptr1);
//   fclose(fptr2);
//   fclose(fptr3);
//
//
//   return 0;
// }
