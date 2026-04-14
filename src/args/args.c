#include "args.h"
#include "decode/decode.h"
#include <sys/mman.h>
#include <fcntl.h>

/* ══════════════════════════════════════════════════════════════════════
   Named-constant tables
   ══════════════════════════════════════════════════════════════════════ */

static const NamedConst open_flags[] = {
    { O_RDONLY,   "O_RDONLY"   },
    { O_WRONLY,   "O_WRONLY"   },
    { O_RDWR,     "O_RDWR"     },
    { O_CREAT,    "O_CREAT"    },
    { O_TRUNC,    "O_TRUNC"    },
    { O_APPEND,   "O_APPEND"   },
    { O_NONBLOCK, "O_NONBLOCK" },
    { O_DSYNC,    "O_DSYNC"    },
    { O_CLOEXEC,  "O_CLOEXEC"  },
    { O_DIRECTORY,"O_DIRECTORY"},
    { O_NOFOLLOW, "O_NOFOLLOW" },
#ifdef O_NOATIME
    { O_NOATIME,  "O_NOATIME"  },
#endif
#ifdef O_PATH
    { O_PATH,     "O_PATH"     },
#endif
#ifdef O_TMPFILE
    { O_TMPFILE,  "O_TMPFILE"  },
#endif
    { 0, NULL }
};

static const NamedConst mode_flags[] = {
    { 0400, "S_IRUSR" }, { 0200, "S_IWUSR" }, { 0100, "S_IXUSR" },
    { 0040, "S_IRGRP" }, { 0020, "S_IWGRP" }, { 0010, "S_IXGRP" },
    { 0004, "S_IROTH" }, { 0002, "S_IWOTH" }, { 0001, "S_IXOTH" },
    { 0, NULL }
};

static const NamedConst mmap_prot[] = {
    { PROT_NONE,  "PROT_NONE"  },
    { PROT_READ,  "PROT_READ"  },
    { PROT_WRITE, "PROT_WRITE" },
    { PROT_EXEC,  "PROT_EXEC"  },
    { 0, NULL }
};

static const NamedConst mmap_flags[] = {
    { MAP_SHARED,     "MAP_SHARED"     },
    { MAP_PRIVATE,    "MAP_PRIVATE"    },
    { MAP_FIXED,      "MAP_FIXED"      },
    { MAP_ANONYMOUS,  "MAP_ANONYMOUS"  },
    { MAP_GROWSDOWN,  "MAP_GROWSDOWN"  },
    { MAP_DENYWRITE,  "MAP_DENYWRITE"  },
    { MAP_EXECUTABLE, "MAP_EXECUTABLE" },
    { MAP_LOCKED,     "MAP_LOCKED"     },
    { MAP_NORESERVE,  "MAP_NORESERVE"  },
    { MAP_POPULATE,   "MAP_POPULATE"   },
    { MAP_STACK,      "MAP_STACK"      },
    { 0, NULL }
};

static const NamedConst whence_vals[] = {
    { SEEK_SET, "SEEK_SET" },
    { SEEK_CUR, "SEEK_CUR" },
    { SEEK_END, "SEEK_END" },
    { 0, NULL }
};

static const NamedConst sig_vals[] = {
    { SIGHUP,  "SIGHUP"  }, { SIGINT,  "SIGINT"  }, { SIGQUIT, "SIGQUIT" },
    { SIGILL,  "SIGILL"  }, { SIGABRT, "SIGABRT" }, { SIGFPE,  "SIGFPE"  },
    { SIGKILL, "SIGKILL" }, { SIGSEGV, "SIGSEGV" }, { SIGPIPE, "SIGPIPE" },
    { SIGALRM, "SIGALRM" }, { SIGTERM, "SIGTERM" }, { SIGUSR1, "SIGUSR1" },
    { SIGUSR2, "SIGUSR2" }, { SIGCHLD, "SIGCHLD" }, { SIGCONT, "SIGCONT" },
    { SIGSTOP, "SIGSTOP" }, { SIGTSTP, "SIGTSTP" }, { SIGTTIN, "SIGTTIN" },
    { SIGTTOU, "SIGTTOU" },
    { 0, NULL }
};

static const NamedConst at_flags[] = {
    { AT_FDCWD,            "AT_FDCWD"            },
    { AT_SYMLINK_NOFOLLOW, "AT_SYMLINK_NOFOLLOW" },
    { AT_REMOVEDIR,        "AT_REMOVEDIR"        },
#ifdef AT_EMPTY_PATH
    { AT_EMPTY_PATH,       "AT_EMPTY_PATH"       },
#endif
    { 0, NULL }
};

static const NamedConst access_modes[] = {
    { F_OK, "F_OK" },
    { R_OK, "R_OK" },
    { W_OK, "W_OK" },
    { X_OK, "X_OK" },
    { 0, NULL }
};

