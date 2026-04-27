#pragma once
#define _GNU_SOURCE
#include "syscall_map/syscall_map.h"
#include "pch.h"
#include <sys/uio.h>
#include <sys/syscall.h>



/*
 * Fast path: process_vm_readv — one syscall, no word-by-word loop.
 * Falls back to PTRACE_PEEKDATA if process_vm_readv fails (e.g. older kernels).
 */

 int fast_decode (pid_t pid, unsigned long addr, char* buffer, size_t max_size);
 int deep_decode(pid_t pid, unsigned long addr, char* buffer, size_t max_size);


/* 
    this method calls decoders 
*/
int decode_str(pid_t pid, unsigned long addr, char* buffer, size_t max_size);

/* ── Format a socket address ─────────────────────────────────────────── */
// void fmt_sockaddr(char *buf, size_t sz, pid_t tracee, unsigned long val);
// int decode_sockaddr(pid_t pid, unsigned long addr, char* buffer, size_t max_size);
int decode_mem(pid_t pid, unsigned long addr, void* buffer, size_t max_size);
