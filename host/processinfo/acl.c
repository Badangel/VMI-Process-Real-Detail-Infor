
/**
* Copyright(c) 2017 dyf
* implement access control list of process
*/

#include <stdio.h>
#include <string.h>
#include "acl.h"


void outputACL(const void* const p)
{
    const ACLPs * pp= p;
    printf("%s(%d)\n", pp->name, pp->id);
}

void* getACLList(VmiInfo* vmivm, MyList * list)
{

    FILE *fp;
    char filepath[100];
    strcpy(filepath,"config/");
    strcat(filepath,vmivm->vmname);
    strcat(filepath,"_ACL.conf");
    fp = fopen(filepath,"r");
    if(fp == NULL)
    {
        printf("open %s failed!\n",filepath);
        return NULL;
    }
    char tempname[50];
    char *tm = tempname; 
    int temppsid;
    int over = 1;
    while(over){
        int resread = fscanf(fp,"%s %d\n",tm,&temppsid);
        ///printf("%s-%d\n",tempname,temppsid);
        if(tempname[0] != '#'){
            if(tempname[0]=='@'){
                over = 0;
            }
            else{
                ACLPs *data = malloc(sizeof(ACLPs));
                strcpy(data->name,tempname);
                data->id = temppsid;
                myListInsertDataAtLast(list, data);
                ///printf("%s [%d]\n",data->name,data->id);
            }
        }
        else{
            resread = fscanf(fp,"%*[^\n]");
        }
    }
    return list;
}

int compare2ps(void* a,void* b)
{
    ACLPs* aa = a;
    ACLPs* bb = b;
    if(strcmp(aa->name,bb->name)==0)
        return 1;
    else
        return 0;
    return 0;
}

int findACLps(MyList* list, char* name, int id)
{
    ACLPs newps;
    strcpy(newps.name,name);
    newps.id = id;
    ACLPs* tmp = &newps;
    return myListFindDataIndex(list,tmp);
}

void showACLList(VmiInfo* vmivm)
{
    MyList * list= createMySearchList(compare2ps);
    getACLList(vmivm,list);
    myListOutput(list, outputACL );
   
}