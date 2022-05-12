#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"

#define BUFSIZE (32)
#define MAX_SIZE (1024)

rec_t inputs[MAX_SIZE];
int numOfKeys;

struct pairOfVals{
  int val, idx; 
} pair;

void
usage(char *prog) 
{
    fprintf(stderr, "usage: %s <-i file>\n", prog);
    exit(1);
}

void
readFile(char* inFile)
{  
    // open and create output file
    int fd = open(inFile, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "can't open file %s for reading\n", inFile);
    }

    rec_t records[BUFSIZE];
    // int cnt = 0;
    
    while (1) {
        int rc;
        if ((rc = read(fd, records, BUFSIZE * sizeof(rec_t))) == 0)
            break;
        int i, j;
        for (i = 0; i < (rc / sizeof(rec_t)); i++) {
	  inputs[numOfKeys].key = records[i].key;
	  for (j = 0; j < NUMRECS; j++)
	    inputs[numOfKeys].record[j] = records[i].record[j];
          numOfKeys++;
	}
    }

    // printf("count of keys:  %d\n", cnt);

    (void) close(fd);
}

void
merge(int l, int m, int r)
{
  rec_t tmp[1024];

  int p = l, q = m + 1, k = 0;
  while(p <= m && q <= r)
  {
    if(inputs[p].key <= inputs[q].key)
    {
      tmp[k++] = inputs[p];
      p++;
    }
    else
    {
      tmp[k++] = inputs[q];
      q++;
    }
  }

  while(p <= m)
  {
    tmp[k++] = inputs[p];
    p++;
  }

  while(q <= r)
  {
    tmp[k++] = inputs[q];
    q++;
  }
  
  for(int i = l; i <= r; ++i)
  {
    inputs[i] = tmp[i - l];
  }
  
}

void
sortf(int l, int r)
{
  if(l >= r) return;
  int m = (l + r) >> 1;
  sortf(l, m);
  sortf(m + 1, r);
  merge(l, m, r);
}

void
printData(char* outFile)
{
    // open and create output file
    int fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fd < 0) {
        fprintf(stderr, "can't open file %s for writing\n", outFile);
    }

    int	recordId   = 0;
    int recordsLeft = numOfKeys;
    
    // pack records into bigger buffers, and write those out
    rec_t records[BUFSIZE];
    
    while (recordsLeft > 0) {
        int howMany = 0;
        if (recordsLeft < BUFSIZE) {
            // end case: write out less than BUFSIZE records
            howMany = recordsLeft;
        } else {
            howMany = BUFSIZE;
        }

        int i;
        for (i = 0; i < howMany; i++) {
          // fill in random key
	  records[i].key = inputs[recordId].key; 
          // fill in random rest of records
          int j;
          for (j = 0; j < NUMRECS; j++) {
              records[i].record[j] = inputs[recordId].record[j];
          }
	  recordId++;
        }
        int writeSize = howMany * sizeof(rec_t);
        int rc = write(fd, records, writeSize);
        if (rc != writeSize) {
            perror("write");
            exit(1);
        }

        recordsLeft -= BUFSIZE;
    }


    // ok to ignore error code here, because we're done anyhow...
    (void) close(fd);
}

int
main(int argc, char *argv[])
{
    // arguments
    char *inFile = "/no/such/file";
    char *outFile = "/no/such/file";
    // input params
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "i:o:")) != -1) {
	switch (c) {
	case 'i':
	    inFile  = strdup(optarg);
	    break;
	case 'o':
	    outFile = strdup(optarg);
	    break;
	default:
	    usage(argv[0]);
	}
    }

    readFile(inFile);
    
    sortf(0, numOfKeys - 1);
    
    printData(outFile);
    
    return 0;
}
