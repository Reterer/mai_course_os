# os_lab_2
### Постановка задачи
#### Цель работы
Приобретение практических навыков в
* Управление процессами в ОС;
* Обеспечение обмена данных между процессами посредством каналов.

#### Задание
Требуется составить и отладить программу на языке Си, которая:
* Создает два дочерних процесса, которые считывают из 
  стандартного потока ввода и записывают в стандартный поток вывода;
* Отправляет дочерним процессам данные с помощью каналов;
* Считывает строки из стандартного потока ввода и 
  отправляет их дочерним процессам по правилу.

Правило задано следующим образом: если длина считанной строки больше 10, 
то она записывается в файл для больших строк. 
Иначе она записыватеся в файл для маленьких строк.
Кроме этого необходимо отфильтровать глассные буквы.
Дочерние процессы должны быть представленны другой программой,
у которых выполненна подмена стандартных потоков ввода и вывода.

Для проверки программы нужно составить итоговые тесты.
В конечном итоге, программа должна состоять из следующих частей:
* Основная программа, которая запускает дочерние процессы и 
  фильтрует строки из потока ввода;
* Программа дочернего процесса, которая считывает данные из потока ввода, 
  а затем записывает их в поток вывода;

***

### Общие сведения о программе
Программа компилируется из файла main.c. Также используется заголовочные файлы: sys/types.h; sys/wait.h; sys/stat.h; unistd.h; fcntl.h; stdio.h; stdlib.h.

В программе используются следующие системные вызовы:
* pipe – создает канал. В качестве аргумента принимает указатель на массив из двух элемнетов типа я int. 
  Возвращает 0, если вызов выполнен успешно и -1, если нет. При успешном вызове, 
  первый элемент массива является файловым дескприптором, 
  из которого можно считать данные канала, а второй – в который можно записать;
* open – открывает файл и возвращает связанный с ним файловый дескриптор.
  Принимает три аргумента: путь, флаги и модификатор доступа. 
  Возвращает -1, если произошла ошибка. С помощью флагов нужно указать каким способом работать с файлом. 
  Файл так же можно создать, используя флаг O_CREAT. В случае создания файла, нужно указать модификаторы доступа;
* close – закрывает файл, связанный с файловым дескрпитором.
  Принимает в качестве аргумента файловый дескриптор.
  Возращает 0, если вызов был успешно выполнен. Иначе -1;
* fork – создает дочерний процесс. Возвращает -1 в случае неудачи.
  Иначе 0, если это дочерний процесс. PID дочернего процесса, если это родительский.
  Дочерний процесс является копией родительского: имеет копию памяти,
  а так же те же самые открытые файловые дескрипторы;
* dup2 – создает дубликат файлового дескриптора.
  Принимает два аргумента: старый дескриптор и новый дескриптор.
  При успешном выполнении новый дескриптор станет синонимом старого дескриптора.
  Возращют номер нового файлового дескриптора, если вызов выполнен успешно. Иначе -1;
* execve – процесс начинает выполнять указанную программу. 
  Данный вызов имеет несколько оболочек с немного разными аргументами для удобства.
  Принимиает 3 аргумента: путь до исполняемого файла; массив аргументов для программы,
  который должен заканчиваться NULL; массив параметров среды выполнения,
  который так же должен заканичваться NULL.
  При успешном вызове execve не возращает управление. При ошибке возращается -1;
* waitpid – процесс ждет завершения указанного процесса.
  Принимает три аргумента: id процесса, который нужно ждать;
  указатель на число, куда будет возращен статус завершенного процесса;
  Options, которые указывают возращать ли управление сразу или ждать до завершения процесса.
  Если первый аргумент равен 0,  то будет происходить ожидание любого дочернего процесса,
  идентификатор группы процессов которого равен идентификатору текущего процесса.
  Если первый аргумент равен -1, то будет происходить ожидание любого дочернего процесса.
  Если же первый аргумент меньше -1, то будет происходить ожидание любого дочернего процесса, 
  идентификатор группы процессов которого равен абсолютному значению pid. 
  Если второй аргумент не равен NULL, то возможно получить полезную информацию о завершенном процессе с помощью макросов. 
  Возвращает -1 в случае ошибки, 0, если ни один из процессов не был завершен 
  (если установлен соотвествующий флаг), либо возращает PID завершенного процесса;
* read – считывает count байт из fd в буффер buf. 
  Возращает количество считанных байт, или -1, в случае ошибки.  
  Количество считанных байт может быть меньше, чем число count;
* write – записывает count байт в fd из буффера buf. Работает аналогично read.

***

### Общий метод и алгоритм решения
Для реализации поставленной задачи необходимо:
1. Изучить системные вызовы, которые указаны выше.
2. Разбить задачу на подзадачи:
    * Попробовать fork и exec
    * Открытие файлов
    * Наладить pipe между процессами
    * Отправка строки по pipe
    * Считывание строки
    * Фильтрация строки
    * Тестирование
3. Выполнить поставленные подзадачи.
4. Рефаторинг кода.
5. Провести тестирование программы. Если были обнаружены ошибки – вернуться к 3-му пункту.

***

### Пример работы
    reterer@retcom:~/os_lab_2/build$ ./parent
    ИСПОЛЬЗОВАНИЕ:
            Программа перенаправляет поток ввода в два указанных файла по правилу:
                    Если длина строки больше 10, то она дописывается в файл large;
                    Иначе в файл small.
            Программа запускается с двумя параметрами:
                    <small file> <large file>
                    small file -- сюда будут записываться короткие строчки;
                    large file -- сюда будут записываться длинные строчки.
                    Если указанных файлов не существует -- они будут созданны.
    reterer@retcom:~/os_lab_2/build$ ./parent less more
    Hello world!
    small
    looooooooooooong 
    reterer@retcom:~/os_lab_2/build$ cat less
    smll
    reterer@retcom:~/os_lab_2/build$ cat more
    Hll wrld!
    lng

***

### Вывод
Выполняя данную лабораторную работу я узнал об некоторых системных вызовах. 
А так же приобрел опыт работы с каналами и процессами. 
Например, что бы корректно работать с каналом, необходимо закрыть не нужный файловый дескриптор 
(если нужно только считывать – закрыть fd для записаи и наоборот).
Познакомился с семейством вызовов exeс.
С помощью процессов можно делегировть выполнение задач.
Например браузеры используют разные процессы для разных вкладок. 
Это позволяет обеспечить межпроцессорную защиту, а так же «параллельное» выполнение задач. 
В *nix подобных системах процессы имеют иерархию. 
У каждого процесса должен быть родитель.
