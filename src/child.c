#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "io.h"

#define BUFSIZE 1024
int main()
{
    char buf[BUFSIZE];
    ssize_t read_bytes;
    while ((read_bytes = read(STDIN_FILENO, buf, sizeof(char) * BUFSIZE)) > 0)
    {
        if(write_str(STDOUT_FILENO, buf, read_bytes) == -1)
        {
            log_error_and_exit("Ошибка записи в файл");
        }
    }
    return 0;
}