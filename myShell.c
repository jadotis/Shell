#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

/* Macro Definitions */
#define MAXLINE 4096
#define MAXPIPES 10
#define MAX_ARGS 10

#define RED "\x1B[31m" // Colors
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"

/* FUNCTION DEFINITIONS */
void pipeCreator(char *tokens[]); //Creates the pipes for later
void doBasecase(char * tokens[], int length); //regular execs
void westWorld(char * tokens[], int length); //Does pretty much everything
void semicolonHandler(char * tokens[], int length);
void getArgsAfter(char *dest[], char * tokens[], int i);
void getArgsBefore(char *dest[], char * tokens[], int i);
void redirStdout(char * tokens[], int i); // Redirects stdout to outputfile
void redirStderr(char * tokens[], int i); //Redicted stderr to outputfile
void redirBoth(char * tokens[], int i); //Redicted stderr to outputfile
void runinBackground(char * tokens[], int i); //Redicted stderr to outputfile
void lovingDemPipes(char * tokens[], int i);
void directoryHanler(char * tokens[]);
void sig_handler(int signo);
void sig_handler2(int signo);
void clear();

/* GLOBAL VARIABLES */
int stdoutCopy;;
int stdinCopy;
int pipes[MAXPIPES][2];
int semiTotal = 0;
int semiCounter = 0;

int pipeTotal = 0;
int pipeIndexer = 0;
pid_t pid;
pid_t parent;

/* MAIN ROUTINE */
int main()
{
    parent = getpid();
    //dup2(STDOUT_FILENO, stdoutCopy);
    //dup2(STDOUT_FILENO, stdoutCopy);
    clear();
    fprintf(stdout, "%sWelcome to the MyShell Processd\n", BLU);
    fprintf(stdout, "%sEnter exit ot quit!\n", BLU);
    /* Setup signal handlers */
    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        perror("Signal Error:");
        exit(-1);
    }
    if (signal(SIGTSTP, sig_handler2) == SIG_ERR)
    {
        perror("Error killing via signal:");
    }
    /*  End Signal handling  */

    /* BEGIN COMMAND LINE PARSING */
    fprintf(stdout, "%sm%sy%ss%sh%se%sl%sl%s %s> ", RED, GRN, YEL, BLU, MAG, CYN, RED, GRN, YEL);

    char *line = "";
    while (strcmp(line, "exit") != 0)
    {
        line = (char *)NULL;
        line = readline("");
        //Exit case:
        if (strcmp(line, "exit") == 0)
        {
            fprintf(stdout, "%sThanks for using our Shell! Its better than the other ones...\n", RED);
            break;
        }
        //Tokenize the input string
        char *string = strtok(line, " ");
        char *tokens[20];
        int i = 0;
        while (string != NULL)
        {
            tokens[i++] = string;
            string = strtok(NULL, " ");
        }
        int length = i; //For keeping track of length of array
        //Count semicolons and pipes
        int j;
        for (j = 0; j < length; j++)
        {
            if (strcmp(tokens[j], "|") == 0)
            {
                pipeTotal++;
            }
            else if (strcmp(tokens[j], ";") == 0)
            {
                semiTotal++;
            }
        }
        //Create pipes if there are pipes to create
        if (pipeTotal > 0)
        {
            pipeCreator(tokens);
        }
        //Begin character checking routine, calling corresponding functions to handle each control character

        westWorld(tokens, length);
        memset(tokens, (char*)NULL,8*length);
        memset(pipes, (char*)NULL, 4*2*MAXPIPES);
        pipeIndexer = 0;
        pipeTotal = 0;
        fprintf(stdout, "%sm%sy%ss%sh%se%sl%sl%s %s> ", RED, GRN, YEL, BLU, MAG, CYN, RED, GRN, YEL);

        } //End main for loop

    } //End main while loop


/*****************************************************************************************
******************************************************************************************
****************************HELPER FUNCTIONS**********************************************
******************************************************************************************
*****************************************************************************************/
int isBasecase(char * tokens[], int length)
{
    int j;
    for(j = 0; j < length; j++)
    {
        if (strcmp(tokens[j], "|") == 0 || strcmp(tokens[j], ">") == 0 || strcmp(tokens[j], "<") == 0 || strcmp(tokens[j], "1>") == 0 || strcmp(tokens[j], "2>") == 0 || strcmp(tokens[j], "&>") == 0 || strcmp(tokens[j], "&") == 0 || strcmp(tokens[j], ";") == 0)
        {
            return 0;
        }
    }
    return 1;
}

