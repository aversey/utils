#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *original = NULL;
FILE *translated = NULL;
FILE *result = NULL;


void myexit()
{
    if (original != NULL) fclose(original);
    if (translated != NULL) fclose(translated);
    if (result != NULL) fclose(result);
    exit(0);
}

void print_help()
{
    printf(
        "This program substitutes TeX commands from original.tex\n"
        "into translated.tex, producing result.tex.\n\n"
        "Press enter to close this message:"
    );
    getchar();
    myexit();
}

char skip_until(const char *, FILE *);
char readchar(FILE *f)
{
    int c = fgetc(f);
    if (c == EOF) myexit();
    if (c == '%') return skip_until("\r\n", f);
    return c;
}

char copychar(FILE *f, FILE *out)
{
    char c = readchar(f);
    fputc(c, out);
    return c;
}

char skip(const char *str, FILE *f)
{
    int fc;
    do {
        fc = readchar(f);
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
        fc = readchar(f);
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
        fc = copychar(f, out);
    } while (strchr(str, fc) == NULL);
    return fc;
}

char copy_until_chr(char c, FILE *f, FILE *out)
{
    char str[2] = { c, 0 };
    return copy_until(str, f, out);
}

void work()
{
    char o = skip_until("$\\", original);
    copy_until_chr(o, translated, result);
    if (o == '\\') {
        o = readchar(original);
        fputc(o, result);
        if (o == '[') {
            copy_until("]", original, result);
            skip_until("]", translated);
        } else if (o == '(') {
            copy_until(")", original, result);
            skip_until(")", translated);
        } else {
            o = copy_until(" \n\t\r{}", original, result);
            skip_until_chr(o, translated);
            if (o == '{') {
                copy_until("}", translated, result);
                skip_until("}", original);
            }
        }
    } else if (o == '$') {
        copy_until("$", original, result);
        skip_until("$", translated);
    }
}

int main(int argc, char **argv)
{
    /* No args expected: */
    if (argc != 1) print_help();
    /* Open the files: */
    original = fopen("original.tex", "rb");
    if (original == NULL) {
        puts("Cannot open oringinal.tex.");
        print_help();
    }
    translated = fopen("translated.tex", "rb");
    if (translated == NULL) {
        puts("Cannot open translated.tex.");
        print_help();
    }
    result = fopen("result.tex", "wb");
    if (result == NULL) {
        puts("Cannot open result.tex.");
        print_help();
    }
    /* Main loop, exit will be called from work: */
    while (1) work();
}
