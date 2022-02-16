#include <fcntl.h>
#include <unistd.h>

#define OUT "text for problem 2\n"

const char * out = OUT;
const int outsz = sizeof(OUT)-1;

// two processes writing to a file concurrently
// results in one blocking the other while they write to that file
// output:
//  text for problem 2
//  text for problem 2
int main()
{
    int fd = open("2-out", O_CREAT | O_RDWR, S_IRWXU);
    fork();
    int num = write(fd, out, outsz);
    if (!num) {return -1;}
    return 0;
}
