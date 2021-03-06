#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <pwd.h>

#define MAX_SIZE 100

const char* prompt_message = "wish> ";
const char* error_message  = "An error has occured\n";
char* delim                = " \n\t\a\r";
char* input[MAX_SIZE];
char* path[MAX_SIZE];
char* file = NULL;
struct passwd *pw;
const char *homedir;
int numOfPath  = 1;

void getToken(char**, char*, char*);
void execute(char**, int, char*);
int getDirectory(char**, char**, char*);
int redirect(char*, char*);
void parallel(char*);

void error() {
   write(STDOUT_FILENO, error_message, strlen(error_message));
   fflush(stdout);
}

void _exit(int status) {
  if(status != 0) {
    error();
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}

void printPrompt() {
  write(STDOUT_FILENO, prompt_message, strlen(prompt_message));
  fflush(stdout);
}

int getDirectory(char* path[], char* input[], char* dir) {
  if(input == NULL) return 1;
  if(input[0] == NULL) return 1;
  bool isfound = false;
  if(path[0] == NULL) {
    error();
    return 1;
  }
  
  for(int i = 0; i < numOfPath; ++i) {
    char tmp[100];
    strcpy(tmp, path[i]);
    int length = strlen(path[i]);
    tmp[length] = '/';
    tmp[length + 1] = '\0';
    strcat(tmp, input[0]);
    if(access(tmp, X_OK) == 0) {
      strcpy(dir, tmp);
      isfound = true;
      break;
    }
  }
  
  if(isfound == false || dir == NULL) {
    error();
    return 1;
  }
  return 0;
}

int readCommand(char* input[], FILE *fp) {
   char *line  = NULL;
   size_t size = 0;
   if(getline(&line, &size, fp) == -1) {
     error();
     return 1;
   }
   
   if((strcmp(line, "\n") == 0) || (strcmp(line, "") == 0)) {
     return -1;
   }
   
   if(line[strlen(line) - 1] == '\n') {
     line[strlen(line) - 1] = '\0';
   }
   
   if(line[0] == EOF) {
     return 1;
   }

   
   char* ret = strchr(line, '&');
   if(ret != NULL) {
     parallel(line);
     return -1;
   }
   
   ret = strchr(line, '>');
   if(ret != NULL) {
     redirect(ret, line);
     return -1;
   }
   
   getToken(input, line, delim);   
   if (input[0] == NULL) {
     return -1;
   }
   
   return 0;
}

int redirect(char* ret, char* line) {
  ret[0] = '\0';
  ret    = ret + 1;
  char* fileArgs[10];
  char dir[MAX_SIZE];
  getToken(fileArgs, ret, delim);
  file = fileArgs[0];

  if(file == NULL) {
    error();
    return -1;
  }
  
  if(fileArgs[1] != NULL) {
    error();
    return -1;
  }
  
  getToken(input, line, delim);
  if(input[0] == NULL) return -1;
  if(getDirectory(path, input, dir) == 1) return -1;
  execute(input, 1, dir);
  return 1;
}

void parallel(char* line) {
  char* commands[MAX_SIZE];
  char dir[MAX_SIZE];
  getToken(commands, line, "&");
  
  for(int i = 0; commands[i] != NULL; ++i) {
    char *ret = strchr(commands[i], '>');
    if(ret != NULL) {
      if(redirect(ret, commands[i]) == -1) break;
      continue;
    }
    getToken(input, commands[i], delim);
    if(input[0] == NULL) break;
    if(getDirectory(path, input, dir) == 1) continue;
    execute(input, 0, dir);
  }
}

void getToken(char* input[], char* line, char* delim) {
  int pos = 0;
  char* save;
  input[pos] = strtok_r(line, delim, &save);
  while(input[pos] != NULL) {
    pos++;
    input[pos] = strtok_r(NULL, delim, &save);
  }
}

int builtIn(char* path[], char* input[]) {
  if (strcmp(input[0], "exit") == 0) {
    if(input[1] != NULL) error();
     _exit(0);
  }
  
  else if (strcmp(input[0], "cd") == 0) {
    if(input[1] == NULL) {
      if(chdir(homedir) == -1) error();
    }
    else if(input[2] != NULL) {
      error();
    }
    else {
      if(chdir(input[1]) == -1) error();
    }
    return -1;
  }
  
  else if(strcmp(input[0], "path") == 0) { 
    if(input[1] == NULL) {
      for(int i = 0; i < numOfPath; ++i) {
	path[i] = NULL;
      }
    }
    int i;
    for(i = 0; input[i + 1] != NULL; ++i) {
      path[i] = input[i + 1];  
    }
    numOfPath = i + 1;
    return -1;
  }

  else if(strcmp(input[0], "pwd") == 0) {
    if(input[1] != NULL) {
      error();
    }
    else {
      char s[100];
      printf("%s\n", getcwd(s, 100));
    }
    return -1;
  }
  
  return 0;
}

void execute(char* input[], int ifRedirect, char* dir) {
  int childpid;
  int childStatus;
  childpid = fork();
  if (childpid < 0) {
    error();
    return;
  }
  else if (childpid == 0) {
    if (ifRedirect == 1) {
      int fd_out = open(file, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
      if (fd_out > 0) {
	dup2(fd_out, STDOUT_FILENO);
	fflush(stdout);
      }
    }  
    if (input[0] == NULL) {
      return;
    }
    execv(dir, input);
  }
  else {
    waitpid(childpid, &childStatus, 0);
  }
}

int main(int argc, char* argv[]) {
  char dir[100];
  pw = getpwuid(getuid());
  homedir = pw -> pw_dir;
  path[0] = "/bin";

  if(argc < 1 || argc > 2) {
    error();
    _exit(0);
  }
  
  while(true) {
    printPrompt();
    int readStatus = readCommand(input, stdin);
    fflush(stdin);

    if (readStatus      == -1) continue;
    else if (readStatus ==  1) break;

    if (builtIn(path, input) == -1) continue;

    if (getDirectory(path, input, dir) == 1) continue;

    execute(input, 0, dir);
  }
    
  return 0;
}