//THE MEAT AND POTATOS
void westWorld(char * tokens[], int length)
{
    semiCounter = 0;
    if(isBasecase(tokens, length) && strcmp(tokens[0],"cd") != 0) // We do not have any control characters. (WORKING)
    {
        doBasecase(tokens, length); 
    }
    else
    {
        int i;
        for(i = 0; i < length; i++) //This is for other cases
        {
            if(strcmp(tokens[i], ";") == 0)
            {
                semiCounter++;
                semicolonHandler(tokens, i);
            }
            else if(strcmp(tokens[i], "cd") == 0)
            {
                directoryHandler(tokens);
            }
            else if(strcmp(tokens[i], ">") == 0 || strcmp(tokens[i],"1>") == 0)
            {
                redirStdout(tokens, i);   
            }
            else if(strcmp(tokens[i], "2>") == 0)
            {
                redirStderr(tokens, i);
            }
            else if(strcmp(tokens[i], "<") == 0)
            {
                redirStdin(tokens, i);
            }
            else if(strcmp(tokens[i], "&>") == 0)
            {
                redirBoth(tokens, i);
            }
            else if(strcmp(tokens[i], "&") == 0)
            {
                runinBackground(tokens, i);
            }
            else if(strcmp(tokens[i], "|") == 0)
            {
                lovingDemPipes(tokens, i);
                pipeIndexer++;
            }
            else //The case that it is just a regular char
            {
                continue; 
            }
        }

    }
}
/************************************END WESTWORLD********************************
*********************************************************************************/
void directoryHandler(char * tokens[]){
    char * buffer = malloc(100);
    buffer = getcwd(buffer, 100);
    int n = strlen(buffer);
    char * newEnv = buffer;
    if (strcmp(tokens[1], "..") == 0)
    {
        while (newEnv[n] != '/')
        {
            newEnv[n] = '\0';
            n--;
        }
    }
    else if (tokens[1][0] == '/')
    {
        newEnv = tokens[1];
    }
    else if (tokens[1][0] == '.' && tokens[1][1] == '/')
    {
        int i = 2;
        char *partial = tokens[1] + 1;
        strcat(newEnv, partial);
    }
    else
    {
        strcat(newEnv, "/");
        strcat(newEnv, tokens[1]);
    }
    if ((setenv("PWD", newEnv, 1)) < 0)
    {
        perror("");
    }
    if ((chdir(newEnv)) < 0)
    {
        perror("");
    }
    return;
              
}







void lovingDemPipes(char * tokens[], int i)
{
    pid_t child;
    if((child = fork()) < 0)
    {
        perror("");
    } 
    else if(child == 0)
    {
        if(pipeIndexer == 0)
        {
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            dup2(pipes[pipeIndexer][1], STDOUT_FILENO); //Direct standard out to write end of pipe.
            close(pipes[pipeIndexer][0]); //close the read end
            char * argv[MAX_ARGS];
            memset(argv, (char *)NULL, 8*MAX_ARGS);
            getArgsBefore(argv, tokens, i);
            if (argv[0] == 0)
            {
                if ((execlp(tokens[i - 1], tokens[i-1], (char*)NULL)) < 0)
                {
                    perror("Exec Error");
                    exit(1);
                }
            }
            else
            {
                if((execvp(argv[0], argv)) < 0)
                {
                    perror("Exec Error");
                    exit(1);
                }
            }

        }
        else
        {
            exit(1);
        }

    }
    else if(child > 0)
    {
        pid_t childTwo;
        sleep(1);
        close(pipes[pipeIndexer][1]);
        if((child = waitpid(child, NULL, 0)) < 0)
        {
            perror("Error waiting on the child");
        }
        if(pipeIndexer == pipeTotal - 1)
        {
                if((childTwo = fork()) < 0)
                {
                        perror("");
                } 
                else if(childTwo == 0)
                {
                    dup2(pipes[pipeIndexer][0], STDIN_FILENO);
                    dup2(pipes[pipeIndexer][1], STDOUT_FILENO);
                    //close(pipes[pipeIndexer][1]);
                    char * argv[MAX_ARGS];
                    memset(argv, (char *)NULL, 8*MAX_ARGS);
                    getArgsAfter(argv, tokens, i);
                    if (argv[0] == 0)
                    {
                        if ((execlp(tokens[i + 1], tokens[i + 1], (char*)NULL)) < 0)
                        {
                            perror("Exec Error");
                            exit(1);
                        }
                    }
                    else
                    {
                        if((execvp(argv[0], argv)) < 0)
                        {
                            perror("Exec Error");
                            exit(1);
                        }
                     }
                }
                else //in the parent again.
                {
                    close(pipes[pipeIndexer][1]);
                    if((childTwo = waitpid(childTwo, NULL, 0)) < 0)
                    {
                        perror("Error waiting on the child");
                    }

                }  
        }
        else //The case of multiple pipes
        {
            pid_t childThree;
            if((childThree = fork()) < 0)
                {
                    perror("");
                } 
                else if(childThree == 0)
                {
                    close(STDIN_FILENO);
                    close(STDOUT_FILENO);
                    dup2(pipes[pipeIndexer][0], STDIN_FILENO); //put readend into STDOUT
                    dup2(pipes[pipeIndexer+1][1], STDOUT_FILENO);
                    //close(pipes[pipeIndexer][1]);
                    close(pipes[pipeIndexer+1][0]);
                    char * argv[MAX_ARGS];
                    memset(argv, (char *)NULL, 8*MAX_ARGS);
                    getArgsAfter(argv, tokens, i);
                    char * buffer = malloc(MAXLINE);
                    if (argv[0] == 0)
                    {
                        if ((execlp(tokens[i + 1], tokens[i + 1], (char*)NULL)) < 0)
                        {
                            perror("Exec Error");
                            exit(1);
                        }
                    }
                    else
                    {
                        if((execvp(argv[0], argv)) < 0)
                        {
                            perror("Exec Error");
                            exit(1);
                        }
                     }
                }
                else // parent
                {
                    close(pipes[pipeIndexer][1]);
                    if((childThree = waitpid(childThree, NULL, 0)) < 0)
                    {
                        perror("Error waiting");
                    }
                }

        }
    }


}



