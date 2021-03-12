#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include "io.h"
#include "shared.h"

#define BUFSIZE 1024

char is_vowel(char ch)
{
    static const char* vowel = "aeyuio";
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

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        //TODO
    }
    
    SharedContainer shared_container = shared_connect(argv[1]);
    
    char bufin[SHARED_BUF_SIZE];
    char bufout[SHARED_BUF_SIZE];
    int read_bytes;
    while ((read_bytes = shared_buf_read(&shared_container, bufin)) > 0)
    {
        ssize_t filted_bytes = filter(bufin, bufout, read_bytes);
        if(write_str(STDOUT_FILENO, bufout, filted_bytes) == -1)
            log_error_and_exit("Ошибка записи в файл");
    }
    shared_deconnect(&shared_container);
    return 0;
}