static const NamedConst clone_flags[] = {
    { 0x00000100, "CLONE_VM"      },
    { 0x00000200, "CLONE_FS"      },
    { 0x00000400, "CLONE_FILES"   },
    { 0x00000800, "CLONE_SIGHAND" },
    { 0x00002000, "CLONE_PTRACE"  },
    { 0x00004000, "CLONE_VFORK"   },
    { 0x00008000, "CLONE_PARENT"  },
    { 0x00010000, "CLONE_THREAD"  },
    { 0x00020000, "CLONE_NEWNS"   },
    { 0x00100000, "CLONE_IO"      },
    { 0, NULL }
};

/* ══════════════════════════════════════════════════════════════════════
   Syscall descriptor table  (x86-64 numbers)
   Add entries here — zero code changes elsewhere.
   ══════════════════════════════════════════════════════════════════════ */

#define S   ARG_STR
#define I   ARG_INT
#define U   ARG_UINT
#define H   ARG_HEX
#define P   ARG_PTR
#define FD  ARG_FD
#define FL  ARG_FLAGS
#define EN  ARG_ENUM
#define OC  ARG_OCTAL
#define NN  ARG_NONE

#define A(k)          { .kind = (k), .consts = NULL }
#define AF(k, tbl)    { .kind = (k), .consts = (tbl) }

