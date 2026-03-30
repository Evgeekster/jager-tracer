#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "trace.h"
#include "pch.h"

bool showErrs = false;
bool showTable = false;




int main(int argc, char* argv[]){

    if (argc < 2){
        printf("Usage is: program [args]\n");
        return 1;
    }

    for (size_t i = 1; argc && argv[i][0] == '-'; ++i){
        switch (argv[i][1])
        {
        case 'h':
            printf("-e: Show error during syscall\n");
            return 0;
            break;
        case 'e':
            showErrs = true;
            break;
        default:
            break;
        }
    }

    start_trace(argv[1], &argv[1]);
    return 0;
}