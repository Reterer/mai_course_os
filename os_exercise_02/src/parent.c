#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "io.h"

// Имплементирует работу с дочерним процессом
typedef struct
{
    enum
    {
        FP_OK,       // Процесс создан успешно
        FP_ERR_FORK, // Ошибка создания нового процесса
        FP_ERR_PIPE, // Ошибка создания канала
    } status;

    pid_t pid; // PID дочернего процесса
    int fd[2]; // Файловые дескрипторы канала
} FileProc;

// Запускает процесс для работы с файлом file
FileProc run_file_process(const char *file)
{
    FileProc ret;

    if (pipe(ret.fd) == -1)
    {
        ret.status = FP_ERR_PIPE;
        return ret;
    }

    int fd = open(file, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        log_error_and_exit("Ошибка открытия файла процесса");

    if ((ret.pid = fork()) == -1)
    {
        ret.status = FP_ERR_FORK;
        return ret;
    } 
    if (ret.pid == 0)
    {
        close(ret.fd[1]);

        if (dup2(fd, STDOUT_FILENO) == -1)
            log_error_and_exit("Ошибка создания дубликата файлового дескриптора");
        close(fd);

        if (dup2(ret.fd[0], STDIN_FILENO) == -1)
            log_error_and_exit("Ошибка создания дубликата файлового дескриптора");
        close(ret.fd[0]);

        if (execl("child", "child", NULL) == -1)
            log_error_and_exit("Ошибка подмены программы");
    }

    close(fd);
    close(ret.fd[0]);
    ret.status = FP_OK;
    return ret;
}

// Отпроваляет msg длины len процессу.
int write_file_process(const FileProc *fp, const char *msg, size_t len)
{
    return write_str(fp->fd[1], msg, len);
}

// Завершает работу дочернего процесса
void close_file_process(const FileProc *fp)
{
    close(fp->fd[1]);
    waitpid(fp->pid, NULL, 0);
}

// Если есть ошибка, то выводит сообщение о ней и выходит.
// Если ошибки нет, то ничего не делает.
void handle_create_error_file_process(const FileProc *fp)
{
    if (fp->status == FP_ERR_FORK)
        log_error_and_exit("Ошибка создания процесса");
    if (fp->status == FP_ERR_PIPE)
        log_error_and_exit("Ошибка создания канала");
}

void validate_args(int argc, char* argv[])
{
    if(argc != 3)
    {
        log_message(
            "ИСПОЛЬЗОВАНИЕ:\n"
            "\tПрограмма перенаправляет поток ввода в два указанных файла по правилу:\n"
            "\t\tЕсли длина строки больше 10, то она дописывается в файл large;\n"
            "\t\tИначе в файл small.\n"
            "\tПрограмма запускается с двумя параметрами:\n"
            "\t\t<small file> <large file>\n"
            "\t\tsmall file -- сюда будут записываться короткие строчки;\n"
            "\t\tlarge file -- сюда будут записываться длинные строчки.\n"
            "\t\tЕсли указанных файлов не существует -- они будут созданны.\n"
        );
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    validate_args(argc, argv);

    FileProc fp_small = run_file_process(argv[1]);
    FileProc fp_large = run_file_process(argv[2]);
    handle_create_error_file_process(&fp_small);
    handle_create_error_file_process(&fp_large);

    int len;
    char* line;
    while(line = get_line(&len), len != 0)
    {
        if(len > 10)
        {
            if(write_file_process(&fp_large, line, len) == -1)
                log_error("Не могу записать в файл");
        }
        else
        {
            if(write_file_process(&fp_small, line, len) == -1)
                log_error("Не могу записать в файл");
        }
        free(line);
    }
    
    close_file_process(&fp_large);
    close_file_process(&fp_small);
    return 0;
}