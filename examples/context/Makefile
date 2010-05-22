TARGET=context
OBJS=context.o

#######################
CFLAGS=-Wall -O2
LDFLAGS=
LIBS=`pkg-config dotconf --libs` ../libpool/libpool.a
CC=gcc

%.o: %.c
	$(CC) $(CFLAGS) -I../libpool -o $@ -c $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $< $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)

