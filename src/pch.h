#pragma once
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/wait.h>   
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <signal.h>     
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


// to store all imports in one file accesable to all others

#ifdef __aarch64__
    #define SYSCALL_NUM regs->regs[8]
    #define SYSCALL_RET regs->regs[0]
    #define SYSCALL_ARG1 regs->regs[0]
    #define SYSCALL_ARG2 regs->regs[1]
    #define SYSCALL_ARG3 regs->regs[2]
    typedef struct user_pt_regs user_regs_struct;
#elif __arm__
    #define SYSCALL_NUM regs->uregs[7]
    #define SYSCALL_RET regs->uregs[0]
    #define SYSCALL_ARG1 regs->uregs[0]
    #define SYSCALL_ARG2 regs->uregs[1]
    #define SYSCALL_ARG3 regs->uregs[2]
    typedef struct user_regs user_regs_struct;
#else
    #define SYSCALL_NUM regs->orig_rax
    #define SYSCALL_RET regs->rax
    #define SYSCALL_ARG1 regs->rdi
    #define SYSCALL_ARG2 regs->rsi
    #define SYSCALL_ARG3 regs->rdx
    typedef struct user_regs_struct user_regs_struct;
#endif


#define ARG_PRINT(ok, arg_val, buf) ((ok) ? ({ \
    static char tmp[32]; \
    snprintf(tmp, sizeof(tmp), "0x%lx", (unsigned long)(arg_val)); \
    tmp; \
}) : (buf))


extern bool showErrs;
extern bool showTable;
