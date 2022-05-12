#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


struct sequence{
  int num;
  char ch;
};
typedef struct sequence seq;

void removeNewLine(char* sentence){
  size_t slen = strlen(sentence);
  if(slen > 0 && sentence[slen-1] == '\n'){
    sentence[slen-1]='\0';
  }
}

FILE* open(char* inFile, char* perm){
  FILE *in = fopen(inFile,perm);
  if(in == NULL){
    printf("can't able to open the file\n");
    exit(1);
  }
  return in;
}

void uncompress(char *inFile,char *outFile){
  FILE* in = open(inFile,"rb");
  FILE* out = open(outFile,"wt");
  seq sq;
  while(fread(&sq,sizeof(sq),1,in) == 1){
    int cnt=sq.num; char c=sq.ch;
    while(cnt--){
      fprintf(out, "%c", c);
    }
  }
  fclose(out);
  fclose(in);
}

int main(int argc,char* argv[]){
  if(argc==1){
    perror("usage ./a.out binFile txtFile\n");
    exit(1);
  }
  uncompress(argv[1],argv[2]);
}
