#pragma once
#include "syscall_map/syscall_map.h"
#include "pch.h"

int decode_str(pid_t pid, unsigned long addr, char* buffer, size_t max_size);


// char* decode_read();
// char* decode_write();
// char* decode_openat();
// char* decode_execve();
