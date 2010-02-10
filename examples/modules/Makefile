TARGET=module
OBJS=module.o

#######################
CFLAGS=-Wall -O2 -g
LDFLAGS=
LIBS=-ldotconf -ldl
CC=gcc

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGET) my_module.so

$(TARGET): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(LIBS)

my_module.so: my_module.o
	$(CC) -o $@ $< -rdynamic -shared

clean:
	rm -f $(OBJS) $(TARGET) my_module.so *.o

