#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include "io.h"

#define BUFSIZE 1024

char is_vowel(char ch)
{
    static const char* vowel = "eyuio";
    ch = tolower(ch);
    for(int i = 0; i < sizeof(vowel); i++)
        if(ch == vowel[i])
            return 1;
    
    return 0;
}

ssize_t filter(char *bufin, char *bufout, ssize_t bufin_bytes)
{
    ssize_t bufout_bytes = 0;
    for(ssize_t i = 0; i < bufin_bytes; i++)
    {
        if(!is_vowel(bufin[i]))
            bufout[bufout_bytes++] = bufin[i];
    } 
    return bufout_bytes;
}

int main()
{
    char bufin[BUFSIZE];
    char bufout[BUFSIZE];
    ssize_t read_bytes;
    while ((read_bytes = read(STDIN_FILENO, bufin, sizeof(char) * BUFSIZE)) > 0)
    {
        ssize_t filted_bytes = filter(bufin, bufout, read_bytes);
        if(write_str(STDOUT_FILENO, bufout, filted_bytes) == -1)
        {
            log_error_and_exit("Ошибка записи в файл");
        }
    }
    return 0;
}