#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_help()
{
    printf(
        "This program substitutes TeX commands from original.tex\n"
        "into translated.tex, producing result.tex.\n\n"
        "Press enter to close this message:"
    );
    getchar();
    exit(0);
}

char skip(const char *str, FILE *f)
{
    int fc;
    do {
        fc = fgetc(f);
        if (fc == EOF) exit(0);
    } while (strchr(str, fc) != NULL);
    return fc;
}

char skip_chr(char c, FILE *f)
{
    char str[2] = { c, 0 };
    return skip(str, f);
}

char skip_until(const char *str, FILE *f)
{
    int fc;
    do {
        fc = fgetc(f);
        if (fc == EOF) exit(0);
    } while (strchr(str, fc) == NULL);
    return fc;
}

char skip_until_chr(char c, FILE *f)
{
    char str[2] = { c, 0 };
    return skip_until(str, f);
}

char copy_until(const char *str, FILE *f, FILE *out)
{
    int fc;
    do {
        fc = fgetc(f);
        if (fc == EOF) exit(0);
        fputc(fc, out);
    } while (strchr(str, fc) == NULL);
    return fc;
}

char copy_until_chr(char c, FILE *f, FILE *out)
{
    char str[2] = { c, 0 };
    return copy_until(str, f, out);
}

void work(FILE *of, FILE *tf, FILE *r)
{
    char o = skip_until("$\\", of);
    copy_until_chr(o, tf, r);
    if (o == '\\') {
        o = copy_until(" \n\t\r{", of, r);
        skip(" ", tf);
        skip_until_chr(o, tf);
        if (o == '{') {
            copy_until("}", tf, r);
            skip_until("}", of);
        }
    } else if (o == '$') {
        int oo = fgetc(of);
        if (oo == EOF) exit(0);
        fputc(oo, r);
        if (oo == '$') {
            do {
                copy_until("$", of, r);
                skip_until("$", tf);
                fgetc(tf);
                oo = fgetc(of);
                if (oo == EOF) exit(0);
                fputc(oo, r);
            } while (oo != '$');
        } else {
            copy_until("$", of, r);
            skip_until("$", tf);
        }
    }
}

int main(int argc, char **argv)
{
    /* No args expected: */
    if (argc != 1) print_help();
    /* Open the files: */
    FILE *original = fopen("original.tex", "rb");
    if (original == NULL) {
        puts("Cannot open oringinal.tex.");
        print_help();
    }
    FILE *translated = fopen("translated.tex", "rb");
    if (translated == NULL) {
        puts("Cannot open translated.tex.");
        print_help();
    }
    FILE *result = fopen("result.tex", "wb");
    if (result == NULL) {
        puts("Cannot open result.tex.");
        print_help();
    }
    /* Main loop, exit will be called from work: */
    while (1) work(original, translated, result);
}
