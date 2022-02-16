#include <stdio.h>
#include <unistd.h>

// closing stdout means the child can't use stdout
int main()
{
    int pid = fork();

    if (pid < 0) {
        return -1;
    } else if (pid == 0) {
        close(STDOUT_FILENO);
        puts("should not work");
    }

    return 0;
}
