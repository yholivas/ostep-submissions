#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    if (argc < 2) {puts("wgrep: searchterm [file ...]"); return 1;}
    FILE * f;
    if (argc == 2) f = stdin;
    else f = fopen(argv[2], "r");
    if (f == NULL) {puts("wgrep: cannot open file"); return 1;}

    char * st = argv[1];
    unsigned long sz = 255;
    char * buf = malloc(sizeof(char)*sz);
    int num = getline(&buf, &sz, f);
    while (num > 0) {
        int match = 0;
        char * ln_ptr = buf;
        char * s_ptr = st;
        while (*ln_ptr != '\n' && !match) {
            if (*ln_ptr == *s_ptr) {
                if (*(s_ptr+1) == '\0') match = 1;
                else s_ptr++;
            } else s_ptr = st;
            ln_ptr++;
        }
        if (match) write(STDOUT_FILENO, buf, num);
        num = getline(&buf, &sz, f);
    }
    free(buf);

    return 0;
}
