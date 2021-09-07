#include "stack.h"
#include "const.h"
#include "multi-lookup.h"
#include "fileMgmt.h"
#include "my_data.h"
#include "util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>



void *requester(struct my_data *data){
  int count = 0;
  char *line = malloc((sizeof(char)) * MAX_NAME_LENGTH);
  //iterate over files
  while(data->file_index < data->number_files){
    //grab file, increment index
    pthread_mutex_lock(&data->file_mutex);
    char *fileName = data->files[data->file_index];
    //printf("file = %s\n",fileName);
    FILE *fptr = openFile(fileName,'r');
    count++;
    data->file_index++;
    pthread_mutex_unlock(&data->file_mutex);
    //process file----------------------------------------------------------
    if (fptr == NULL){
      continue;
    }
    //iterate though lines
    while(fgets(line,MAX_NAME_LENGTH,fptr) != NULL){
      pthread_mutex_lock(&data->mutex);
      while(isFull(data->stack)){
        pthread_cond_wait(&data->space_avail, &data->mutex);
      }
      push(data->stack, line);
      //printf("Producer: Insert Item %s, in %d\n", peek(data->stack), data->stack->top);
      pthread_cond_signal(&data->items_avail);
      pthread_mutex_unlock(&data->mutex);
      //write to log
      pthread_mutex_lock(&data->req_file);
      fprintf(data->req_log, "%s",line);
      pthread_mutex_unlock(&data->req_file);
    }
    //----------------------------------------------------------------------
    fclose(fptr);
  }
  free(line);
  pthread_mutex_lock(&data->muti);
  data->req_serviced++;
  if(data->req_serviced == data->req_tot){
    data->req_done = 1;
    for(int i = 0; i < data->res_tot; i++){
      pthread_mutex_lock(&data->mutex);
      pthread_cond_signal(&data->items_avail);
      pthread_mutex_unlock(&data->mutex);
    }
  }
  pthread_mutex_unlock(&data->muti);
  pthread_t tid = pthread_self();
  printf("thread %ld serviced %d files\n",(unsigned long)tid,count);
  return NULL;
}



void *resolver(struct my_data *data){
  //initialize count variable
  int count = 0;
  char *item = NULL;
  item = (char *)malloc((sizeof(char)) * MAX_NAME_LENGTH);
  char ipAddress[MAX_IP_LENGTH];
  while((data->req_serviced != data->req_tot) || (!isEmpty(data->stack))){ //not finish or not empty
    //printf("resolver in \n");
    //consume element from buffer
    pthread_mutex_lock(&data->mutex);//--------------------------------mutex
    while(isEmpty(data->stack)){
      //--------------------------------------1
      if(data->req_done && isEmpty(data->stack)){
        break;
      }
      //--------------------------------------2
      //printf("I'm still here\n");
      pthread_cond_wait(&data->items_avail, &data->mutex);
    }
    //--------------------------------------1
    if(data->req_done && isEmpty(data->stack)){
      pthread_mutex_unlock(&data->mutex);
      break;
    }
    //--------------------------------------2
    //char *item = malloc((sizeof(char)) * MAX_NAME_LENGTH);
    strcpy(item,peek(data->stack));
    //int before_pop = data->stack->top;
    pop(data->stack);
    //perhaps pass to resolver func
    //printf("Consumer: Remove Item %s, from %d\n",item,before_pop);
    //increment count here
    pthread_cond_signal(&data->space_avail);
    pthread_mutex_unlock(&data->mutex);//--------------------------------mutex
    item[strlen(item)-1] = '\0';
    if(dnslookup(item,ipAddress,MAX_IP_LENGTH) == UTIL_FAILURE){
      strcpy(ipAddress,"NOT RESOLVED");
    }
    else{
      count++;
    }
    pthread_mutex_lock(&data->res_file);
    fprintf(data->res_log, "%s, %s\n",item,ipAddress);
    pthread_mutex_unlock(&data->res_file);

  }
  free(item);
  pthread_t tid = pthread_self();
  printf("thread %ld resolved %d hostnames\n",(unsigned long)tid,count);
  return NULL;
}





