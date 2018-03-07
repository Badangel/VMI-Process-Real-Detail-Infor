/**
* Copyright(c) 2018 dyf
* mymodinfo.h
* something about module
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymodinfo.h"
int get_module_info(VmiInfo* vmiinfo,MyList* list){
    addr_t next_module, list_head;
    vmi_instance_t vmi = vmiinfo->vmi;
    vmi_pause_vm(vmi);
    vmi_read_addr_ksym(vmi, "modules", &next_module);
    list_head = next_module;
    int modulenum = 0;
    while (1) {

        /* follow the next pointer */
        addr_t tmp_next = 0;
        vmi_read_addr_va(vmi, next_module, 0, &tmp_next);

        if (list_head == tmp_next) {
            break;
        }

        char *modname = NULL;
	    int num_syms_mod = -1;
	    int num_kp_mod = -1;
        modulenum++;
        ModuleNode* onemodule = malloc(sizeof(ModuleNode));


        if (VMI_PM_IA32E == vmi_get_page_mode(vmi, 0)) {   // 64-bit paging
            modname = vmi_read_str_va(vmi, next_module + 16, 0);
        }
        else {
            modname = vmi_read_str_va(vmi, next_module + 8, 0);
		
        }
        strcpy(onemodule->name,modname);
	    vmi_read_32_va(vmi, next_module + 224, 0, &num_syms_mod);
	    vmi_read_32_va(vmi, next_module + 280, 0, &num_kp_mod);
        onemodule->num_syms = num_syms_mod;
        onemodule->num_kp = num_kp_mod;
        myListInsertDataAtLast(list, onemodule);
        //printf("%s %d %d\n", modname, num_syms_mod, num_kp_mod);
        free(modname);
        
        next_module = tmp_next;
    }
    vmi_resume_vm(vmi);

    return modulenum;
}

void set_module_info(VmiInfo* vmiinfo,MyList* list,int num_module){
    if(vmiinfo->modulelist!=NULL){
        MyList* oldlist = vmiinfo->modulelist;
        freeMyList(oldlist);
    }
    vmiinfo->modulelist = list;
    vmiinfo->module_num = num_module;
}

void find_hide_module(VmiInfo* vmiinfo,MyList* list,int hidenum){
    MyList* oldlist = vmiinfo->modulelist;
    MyNode* p = oldlist->first;
    FILE *pf = fopen("log/warning.log","a");
    while (p)
    {
        ModuleNode* hidemod = p->data;
        if(myListFindDataIndex(list,p->data)<0){
            fprintf(pf, "%s(module) is hided!! \n", hidemod->name);
            hidenum--;
            if(hidenum==0){
                break;
            }
        }
        p = p->next;
    }
    fclose(pf);
}
int compare2mod(void* a, void* b){
    ModuleNode* aa = a;
    ModuleNode* bb = b;
    if(strcmp(aa->name,bb->name)==0)
        return 1;
    else
        return 0;
    return 0;
}