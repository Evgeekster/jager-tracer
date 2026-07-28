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

// int decode_sockaddr(pid_t pid, unsigned long addr, char* buffer, size_t max_size){
//     struct socketaddr sa;

    
//     // for now just decode the whole struct as a string of bytes
//     return decode_str(pid, addr, buffer, max_size);
// }


/* to decode memory without anything else/ WOuld be used to decode binary strings and *buf to be used later */
int decode_mem(pid_t pid, unsigned long addr, void* buffer, size_t max_size){
    if (!buffer || max_size == 0) return -1;
    struct iovec local  =    {    .iov_base = buffer,        .iov_len = max_size - 1};
    struct iovec remote =    {    .iov_base = (void*)addr,   .iov_len = max_size - 1};
    

    ssize_t n = process_vm_readv(
        pid, &local, 1, &remote, 1, 0
    );

    if (n < 0){
        return -1; // error
    }
    return 0;
}
