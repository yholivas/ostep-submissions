#include <fcntl.h>
#include <unistd.h>

#define ERR "wcat: cannot open file\n"

const char * err = ERR;
const int err_sz = sizeof(ERR)-1;

int main(int argc, char * argv[])
{
    char buf[255];
    if (argc < 2) {return 0;}

    for (int i = 1; i < argc; i++)  {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {write(STDOUT_FILENO, err, err_sz); return 1;}
        
        int num = read(fd, buf, 255);
        write(STDOUT_FILENO, buf, num);
        while (num > 0) {
            num = read(fd, buf, 255);
            write(STDOUT_FILENO, buf, num);
        }
        close(fd);
    }

    return 0;
}
