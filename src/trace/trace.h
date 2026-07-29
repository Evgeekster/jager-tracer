#pragma once
#include "pch.h"
#include "syscall_map.h"
#include "util/util.h"
#include "decode/decode.h"
#include "args/args.h"

typedef struct  {
    pid_t               pid;
    short               in_syscall;
    long                syscall_num;
    struct timespec     call_start; // syscall entrance time
    int                 is_thread; 
    pid_t               parent_pid; 
    short               alive; // 0 if dead 


    char                pending_line[1024]; // buffer for syscall line to print on exit
    long                pending_syscall_num; // syscall number to print on exit
}Tracee;

typedef struct {
    Tracee*     tracees;
    size_t      used_size;
    size_t      length;
} TraceeList;

Tracee*     getTracee(TraceeList* list, pid_t pid);
Tracee*     addTracee(TraceeList* list, pid_t pid, pid_t parent_pid, short is_thread);
void        traceeRemove(TraceeList* list, pid_t pid); 
void        freeTraceeList(TraceeList* list);


// void trace_loop(pid_t pid, List* stats);
void trace_loop(TraceeList *tracees, List *stats);

void syscall_entry(pid_t pid, struct user_regs_struct *regs);

void syscall_exit(struct user_regs_struct *regs, TableElement* t, long nr, pid_t pid, Tracee **tr);

void start_trace(char* prog, char** argv);