int main(int argc, char **argv){
  //
  struct timeval start, stop;
  gettimeofday(&start, NULL);
  //-------------------------------------------------------
  int reqNumber = 0;
  int resNumber = 0;
  FILE *reqLogFile = NULL;
  FILE *resLogFile = NULL;
  struct my_data *dat = (struct my_data*) malloc(sizeof(struct my_data));
  //dat = malloc(sizeof(my_data));
  dat->stack = createStack();
  dat->file_index = 0;
  dat->req_serviced = 0;
  dat->req_done = 0;
  //check for errors-------------------------------------------------------
  if(argc < MIN_ARG){
    printf("Missing arguments!\n");
    free(dat);
    exit(EXIT_FAILURE);
  }
  else if(argc > MAX_ARG){
    fprintf(stderr,"Too many arguments!\n");
    free(dat);
    exit(EXIT_FAILURE);
  }
  reqNumber = atoi(argv[1]);
  resNumber = atoi(argv[2]);
  if(reqNumber <= 0 || reqNumber > MAX_REQUESTER_THREADS){
    fprintf(stderr,"The number of requester threads is out of range.\n");
    free(dat);
    exit(EXIT_FAILURE);
  }
  else if(resNumber <= 0 || resNumber > MAX_RESOLVER_THREADS){
    fprintf(stderr,"The number of resolver threads is out of range.\n");
    free(dat);
    exit(EXIT_FAILURE);
  }
  //-----------------------------------------------------------------------
  //open log files
  reqLogFile = openFile(argv[3],'w');//changed to w from r!!!!!
  // printf("reqfile = %p\n",reqLogFile);
  resLogFile = openFile(argv[4],'w');
  // printf("resfile = %p\n",resLogFile);
  dat->req_log = reqLogFile;//not sure what happens in this file does not exist
  dat->res_log = resLogFile;
  //take all the files in argv and put them in an array
  int num_files = argc-5;
  dat->files = malloc(num_files * (sizeof(char*)));
  //printf("num_files = %d\n",num_files);
  // for(int i = 0; i < num_files; i++){
  //   dat->files[i] = (char*)malloc(MAX_NAME_LENGTH);
  // }
  for(int i = 0; i < num_files; i++){
    dat->files[i] = argv[i+5];
  }
  dat->number_files = num_files;
  // //test
  // for(int i = 0; i < num_files; i++){
  //   printf("file = %s\n",dat->files[i]);
  // }

  //initialize synch mechanisms
  pthread_t pro[reqNumber],con[resNumber];
  dat->req_tot = reqNumber;
  dat->res_tot = resNumber;
  pthread_mutex_init(&dat->muti, NULL);

  pthread_mutex_init(&dat->mutex, NULL);
  pthread_mutex_init(&dat->req_file, NULL);
  pthread_mutex_init(&dat->res_file, NULL);
  pthread_mutex_init(&dat->file_mutex, NULL);
  pthread_cond_init(&dat->space_avail, NULL);
  pthread_cond_init(&dat->items_avail, NULL);
  //thread pools are created
  for(int i = 0; i < reqNumber; i++) {
      pthread_create(&pro[i], NULL, (void *)requester, dat);
  }
  for(int i = 0; i < resNumber; i++) {
      pthread_create(&con[i], NULL, (void *)resolver, dat);
  }
  //thread pools joined
  for(int i = 0; i < reqNumber; i++) {
      pthread_join(pro[i], NULL);
  }
  for(int i = 0; i < resNumber; i++) {
      pthread_join(con[i], NULL);
  }

  reqLogFile = dat->req_log;
  resLogFile = dat->res_log;



  //clean
  pthread_mutex_destroy(&dat->mutex);
  pthread_mutex_destroy(&dat->muti);
  pthread_mutex_destroy(&dat->req_file);
  pthread_mutex_destroy(&dat->res_file);
  pthread_mutex_destroy(&dat->file_mutex);
  destroyStack(dat->stack);
  free(dat->files);
  // fclose(dat->req_log);
  // fclose(dat->res_log);
  free(dat);
  fclose(reqLogFile);
  fclose(resLogFile);
  gettimeofday(&stop, NULL);
  double duration = (stop.tv_sec - start.tv_sec)+((stop.tv_usec - start.tv_usec) / 1000000.0);
  printf("./multi-lookup: total time is %f seconds\n",duration);
  return 0;
}
