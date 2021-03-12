#pragma once
// Записывает len байт из msg в fd.
// Возвращает 0, если успешно, иначе -1.
int write_str(int fd, const char *msg, int len);

// Считывает строку, которая будет иметь длину len и возращает ее.
// Не забудь освободить память, выделенную под строку.
char* get_line(int *len);

// Выводит msg (perror) и выходит из программы.
void log_error_and_exit(const char *msg);
// Выводит msg (perror).
void log_error(const char *msg);
// Выводит сообщение msg
void log_message(const char *msg);