static const SyscallDesc syscall_table[] = {
/*  nr   name            argc  arg0              arg1                    arg2                    arg3  arg4  arg5  */
  {  0, "read",          3, { A(FD),             A(P),                   A(U)                                   } },
  {  1, "write",         3, { A(FD),             A(P),                   A(U)                                   } },
  {  2, "open",          3, { A(S),              AF(FL, open_flags),     AF(OC, mode_flags)                     } },
  {  3, "close",         1, { A(FD)                                                                             } },
  {  4, "stat",          2, { A(S),              A(P)                                                           } },
  {  5, "fstat",         2, { A(FD),             A(P)                                                           } },
  {  6, "lstat",         2, { A(S),              A(P)                                                           } },
  {  7, "poll",          3, { A(P),              A(U),                   A(I)                                   } },
  {  8, "lseek",         3, { A(FD),             A(I),                   AF(EN, whence_vals)                    } },
  {  9, "mmap",          6, { A(P),              A(U),                   AF(FL, mmap_prot),  AF(FL, mmap_flags), A(FD), A(I) } },
  { 10, "mprotect",      3, { A(P),              A(U),                   AF(FL, mmap_prot)                      } },
  { 11, "munmap",        2, { A(P),              A(U)                                                           } },
  { 12, "brk",           1, { A(P)                                                                              } },
  { 16, "ioctl",         3, { A(FD),             A(H),                   A(H)                                   } },
  { 17, "pread64",       4, { A(FD),             A(P),                   A(U),               A(I)               } },
  { 18, "pwrite64",      4, { A(FD),             A(P),                   A(U),               A(I)               } },
  { 19, "readv",         3, { A(FD),             A(P),                   A(I)                                   } },
  { 20, "writev",        3, { A(FD),             A(P),                   A(I)                                   } },
  { 21, "access",        2, { A(S),              AF(FL, access_modes)                                           } },
  { 22, "pipe",          1, { A(P)                                                                               } },
  { 32, "dup",           1, { A(FD)                                                                              } },
  { 33, "dup2",          2, { A(FD),             A(FD)                                                          } },
  { 39, "getpid",        0, { { 0 }                                                                              } },
  { 41, "socket",        3, { A(I),              A(I),                   A(I)                                   } },
  { 42, "connect",       3, { A(FD),             A(P),                   A(U)                                   } },
  { 43, "accept",        3, { A(FD),             A(P),                   A(P)                                   } },
  { 44, "sendto",        6, { A(FD),             A(P),                   A(U),               A(I), A(P), A(U)  } },
  { 45, "recvfrom",      6, { A(FD),             A(P),                   A(U),               A(I), A(P), A(P)  } },
  { 48, "shutdown",      2, { A(FD),             A(I)                                                           } },
  { 49, "bind",          3, { A(FD),             A(P),                   A(U)                                   } },
  { 50, "listen",        2, { A(FD),             A(I)                                                           } },
  { 56, "clone",         5, { AF(FL, clone_flags), A(P),                 A(P),               A(P), A(P)        } },
  { 57, "fork",          0, { { 0 }                                                                              } },
  { 58, "vfork",         0, { { 0 }                                                                              } },
  { 59, "execve",        3, { A(S),              A(P),                   A(P)                                   } },
  { 60, "exit",          1, { A(I)                                                                               } },
  { 61, "wait4",         4, { A(I),              A(P),                   A(I),               A(P)               } },
  { 62, "kill",          2, { A(I),              AF(EN, sig_vals)                                               } },
  { 72, "fcntl",         3, { A(FD),             A(I),                   A(I)                                   } },
  { 74, "fsync",         1, { A(FD)                                                                              } },
  { 75, "fdatasync",     1, { A(FD)                                                                              } },
  { 76, "truncate",      2, { A(S),              A(I)                                                           } },
  { 77, "ftruncate",     2, { A(FD),             A(I)                                                           } },
  { 78, "getdents",      3, { A(FD),             A(P),                   A(U)                                   } },
  { 79, "getcwd",        2, { A(P),              A(U)                                                           } },
  { 80, "chdir",         1, { A(S)                                                                               } },
  { 81, "fchdir",        1, { A(FD)                                                                              } },
  { 82, "rename",        2, { A(S),              A(S)                                                           } },
  { 83, "mkdir",         2, { A(S),              AF(OC, mode_flags)                                             } },
  { 84, "rmdir",         1, { A(S)                                                                               } },
  { 86, "link",          2, { A(S),              A(S)                                                           } },
  { 87, "unlink",        1, { A(S)                                                                               } },
  { 88, "symlink",       2, { A(S),              A(S)                                                           } },
  { 89, "readlink",      3, { A(S),              A(P),                   A(U)                                   } },
  { 90, "chmod",         2, { A(S),              AF(OC, mode_flags)                                             } },
  { 91, "fchmod",        2, { A(FD),             AF(OC, mode_flags)                                             } },
  { 95, "umask",         1, { AF(OC, mode_flags)                                                                 } },
  {102, "getuid",        0, { { 0 }                                                                              } },
  {104, "getgid",        0, { { 0 }                                                                              } },
  {217, "getdents64",    3, { A(FD),             A(P),                   A(U)                                   } },
  {231, "exit_group",    1, { A(I)                                                                               } },
  {257, "openat",        4, { AF(EN, at_flags),  A(S),                   AF(FL, open_flags), AF(OC, mode_flags) } },
  {258, "mkdirat",       3, { AF(EN, at_flags),  A(S),                   AF(OC, mode_flags)                     } },
  {260, "fchownat",      5, { AF(EN, at_flags),  A(S),                   A(U), A(U), A(I)                      } },
  {263, "unlinkat",      3, { AF(EN, at_flags),  A(S),                   A(I)                                   } },
  {264, "renameat",      4, { AF(EN, at_flags),  A(S),                   AF(EN, at_flags), A(S)                 } },
  {267, "readlinkat",    4, { AF(EN, at_flags),  A(S),                   A(P), A(U)                             } },
  {268, "fchmodat",      3, { AF(EN, at_flags),  A(S),                   AF(OC, mode_flags)                     } },
  {269, "faccessat",     3, { AF(EN, at_flags),  A(S),                   AF(FL, access_modes)                   } },
  {292, "dup3",          3, { A(FD),             A(FD),                  AF(FL, open_flags)                     } },
  {293, "pipe2",         2, { A(P),              AF(FL, open_flags)                                              } },
  {302, "prlimit64",     4, { A(I),              A(I),                   A(P), A(P)                             } },
  {318, "getrandom",     3, { A(P),              A(U),                   A(I)                                   } },
  {319, "memfd_create",  2, { A(S),              A(I)                                                           } },
};

#undef S
#undef I
#undef U
#undef H
#undef P
#undef FD
#undef FL
#undef EN
#undef OC
#undef NN
#undef A
#undef AF

#define TABLE_SIZE  (sizeof(syscall_table) / sizeof(syscall_table[0]))

/* ══════════════════════════════════════════════════════════════════════
   Lookup
   ══════════════════════════════════════════════════════════════════════ */
const SyscallDesc *syscall_desc_find(long nr)
{
    /* Linear scan is fine for ~60 entries; replace with sorted bsearch
       if the table grows past a few hundred entries. */
    for (size_t i = 0; i < TABLE_SIZE; i++)
        if (syscall_table[i].nr == (int)nr)
            return &syscall_table[i];
    return NULL;
}

/* ══════════════════════════════════════════════════════════════════════
   Formatters
   ══════════════════════════════════════════════════════════════════════ */

