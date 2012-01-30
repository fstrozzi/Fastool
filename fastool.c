/*
Copyright (C) 2012 Francesco Strozzi <francesco.strozzi@gmail.com>

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include "kseq.h"

KSEQ_INIT(gzFile, gzread)


int print_fasta(char *append, char *s[]) {
	if (append == NULL) {
		printf(">%s\n", s[0]);
	}
	else {
		printf(">%s%s\n", s[0], append);		
	}
	printf("%s\n", s[1]);
	return 0;
}

int print_fastq(char *append, char *s[]) {
	if (append == NULL) {
		printf("@%s\n", s[0]);		
	}
	else {
		printf("@%s%s\n", s[0], append);		
	}
	printf("%s\n", s[1]);
	printf("+\n%s\n",s[2]);
	return 0;
}

int print_seq(char *append, int to_fasta, char *s[3]) {
	if (to_fasta || s[2] == NULL) print_fasta(append, s);
	else print_fastq(append, s);
}

int main(int argc, char *argv[])
{

	if (argc == 1) {
		printf("Usage: %s sequences.fastq/a (--rev) (--append [string_to_append_to_header]) (--to-fasta) \n",argv[0]);
		exit(0);
	}


	gzFile fp;

	if (!(fp = gzopen(argv[1],"r"))) {
		printf("No %s file found!\n", argv[1]);
		exit(0);
	}


	kseq_t *seq;
	seq = kseq_init(fp);

	int reverse_complement = 0;
	char *string_to_append = NULL;
	int to_fasta = 0;

	for(int i = 2; i < argc; ++i)
	{
		if(strcmp(argv[i],"--rev") == 0) reverse_complement = 1;
		if(strcmp(argv[i],"--to-fasta") == 0) to_fasta = 1;
		if((strcmp(argv[i],"--append") == 0)) {
			if (i+1 == argc) {
				printf("String to append is missing!\n");
				exit(0);
			}
			else {
				string_to_append = argv[i+1];
			}
		}
	}

	if (reverse_complement) {
		while (kseq_read(seq) >= 0) {
			char tmp_seq[seq->seq.l];
			char *sequence_to_print[3];
			for(int i = 0; i < seq->seq.l; ++i) {
				if (*(seq->seq.s + seq->seq.l-1 - i) == 'A') tmp_seq[i] = 'T';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'C') tmp_seq[i] = 'G';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'T') tmp_seq[i] = 'A';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'G') tmp_seq[i] = 'C';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'N') tmp_seq[i] = 'N';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'U') tmp_seq[i] = 'A';
			}
			tmp_seq[seq->seq.l] = '\0';
			sequence_to_print[0] = seq->name.s;
			sequence_to_print[1] = tmp_seq;
			
			char quality[seq->qual.l];
			if (seq->qual.l) {
				for (int i = 0; i < seq->qual.l; ++i)
				{
					quality[i] = *(seq->qual.s + seq->qual.l -1 -i);
				}
				quality[seq->qual.l] = '\0';

				sequence_to_print[2] = quality;
			}
			print_seq(string_to_append, to_fasta, sequence_to_print);
		}
	}
	else {
		while (kseq_read(seq) >= 0) {
			char *sequence_to_print[3];
			sequence_to_print[0] = seq->name.s;
			sequence_to_print[1] = seq->seq.s;
			sequence_to_print[2] = seq->qual.s;
			print_seq(string_to_append, to_fasta, sequence_to_print);
		}
	}

	kseq_destroy(seq);
	gzclose(fp);
	return 0;
}



