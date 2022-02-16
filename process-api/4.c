#include <sys/wait.h>
#include <unistd.h>

// l: use argument list in function call
// v: pass in argv
// p: look up executable in PATH
// e: set up environment variable
int main()
{
    int wstatus = 0;
    int fd = fork();
    if (fd < 0) {
        return -1;
    } else if (fd == 0) {
        char * env[2] = {"LS_COLORS=di=1:fi=34:ln=31:pi=5:so=5:bd=93:cd=36:or=31", NULL};
        return execle("/bin/ls", "/bin/ls", "/dev", "--color=auto", (char *) NULL, env);
        //char * argv[3] = {"ls", "-alth", NULL};
        //return execvp("ls", argv);
    } else {
        wait(&wstatus);
    }

    return WEXITSTATUS(wstatus);
}
