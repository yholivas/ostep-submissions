#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define FILE_ERR(x) do {    \
                        fprintf(stderr, "reverse: cannot open file '%s'\n", x);\
                        exit(1);\
                    } while (0)

int fmatch(char * f1, char * f2)
{
    struct stat s1, s2;
    if(stat(f1, &s1) != 0) return 0;
    if(stat(f2, &s2) != 0) return 0;
    return s1.st_ino == s2.st_ino;
}

int main(int argc, char * argv[])
{
    if (argc > 3) {
        fputs("usage: reverse <input> <output>\n", stderr);
        return 1;
    }

    FILE * fin = stdin;
    FILE * fout = stdout;
    if (argc > 1) {
        fin = fopen(argv[1], "r");
        if (fin == NULL) {FILE_ERR(argv[1]);}
    }
    if (argc == 3) {
        if (fmatch(argv[1], argv[2])) {
            fputs("reverse: input and output file must differ\n", stderr);
            return 1;
        }
        fout = fopen(argv[2], "w");
        if (fout == NULL) {FILE_ERR(argv[2]);}
    }

    size_t bufsz;
    char ** bufs = calloc(255, sizeof(char*));
    int curbuf = -1;
    int num = 1;
    while (num > 0) num = getline(&bufs[++curbuf], &bufsz, fin);
    while (curbuf >= 0) {
        fputs(bufs[curbuf], fout);
        free(bufs[curbuf--]);
    }

    free(bufs);
    fclose(fin);
    fclose(fout);
}
