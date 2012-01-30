all:kseq.h fastool.c
	$(CC) -std=c99 -O2 fastool.c -o fastool -lz

clean:
	rm -f *.o fastool