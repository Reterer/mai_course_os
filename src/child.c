#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define BUFSIZE 1024
int main()
{
  char buf[BUFSIZE];
  ssize_t read_bytes;
  while ((read_bytes = read(STDIN_FILENO, buf, sizeof(char) * BUFSIZE)) > 0)
  {
    write(STDOUT_FILENO, buf, read_bytes);
  }
  fprintf(stderr, "Конец");
  return 0;
}