#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "io.h"
#include "shared.h"

// Создает SharedContainer, а так же инициализирует его
// Если произошла ошибка, возвращает NULL
SharedContainer shared_init(const char* name)
{
    SharedContainer shared_container;
    shared_container.fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if(shared_container.fd == -1)
        log_error_and_exit("shm_open");

    if(ftruncate(shared_container.fd, sizeof(Shared)) == -1)
        log_error_and_exit("ftruncate");

    shared_container.shared = mmap(0, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED, shared_container.fd, 0);
    if(shared_container.shared == MAP_FAILED)
        log_error_and_exit("mmap");
    
    if(sem_init(&shared_container.shared->write_mutex, 1, 1) == -1)
    {
        log_error("Не могу создать семафор");
        munmap(shared_container.shared, sizeof(Shared));
    }
    if(sem_init(&shared_container.shared->read_mutex, 1, 0) == -1)
    {
        log_error("Не могу создать семафор");
        munmap(shared_container.shared, sizeof(Shared));
    }

    shared_container.shared->buf_len = 0;

    return shared_container;
}

// Возвращает указатель на общий shared
SharedContainer shared_connect(const char* name)
{
    SharedContainer shared_container;
    shared_container.fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
    if(shared_container.fd == -1)
        log_error_and_exit("shm_open");

    shared_container.shared = mmap(0, sizeof(Shared), PROT_READ | PROT_WRITE, MAP_SHARED, shared_container.fd, 0);
    if(shared_container.shared == MAP_FAILED)
        log_error_and_exit("mmap");

    return shared_container;
}

// Отключает данный shared
void shared_deconnect(SharedContainer* shared_container)
{
    if(shared_container == NULL)
        return;

    if(munmap(shared_container->shared, sizeof(Shared)) == -1)
        log_error("Не могу освободить общую память");
    if(close(shared_container->fd) == -1)
        log_error("close");
}

// Уничтожает данный shared
void shared_destroy(SharedContainer* shared_container, const char* name)
{
    // Ожидание конца записи сообщения
    sem_wait(&shared_container->shared->write_mutex);
    shared_container->shared->buf_len = -1;
    sem_post(&shared_container->shared->read_mutex);

    if(sem_destroy(&shared_container->shared->write_mutex) == -1)
        log_error("Не могу уничтожить семафор");
        
    if(sem_destroy(&shared_container->shared->read_mutex) == -1)
        log_error("Не могу уничтожить семафор");

    if(munmap(shared_container->shared, sizeof(Shared)) == -1)
        log_error("Не могу освободить общую память");

    if(close(shared_container->fd) == -1)
        log_error("close");
    
    if(shm_unlink(name) == -1)
        log_error_and_exit("unlink");
}

// Запись в сообщения msg длиной len в shared, 
// Eсли длина сообщения len больше, чем
// SHARED_BUF_SIZE, тогда будет записано только SHARED_BUF_SIZE символов.
// Возвращает кол-во записаных символов
// -1, если произошла ошибка, связанная с семафором
// -2 - остальные
int shared_buf_write(SharedContainer* shared_container, const char* msg, int len)
{
    if(shared_container == NULL)
        return -2;
    if(msg == NULL && len != 0)
        return -2;

    // Заблокирвать
    if(sem_wait(&shared_container->shared->write_mutex) == -1)
        return -1;

    if(shared_container->shared->buf_len == -1)
        return -1;

    // Записать данные
    int need_to_write = (len < SHARED_BUF_SIZE) ? len : SHARED_BUF_SIZE;
    for(int i = 0; i < need_to_write; ++i) 
    {
        shared_container->shared->buf[i] = msg[i];
    }
    shared_container->shared->buf_len = need_to_write;
    
    // Разблокировать
    if(sem_post(&shared_container->shared->read_mutex) == -1)
        return -1;

    return need_to_write;
}

// Запись буфера из shared в buf. Будет считано не более shared->buf_size элементов
// Возращает кол-во записанных файлов
// -1, если произошла ошибка, связанная с семафором
// -2 - остальные
// Buf должен иметь как минимум SHARED_BUF_SIZE элементов
int shared_buf_read(SharedContainer* shared_container, char* buf)
{
    if(shared_container == NULL || buf == NULL)
        return -2;

    // Заблокировать
    if(sem_wait(&shared_container->shared->read_mutex) == -1)
        return -1;

    // Считать данные
    int need_to_read = shared_container->shared->buf_len;
    for(int i = 0; i < need_to_read; ++i) 
    {
        buf[i] = shared_container->shared->buf[i];
    }
    shared_container->shared->buf_len = 0;
    
    // Разблокировать
    if(sem_post(&shared_container->shared->write_mutex) == -1)
        return -1;
    
    return need_to_read;
}