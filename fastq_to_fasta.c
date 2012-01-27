/*
Copyright (C) Francesco Strozzi <francesco.strozzi@gmail.com>

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
	char *file;
	file = argv[1];
	gzFile fp;

	if (!(fp = gzopen(file,"r"))) {
		printf("No %s file found!\n", file);
		exit(0);
	}

	kseq_t *seq;
	seq = kseq_init(fp);
	if (argc >= 3) {
		if (strcmp(argv[2],"--rev") == 0) {
			while (kseq_read(seq) >= 0) {
				int i = 0;
				char rev_seq[seq->seq.l];
				while(i < seq->seq.l) {
					if (*(seq->seq.s + seq->seq.l-1 - i) == 'A') rev_seq[i] = 'T';
					if (*(seq->seq.s + seq->seq.l-1 - i) == 'C') rev_seq[i] = 'G';
					if (*(seq->seq.s + seq->seq.l-1 - i) == 'T') rev_seq[i] = 'A';
					if (*(seq->seq.s + seq->seq.l-1 - i) == 'G') rev_seq[i] = 'C';
					i++;
				}
				printf(">%s\n", seq->name.s);
				printf("%s\n", rev_seq);	
			}
		}
	}	
	else {
		while (kseq_read(seq) >= 0) {
			printf(">%s\n", seq->name.s);
			printf("%s\n", seq->seq.s);	
		}
	}
	return(0);
}