#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// TODO: path command
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

    char *saveptr;
    char *tok = strtok_r(ln, " \n", &saveptr);
    do {
        addarg(args, tok);
        tok = strtok_r(NULL, " \n", &saveptr);
    } while (tok != NULL);
    return args;
}

char *chkredir(struct args *args)
{
    char **argv = args->argv;
    // TODO: check for multiple symbols, if present raise an error
    // TODO: also check if > is present in the beginning of a string
    //  return the rest of that string
    for (unsigned long i = 1; i < args->argc; i++) {
        if (strcmp(argv[i], ">") == 0) {
            argv[i] = NULL;
            return argv[i+1];
        }
    }
    return NULL;
}

int execcmd(char *ln)
{
    struct args *args = mkargs(ln);
    char **argv = args->argv;
    if (argv[0] == NULL) {
        free(args);
        return -1;
    }

    int pid = -1;
    // TODO: pass error if argc > 1
    if (strcmp(argv[0], "exit") == 0)
        exit(0);
    // TODO: handle error from chdir
    else if (strcmp(argv[0], "cd") == 0) {
        if (args->argc > 1)
            chdir(argv[1]);
    }
    // TODO: implement for path
    //else if (strcmp(argv[0], "path") == 0)
    else {
        char *fredir = chkredir(args);
        pid = fork();
        if (pid == 0) {
            if (fredir != NULL) {
                close(STDOUT_FILENO);
                dup2(open(fredir, O_RDWR | O_CREAT, 0644), STDOUT_FILENO);
            }
            execvp(argv[0], argv);
            exit(1);
        }
    }
    free(argv);
    free(args);
    return pid;
}

struct cmds {
    int *pids;
    unsigned long count;
    size_t sz;
};

int addpid(struct cmds *cmds, int pid)
{
    if (pid < 0) return 0;
    if (cmds->count > cmds->sz) {
        cmds->sz *= 2;
        cmds->pids = realloc(cmds->pids, cmds->sz * sizeof(int));
    }
    cmds->pids[cmds->count++] = pid;
    return 1;
}

void execln(char *ln)
{
    struct cmds cmds = {.count = 0, .sz = 5};
    cmds.pids = calloc(cmds.sz, sizeof(int));
    char *saveptr;
    char *tok = strtok_r(ln, "&", &saveptr);
    do {
        addpid(&cmds, execcmd(tok));
        tok = strtok_r(NULL, "&", &saveptr);
    } while (tok != NULL);
    for (unsigned long i = 0; i < cmds.count; i++) {
        waitpid(cmds.pids[i], NULL, 0);
    }
    free(cmds.pids);
}

void prompt()
{
    size_t bufsz = 255;
    char *buf = malloc(sizeof(char)*bufsz);
    for (;;) {
        fputs("wish> ", stdout);
        getline(&buf, &bufsz, stdin);
        if (feof(stdin)) exit(0);
        execln(buf);
    }
    free(buf);
}

void batch(char *fname)
{
    // TODO: handle error if fname is invalid
    FILE *f = fopen(fname, "r");
    size_t bufsz = 255;
    char *buf = malloc(sizeof(char) * bufsz);
    int num = getline(&buf, &bufsz, f);
    while (num > 0) {
        execln(buf);
        num = getline(&buf, &bufsz, f);
    }
    free(buf);
}

int main(int argc, char *argv[])
{
    switch (argc) {
        case 1: prompt();
        default: batch(argv[1]);
    }
    return 0;
}
