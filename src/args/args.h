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
    ARG_NONE,       /* do not print                                      */
    ARG_INT,        /* signed decimal                                    */
    ARG_UINT,       /* unsigned decimal                                  */
    ARG_HEX,        /* unsigned hex (0x...)                              */
    ARG_PTR,        /* pointer (NULL or 0x...)                           */
    ARG_STR,        /* NUL-terminated string in tracee memory            */
    ARG_FD,         /* file descriptor                                   */
    ARG_FLAGS,      /* bitwise OR of NamedConst values                   */
    ARG_ENUM,       /* exactly one NamedConst value                      */
    ARG_OCTAL,      /* unsigned octal (mode_t)                           */

    /* --- memory & buffers --- */
    ARG_BUF,        /* raw byte buffer (print as hex dump or size only)  */
    ARG_SIZET,      /* size_t (unsigned, printed as decimal)             */
    ARG_SSIZET,     /* ssize_t (signed)                                  */
    ARG_OFFSET,     /* off_t (file offset, signed)                       */

    /* --- process & signals --- */
    ARG_PID,        /* pid_t                                             */
    ARG_SIG,        /* signal number (SIGKILL etc.)                      */
    ARG_SIGSET,     /* sigset_t * (bitmask of signals)                   */
    ARG_SIGACTION,  /* struct sigaction *                                */

    /* --- time --- */
    ARG_TIMESPEC,   /* struct timespec *                                 */
    ARG_TIMEVAL,    /* struct timeval *                                  */
    ARG_CLOCKID,    /* clockid_t (CLOCK_MONOTONIC etc.)                  */

    /* --- filesystem --- */
    ARG_PATH,       /* pathname string (same decode as ARG_STR)          */
    ARG_ATFD,       /* dirfd: AT_FDCWD or regular fd                     */
    ARG_STATBUF,    /* struct stat *                                     */
    ARG_IOVEC,      /* struct iovec * + iovcnt pair                      */

    /* --- networking --- */
    ARG_SOCKADDR,   /* struct sockaddr * (needs paired addrlen arg)      */
    ARG_SOCKLEN,    /* socklen_t or socklen_t* (addrlen for accept/etc.) */
    ARG_SOCKDOM,    /* socket domain  (AF_INET, AF_UNIX …)               */
    ARG_SOCKTYPE,   /* socket type    (SOCK_STREAM | SOCK_NONBLOCK …)    */
    ARG_SOCKPROT,   /* socket protocol (IPPROTO_TCP etc.)                */
    ARG_CMSG,       /* struct msghdr * (sendmsg / recvmsg)               */

    /* --- ioctl / mmap / misc --- */
    ARG_IOCTL_REQ,  /* ioctl request code (decoded to name if known)     */
    ARG_MMAPPROT,   /* mmap prot flags (PROT_READ|PROT_WRITE …)          */
    ARG_MMAPFLAGS,  /* mmap flags (MAP_SHARED|MAP_ANON …)                */
    ARG_WHENCE,     /* lseek whence (SEEK_SET / SEEK_CUR / SEEK_END)     */

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
                pid_t tracee, const user_regs_struct *regs);

/* ── Format the whole syscall entry line ─────────────────────────────── */
void syscall_format_entry(pid_t tracee, const user_regs_struct *regs,
                          char *buf, size_t sz);


