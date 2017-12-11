#ifndef ACL_H_INCLUDED
#define ACL_H_INCLUDED

#include "myList.h"

typedef struct aclps
{
    char name[50];
    int id;
}ACLPs;

void ACLoutput(const void* const p);

void* getACLList(MyList * list);

int compare2ps(void* a,void* b);

void showACLList();

#endif