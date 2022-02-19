#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char ** mkargv(char * ln)
{
    int argc = 0;
    size_t argvsz = 5;
    char ** argv = calloc(argvsz, sizeof(char*));

    char * tok = strtok(ln, " \n");
    do {
        argv[argc++] = tok;
        tok = strtok(NULL, " \n");
    } while (tok != NULL);
    return argv;
}

void execln(char * ln)
{
    int pid = fork();
    char ** argv;
    switch (pid) {
        case -1:
            break;
        case 0:
            argv = mkargv(ln);
            execvp(argv[0], argv);
        default:
            wait(NULL);
            break;
    }
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
    // argv[1] should be the batch file
    // go through batch file line by line
    // execute those programs with fork and exec
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
