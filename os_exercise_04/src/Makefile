CPP=gcc
CPPFLAGS=-c -Wall -pedantic -O2

all: parent child
	
parent: parent.o io.o shared.o
	$(CPP) -O2 $^ -o $@ -lpthread -lrt

child: child.o io.o shared.o
	$(CPP) -O2 $^ -o $@ -lpthread -lrt

.cpp.o:
	$(CPP) $(CPPFLAGS) $< -o $@

# Зависимости
parent.o : io.h shared.h
child.o : io.h shared.h
shared.o : io.h shared.h
io.o : io.h

clean:
	rm -f *.o child parent
.PHONY: all clean