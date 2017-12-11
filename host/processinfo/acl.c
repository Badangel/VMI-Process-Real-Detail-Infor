
#include <stdio.h>
#include <stdint.h>
#include "acl.h"
#include "myList.h"

void ACLoutput(const void* const p)
{
    ACLPs * pp= p;
    printf("%s(%d)\n", pp->name, pp->id);
}

void* getACLList(MyList * list)
{

    FILE *fp;
    fp = fopen("config/ACL.config","r");
    char tempname[50];
    int temppsid;
    int over = 1;
    while(over){
        fscanf(fp,"%s %d\n",&tempname,&temppsid);
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
                printf("%s [%d]\n",data->name,data->id);
            }
        }
        else{
            fscanf(fp,"%*[^\n]");
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
}

void showACLList(){
    MyList * list= createMySearchList(compare2ps);
    getACLList(list);
    myListOutput(list, ACLoutput );
   
}