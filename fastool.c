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

int main(int argc, char const *argv[])
{

	if (argc == 1) {
		printf("Usage: %s sequences.fastq/a (--rev) [for reverse complement] (--append) string_to_append_to_header \n",argv[0]);
		exit(0);
	}

	char *file;
	file = argv[1];
	gzFile fp;

	if (!(fp = gzopen(file,"r"))) {
		printf("No %s file found!\n", file);
		exit(0);
	}

	int reverse_complement = 0;
	char *string_to_append;
	int c;
	for(c = 2; c < argc; ++c)
	{
		if(strcmp(argv[c],"--rev") == 0) reverse_complement = 1;
		if((strcmp(argv[c],"--append") == 0)) {
			if (c+1 == argc) {
				printf("String to append is missing!\n");
				exit(0);
			}
			else {
				string_to_append = argv[c+1];	
			}
		}
	}
	kseq_t *seq;
	seq = kseq_init(fp);
	if (reverse_complement) {
		while (kseq_read(seq) >= 0) {
			char rev_seq[seq->seq.l];
			int i;
			for(i = 0; i < seq->seq.l; ++i) {
				if (*(seq->seq.s + seq->seq.l-1 - i) == 'A') rev_seq[i] = 'T';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'C') rev_seq[i] = 'G';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'T') rev_seq[i] = 'A';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'G') rev_seq[i] = 'C';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'N') rev_seq[i] = 'N';
				else if (*(seq->seq.s + seq->seq.l-1 - i) == 'U') rev_seq[i] = 'A';
			}
			rev_seq[i] = '\0';
			print_seq(seq->name.s, &rev_seq, string_to_append);	
		}
	}	
	else {
		while (kseq_read(seq) >= 0) {
			print_seq(seq->name.s, seq->seq.s, string_to_append);
		}
	}

	kseq_destroy(seq);
	gzclose(fp);
	return 0;
}


int print_fasta(char *name, char *sequence, char *string_to_append) {

	if (string_to_append) {
		printf(">%s%s\n", name, string_to_append);
	}
	else {
		printf(">%s\n", name);
	}
	printf("%s\n", sequence);
	return 0;
}

int print_fastq(char *name, char *sequence, char *string_to_append) {

	if (string_to_append) {
		printf(">%s%s\n", name, string_to_append);
	}
	else {
		printf(">%s\n", name);
	}
	printf("%s\n", sequence);
	return 0;
}
