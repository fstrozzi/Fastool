all:kseq.h fastq_to_fasta.c
	$(CC) -g -O2 fastq_to_fasta.c -o fastq_to_fasta -lz

clean:
	rm -f *.o fastq_to_fasta
	rm -fr *.dSYM