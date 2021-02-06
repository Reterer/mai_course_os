#include <stdio.h>
#include <stdarg.h>

#include "io.h"

int read_from_readed_buffer(char* out_str, int nbytes) {
    static const int BUF_MAX_SIZE = 4096;
    static char buf[4096]; // Здесь значение BUF_MAX_SIZE // Он думает, что BUF_MAX_SIZE не константное значение
    static int size = 0;
    static int pos = 0;

    int res_len = nbytes;
    while(nbytes > 0) {
        if(size == pos) {
            pos = 0;
            size = read(STDIN_FILENO, buf, BUF_MAX_SIZE);
            if(size < 0)
                return size;
            if(size == 0)
                break;
        }
        *out_str++ = buf[pos++];
        nbytes--;
    }

    return res_len - nbytes;
}

int get_line(char* str, int max_size, char end) {
    int str_len = 0;
    int max_size_str = max_size - 1;

    for(;str_len < max_size_str; str_len++) {
        if(read_from_readed_buffer(&str[str_len], 1) == 0)
            break;
        if(str[str_len] == end)
            break;
    }
    str[str_len] = '\0';

    return str_len;
}

int write_str(int fd, char* format, ...) {
    const int BUF_MAX_SIZE = 1024;
    char buf[BUF_MAX_SIZE];

    va_list args;
    va_start(args, format);
    int len = vsnprintf(buf, BUF_MAX_SIZE, format, args);
    va_end(args);

    int writed_bytes = 0;
    while(len > 0) {
        int writed = write(fd, buf + writed_bytes, len);
        if(write < 0)
            return -1;
        writed_bytes += writed;
        len -= writed;
    }

    return writed_bytes;
}