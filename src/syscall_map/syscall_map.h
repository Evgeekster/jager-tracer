#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern const char* syscall_names_arr[]; // AUTO-GENERATED, DO NOT CHANGE MANUALLY

char* get_syscall_name(long sys_num);

char* parse_syscall_args(long sys_num);

char* get_syscall_map();