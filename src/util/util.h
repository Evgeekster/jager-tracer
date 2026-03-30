#pragma once
#include "pch.h"
#include "decode/decode.h"

#define DEFAULT_LIST_SIZE 2

typedef struct TableElement{
    int         syscall_id;
    const char* syscall_name;
    int         errs;
    int         calls;
    double      time_spent;    
} TableElement;


typedef struct {
    TableElement*   elements; // should be changed to void** later for 
    size_t          used_size;
    size_t          length;
} List;

void initList       (List *l, size_t initSize);
void appendToList   (List *l, TableElement element);
void freeList       (List* l);

TableElement*       findElementById(List* l, int id);
// TableElement*       updateElement(List* l, TableElement* element);

// extern struct TableStruct;
// extern struct List;

void syscall_print      (pid_t pid, struct user_regs_struct *regs, const char *sys_name);


void init_table         (TableElement* Element);
void update_stat_table  (TableElement* Elemen);
void free_table         (TableElement* Elemen);

void print_stats_table  (List* l);


