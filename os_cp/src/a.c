#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "io.h"

// from A
int pipeAC[2];
int pipeAB[2];
// from C
int pipeCA[2];
int pipeCB[2];

pid_t pid_b, pid_c;

void pipe_wrapper(int fds[2]){
    if(pipe(fds) == 0)
        return;
    
    perror("Ошибка создания pipe");
    exit(-1);
}

void close_wrapper(int fd) {
    if(close(fd) == -1) {
        perror("Ошибка закрытия файлового дескриптора");
        exit(-1);
    }
}

void init_pipes() {
    pipe_wrapper(pipeAC);
    pipe_wrapper(pipeAB);

    pipe_wrapper(pipeCA);
    pipe_wrapper(pipeCB);
}

void fork_b_wrapper() {
    pid_t pid = fork();
    if(pid == -1) {
        perror("Ошибка fork для процесса b");
        exit(-1);
    }
    else if(pid == 0) {
        char fd1[3], fd2[3];
        sprintf(fd1, "%d", pipeAB[0]);
        sprintf(fd2, "%d", pipeCB[0]);

        close_wrapper(pipeAB[1]);
        close_wrapper(pipeCB[1]);

        if(execl("b", "b", fd1, fd2, NULL) == -1) {
            perror("Ошибка execl для процесса b");
        }
    }
    pid_b = pid;
    close_wrapper(pipeAB[0]);
    close_wrapper(pipeCB[0]);
}

void fork_c_wrapper() {
    pid_t pid = fork();
    if(pid == -1) {
        perror("Ошибка fork для процесса c");
        exit(-1);
    }
    else if(pid == 0) {
        char fd1[3], fd2[3], fd3[3];
        sprintf(fd1, "%d", pipeAC[0]);
        sprintf(fd2, "%d", pipeCA[1]);
        sprintf(fd3, "%d", pipeCB[1]);

        close_wrapper(pipeAC[1]);
        close_wrapper(pipeCA[0]);

        if(execl("c", "c", fd1, fd2, fd3, NULL) == -1) {
            perror("Ошибка execl для процесса c");
        }
    }
    pid_c = pid;
    close_wrapper(pipeAC[0]);
    close_wrapper(pipeCA[1]);
    close_wrapper(pipeCB[1]);
}

void init_processes() {
    // process B
    fork_b_wrapper();
    // process C
    fork_c_wrapper();
}

void run() {
    int to_c = pipeAC[1];
    int to_b = pipeAB[1];
    int from_c = pipeCA[0];

    char* str;
    int len;
    while(str = get_line(&len), len > 0) {
        if(write_str(to_c, (char*)&len, sizeof(len)) == -1){
            perror("Ошибка записи");
            exit(-1);
        }
        if(write_str(to_c, str, len + 1) == -1){
            perror("Ошибка записи");
            exit(-1);
        }
        if(write_str(to_b, (char*)&len, sizeof(len)) == -1){
            perror("Ошибка записи");
            exit(-1);
        }
        free(str);

        //Ожидание ответа о получении сообщения
        int msg = 0;
        if(read_str(from_c, (char*)&msg, sizeof(msg)) == -1) {
            perror("Ошибка записи");
            exit(-1);
        }
    }
}

void deinit() {
    kill(pid_c, SIGTERM);
    kill(pid_b, SIGTERM);
}

int main() {
    init_pipes();
    init_processes();
    run();
    deinit();
    return 0;
}