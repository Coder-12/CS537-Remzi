#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


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

FILE* open(char* filePath){
  FILE *fp = fopen(filePath,"r");
  if(fp == NULL){
    printf("can't able to open the file\n");
    exit(1);
  }
  return fp;
}


struct dict{
  struct dict* children[26];
};
typedef struct dict dict;

dict* alloc(){
  dict* node=(dict*)malloc(sizeof(dict));
  if(node==NULL){
    perror("can't able to allocate memory for node\n");
    exit(1);
  }
  for(int i=0;i<26;++i){
    node->children[i]=NULL;
  }
  return node;
}

void dealloc(dict* node){
  if(node == NULL) return;
  for(int i=0;i<26;++i){
    if(node!=NULL && node->children[i]==NULL) continue;
    dealloc(node->children[i]);
  }
  free(node);
  node=NULL;
}

void insertWord(dict* root, char* word){
  int wsize=strlen(word);
  dict* tmp=root;
  for(int i=0;i<wsize;++i){
    if(tmp->children[(int)(word[i]-'a')]==NULL){
      tmp->children[(int)(word[i]-'a')]=(dict*)alloc();
    }
    tmp=tmp->children[(int)(word[i]-'a')];
  }
}

bool search(dict* root, char* word){
  dict* tmp=root;
  int wsize=strlen(word);
  for(int i=0;i<wsize;++i){
    if(tmp->children[(int)(word[i]-'a')]==NULL) return false;
    tmp=tmp->children[(int)(word[i]-'a')];
  }
  return true;
}

void printWord(dict* root){
  for(int i=0;i<26;++i){
    if(root->children[i]){
      char ch=(char)(i+'a');
      printf("%c",ch);
      printWord(root->children[i]);
    }
  }  
}

bool findWord(char* sentence,char* wordToSearch){
  int slen=strlen(sentence);
  char *sent=(char*)malloc(sizeof(char)*slen);
  for(int i=0;i<slen;++i){
    sent[i]=sentence[i];
  }
  char* token = strtok(sent," ");
  while(token != NULL){
    if(strlen(wordToSearch)<=strlen(token)){
      for(int i=0;i<=strlen(token)-strlen(wordToSearch);++i){
	bool isFound=true;
	for(int j=i;j<i+strlen(wordToSearch);++j){
	  if(wordToSearch[j-i] != token[j]){isFound=0;break;}
	}
	if(isFound) return true;
      }
    }
    token = strtok(NULL," ");
  }
  return false;
}

void searchFile(FILE* fp,char* wordToSearch){
  // dict* root = NULL;
  char* buffer;
  size_t characters;
  size_t bufsize = 1024;
  buffer = (char*)malloc(bufsize * sizeof(char));
  checkValidity(buffer);
  
  while(!feof(fp)){
    characters=getline(&buffer,&bufsize,fp);
    removeNewLine(buffer);
    if(findWord(buffer,wordToSearch)){
      printf("%s\n",buffer);
    }
  }
}

int main(int argc,char* argv[]){
  if(argc==1){
    perror("usage ./a.out file_name wordToSearch\n");
    exit(1);
  }
  if(argc==2){
    printf("");
    return 0;
  }
  FILE *fp = open(argv[1]);
  searchFile(fp,argv[2]);
  fclose(fp);
}
