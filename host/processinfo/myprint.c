#include "myprint.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
void printlog(char* s){
    FILE *pf = fopen("log/file.log","a");
    time_t timep;
    time(&timep);
    fprintf(pf,"%s%s",ctime(&timep),s);
    fclose(pf);
}