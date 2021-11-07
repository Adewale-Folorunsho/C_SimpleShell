/*
 * Worked with Adewale Folorunsho
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
char pipe_delimiters[] = "|\t\r\n";
extern char **environ;
pid_t pid;
char cwd[256];
int gt;
int filefd;
int stdOutFd;

void ChildProcess(char *instr[], int size, char *command_tok[], bool isBackground, int filefd, int stdOutFd, int gt);
void signal_ctrlc(int signum);
void signal_alarm(int signum);
void pipedCommands(char *pipe_arguments);
void pipedCommandsIndex(char *pipe_command_tok);

int main() {
    // Stores the string typed into the command line.
    char command_line[MAX_COMMAND_LINE_LEN];
    char cmd_bak[MAX_COMMAND_LINE_LEN];
    char command[MAX_COMMAND_LINE_LEN];
    int status;
    signal(SIGINT, signal_ctrlc);
  
    // Stores the tokenized command line input.
    //char *arguments[MAX_COMMAND_LINE_ARGS];
    	
    while (true) {
      
        do{ 
            // Print the shell prompt.
            
            if( getcwd(cwd, sizeof(cwd)) == NULL)
              perror("getcwd() error");
            else
             printf("%s%s", cwd, prompt);
              

            // Read input from stdin and store it in command_line. If there's an
            // error, exit immediately. (If you want to learn more about this line,
            // you can Google "man fgets")
            
            if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
                fprintf(stderr, "fgets error");
                exit(0);
            }
 
        }while(command_line[0] == 0x0A);  // while just ENTER pressed

      
        // If the user input was EOF (ctrl+d), exit the shell.
        if (feof(stdin)) {
            printf("\n");
            fflush(stdout);
            fflush(stderr);
            return 0;
        }

        // TODO:
        // 
        
			  // 0. Modify the prompt to print the current working directory
			  
			
        // 1. Tokenize the command line input (split it on whitespace)
        char *arguments = strtok(command_line, delimiters);
        char *command_tok[MAX_COMMAND_LINE_ARGS];
        int i = 0;
        int j = 0;
        int numArgs = 0;
        gt = 0;
    
        while (arguments) {
          char checkVar = *arguments;

					char *firstChar[1];
					*firstChar = "$";

					char *envEq[20];

					if('$' == checkVar){
						*envEq = arguments +1;
						if(!getenv(*envEq)){
							*envEq = " ";
						}else{
							*envEq = getenv(*envEq);
						}
					}else{
						*envEq = arguments;
					}
          if(strcmp(*envEq, ">") == 0){
            gt = 1;
          }
          command_tok[i++] = *envEq;
          arguments = strtok(NULL, delimiters);
          numArgs++;
        }
        command_tok[i] = NULL;
        bool isBackground = command_tok[numArgs-1] == "&";
     
        char *instr[10];
				*instr = command_tok[0];
        // 2. Implement Built-In Commands
        // 
        //pwd

				char cwd[256];
        stdOutFd = dup(STDOUT_FILENO);
        filefd;
        bool useChildPprocess;
				if (gt == 1){
          filefd = open(command_tok[numArgs - 1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
          command_tok[numArgs - 1] = NULL;
          command_tok[numArgs - 2] = NULL;
          
          dup2(filefd, STDOUT_FILENO);
          // pipedCommands(pipe_arguments);  //only calling the function here. Might want to pass it's return statement to something
        }

        //Pipe extra credit
        if(strcmp(*instr, "pwd") == 0){
					if( getcwd(cwd, sizeof(cwd)) == NULL)
              perror("getcwd() error");
            else
             printf("%s\n", cwd);
				} 
        
        //setenv
        else if (strcmp(*instr, "setenv") == 0) {  //check if command is exit
          //printf("%s", command_tok[1]);

          char name[] = "=\t\r\n";
          char *env_vars = strtok(command_tok[1], name);
          char *setenv_tok[MAX_COMMAND_LINE_ARGS];
          int i = 0;
          int j = 0;
      
          while (env_vars) {
            setenv_tok[i++] = env_vars;
            env_vars = strtok(NULL, " ");
          }
          setenv_tok[i] = NULL;

          const char *name_env = setenv_tok[0];
          //printf("%s\n", name_env);

          const char *value_env = setenv_tok[1];
          //printf("%s\n", value_env);

          setenv(name_env, value_env, 1);
        }
        //echo
        else if(strcmp(*instr, "echo") == 0){
          int j = 1;
					while (j < i) {
  					printf("%s ", command_tok[j]);
            j++;
					}
					printf("\n");
				}
        
        // env 
        else if(strcmp(*instr, "env") == 0){
          if(i == 1){
            i = 0;
            while(environ[i]) {
              printf("%s\n", environ[i++]); // prints in form of "variable=value"
            }
          }else{
            printf("%s\n", getenv(command_tok[1]));
          }
        }
        
        //cd
        else if(strcmp(*instr, "cd") == 0){
          chdir(command_tok[1]);
        }
      
        //exit
        else if (strcmp(*instr, "exit") == 0) {//check if command is exit
          //free(0);
          exit(0);
        }
    
        else{
          useChildPprocess = true;
          pid = fork();
          //check for child process
          if (pid == 0){
            ChildProcess(instr, numArgs, command_tok, isBackground, filefd, stdOutFd, gt);
          }else{
            //check if it is a background. if it isn't, wait for the child process
            if(!isBackground){
              wait(&status);
            }
          }
        }
        if (gt == 1) {
          dup2(stdOutFd, STDOUT_FILENO);
          close(filefd);
          close(stdOutFd);
        }
        
      
        // 3. Create a child process which will execute the command line input
        

  
        // 4. The parent process should wait for the child to complete unless its a background process
      
      
        // Hints (put these into Google):
        // man fork
        // man execvp
        // man wait
        // man strtok
        // man environ
        // man signals
        
        // Extra Credit
        // man dup2
        // man open
        // man pipes
        free(arguments);
        arguments = NULL;
        
        *command_tok = NULL;
        free(*command_tok);
    }
    // This should never be reached.
    return -1;
}

void signal_alarm(int signum){
  exit(1);
}

void signal_ctrlc(int signum){
  kill -9 %1;
}

void ChildProcess(char *instr[], int size, char *command_tok[], bool isBackground, int filefd, int stdOutFd, int gt){
  int x = 1;
  int checkExecVp = 0;
  char cwd[256];
  
  command_tok[size] = NULL;
  signal(SIGALRM, signal_alarm);
  if(!isBackground){
    alarm(10);
  }
   
 bool execfail = execvp(command_tok[0], command_tok) == -1;

 if (gt == 1) {
    dup2(stdOutFd, STDOUT_FILENO);
    close(stdOutFd);
    close(filefd);
  }
  if(execfail){
    printf("Invalid command: %d\n", errno);
  } 
}