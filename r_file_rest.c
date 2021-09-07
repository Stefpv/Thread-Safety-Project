#include "stack.h"
#include "const.h"
#include "multi-lookup.h"
#include "fileMgmt.h"
#include "my_data.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>




//need to change fileMgmt !!!!!!!
//--> DONE



int fileReader(char *filename){
  FILE *fptr = NULL;
  // char *line = malloc(MAX_NAME_LENGTH);
  fptr = openFile(filename,'r');
  if(fptr == NULL){
    return 0;
  }
  char line[MAX_NAME_LENGTH];
  while(fgets(line,MAX_NAME_LENGTH,fptr) != NULL){ //fgets returns NULL when EOF is read or no char are read
    printf("line read: %s\n",line);
  }

  fclose(fptr);
  return 1;
}



int main(){

  char *filename = "input/names1.txt";
  int status = fileReader(filename);

  printf("status = %d\n",status);

  return 0;
}
