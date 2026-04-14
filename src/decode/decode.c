#include "decode.h"

 int fast_decode (pid_t pid, unsigned long addr, char* buffer, size_t max_size){
    struct iovec local  =    {    .iov_base = buffer,        .iov_len = max_size - 1};
    struct iovec remote =    {    .iov_base = (void*)addr,   .iov_len = max_size - 1};
    

    ssize_t n = process_vm_readv(
        pid, &local, 1, &remote, 1, 0
    );

    if (n < 0){
        return -1; // error
    }

    buffer[n] = '\0';
    buffer[strnlen(buffer, (size_t)n)] = '\0';
    return 0;
}


 int deep_decode(pid_t pid, unsigned long addr, char* buffer, size_t max_size){
    unsigned long offset    = 0;
    unsigned long word      = 0;
    while (1){
        errno = 0;
        
        word = ptrace(PTRACE_PEEKDATA, pid, addr + offset, NULL);
        if (errno != 0) {return 1;};

        memcpy(buffer + offset, &word, sizeof(word));

        if (memchr(&word, 0, sizeof(word)) != NULL) break;
        
        offset += sizeof(word);

        if (offset >= max_size) break;

    }

    if (offset >= max_size )     buffer[max_size - 1] = '\0';
    else                         buffer[offset]       = '\0';

    return 0;
}


int decode_str(pid_t pid, unsigned long addr, char* buffer, size_t max_size)
{
    if (!buffer || max_size == 0) return -1;

    buffer[0] = '\0';
    if (addr == 0) return -1;

    if (fast_decode(pid, addr, buffer, max_size) == 0) return 0;

    return deep_decode(pid, addr, buffer, max_size);
}

