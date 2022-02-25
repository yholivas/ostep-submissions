#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define ERR()   do {\
                fputs("An error has occurred\n", stderr);\
                } while (0)

#define PATHSZ 5
#define PATHLEN 50
char paths[PATHSZ][PATHLEN];

struct args {
    char **argv;
    unsigned long argc;
    size_t sz;
};

int addarg(struct args *args, char *arg)
{
    if (arg == NULL) return 0;
    if (args->argc >= args->sz) {
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

int redir(char * fname)
{
    if (fname == NULL) return 0;
    int fd = open(fname, O_RDWR | O_CREAT, 0644);
    if (fd < 0) return -1;
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    if (dup2(fd, STDOUT_FILENO) < 0)
        return -1;
    if (dup2(fd, STDERR_FILENO) < 0)
        return -1;
    return 0;
}

struct proc {
    char *cmd;
    char *redir;
};

int getredir(char *str, struct proc *proc)
{
    char *saveptr;
    char *tok = strtok_r(str, " \n", &saveptr);
    if (tok == NULL) {
        ERR();
        return -1;
    }
    proc->redir = tok;
    if (strtok_r(NULL, " \n", &saveptr) != NULL) {
        ERR();
        return -1;
    }
    return 0;
}

int chkredir(char *ln, struct proc *proc)
{
    proc->cmd = strsep(&ln, ">");
    if (ln != NULL)
        return getredir(ln, proc);
    return 0;
}

char exebuf[100];

char *getexe(char * cmd)
{
    for (int i = 0; i < PATHSZ; i++) {
        if (paths[i] == NULL) break;
        if (strlen(cmd) + strlen(paths[i]) < 99) {
            strcpy(exebuf, paths[i]);
            strcat(exebuf, "/");
            strcat(exebuf, cmd);
            if (access(exebuf, X_OK) == 0)
                return exebuf;
        }
    }
    ERR();
    return NULL;
}

int execcmd(char *ln)
{
    struct proc proc = {.cmd = NULL, .redir = NULL};
    if (chkredir(ln, &proc) < 0) return -1;
    struct args *args = mkargs(proc.cmd);
    char **argv = args->argv;
    if (argv[0] == NULL) {
        if (proc.redir != NULL)
            ERR();
        free(argv);
        free(args);
        return -1;
    }

    int pid = -1;
    if (strcmp(argv[0], "exit") == 0) {
        if (args->argc > 1) {
            ERR();
        } else {
            exit(0);
        }
    } else if (strcmp(argv[0], "cd") == 0) {
        if (args->argc != 2 || chdir(argv[1]) < 0) ERR();
    } else if (strcmp(argv[0], "path") == 0) {
        for (unsigned long i = 0; i < PATHSZ; i++) {
            if (i >= args->argc-1)
                strcpy(paths[i], "");
            else if (strlen(argv[i+1]) < 50)
                strcpy(paths[i], argv[i+1]);
        }
    } else {
        // look up executable from argv[0]
        char * exe = getexe(argv[0]);
        pid = fork();
        if (pid == 0) {
            if (exe != NULL && redir(proc.redir) == 0)
                execv(exe, argv);
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

void batch(char *fname, int argc)
{
    if (argc > 2) {ERR(); exit(1);}
    FILE *f = fopen(fname, "r");
    if (f == NULL) {ERR(); exit(1);}
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
    strcpy(paths[0], "/bin");
    switch (argc) {
        case 1: prompt();
        default: batch(argv[1], argc);
    }
    return 0;
}
