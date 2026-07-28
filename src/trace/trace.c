#include "trace.h"

Tracee* getTracee(TraceeList* list, pid_t pid)
{
    for (size_t i = 0; i < list->used_size; ++i){
        if (list->tracees[i].pid == pid) return &list->tracees[i];
    }
    return NULL;
}

Tracee* addTracee(TraceeList* list, pid_t pid, pid_t parent_pid, short is_thread)
{
    if (list->used_size == list->length) {
        size_t new_length = list->length * 2;
        Tracee* new_tracees = realloc(list->tracees, new_length * sizeof(Tracee));
        if (!new_tracees) {
            perror("Failed to allocate memory for tracees");
            exit(EXIT_FAILURE);
        }
        list->tracees = new_tracees;
        list->length = new_length;
    }

    Tracee* new_tracee = &list->tracees[list->used_size++];
    new_tracee->pid = pid;
    new_tracee->parent_pid = parent_pid;
    new_tracee->is_thread = is_thread;
    new_tracee->in_syscall = 0;
    new_tracee->alive = 1;

    return new_tracee;
}

void traceeRemove(TraceeList* list, pid_t pid)
{
    for (size_t i = 0; i < list->used_size; ++i){
        if (list->tracees[i].pid == pid){
            list->tracees[i] = list->tracees[--list->used_size];
            return;
        }
    }
}

void freeTraceeList(TraceeList* list)
{
    free(list->tracees);
    list->tracees = NULL;
    list->used_size = 0;
    list->length = 0;
}



void syscall_entry(pid_t pid, struct user_regs_struct *regs)
{
    char line[1024];

    syscall_format_entry(pid, regs, line, sizeof(line));
    color(COL_CYAN);
    fputs(line, stdout);
    color(COL_RESET);
    fputc('\n', stdout);
    fflush(stdout);

}

void syscall_exit(struct user_regs_struct *regs, TableElement* t)
{   
    if ((long long)SYSCALL_RET < 0){
        t->errs++;
        fprintf(stderr, "[DEBUG] retval=%lld\n", (long long)SYSCALL_RET);

        if (g_showErrs){

            color(COL_RED);
            fprintf(stderr, "   => error  %lld: %s\n",
                  -SYSCALL_RET,
                  strerror((int)-SYSCALL_RET)
            );
            color(COL_RESET);
        }
    }
}

void trace_loop(TraceeList *tracees, List *stats)
{
    int status;

    while (tracees->used_size > 0) {

        pid_t pid = waitpid(-1, &status, 0);
        if (pid < 0) break;

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            traceeRemove(tracees, pid);
            continue;
        }

        // event handling
        int ev = status >> 8;
        if (ev == (SIGTRAP | (PTRACE_EVENT_FORK  << 8)) ||
            ev == (SIGTRAP | (PTRACE_EVENT_VFORK << 8)) ||
            ev == (SIGTRAP | (PTRACE_EVENT_CLONE << 8))) {

            unsigned long new_pid;
            ptrace(PTRACE_GETEVENTMSG, pid, 0, &new_pid);
            short is_thread = (ev == (SIGTRAP | (PTRACE_EVENT_CLONE << 8)));
            addTracee(tracees, (pid_t)new_pid, pid, is_thread);
            ptrace(PTRACE_SYSCALL, pid, 0, 0);
            ptrace(PTRACE_SYSCALL, (pid_t)new_pid, 0, 0);
            continue;
        }

        if (ev == (SIGTRAP | (PTRACE_EVENT_EXEC << 8))) {
            // Tracee *tr = getTracee(tracees, pid);
            // if (tr) tr->in_syscall = 0;
            ptrace(PTRACE_SYSCALL, pid, 0, 0);
            continue;
        }
        else if (ev == (SIGTRAP | (PTRACE_EVENT_EXIT << 8))) {
            Tracee *tr = getTracee(tracees, pid);
            if (tr) tr->alive = 0;
            ptrace(PTRACE_SYSCALL, pid, 0, 0);
            continue;
        }


        int sig = WSTOPSIG(status);
        if (sig != (SIGTRAP | 0x80)) {
            ptrace(PTRACE_SYSCALL, pid, 0, sig);
            continue;
        }

        Tracee *tr = getTracee(tracees, pid);
        if (!tr) {
            tr = addTracee(tracees, pid, 0, 0);
        }

        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, 0, &regs);

        TableElement *t = findElementById(stats, regs.orig_rax);
        if (!t) {
            TableElement new_elem = {
                .syscall_id   = regs.orig_rax,
                .syscall_name = syscall_names_arr[regs.orig_rax],
                .calls        = 0,
                .errs         = 0,
                .time_spent   = 0
            };
            appendToList(stats, new_elem);
            t = &stats->elements[stats->used_size - 1];
        }

        if (!tr->in_syscall) {
            clock_gettime(CLOCK_MONOTONIC, &tr->call_start);
            syscall_entry(pid, &regs);
            t->calls++;
            tr->in_syscall = 1;
        } else {
            struct timespec time_end;
            clock_gettime(CLOCK_MONOTONIC, &time_end);
            t->time_spent += (double)(time_end.tv_sec  - tr->call_start.tv_sec)
                           + (time_end.tv_nsec - tr->call_start.tv_nsec) / 1e9;
            syscall_exit(&regs, t);
            tr->in_syscall = 0;
        }

        ptrace(PTRACE_SYSCALL, pid, 0, 0);
    }
}


void start_trace(char *prog, char **argv)
{
    List stats;
    initList(&stats, DEFAULT_LIST_SIZE);

    TraceeList tracees;
    tracees.tracees   = malloc(16 * sizeof(Tracee));
    tracees.length    = 16;
    tracees.used_size = 0;

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); exit(1); }

    if (pid == 0) {
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1) { perror("ptrace"); exit(1); }
        raise(SIGSTOP);
        execvp(prog, argv);
        perror("execvp");
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);

    ptrace(PTRACE_SETOPTIONS, pid, 0,
           PTRACE_O_TRACEFORK  |
           PTRACE_O_TRACEVFORK |
           PTRACE_O_TRACECLONE |
           PTRACE_O_TRACEEXEC  |
           PTRACE_O_TRACEEXIT  |
           PTRACE_O_TRACESYSGOOD
        );

    ptrace(PTRACE_SYSCALL, pid, 0, 0);
    addTracee(&tracees, pid, 0, 0);

    trace_loop(&tracees, &stats);

    if (g_showTable) print_stats_table(&stats);

    freeTraceeList(&tracees);
    freeList(&stats);
}

