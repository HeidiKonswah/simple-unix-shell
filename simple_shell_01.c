#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
//------------------
#define RL_BUFFSIZE 1024
//define a signal handler type which is a func accepting an int and returning void
typedef void (*sighandler)(int);
//first arg is the signal number, second is a pointer to the handler
sighandler signal(int signumber, sighandler handler);


//function declarations 
void prompt(); 
char *rl_readline();
char **parseInput(char *);

// void childSignalHandler(int signum) {
    
// }
 
int main(){
    char **cmd;
    char *input;
    int status;
    int terminated = 0;
    int wait_res;
    // signal(SIGCHLD, childSignalHandler);

    while (!terminated){
        int isBG = 0;
        prompt();
        input = rl_readline();
        cmd = parseInput(input);
        if(cmd[1] && strcmp(cmd[1], "&")==0){
            isBG = 1; 
            cmd[1] = NULL;
        }
   
        if(strcmp(cmd[0], "exit") == 0){
            terminated = 1; 
            continue;
        }
        if(strcmp(cmd[0], "cd") == 0){
            if (chdir(cmd[1]) == -1){
               perror("An error occured\n"); 
            }
            //skip fork
            continue;
        }


        pid_t childPID = fork();
        if (childPID < 0){
            perror("An Error occured\n");
            //terminate program
            exit(1);
        }else if (childPID == 0){
            if (isBG) {
                cmd[1] = NULL;
            }
            execvp(cmd[0], cmd);
            perror("An error occured\n");
            //only terminate child
            exit(1);
            
        } else if (!isBG){
            childPID = wait(&status);
        }     
       
        free(input);
        free(cmd);
    }

    return 0;
}


//clear screen
void prompt(){
    static int flag = 1;
    if (flag){
        const char *CLEAR_SCREEN_ANSI = " \e[1;1H\e[2J";
        write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
        flag = 0;
    }
    printf(">>");
}

//read a line from user
char *rl_readline(void){
    int bufferSize = RL_BUFFSIZE;
    char *buffer = malloc(sizeof(char) * bufferSize);
    int position = 0;
    int c;

    while (1){
        //read a character
        char c = getchar();

        // if character is an end of line, replace it with a null and return
        if (c == EOF || c == '\n')
        {
            buffer[position] = '\0';
            return buffer;
        }
        else
        {
            buffer[position] = c;
        }

        //increment position to access the next array element
        position++;

        // if next character will go outside current buffer size, reallocate
        if (position >= bufferSize)
        {
            bufferSize += RL_BUFFSIZE;
            buffer = realloc(buffer, bufferSize);
        }
    }
}

//parse input
char **parseInput(char *input) {
    char **cmd = malloc(8 * sizeof(char *));
    if(cmd == NULL){
        printf("Allocation failed\n"); 
        exit(1);
    }
    int i = 0;
    char *parsed;
    parsed = strtok(input, " ");

    while (parsed != NULL){
        cmd[i] = parsed;
        i++;
        parsed = strtok(NULL, " ");
    }
    cmd[i] = NULL;
    return cmd;
}