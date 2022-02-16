#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

// output:
//  x: 100
//  x: 200
//  x: 100
// child process and parent process have their own variables after forking
int main()
{
    int x = 100;
    printf("x: %d\n", x);
    int pid = fork();
    if (pid < 0) {return -1;}
    else if (pid == 0) {
        x = 200;
    } else {
        wait(NULL);
    }
    printf("x: %d\n", x);

    return 0;
}
