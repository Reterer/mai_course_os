#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

  ret.pid = fork();
  switch (ret.pid)
  {
  // Ошибка
  case -1:
    ret.status = FP_ERR_FORK;
    return ret;
    break;

  // Процесс потомка
  case 0:;
    int fd = open(file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
      printf("open");

    if (dup2(fd, STDOUT_FILENO))
      printf("dup2 out");
    close(fd);

    if (dup2(ret.fd[0], STDIN_FILENO))
      printf("dup2 in");
    close(ret.fd[0]);

    if (execl("child", "child", NULL))
      printf("child");

    break;
  }

  ret.status = FP_OK;
  return ret;
}

int write_file_process(const FileProc *fp, const char *msg)
{
  size_t msg_len = strlen(msg);
  return write(fp->fd[1], msg, msg_len) != msg_len;
}

int close_file_process(const FileProc *fp)
{
  return close(fp->fd[1]);
}

void log_error_and_exit(const char *msg)
{
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

int main(void)
{
  FileProc fp = run_file_process("test");
  if (fp.status == FP_ERR_FORK)
    log_error_and_exit("Ошибка создания процесса");
  if (fp.status == FP_ERR_PIPE)
    log_error_and_exit("Ошибка создания канала");

  write_file_process(&fp, "Привет мир!\n");
  close_file_process(&fp);
  wait();
  return 0;
}