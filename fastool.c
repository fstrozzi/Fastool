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


int process_input(FILE *stream, int rev_comp, char *string, int to_fa) {


	kseq_t *seq;
	seq = kseq_init(stream);
	if (rev_comp) {
	while (kseq_read(seq) >= 0) {
		char rev_seq[seq->seq.l];
		char *sequence_to_print[3];
		for(int i = 0; i < seq->seq.l; ++i) {
			if (*(seq->seq.s + seq->seq.l-1 - i) == 'A') rev_seq[i] = 'T';
			else if (*(seq->seq.s + seq->seq.l-1 - i) == 'C') rev_seq[i] = 'G';
			else if (*(seq->seq.s + seq->seq.l-1 - i) == 'T') rev_seq[i] = 'A';
			else if (*(seq->seq.s + seq->seq.l-1 - i) == 'G') rev_seq[i] = 'C';
			else if (*(seq->seq.s + seq->seq.l-1 - i) == 'N') rev_seq[i] = 'N';
			else if (*(seq->seq.s + seq->seq.l-1 - i) == 'U') rev_seq[i] = 'A';
		}
		rev_seq[seq->seq.l] = '\0';
		sequence_to_print[0] = seq->name.s;
		sequence_to_print[1] = rev_seq;
		
		char quality[seq->qual.l];
		if (seq->qual.l) {
			for (int i = 0; i < seq->qual.l; ++i)
			{
				quality[i] = *(seq->qual.s + seq->qual.l -1 -i);
			}
			quality[seq->qual.l] = '\0';
			sequence_to_print[2] = quality;
		}
		print_seq(string, to_fa, sequence_to_print);
		}
	}
	else {
		while (kseq_read(seq) >= 0) {
			char *sequence_to_print[3];
			sequence_to_print[0] = seq->name.s;
			sequence_to_print[1] = seq->seq.s;
			sequence_to_print[2] = seq->qual.s;
			print_seq(string, to_fa, sequence_to_print);
		}
	}

	kseq_destroy(seq);
	return 0;
}

int print_help(char *command_line) {
		printf("Usage: %s (--rev) (--append [string_to_append_to_header]) (--to-fasta) sequences_1.fastq/a sequences_2.fastq/a ... \n",command_line);
}

int main(int argc, char *argv[])
{

	int reverse_complement = 0;
	char *string_to_append = NULL;
	int to_fasta = 0;
	int read_from_file = 0;

	if(argc == 1) {
		print_help(argv[0]);
		exit(0);
	}

	for(int i = 1; i < argc; ++i)
	{
		if(strcmp(argv[i],"--rev") == 0) reverse_complement = 1;
		else if (strcmp(argv[1],"-h") == 0) {
			print_help(argv[0]);
			exit(0);
		}
		else if(strcmp(argv[i],"--to-fasta") == 0) to_fasta = 1;
		else if((strcmp(argv[i],"--append") == 0)) {
			if (i+1 == argc) {
				printf("String to append is missing!\n");
				exit(0);
			}
			else {
				string_to_append = argv[i+1];
				i++;
			}
		}
		else {
				read_from_file = 1;
				gzFile fp;
				if (!(fp = gzopen(argv[i],"r"))) {
					printf("No %s file found!\n", argv[i]);
					exit(0);
				}
				process_input(fp,reverse_complement, string_to_append, to_fasta);
				gzclose(fp);
		}
	}
	if (!read_from_file)
	{
		gzFile fp_stdin;
		fp_stdin = gzdopen(fileno(stdin), "rb");
		process_input(fp_stdin, reverse_complement, string_to_append, to_fasta);
		gzclose(fp_stdin);
	}

}