void runinBackground(char * tokens[], int i)
{
    pid_t child;
    if((child = fork()) < 0)
    {
        perror("");
    }
    else if(child == 0)
    {
        char * argv[MAX_ARGS];
        memset(argv, (char *)NULL, 8*MAX_ARGS);
        getArgsBefore(argv, tokens, i);

        if (argv[0] == 0)
        {
            sleep(1);
            if ((execlp(tokens[i - 1], tokens[i-1], (char*)NULL)) < 0)
            {
                perror("Exec Error");
            }
        }

        else
        {
            sleep(1);
            if((execvp(argv[0], argv)) < 0)
            {
                perror("Exec Error");
            }
        }

    }

    else
    {
        fprintf(stdout, "Executing process number [%d] -- %s.\n", child, tokens[i-1]);
        fflush(stdout);
        sleep(2);
    }
}



void redirBoth(char * tokens[], int i)
{
    pid_t child;
    if ((child = fork()) < 0)
    {
        perror("Error Forking");
    }

    if (child == 0)
    {
        int fd;
        fd = open(tokens[i+1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if(fd < 0)
        {
            perror("");
            exit(1);    
        }
        dup2(fd, 2);
        dup2(fd, 1);
        char * argv[MAX_ARGS];
        memset(argv, (char *) NULL, 8*MAX_ARGS);
        getArgsBefore(argv, tokens, i);
        if(argv[0] == NULL)
        {
            if((execlp(tokens[i-1],tokens[i-1],(char*) NULL )) < 0)
            {
                perror("Exec error");
                exit(1);
            }
        }
        else
        {
            if((execvp(argv[0], argv)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
    }
    else
    {
        if((child = waitpid(child, NULL, 0)) < 0)
        {
            perror("couldnt wait");
        }
    }

}


void redirStdin(char * tokens[], int i)
{
    pid_t child;
    if ((child = fork()) < 0)
    {
        perror("Error Forking");
    }

    if (child == 0)
    {
        int fd;
        fd = open(tokens[i+1], O_RDONLY, S_IRUSR | S_IWUSR);
        if(fd < 0)
        {
            perror("");
            exit(1);    
        }
        dup2(fd, 0);
        char * argv[MAX_ARGS];
        memset(argv, (char *) NULL, 8*MAX_ARGS);
        getArgsBefore(argv, tokens, i);
        if(argv[0] == NULL)
        {
            if((execlp(tokens[i-1],tokens[i-1],(char*) NULL )) < 0)
            {
                perror("Exec error");
                exit(1);
            }
        }
        else
        {
            if((execvp(argv[0], argv)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
    }
    else
    {
        if((child = waitpid(child, NULL, 0)) < 0)
        {
            perror("couldnt wait");
        }
    }

}



void redirStderr(char * tokens[], int i)
{
    pid_t child;
    if ((child = fork()) < 0)
    {
        perror("Error Forking");
    }

    if (child == 0)
    {
        int fd;
        fd = open(tokens[i+1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        dup2(fd, 2);
        char * argv[MAX_ARGS];
        memset(argv, (char *) NULL, 8*MAX_ARGS);
        getArgsBefore(argv, tokens, i);
        if(argv[0] == NULL)
        {
            if((execlp(tokens[i-1],tokens[i-1],(char*) NULL )) < 0)
            {
                perror("Exec error");
                exit(1);
            }
        }
        else
        {
            if((execvp(argv[0], argv)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
    }
    else
    {
        if((child = waitpid(child, NULL, 0)) < 0)
        {
            perror("couldnt wait");
        }
    }

}


void redirStdout(char * tokens[], int i)
{
    int fd;
    fd = open(tokens[i+1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    pid_t child;
    if ((child = fork()) < 0)
    {
        perror("Error Forking");
    }
    if (child == 0)
    {
        dup2(fd, 1);
        char * argv[MAX_ARGS];
        memset(argv, (char *) NULL, 8*MAX_ARGS);
        getArgsBefore(argv, tokens, i);
        if(argv[0] == NULL)
        {
            if((execlp(tokens[i-1],tokens[i-1],(char*) NULL )) < 0)
            {
                perror("Exec error");
                exit(1);
            }
        }
        else
        {
            if((execvp(argv[0], argv)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
    }
    else
    {
        close(fd);
        if((child = waitpid(child, NULL, 0)) < 0)
        {
            perror("couldnt wait");
        }
    }

}

void semicolonHandler(char * tokens[], int i)
{
    pid_t child;
    if((child = fork()) < 0)
    {
        perror("Fork Error");
    }
    else if(child == 0 && semiCounter == 1) //Child processes
    {
        char * argv[MAX_ARGS];
        memset(argv, (char*)NULL, 8*MAX_ARGS);
        argv[0] = NULL;
        getArgsBefore(argv, tokens, i);
        if (argv[0] == NULL)
        {
            if ((execlp(tokens[i-1], tokens[i-1], (char*)NULL)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }

        else
        {
            if ((execvp(argv[0], argv)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
    }

    else if(child == 0)
    {
        exit(1);
    }

    else if (child > 0)
    {
        if ((child = waitpid(child, NULL, 0)) < 0)
        {
            perror("Error waiting on child");
        }
    }
    pid_t child2;
    if ((child2 = fork()) < 0)
    {
        perror("Forking Error");
    }
    else if (child2 == 0)
    {
        char * argp[MAX_ARGS];
        memset(argp, (char*)NULL, 8*MAX_ARGS);

        getArgsAfter(argp, tokens, i);
        if (argp[0] == NULL)
        {
            if((execlp(tokens[i+1], tokens[i+1], (char*)NULL))<0)
            {
                perror("Exec Error");
                exit(1);
            }
        }

        else
        {
            if ((execvp(argp[0], argp)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
    }

    else if (child2 > 0)
    {
        if ((child2 = waitpid(child2, NULL, 0)) < 0)
        {
            perror("Error Waiting");
        }
    }
}
void getArgsAfter(char *dest[], char * tokens[], int i)
{
    int numAfter = 0;
    int n = i + 1;

    while (tokens[n] != NULL && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
            strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 && 
            strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
            {
                numAfter++;
                n++;
            }

    if (numAfter == 1)
    {

        fflush(stdout);
        return;
    }

    else
    {
        for (n = 0; n < numAfter; n++)
        {
            dest[n] = tokens[i + n + 1];
        }
        dest[numAfter] = (char*) NULL;
    }
}


void getArgsBefore(char *dest[], char * tokens[], int i)
{
    int numBefore = 0;
    int n = i - 1;

    while (n >= 0 && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
            strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 && 
            strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
            {
                numBefore++;
                n--;
            }

    if (numBefore == 1)
    {
        return; // If there is only one argument before, the dest array will be null as an indication that we have no args
    }

    else
    {
        for (n = 0; n <= numBefore; n++)
        {
            dest[n] = tokens[i - numBefore--];
        }
    }
    
    dest[n] = (char*)NULL;
}


void doBasecase(char * tokens[], int length) //BaseCase logic
{
    pid_t pid;
    if((pid = fork()) < 0)
    {
        perror("Error in the basecase");
    }
    else if(pid == 0)
    {
        if (length == 1)
        { 
            if((execlp(tokens[0], tokens[0], (char*) NULL)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
        else
        {
            if((execvp(tokens[0], tokens)) < 0)
            {
                perror("Exec Error");
                exit(1);
            }
        }
    }
    else
    {
        if((pid = waitpid(pid, NULL, 0)) < 0)
        {
            perror("");
        }
    }
}

/* clear() -- clears the screen. */
void clear()
{
    printf("\033[H\033[J");
}

/* sig_handler() -- signal handler for SIGINT */
void sig_handler(int signo)
{
    fprintf(stdout,"\n%sGuess that program was not fun\n",RED);
    if (signo == SIGINT && parent != getpid())
    {
        exit(0);
    }
}

/* sig_handler2() -- signal handler for SIGTSTP */
void sig_handler2(int signo)
{
    if (signo == SIGTSTP)
    {
        fprintf(stdout, "\n%sThanks for using the shell have a shitty day\n",RED);
        exit(0);
    }
}

/* pipeCreator() -- creates pipes and stores them in global array pipe */
void pipeCreator(char *tokens[])
{
    int i;
    for (i = 0; i < pipeTotal; i++)
    {
        if ((pipe(&pipes[i])) < 0)
        {
            perror("Error creating pipes:");
        }
    }
}