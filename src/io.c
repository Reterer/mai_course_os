#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"

int write_str(int fd, const char *msg, int len)
{
    ssize_t write_bytes = 0;
    while(write_bytes != len)
    {
        ssize_t w = write(fd, msg, len-write_bytes);
        if(w == -1)
            return -1;
        write_bytes += w;
    }
    return 0;
}

char* get_line(int *len)
{
    *len = 0;
    int cap = 4;
    char *line = (char*)malloc(sizeof(char)*cap);
    if(line == NULL)
    {
     log_error("Не удалось выделить память под строку");    
    }
    
    int ch;
    while((ch = getchar()) != EOF)
    {
        line[(*len)++] = ch;
        if(*len >= cap)
        {
            cap *= 2;
            char *new_line = (char*)realloc(line, cap);
            if(new_line == NULL)
            {
                log_error("Не удалось увеличить размер строчки");
                *len = cap-1;
                break;
            }
        line = new_line;
        }
        if(ch == '\n')
        break;
    }
    line[*len] = '\0';
    return line;
}

void log_error_and_exit(const char *msg)
{
    // fprintf(stderr, "%s\n", msg);
    perror(msg);
    exit(1);
}

void log_error(const char *msg)
{
    perror(msg);
}

void log_message(const char* msg)
{
    write_str(STDERR_FILENO, msg, strlen(msg));
    // fprintf(stderr, "%s", msg);
}