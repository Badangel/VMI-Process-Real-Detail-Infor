#include "myprint.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
void printlog(char* s){
    FILE *pf = fopen("/home/vmi/Downloads/code/VmiXen/host/log/file.log","a");
    time_t timep;
    time(&timep);
    fprintf(pf,"%s%s",ctime(&timep),s);
    fclose(pf);
}

void clear_filelog(){
    FILE *pf = fopen("/home/vmi/Downloads/code/VmiXen/host/log/file.log","w");
    fclose(pf);
}