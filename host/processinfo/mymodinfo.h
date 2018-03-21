/**
* Copyright(c) 2018 dyf
* mymodinfo.h
* something about module
*/

#ifndef MYMODINFO_H_INCLUDED
#define MYMODINFO_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include <libvmi/libvmi.h>
#include <mysql/mysql.h>
#include "myList.h"
#include "vminit.h"
int get_module_info(VmiInfo* vmiinfo,MyList* list);

void set_module_info(VmiInfo* vmiinfo,MyList* list,int num_module);

int find_hide_module(VmiInfo* vmiinfo,MYSQL *mysql,MyList* list,int hidenum);

void initModule(VmiInfo* vmiinfo);

int detect_hide_module(VmiInfo* vmiinfo,MYSQL *mysql,int module_change);

int compare2mod(void* a, void* b);

#endif