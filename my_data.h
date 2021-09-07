#include "const.h"
#include <pthread.h>
#ifndef MY_DATA_H
#define MY_DATA_H

struct my_data {
  struct Stack *stack;
  char **files;
  pthread_mutex_t mutex;
  pthread_mutex_t muti;
  pthread_mutex_t req_file;
  pthread_mutex_t res_file;
  pthread_mutex_t file_mutex;
  pthread_cond_t space_avail;
  pthread_cond_t items_avail;
  int file_index;
  int number_files;
  int req_serviced;
  int req_tot;
  int req_done;
  int res_tot;
  FILE *req_log;
  FILE *res_log;
}; //my_data

#endif
