TARGET=context
OBJS=context.o

#######################
CFLAGS=-Wall -O2
LDFLAGS=
LIBS=`dotconf-config --libs --pool`
CC=gcc

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

