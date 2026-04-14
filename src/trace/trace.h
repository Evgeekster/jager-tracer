#pragma once
#include "pch.h"
#include "syscall_map.h"
#include "util/util.h"
#include "decode/decode.h"
#include "args/args.h"

void trace_loop(pid_t pid, List* stats);

void syscall_entry(pid_t pid, struct user_regs_struct *regs);

void syscall_exit(struct user_regs_struct *regs, TableElement* t);

void start_trace(char* prog, char** argv);

