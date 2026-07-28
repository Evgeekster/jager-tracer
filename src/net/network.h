#pragma once

#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include "decode/decode.h"

void fmt_sockaddr(pid_t pid, unsigned long addr_ptr, socklen_t addr_len,
                     char *out, size_t out_size);
