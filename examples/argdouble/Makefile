TARGET=argdouble
OBJS=argdouble.o

#######################
CFLAGS=-Wall -O2
LDFLAGS=
LIBS=-ldotconf
CC=gcc

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

