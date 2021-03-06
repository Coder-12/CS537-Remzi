#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc,char* argv[]){
  FILE *fp=fopen(argv[1],"r");
  if(fp==NULL){
    printf("can't able to open the file\n");
    exit(1);
  }
  char* buffer;
  size_t bufsize = 1024;
  size_t characters;
  buffer = (char *)malloc(bufsize * sizeof(char));
  if( buffer == NULL)
  {
     perror("Unable to allocate buffer");
     exit(1);
  }

  while(!feof(fp)){
    characters=getline(&buffer,&bufsize,fp);
    printf("%s",buffer);
  }
  fclose(fp);
}
