#include "decode.h"

int decode_str(pid_t pid, unsigned long addr, char* buffer, size_t max_size)
{

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

