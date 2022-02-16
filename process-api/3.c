#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

// "You should try to ensure that the child process always prints first; can you do
//  this without calling wait() in the parent?"
// a: I don't think so :) (calling sleep doesn't count)
int main()
{
    int pid = fork();
    if (pid < 0) {return -1;}
    if (pid == 0) {
        puts("hello");
    } else {
        wait(NULL);
        puts("goodbye");
    }

    return 0;
}
