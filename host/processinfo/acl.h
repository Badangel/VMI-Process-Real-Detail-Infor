/**
* Copyright(c) 2017 dyf
* head file access control list of process
*/
#ifndef ACL_H_INCLUDED
#define ACL_H_INCLUDED
#include "vminit.h"
#include "myList.h"

typedef struct aclps
{
    char name[50];
    int id;
}ACLPs;

void outputACL(const void* const p);

void* getACLList(VmiInfo* vmivm, MyList* list);

int compare2aclps(void* a, void* b);

int findACLps(MyList* list, char* name, int id);

void showACLList(VmiInfo* vmivm);

#endif