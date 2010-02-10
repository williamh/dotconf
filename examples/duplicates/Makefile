CFLAGS=-Wall -O2 -g
LDFLAGS=
LIBS=-ldotconf -lpool -ldl
CC=gcc

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: duplicate first_module.so second_module.so

duplicate: duplicate.o
	$(CC) -o $@ $< $(LDFLAGS) $(LIBS)

first_module.so: first_module.o
	$(CC) -o $@ $< -rdynamic -shared $(LIBS)
second_module.so: second_module.o
	$(CC) -o $@ $< -rdynamic -shared $(LIBS)

clean:
	rm -f *.so *.o duplicate


