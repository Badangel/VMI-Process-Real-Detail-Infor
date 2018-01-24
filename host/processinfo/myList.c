#include "myList.h"
#include <stdlib.h>
#include <vminit.h>
//init mylist
MyList * createMyList()
{
    MyList * re = (MyList *) malloc(sizeof(MyList));
    re->count = 0;
    re->first = NULL;
    re->last = NULL;
    re->equal = NULL;
    return re;
}

//free list
void freeMyList(MyList * list)
{
    MyNode * p;
    while (list->first)
    {
        p = list->first->next;
        ///SocketSR *sock = list->first->data;
        ///printf("class:%d saddr:%s:%d -> raddr:%s:%d\n",sock->classify,sock->sendaddr,sock->sendport,sock->recvaddr,sock->recieveport);
        free(list->first->data);
        free(list->first);
        list->first = p;
    }
    free(list);
}

//insert data at last
void myListInsertDataAtLast(MyList * const list, void* const data)
{
    MyNode * node = (MyNode *) malloc(sizeof(MyNode));
    node->data = data;
    node->next = NULL;
    if (list->count)
    {
        list->last->next = node;
        list->last = node;
    }
    else
    {
        list->first = node;
        list->last = node;
    }
    (list->count)++;
}

//insert data at first
void myListInsertDataAtFirst(MyList * const list, void* const data)
{
    MyNode * node = (MyNode *) malloc(sizeof(MyNode));
    node->data = data;
    node->next = NULL;

    if (list->count)
    {
        node->next = list->first;
        list->first = node;
    }
    else
    {
        list->first = node;
        list->last = node;
    }
    (list->count)++;
}

//get long of list
int myListGetSize(const MyList * const list)
{
    return list->count;
}

//print the list
void myListOutput(const MyList * const list, void(*pt)(const void * const))
{
    MyNode * p = list->first;
    while (p)
    {
        (*pt)(p->data);
        p = p->next;
    }
}

//delete data at last
void* myListRemoveDataAtLast(MyList* const list)
{
    if (list->count == 1)
    {
        return myListRemoveDataAtFirst(list);
    }
    MyNode * p = list->first;
    while (p->next != list->last)
    {
        p = p->next;
    }
    void *re = list->last->data;
    free(list->last);
    p->next = NULL;
    list->last = p;
    (list->count)--;
    return re;
}

//delete data at first
void* myListRemoveDataAtFirst(MyList * const list)
{
    MyNode *p = list->first;
    list->first = p->next;
    void * re = p->data;
    free(p);
    (list->count)--;
    if (list->count == 0)
    {
        list->last = NULL;
    }
    return re;
}

//insert data at index
void myListInsertDataAt(MyList * const list, void* const data, int index)
{
    if (index == 0)
    {
        myListInsertDataAtFirst(list, data);
        return;
    }
    if (index == list->count)
    {
        myListInsertDataAtLast(list, data);
        return;
    }
    MyNode * node = (MyNode *) malloc(sizeof(MyNode));
    node->data = data;
    node->next = NULL;

    MyNode * p = list->first;
    int i = 0;
    for (; i < index - 1; i++)
    {
        p = p->next;
    }
    node->next = p->next;
    p->next = node;

    (list->count)++;
}

//delete data at index
void* myListRemoveDataAt(MyList* const list, int index)
{
    if (index == 0)
    {
        return myListRemoveDataAtFirst(list);
    }
    if (index == list->count - 1)
    {
        return myListRemoveDataAtLast(list);
    }

    MyNode * p = list->first;
    int i = 0;
    for (; i < index - 1; i++)
    {
        p = p->next;
    }
    MyNode *tp = p->next;
    p->next = p->next->next;
    void * re = tp->data;
    free(tp);
    (list->count)--;
    return re;
}

//get the index data of list
void* myListGetDataAt(const MyList * const list, int index)
{
    if (index == list->count - 1)
    {
        return myListGetDataAtLast(list);
    }
    MyNode * p = list->first;
    int i = 0;
    for (; i < index; i++)
    {
        p = p->next;
    }
    return p->data;
}

//get the first data of list
void* myListGetDataAtFirst(const MyList * const list)
{
    return list->first->data;
}

//get the last data of list
void* myListGetDataAtLast(const MyList * const list)
{
    return list->last->data;
}

//find a data index,
//if list have the data return first pos index, else return -1
int myListFindDataIndex(const MyList * const list, void * data)
{
    MyNode * p = list->first;
    int re = 0;
    if (list->equal)
    {
        while (p)
        {
            if (p->data == data || (*(list->equal))(p->data, data))
            {
                return re;
            }
            re++;
            p = p->next;
        }

    }
    else
    {
        while (p)
        {
            if (p->data == data)
            {
                return re;
            }
            re++;
            p = p->next;
        }
    }
    return -1;
}

//init mylist have its only equal
MyList * createMySearchList(int(*equal)(void * a, void * b))
{
    MyList * re = createMyList();
    re->equal = equal;
    return re;
}

//creat iterator
MyListIterator* createMyListIterator(const MyList * const list)
{
    MyListIterator * re = (MyListIterator *) malloc(sizeof(MyListIterator));
    re->p = list->first;
    re->allSize = list->count;
    re->count = 0;
    return re;
}

//free iterator
void freeMyListIterator(MyListIterator* iterator)
{
    free(iterator);
}

//next iterator is null or not
int myListIteratorHasNext(const MyListIterator* const iterator)
{
    return iterator->count < iterator->allSize;
}

//return next data of iterator
void * myListIteratorNext(MyListIterator* const iterator)
{
    void * re = iterator->p->data;
    iterator->p = iterator->p->next;
    (iterator->count)++;
    return re;
}

//delete object
int myListRemoveDataObject(MyList* const list, void * data)
{
    MyListIterator * it = createMyListIterator(list);
    int a = 0;
    while (myListIteratorHasNext(it))
    {
        void * ld = myListIteratorNext(it);
        if (data == ld || (list->equal != NULL && (*(list->equal))(ld, data)))
        {
            a = 1;
            break;
        }
    }
    if (a)
    {
        myListRemoveDataAt(list, it->count - 1);
    }
    return a;
}
