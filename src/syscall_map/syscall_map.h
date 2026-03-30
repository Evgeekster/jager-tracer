#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern const char* syscall_names_arr[];

// enum arg_types {
//     PARAM_INT,
//     PARAM_STR,
//     PARAM_PTR,
//     PARAM_FLAGS,
//     PARAM_FD
// } arg_types;


char* get_syscall_name(long sys_num);

char* parse_syscall_args(long sys_num);

char* get_syscall_map();