#ifndef MYLIST_H_INCLUDED
#define MYLIST_H_INCLUDED
#include <stdio.h>

typedef struct myNode
{
    void * data;
    struct myNode *next;
} MyNode;

typedef struct myList
{
    MyNode * first;
    MyNode * last;
    int count;
    int (*equal)(void * a, void * b);
} MyList;

typedef struct myListIterator
{
    MyNode * p;
    int count;
    int allSize;
} MyListIterator;

//init mylist
MyList * createMyList();

//init mylist with its only equal for search
MyList * createMySearchList(int(*equal)(void * a, void * b));

//free list
void freeMyList(MyList * list);

//insert data at last
void myListInsertDataAtLast(MyList* const list, void* const data);

//insert data at first
void myListInsertDataAtFirst(MyList * const list, void* const data);

//insert data at index
void myListInsertDataAt(MyList * const list, void* const data, int index);

//delete data at last
void* myListRemoveDataAtLast(MyList* const list);

//delete data at first
void* myListRemoveDataAtFirst(MyList * const list);

//delete data at index
void* myListRemoveDataAt(MyList* const list, int index);

//delete object
int myListRemoveDataObject(MyList* const list, void * data);

//get long of list
int myListGetSize(const MyList * const list);

//print the list
void myListOutput(const MyList * const list, void(*pt)(const void * const));

//get the index data of list
void* myListGetDataAt(const MyList * const list, int index);

//get the first data of list
void* myListGetDataAtFirst(const MyList * const list);

//get the last data of list
void* myListGetDataAtLast(const MyList * const list);

//find a data index,
//if list have the data return first pos index, else return -1
int myListFindDataIndex(const MyList * const list, void * data);

//creat iterator
MyListIterator* createMyListIterator(const MyList * const list);

//free iterator
void freeMyListIterator(MyListIterator* iterator);

//next iterator is null or not
int myListIteratorHasNext(const MyListIterator* const iterator);

//return next data of iterator
void * myListIteratorNext(MyListIterator* const iterator); 

#endif // MYLIST_H_INCLUDED
