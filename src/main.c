#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "trace.h"
#include "pch.h"

bool g_showErrs     = false;
bool g_showTable    = true;
bool g_color        = false;

void color(const char *c) {
    if (g_color) fputs(c, stdout);
}

static void usage(const char *argv0)
{
    fprintf(stderr,
        "Usage: %s [options] <program> [args...]\n"
        "\n"
        "Options:\n"
        "  -e        Print error message on failed syscalls\n"
        "  -S        Suppress the summary stats table\n"
        "  -c        Enable ANSI colour output\n"
        "  -h        Show this help and exit\n"
        "\n"
        "Example:\n"
        "  %s -e -c ls -la /tmp\n",
        argv0, argv0);
}

int main(int argc, char* argv[]){

    if (argc < 2){
        usage(argv[0]);
        return 1;
    }

    for (size_t i = 1; argc && argv[i][0] == '-'; ++i){
        switch (argv[i][1])
        {
        case 'h':
            usage(argv[0]);
            return 0;
            break;
        case 'e':
            g_showErrs = true;
            break;
        case 'S':
            g_showTable = false;
        
        default:
            break;
        }
    }

    if (!g_color && isatty(STDOUT_FILENO)) g_color = true;

    start_trace(argv[1], &argv[1]);

    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);                    // Get current time
    timeinfo = localtime(&rawtime);    // Convert to local time structure

    // Format: "YYYY-MM-DD HH:MM:SS"
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("Trace finished at: %s\n", buffer);
    return 0;
}