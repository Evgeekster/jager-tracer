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

    #define REG_SYSCALL(r)  ((r).regs[8])
    #define REG_RET(r)      ((r).regs[0])
    #define REG_ARG(r, n)   ((r).regs[n]) 
    typedef struct user_pt_regs user_regs_struct;
#elif __arm__
    #define SYSCALL_NUM regs->uregs[7]
    #define SYSCALL_RET regs->uregs[0]
    #define SYSCALL_ARG1 regs->uregs[0]
    #define SYSCALL_ARG2 regs->uregs[1]
    #define SYSCALL_ARG3 regs->uregs[2]

    #define REG_SYSCALL(r)  ((r).uregs[7])
    #define REG_RET(r)      ((r).uregs[0])
    #define REG_ARG(r, n)   ((r).uregs[n])
    typedef struct user_regs user_regs_struct;
#else
    #define SYSCALL_NUM regs->orig_rax
    #define SYSCALL_RET regs->rax
    #define SYSCALL_ARG1 regs->rdi
    #define SYSCALL_ARG2 regs->rsi
    #define SYSCALL_ARG3 regs->rdx
    typedef struct user_regs_struct user_regs_struct;

    #define REG_SYSCALL(r)  ((r).orig_rax)
    #define REG_RET(r)      ((r).rax)

    static inline unsigned long long _x86_arg(const user_regs_struct *r, int n) {
        switch (n) {
            case 0: return r->rdi;
            case 1: return r->rsi;
            case 2: return r->rdx;
            case 3: return r->r10;
            case 4: return r->r8;
            case 5: return r->r9;
            default: return 0;
        }
    }
    #define REG_ARG(r, n)   _x86_arg(&(r), n)
#endif


#define ARG_PRINT(ok, arg_val, buf) ((ok) ? ({ \
    static char tmp[32]; \
    snprintf(tmp, sizeof(tmp), "0x%lx", (unsigned long)(arg_val)); \
    tmp; \
}) : (buf))


/* ── ANSI colour helpers ─────────────────────────────────────────────── */
#define COL_RESET   "\033[0m"
#define COL_BOLD    "\033[1m"
#define COL_CYAN    "\033[36m"
#define COL_MAGENTA "\033[35m"
#define COL_RED     "\033[31m"
#define COL_YELLOW  "\033[33m"
#define COL_GREEN  "\033[32m"




extern bool g_showErrs;
extern bool g_showTable;
extern bool g_color;


void color(const char *c);

