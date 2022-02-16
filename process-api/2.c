#include <fcntl.h>
#include <unistd.h>

#define BUFSZ  255
#define ERR "can't read file\n"

const char * err = ERR;
const int errsz = sizeof(ERR)-1;

// output is child/parent writing 255 bytes of 1.c, then the other
// process writing the last half to stdout
// two processes can share the same file desc
int main()
{
    int fd = open("1.c", O_RDONLY);
    char buf[BUFSZ];
    int pid = fork();
    if (pid < 0) {return -1;}
    int num = read(fd, buf, BUFSZ);
    if (!num) {write(STDOUT_FILENO, err, errsz); return -1;}
    if (!write(STDOUT_FILENO, buf, num)) {return -1;}

    return 0;
}
