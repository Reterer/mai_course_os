CPP=gcc
CPPFLAGS=-Wall -std=c99 -pedantic -O2 -c

all: app1 app2
	
app1: app1.o libs/libimp1.so
	$(CPP) -O2 app1.o -o $@ -Llibs -limp1 -Wl,-rpath,libs

app2: app2.o libs/libimp1.so libs/libimp2.so
	$(CPP) -O2 app2.o -o $@ -ldl 


libs/libimp1.so: libs/first/first_imp_1.o libs/second/second_imp_1.o
	$(CPP) -shared -o $@ $^ -lm
libs/first/first_imp_1.o: libs/first/first_imp_1.c
	$(CPP) $(CPPFLAGS) -fPIC libs/first/first_imp_1.c -o $@
libs/second/second_imp_1.o: libs/second/second_imp_1.c
	$(CPP) $(CPPFLAGS) -fPIC libs/second/second_imp_1.c -o $@

libs/libimp2.so: libs/first/first_imp_2.o libs/second/second_imp_2.o
	$(CPP) -shared -o $@ $^ -lm
libs/first/first_imp_2.o: libs/first/first_imp_2.c
	$(CPP) $(CPPFLAGS) -fPIC libs/first/first_imp_2.c -o $@
libs/second/second_imp_2.o: libs/second/second_imp_2.c
	$(CPP) $(CPPFLAGS) -fPIC libs/second/second_imp_2.c -o $@

.c.o:
	$(CPP) $(CPPFLAGS) $< -o $@

clean:
	rm -rf *.o libs/*.a libs/*.so libs/first/*.o libs/second/*.o app1 app2
.PHONY: all clean