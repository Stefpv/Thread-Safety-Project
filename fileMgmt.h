#include "const.h"

#ifndef FILE_MGMT_H
#define FILE_MGMT_H

// void writeF();
// void readF();
void invalidFile(char *fileName);
FILE *openFile(char *fileName, char action);

#endif
