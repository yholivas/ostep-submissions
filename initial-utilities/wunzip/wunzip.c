#include <stdio.h>

int main(int argc, char * argv[])
{
    if (argc < 2) {puts("wunzip: file1 [file2 ...]"); return 1;}

    FILE * f;
    int freq;
    char c;
    for (int i = 1; i < argc; i++) {
        f = fopen(argv[i], "r");
        int num = 1;
        while (num > 0) {
            num = fread(&freq, sizeof(int), 1, f);
            fread(&c, sizeof(char), 1, f);
            while (freq > 0) {
                putchar(c);
                freq--;
            }
        }
        fclose(f);
    }
    return 0;
}
