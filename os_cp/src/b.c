#include <stdio.h>
#include <stdlib.h>
#include "io.h"

int main(int argc, char* argv[]) {
    int from_a, from_c;
    from_a = atoi(argv[1]);
    from_c = atoi(argv[2]);

    int len_a, len_c;
    while(read_str(from_a, (char*)&len_a, sizeof(len_a)) == 0 && 
          read_str(from_c, (char*)&len_c, sizeof(len_c)) == 0) {
        printf("Процесс A отправил: %d символов\n"
               "Процесс B отправил: %d символов\n\n",
               len_a, len_c);
    }

    return 0;
}