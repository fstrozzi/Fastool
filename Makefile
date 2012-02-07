CC     = c99
CFLAGS = -O2

all:kseq.h fastool.c
	$(CC) $(CFLAGS) fastool.c -o fastool -lz

clean:
	rm -f *.o fastool