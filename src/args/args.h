#pragma once
#include "pch.h"

/*
 THE FOLLOWING CODE IS AI-GENERATED.
 my excuse - i wont be able to create this all on my own

 Simple args decoder engine
*/

/* ── Named constant (one entry in a flags/enum table) ────────────────── */
typedef struct {
    long        value;
    const char *name;
} NamedConst;

/* ── Argument kinds ───────────────────────────────────────────────────── */
typedef enum {
    ARG_NONE,       /* do not print                             */
    ARG_INT,        /* signed decimal                           */
    ARG_UINT,       /* unsigned decimal                         */
    ARG_HEX,        /* unsigned hex (0x...)                     */
    ARG_PTR,        /* pointer (NULL or 0x...)                  */
    ARG_STR,        /* NUL-terminated string in tracee memory   */
    ARG_FD,         /* file descriptor                          */
    ARG_FLAGS,      /* bitwise OR of NamedConst values          */
    ARG_ENUM,       /* exactly one NamedConst value             */
    ARG_OCTAL,      /* unsigned octal (mode_t)                  */
} ArgKind;

/* ── Descriptor for one argument ─────────────────────────────────────── */
typedef struct {
    ArgKind           kind;
    const NamedConst *consts;   /* for ARG_FLAGS / ARG_ENUM, else NULL */
} ArgDesc;

/* ── Descriptor for one syscall ──────────────────────────────────────── */
#define SYSCALL_MAX_ARGS 6

typedef struct {
    int         nr;
    const char *name;
    int         argc;
    ArgDesc     args[SYSCALL_MAX_ARGS];
} SyscallDesc;

/* ── Lookup ───────────────────────────────────────────────────────────── */
const SyscallDesc *syscall_desc_find(long nr);

/* ── Format one argument value into buf ──────────────────────────────── */
void arg_format(char *buf, size_t sz,
                const ArgDesc *desc, unsigned long long val,
                pid_t tracee);

/* ── Format the whole syscall entry line ─────────────────────────────── */
void syscall_format_entry(pid_t tracee, const user_regs_struct *regs,
                          char *buf, size_t sz);