/* Format a bitwise-OR flags value */
static void fmt_flags(char *buf, size_t sz,
                      unsigned long long val,
                      const NamedConst *consts)
{
    size_t pos = 0;
    unsigned long long remaining = val;
    bool first = true;

    for (const NamedConst *c = consts; c->name; c++) {
        if (c->value == 0) continue;
        if ((remaining & c->value) == (unsigned long long)c->value) {
            if (!first) {
                if (pos + 1 < sz) buf[pos++] = '|';
            }
            size_t len = strlen(c->name);
            if (pos + len + 1 < sz) {
                memcpy(buf + pos, c->name, len);
                pos += len;
            }
            remaining &= ~(unsigned long long)c->value;
            first = false;
        }
    }

    /* Leftover bits not covered by any named constant */
    if (remaining || first) {
        int written = snprintf(buf + pos, sz - pos,
                               first ? "0x%llx" : "|0x%llx", remaining);
        if (written > 0) pos += (size_t)written;
    }
    buf[pos < sz ? pos : sz - 1] = '\0';
}

/* Format an enum (exactly one named value).
 * `val` arrives as a zero-extended register value.  Many kernel constants
 * are negative ints sign-extended to 64 bits, but the register holds the
 * unsigned 32-bit pattern (e.g. AT_FDCWD=-100 → 0xffffff9c → 4294967196).
 * We therefore compare both the raw 64-bit value and its sign-extended
 * 32-bit interpretation against each table entry. */
static void fmt_enum(char *buf, size_t sz,
                     unsigned long long val,
                     const NamedConst *consts)
{
    long long sval   = (long long)val;
    long long sval32 = (long long)(int)(unsigned int)val; /* sign-extend low 32 bits */

    for (const NamedConst *c = consts; c->name; c++) {
        long long cval = (long long)c->value;
        if (sval == cval || sval32 == cval) {
            snprintf(buf, sz, "%s", c->name);
            return;
        }
    }
    snprintf(buf, sz, "%lld", sval32);
}

/* Public: format one argument */
void arg_format(char *buf, size_t sz,
                const ArgDesc *desc, unsigned long long val,
                pid_t tracee)
{
    switch (desc->kind) {
    case ARG_NONE:
        buf[0] = '\0';
        break;

    case ARG_INT:
        snprintf(buf, sz, "%lld", (long long)val);
        break;

    case ARG_UINT:
        snprintf(buf, sz, "%llu", val);
        break;

    case ARG_HEX:
        snprintf(buf, sz, "0x%llx", val);
        break;

    case ARG_OCTAL:
        snprintf(buf, sz, "0%llo", val);
        break;

    case ARG_PTR:
        if (val == 0)
            snprintf(buf, sz, "NULL");
        else
            snprintf(buf, sz, "0x%llx", val);
        break;

    case ARG_FD:
        snprintf(buf, sz, "%lld", (long long)val);
        break;

    case ARG_STR: {
        char tmp[256];
        if (decode_str(tracee, (unsigned long)val, tmp, sizeof(tmp)) == 0) {
            /* Escape non-printable bytes */
            size_t pos = 0;
            buf[pos++] = '"';
            for (char *p = tmp; *p && pos + 4 < sz; p++) {
                unsigned char c = (unsigned char)*p;
                if (c == '"' || c == '\\') {
                    buf[pos++] = '\\';
                    buf[pos++] = c;
                } else if (c < 0x20 || c == 0x7f) {
                    pos += snprintf(buf + pos, sz - pos, "\\x%02x", c);
                } else {
                    buf[pos++] = c;
                }
            }
            if (pos + 1 < sz) buf[pos++] = '"';
            buf[pos] = '\0';
        } else {
            snprintf(buf, sz, "0x%llx", val);
        }
        break;
    }

    case ARG_FLAGS:
        fmt_flags(buf, sz, val, desc->consts);
        break;

    case ARG_ENUM:
        fmt_enum(buf, sz, val, desc->consts);
        break;

    default:
        snprintf(buf, sz, "0x%llx", val);
        break;
    }
}

/* ══════════════════════════════════════════════════════════════════════
   Whole-line formatter
   ══════════════════════════════════════════════════════════════════════ */
void syscall_format_entry(pid_t tracee, const user_regs_struct *regs,
                          char *buf, size_t sz)
{
    long nr               = (long)REG_SYSCALL(*regs);
    const SyscallDesc *d  = syscall_desc_find(nr);

    size_t pos = 0;

    if (!d) {
        pos += snprintf(buf, sz, "syscall_%ld(?)", nr);
        return;
    }

    pos += snprintf(buf + pos, sz - pos, "%s(", d->name);

    for (int i = 0; i < d->argc && pos < sz; i++) {
        char arg_buf[512];
        unsigned long long val = REG_ARG(*regs, i);
        arg_format(arg_buf, sizeof(arg_buf), &d->args[i], val, tracee);

        if (d->args[i].kind != ARG_NONE) {
            pos += snprintf(buf + pos, sz - pos,
                            "%s%s", arg_buf, (i + 1 < d->argc) ? ", " : "");
        }
    }

    if (pos < sz) buf[pos++] = ')';
    if (pos < sz) buf[pos]   = '\0';
}
