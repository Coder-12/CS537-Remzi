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

void checkValidity(char* buffer){
  if(buffer == NULL){
    perror("Unable to allocate buffer\n");
    exit(1);
  }
}

FILE* open(char* fileName, char* perm){
  FILE *fp = fopen(fileName,perm);
  if(fp == NULL){
    printf("can't able to open the file\n");
    exit(1);
  }
  return fp;
}

void zipToken(FILE *out,char* word){
  int wsize = strlen(word);
  word[wsize] = '$';
  for(int i=0;i<wsize;++i){
    int cnt = 1;
    char ch = word[i];
    while(i < wsize && word[i] == word[i+1]){
      ++i; ++cnt;
    }
    seq sq = {cnt,ch};
    fwrite(&sq, sizeof(sq), 1, out);  
  }
}

void compress(FILE *out, char* sentence){
  int slen = strlen(sentence);
  char *sent = (char*)malloc(sizeof(char)*slen);
  for(int i=0;i<slen;++i){
    sent[i]=sentence[i];
  }
  char* token = strtok(sent," ");
  while(token != NULL){
    zipToken(out,token);
    token = strtok(NULL," ");
  }
}

void readInputFile(char* inFile,char* outFile){  
  FILE *in = open(inFile,"r");
  FILE *out = open(outFile,"wb");
  char* buffer;
  size_t characters;
  size_t bufsize = 1024;
  buffer = malloc(bufsize * sizeof(char));
  checkValidity(buffer);
  
  while(!feof(in)){
    characters=getline(&buffer,&bufsize,in);
    removeNewLine(buffer);
    printf("%s",buffer);
    compress(out,buffer);
  }
}

int main(int argc,char* argv[]){
  if(argc<3){
    perror("usage ./a.out txtFile binFile\n");
    exit(1);
  }
  readInputFile(argv[1],argv[2]);
}
