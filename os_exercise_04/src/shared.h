#pragma once
#include <semaphore.h>
#include <stdbool.h>

#define SHARED_BUF_SIZE 1024 


typedef struct 
{
    sem_t write_mutex;
    sem_t read_mutex;

    char buf[SHARED_BUF_SIZE];      // Буфер для передачи текста
    int buf_len;                    // Длина текста в буфере
}   Shared;

typedef struct
{
    int fd;
    Shared* shared;

} SharedContainer;

// Создает SharedContainer, а так же инициализирует его
// Если произошла ошибка, возвращает NULL
SharedContainer shared_init(const char* name);

// Возвращает указатель на общий shared
SharedContainer shared_connect(const char* name);

// Отключает данный shared
void shared_deconnect(SharedContainer* shared_container);

// Уничтожает shared, должен быть вызван на стороне производителя
void shared_destroy(SharedContainer* shared_container, const char* name);

// Запись в сообщения msg длиной len в shared, 
// Eсли длина сообщения len больше, чем
// SHARED_BUF_SIZE, тогда будет записано только SHARED_BUF_SIZE символов.
// Возвращает кол-во записаных символов
// -1, если произошла ошибка, связанная с семафором
// -2 - остальные
int shared_buf_write(SharedContainer* shared_container, const char* msg, int len);

// Запись буфера из shared в buf. Будет считано не более shared->buf_size элементов
// Возращает кол-во считанных символов
// -1, если произошла ошибка, связанная с семафором
// -2 - остальные
// Buf должен иметь как минимум SHARED_BUF_SIZE элементов
int shared_buf_read(SharedContainer* shared_container, char* buf);

/*
1. Открываем общий файл (посмотри, можно ли его сделать безымянным)
2. Делаем его маппинг
3. Профит
*/