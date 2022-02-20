#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// TODO: exit, cd, path commands
// TODO: shell redirection
// TODO: parallel commands with &
// TODO: error handling
// TODO: custom paths

struct args {
    char **argv;
    unsigned long argc;
    size_t sz;
};

int addarg(struct args *args, char *arg)
{
    if (arg == NULL) return 0;
    if (args->argc > args->sz) {
        args->sz *= 2;
        args->argv = realloc(args->argv, args->sz * sizeof(char *));
    }
    args->argv[args->argc++] = arg;
    return 1;
}

struct args *mkargs(char * ln)
{
    struct args *args = malloc(sizeof(struct args));
    args->argc = 0;
    args->sz = 5;
    args->argv = calloc(args->sz, sizeof(char *));

    char * saveptr;
    char * tok = strtok_r(ln, " \n", &saveptr);
    do {
        addarg(args, tok);
        tok = strtok_r(NULL, " \n", &saveptr);
    } while (tok != NULL);
    return args;
}

void execln(char * ln)
{
    struct args * args = mkargs(ln);
    if (args->argv[0] == NULL) {
        free(args);
        return;
    }

    // TODO: pass error if argc > 1
    if (strcmp(args->argv[0], "exit") == 0)
        exit(0);
    // TODO: handle error from chdir
    else if (strcmp(args->argv[0], "cd") == 0) {
        if (args->argc > 1)
            chdir(args->argv[1]);
    }
    // TODO: implement for path
    //else if (strcmp(argv[0], "path") == 0)
    else {
        // TODO: check all argv for redirection
        int pid = fork();
        switch (pid) {
            case -1:
                break;
            case 0:
                execvp(args->argv[0], args->argv);
                exit(1);
            default:
                wait(NULL);
                break;
        }
    }
    free(args->argv);
    free(args);
}

void prompt()
{
    size_t bufsz = 255;
    char * buf = malloc(sizeof(char)*bufsz);
    for (;;) {
        fputs("wish> ", stdout);
        getline(&buf, &bufsz, stdin);
        if (feof(stdin)) exit(0);
        execln(buf);
    }
    free(buf);
}

void batch(char * fname)
{
    FILE * f = fopen(fname, "r");
    size_t bufsz = 255;
    char * buf = malloc(sizeof(char)*bufsz);
    int num = getline(&buf, &bufsz, f);
    while (num > 0) {
        execln(buf);
        num = getline(&buf, &bufsz, f);
    }
    free(buf);
}

int main(int argc, char * argv[])
{
    switch (argc) {
        case 1: prompt();
        default: batch(argv[1]);
    }
    return 0;
}
