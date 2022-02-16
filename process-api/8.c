#include <unistd.h>

// output:
//  int main()
//  int x = 100;
//  printf("x: %d\n", x);
//  int pid = fork();
//  printf("x: %d\n", x);
// piped source of 1.c into grep for "int"
int main()
{
    int fd[2];
    int stat = pipe(fd);
    if (stat == -1) {return stat;}

    int pid1 = fork();
    if (pid1 < 0) {
        return -1;
    } else if (pid1 == 0) {
        close(fd[0]);
        close(STDOUT_FILENO);
        dup2(fd[1], STDOUT_FILENO);
        char * argv[3] = {"cat", "1.c", NULL};
        return execvp("cat", argv);
    }

    int pid2 = fork();
    if (pid2 < 0) {
        return -1;
    } else if (pid2 == 0) {
        close(fd[1]);
        close(STDIN_FILENO);
        dup2(fd[0], STDIN_FILENO);
        char * argv[3] = {"grep", "int", NULL};
        return execvp("grep", argv);
    }

    return 0;
}
