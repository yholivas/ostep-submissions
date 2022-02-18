#include <stdio.h>

int main(int argc, char * argv[])
{
    if (argc < 2) {puts("wzip: file1 [file2 ...]"); return 1;}
    FILE * f = fopen(argv[1], "r");
    char c = fgetc(f);
    int freq = 0;
    fclose(f);
    for (int i = 1; i < argc; i++) {
        FILE * f = fopen(argv[i], "r");
        char c_new = fgetc(f);
        while (c_new != EOF) {
            if (c == c_new) {
                freq++;
            } else {
                fwrite(&freq, sizeof(int), 1, stdout);
                fwrite(&c, sizeof(char), 1, stdout);
                freq = 1;
                c = c_new;
            }
            c_new = fgetc(f);
        }
        fclose(f);
    }
    fwrite(&freq, sizeof(int), 1, stdout);
    fwrite(&c, sizeof(char), 1, stdout);
    return 0;
}
