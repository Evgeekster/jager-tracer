#include "util.h"

void initList(List *l, size_t initSize)
{
    l->elements = malloc(initSize * sizeof(TableElement));
    if (!l->elements){
        perror("malloc failed");
        exit(1);
    }
    l->used_size = 0;
    l->length = initSize;
}

void appendToList(List *l, TableElement element)
{
    if (l->length == l->used_size){
        l->length *= 2;
        l->elements = realloc(l->elements, l->length * sizeof(TableElement));      // i wanna keep it void* till the moment i figure out can i use my list multi-purposly with void**
    }
    l->elements[l->used_size] = element;
    l->used_size++;     // ok = !ok
}

void freeList(List* l)
{
    free(l->elements);
    l->elements = NULL;
    l->length = 0;
    l->used_size = 0;
}

void syscall_print(pid_t pid, struct user_regs_struct *regs, const char *sys_name)
{
    char s_arg1[1024];
    char s_arg2[1024];
    char s_arg3[1024];

    int ok1 = decode_str(pid, SYSCALL_ARG1, s_arg1, sizeof(s_arg1));
    int ok2 = decode_str(pid, SYSCALL_ARG2, s_arg2, sizeof(s_arg2));
    int ok3 = decode_str(pid, SYSCALL_ARG3, s_arg3, sizeof(s_arg3));

    char tmp1[32], tmp2[32], tmp3[32];

    printf("%s(%s, %s, %s);\n",
        sys_name,
        ok1 ? ({ snprintf(tmp1, sizeof(tmp1), "0x%llx", SYSCALL_ARG1); tmp1; }) : s_arg1,
        ok2 ? ({ snprintf(tmp2, sizeof(tmp2), "0x%llx", SYSCALL_ARG2); tmp2; }) : s_arg2,
        ok3 ? ({ snprintf(tmp3, sizeof(tmp3), "0x%llx", SYSCALL_ARG3); tmp3; }) : s_arg3
    );
}


TableElement* findElementById(List* l, int id){

    for (int i = 0; i < l->used_size; ++i){
        if (l->elements[i].syscall_id == id)
            return &l->elements[i];
    }
    return NULL;
}

void print_stats_table(List *stats)
{

    double total_time = 0.0;
    int total_calls = 0;
    int total_errs = 0;

    for (size_t i = 0; i < stats->used_size; i++) {
        total_time += stats->elements[i].time_spent;
        total_calls += stats->elements[i].calls;
        total_errs += stats->elements[i].errs;
    }

    printf("=====================================================================================================\n");
    printf("| %10s | %12s | %10s | %10s | %12s | %-20s |\n",
           "% time", "seconds", "calls", "errors", "avg (ms)", "syscall");
    printf("=====================================================================================================\n");

    for (size_t i = 0; i < stats->used_size; i++) {
        TableElement *e = &stats->elements[i];

        double percent = total_time > 0 ? (e->time_spent / total_time) * 100.0 : 0;
        double avg = e->calls > 0 ? (e->time_spent / e->calls) * 1000.0 : 0;
        double success = e->calls > 0 ? ((double)(e->calls - e->errs) / e->calls) * 100.0 : 0;

        printf("| %10.2f | %12.6f | %10d | %10d | %12.3f | %-20s |\n",
               percent,
               e->time_spent,
               e->calls,
               e->errs,
               avg,
               e->syscall_name);
    }

    printf("=====================================================================================================\n");

    double total_success = total_calls > 0 ?
        ((double)(total_calls - total_errs) / total_calls) * 100.0 : 0;

    printf("| %10s | %12.6f | %10d | %10d | %12s | %-20s |\n",
           "TOTAL",
           total_time,
           total_calls,
           total_errs,
           "",
           "");

    printf("=====================================================================================================\n");
    printf("Success rate: %.2f%%\n", total_success);

}