#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
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
                //fprintf(stdout, "Command not found: %s \n", buffer);
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
                    break;
                }
            }
            if (basecase == 1) //The case that we do not have any REGEX chars
            {
                char *program_name = valueArray[0];
                int x;
                for (x = 0; x < strlen(valueArray[length - 1]); x++)
                {
                    if (valueArray[length - 1][x] == '\n')
                    {
                        valueArray[length - 1][x] = '\0';
                    }
                }
                execvp(program_name, valueArray);
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
                for (x = 0; x < strlen(valueArray[length - 1]); x++)
                {
                    if (valueArray[length - 1][x] == '\n')
                    {
                        valueArray[length - 1][x] = '\0';
                    }
                }
                int indexer = 0;
                int semicolon_counter = 0;
                for (indexer; indexer < length; indexer++)
                {
                    if (strcmp(valueArray[indexer], ";") == 0)
                    {
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
                        else if(child > 0)
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
                        continue;
                    }
                }
            }

            exit(1);
        }
        if ((pid = waitpid(pid, &status, 0)) < 0)
        {
            strerror(errno);
        }
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