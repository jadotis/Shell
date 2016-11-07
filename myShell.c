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
#define MAXLINE 4096
void sig_handler(int signo);
void sig_handler2(int signo);

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"

void clearArray(char * array[]);
pid_t pid;

//sudo code --user-data-dir="~/.vscode-root"
void clear()
{
    printf("\033[H\033[J");
}

int main(void)
{
    char buffer[MAXLINE];
    int status;
    clear();
    fprintf(stdout, "Welcome to the MyShell Process\n");
    fprintf(stdout, "Enter exit to quit!\n");
    fprintf(stdout, "%sm%sy%ss%sh%se%sl%sl%s %s> ", RED, GRN, YEL, BLU, MAG, CYN, RED, GRN, YEL);

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        perror("There was a signal ERROR\n");
        exit(-1);
    }
    if (signal(SIGTSTP, sig_handler2) == SIG_ERR)
    {
        perror("Error killing the process\n");
    }
    int semicolonruns;
    //bgin commandLine Parsing
    while (fgets(buffer, MAXLINE, stdin) && strcmp(buffer, "exit\n") != 0)
    {

        char *string = strtok(buffer, " ");
        char *valueArray[10];
        int i = 0;
        while (string != NULL)
        {
            valueArray[i++] = string;
            string = strtok(NULL, " ");
        }
        int length = i;
        if ((pid = fork()) < 0)
        {
            perror("ERROR in the FORK\n");
        }
        else if (pid == 0 && length == 1) //case of single command
        {
            buffer[strlen(buffer) - 1] = 0; //Create a null terminator
            if ((execlp(buffer, buffer, (char *)0)) < 0)
            {
                fprintf(stdout, "%s\n", strerror(errno));
            }
            exit(1);
        }
        else if (pid == 0 && length != 1)
        {
            int basecase = 1;
            int j = 0;
            for (j; j < length; j++)
            {
                if (strcmp(valueArray[j], "|") == 0 || strcmp(valueArray[j], ">") == 0 || strcmp(valueArray[j], "<") == 0 ||
                    strcmp(valueArray[j], "1>") == 0 || strcmp(valueArray[j], "2>") == 0 || strcmp(valueArray[j], "&>") == 0 ||
                    strcmp(valueArray[j], "&") == 0 || strcmp(valueArray[j], ";") == 0)
                {
                    basecase = 0;
                }
            }

            if (basecase == 1) //The case that we do not have any REGEX chars
            {
                char *program_name = valueArray[0];
                int x;
                int y;
                for(x = 0; x < length; x++)
                {
                    for(y = 0; y < strlen(valueArray[x]); y++)
                    {
                        if(valueArray[x][y] == '\n')
                        {
                            valueArray[x][y] = '\0';
                        }
                    }
                }
                char * temp[length];
                if(semicolonruns)
                {
                    int y;
                    for(y = 0; y < length; y++)
                    {
                        temp[y] = valueArray[y];
                        //fprintf(stdout, "temp is: %s\n", temp[y]);
                    }
                }
                pid_t child;
                if((child = fork()) < 0)
                {
                    perror("fork error");
                }
                if(child == 0 && semicolonruns == 0)
                {
                    //fprintf(stdout, "regular exec\n");
                    execvp(program_name, valueArray);
                    exit(1);
                }
                else if(child == 0 && semicolonruns)      //The error lies here
                {
                    temp[length] = (char * ) NULL;
                    execvp(temp[0], temp);
                    exit(1);
                }
                else
                {
                    int statflag;
                    child = waitpid(child, &statflag, 0);
                }
            }
            else
            {
                //Cases are ;
                //     >
                //     <
                //     1>
                //     2>
                //     &>
                //     PIPE
                //     &
                int x;
                int y;
                for(x = 0; x < length; x++)
                {
                    for(y = 0; y < strlen(valueArray[x]); y++)
                    {
                        if(valueArray[x][y] == '\n')
                        {
                            valueArray[x][y] = '\0';
                        }
                    }

                }
                int indexer = 0;
                int semicolon_counter = 0;
                for (indexer; indexer < length; indexer++)
                {
                    if (strcmp(valueArray[indexer], ";") == 0)
                    {
                        semicolonruns++;
                        semicolon_counter++;
                        pid_t child;
                        if ((child = fork()) < 0)
                        {
                            perror("Execution Error");
                        }
                        if (child == 0)
                        {
                            execlp(valueArray[indexer - 1], valueArray[indexer - 1], (char *)NULL);
                            exit(1);
                        }
                        else if(child > 0 && semicolon_counter < 2)
                        {
                            int statflag;
                            if ((child = waitpid(child, &statflag, 0)) < 0)
                            {
                            perror("Error waiting on the child\n");
                            }
                            pid_t child2;
                            if((child2 = fork()) < 0)
                            {
                                perror("Forking Error");
                            }
                            if(child2 == 0 && semicolon_counter == 1)
                            {
                                execlp(valueArray[indexer + 1], valueArray[indexer + 1], (char *)NULL);
                                exit(1);
                            }
                            else
                            {
                                child2 = waitpid(child2, &statflag, 0);
                            }
                      
                        }
                        int statflag;
                        child = waitpid(child, &statflag, 0);
                        continue;
                    }
                }
                

/*



This is the end of the cases for non-argument colon cancer



*/
                indexer = 0;
                for (indexer; indexer < length; indexer++)
                {
                    if (strcmp(valueArray[indexer], ">") == 0)
                    {
                        semicolonruns = 1;
                        pid_t child;
                        if((child = fork()) < 0)
                        {
                            perror("Forking Error");
                        }
                        if(child == 0)
                        {
                            int fd = 0;
                            fd = open(valueArray[indexer + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                            dup2(fd, 1);
                            execlp(valueArray[indexer -1], valueArray[indexer -1], (char *) NULL);
                            exit(1);
                        }
                        int statflag;
                        child = waitpid(child, &statflag, 0);        
                    }
                }







            }
            if(semicolonruns)
            {
                exit(1);
            }
            else
            {
                exit(0);
            }
        }
        if ((pid = waitpid(pid, &status, 0)) < 0)
        {
            strerror(errno);
        }
        semicolonruns = status;
        fprintf(stdout, "%sm%sy%ss%sh%se%sl%sl%s %s> ", RED, GRN, YEL, BLU, MAG, CYN, RED, GRN, YEL);
    }
    printf("Thank you for using the shell!\n");
    exit(1);
}

//Signal Handlers
void sig_handler(int signo)
{
    if (signo == SIGINT && pid == 0)
    {
        printf("\nExiting...\n");
        exit(0);
    }
}

void sig_handler2(int signo)
{
    if (signo == SIGTSTP)
    {
        printf("\nThank you for using the shell\n");
        printf("HAVE A SHITTY DAY\n");
        exit(0);
    }
}

void clearArray(char* array[])
{
    int i = 0;
    while(array[i] != NULL)
    {
        fprintf(stdout, "The value is in the index: %d is : %s\n", i, array[i]);
        i++;
    }
}