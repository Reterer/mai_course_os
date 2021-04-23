# os_lab_4

### Постановка задачи

#### Цель работы

Приобретение практических навыков в

- Управление потоками в ОС;
- Обеспечение синхронизации между потоками.

#### Задание

Требуется составить многопоточную программу на языке Си, которая:

- Освоение принципов работы с файловыми системами;
- Обеспечение обмена данных между процессами посредством технологии «File mapping».

---

### Общие сведения о программе

Для создания многопоточной программы нужно использовать заголовочный файл pthread. Также необходимо указать линковщику ключ -pthread.

Исходный код включает следующие файлы:

Для успешной компиляции нужно использовать дополнительные библиотеки: pthread - для использования семафоров и mmap; rt - для общей памяти (shm). Для упрощения процесса компиляции я использовал make.

Исходный код включает следующие файлы:

- parent.c - основной процесс, который запускает дочерний и ожидает ввода строк;
- child.c - дочерний процесс, который обрабатывает полученные строки по правилу и записывает в файл;
- shared.c - содержит функции и структуры данных, обеспечивающих обмен информацией между процессами с помощью общей памяти.
- io.c, io.h - так как запрещено использовать абстракции над системными вызовами ввода и вывода, здесь реализованы более высокоуровневые функции ввода-вывода;

POSIX функции:

- int shm_open(const char \*name, int oflag, mode_t mode) - открывает объект разделяемой памяти posix. Где name - имя объекта; oflag и mode - по аналогии с mode. Возвращает файловый дескриптор, по которому можно обратиться к разделяемой памяти. В случае ошибки возвращает -1. Обычно используется вместе с mmap, что бы получить параллельный доступ к памяти;
- int shm_unlink(const char \*name) - Закрывает объект разделяемой памяти. Где name - имя объекта. Возвращает -1 в случае ошибки;
- int ftruncate(int fd, off_t length) - устанавливает длину size у заданного файла fd. Возвращает -1 в случае ошибки
- void * mmap(void *start, size_t length, int prot , int flags, int fd, off_t offset) - создаёт отображение файла на область в памяти. Где start - рекомендуемая область отображения; length - размер области; prot - режим защиты памяти, похоже на режим работы с файлом, только константы по другому называются; flags - режим работы с файлом, мы можем указать MAP_SHARED для того, чтобы изменения были видны другим процессам; fd - файловый дескриптор отображаемого файла, можно указать значение -1 для анонимной области памяти, но это не будет работать для других процессов после вызова exec; offset - указывает отступ от начала файла. Возвращает указатель на отображение, в случае ошибки возвращает MAP_FAILTURE;
- int munmap(void \*start, size_t length) - Освобождает выделенную область памяти. Где start - указатель на отображение файла; length - размер отображения. Возвращает -1 в случае ошибки;
- int sem_init(sem_t \*sem, int pshared, unsigned int value) - создаёт семафор, сохраняя его в переменной sem; pshared - указывает возможность работы с другими процессами; value - значение семафора по умолчанию;
- int sem_destroy(sem_t \*sem) - уничтожает семафор sem. Возвращает -1 в случае ошибки;
- int sem_wait(sem_t \*sem) - уменьшает значение счетчика семафора sem. Если счетчик равен нулю, то поток блокируется, пока кто-то не увеличит его. Возвращает -1 в случае ошибки;
- int sem_post(sem_t \*sem) - увеличивает значение счетчика семафора sem. Возвращает -1 в случае ошибки.

---

### Общий метод и алгоритм решения

Основная логика программы не изменилась. Изменился только способ взаимодействия между процессами.

Создание разделяемой области памяти происходит в три этапа:

1. Создание shm объекта;
2. Установка размера общей памяти;
3. Отображение shm с помощью mmap на процессе родителя и ребенка.

Общение обеспечивается с помощью двух семафоров read и write. Изначально семафор read заблокирован, а write разблокирован:

1. При записи в буфер, родительский процесс блокирует семафор write и, в конце работы, разблокирует семафор read;
2. При чтении из буфера, дочерний процесс блокирует семафор read и , в конце своей работы, разблокирует семафор write.

Обмен информацией происходит с помощью буфера.

---

### Пример работы

reterer@serv:~/OS/os_exercise_04/src$ make
cc -c -Wall -pedantic -O2 -c -o parent.o parent.c
cc -c -Wall -pedantic -O2 -c -o io.o io.c
cc -c -Wall -pedantic -O2 -c -o shared.o shared.c
gcc -O2 parent.o io.o shared.o -o parent -lpthread -lrt
cc -c -Wall -pedantic -O2 -c -o child.o child.c
gcc -O2 child.o io.o shared.o -o child -lpthread -lrt
reterer@serv:~/OS/os_exercise_04/src$ ./parent small large
Hello world!
Aloha
reterer@serv:~/OS/os_exercise_04/src$ cat small
lh
reterer@serv:~/OS/os_exercise_04/src$ cat large
Hll wrld!

---

### Вывод

Разделяемая память очень быстрый способ взаимодействия между процессами, так как не надо производить лишние копирования и обращения к операционной системе. Однако стоит помнить, что в данном случае возможны гонки данных, поэтому нужно позаботится о синхронизации процессов для чтения и изменения информации.

Функция mmap позволяет делать отображение обычных файлов это может ускорить считывание и запись. Со случаем записи необходимо заранее установить размер окончательного файла.

В конечном итоге можно сказать, что отображение файлов в оперативную память является довольно полезной технологией.