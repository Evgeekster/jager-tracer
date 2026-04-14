#include "trace.h"

void syscall_entry(pid_t pid, struct user_regs_struct *regs)
{
    // long l_syscall = SYSCALL_NUM;
    char line[1024];
    // const char *sys_name = syscall_names_arr[l_syscall];

    syscall_format_entry(pid, regs, line, sizeof(line));
    color(COL_CYAN);
    fputs(line, stdout);
    color(COL_RESET);
    fputc('\n', stdout);

}

void syscall_exit(struct user_regs_struct *regs, TableElement* t)
{   
    if ((long long)SYSCALL_RET < 0){
        t->errs++;
        if (g_showErrs){

            color(COL_RED);
            fprintf(stderr, "   => error  %lld: %s\n",
                  -SYSCALL_RET,
                  strerror((int)-SYSCALL_RET)
            );
            color(COL_RESET);
        }
        // this is broken.. do i actually need it???
        // else{
        //     color(COL_MAGENTA);
        //     printf("  = %lld\n", SYSCALL_RET);
        //     color(COL_RESET);
        // }
    }
}



void trace_loop(pid_t pid, List* stats)
{
    short in_syscall = 0;
    int status = 0;

    while (1){
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) break;

        struct timespec time_start;
        clock_gettime(CLOCK_MONOTONIC, &time_start);

        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, 0, &regs);

        TableElement* t = findElementById(stats, regs.orig_rax);

        if (!t){
            TableElement new_elem = {
                .syscall_id   = regs.orig_rax,
                .syscall_name = syscall_names_arr[regs.orig_rax],
                .calls        = 1,
                .errs         = 0,
                .time_spent   = 0
            };

            appendToList(stats, new_elem);
            t = &stats->elements[stats->used_size - 1];
        } else {
            t->calls += 1;
        }

        if (!in_syscall){
            syscall_entry(pid, &regs);  
            in_syscall = 1;
        } else {
            syscall_exit(&regs, t);
            in_syscall = 0;
        }

        struct timespec time_end;
        clock_gettime(CLOCK_MONOTONIC, &time_end);

        t->time_spent += (double)(time_end.tv_sec  - time_start.tv_sec)
                       + (time_end.tv_nsec - time_start.tv_nsec) / 1e9;

        ptrace(PTRACE_SYSCALL, pid, 0, 0);
    }
}

void start_trace(char *prog, char **argv)
{
    printf("start_trace\n");
    List stats;
    initList(&stats, DEFAULT_LIST_SIZE);

    pid_t pid = fork();
    if (pid == -1){
        perror("process create error");
        exit(1);
    }

    if (pid == 0){
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1){
            perror("ptrace PTRACE");
            exit(1);
        }
        raise(SIGSTOP);
        execvp(prog, argv);
        perror("execvp");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) == -1){
            perror("ptrace SYSCALL");
            exit(1);
        }

        trace_loop(pid, &stats);
    }
    if (g_showTable)  print_stats_table(&stats);

    freeList(&stats);
}