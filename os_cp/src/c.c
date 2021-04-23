#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "io.h"

int main(int argc, char* argv[]) {
    int from_a, to_a, to_b;
    from_a = atoi(argv[1]);
    to_a = atoi(argv[2]);
    to_b = atoi(argv[3]);

    
    bool run = true;
    while(run){
        int len;
        if(read_str(from_a, (char*)&len, sizeof(len)) == -1){
            perror("Ошибка записи");
            exit(-1);
        }
        char* str = malloc(len+1);
        if(read_str(from_a, str, len+1) == -1){
            perror("Ошибка записи");
            exit(-1);
        }
        if(write_str(1, str, len) == -1){
            perror("Ошибка записи");
            exit(-1);
        }
        free(str);

        //Отправить принятый размер
        if(write_str(to_b, (char*)&len, sizeof(len)) == -1) {
            perror("Ошибка записи");
            exit(-1);
        }
        //Отправить сообщение, что все ОК
        int msg = 0;
        if(write_str(to_a, (char*)&msg, sizeof(msg)) == -1) {
            perror("Ошибка записи");
            exit(-1);
        }
    }   

    return 0;
}