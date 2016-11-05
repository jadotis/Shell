#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#define  MAXLINE  4096
void sig_handler(int signo);
void sig_handler2(int signo);

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
    //execl("/usr/bin/clear", NULL);
    clear();
    fprintf(stdout,"Welcome to the MyShell Process\n");
    fprintf(stdout,"Enter exit to quit!\n");
    fprintf(stdout,"myshell > ");

    if(signal(SIGINT, sig_handler) == SIG_ERR)
    {
        perror("There was a signal ERROR\n");
        exit(-1);
    }
    if(signal(SIGTSTP, sig_handler2) == SIG_ERR)
    {
        perror("Error killing the process\n");
    }

    //bgin commandLine Parsing
    while(fgets(buffer, MAXLINE, stdin) && strcmp(buffer,"exit\n") != 0)
    {
        buffer[strlen(buffer) -1] = 0; //Create a null terminator
        char * string = strtok(buffer, " ");
        char * valueArray[10];
        int i = 0;
        while(string != NULL)
        {
            valueArray[i++] = string;
            string = strtok(NULL, " ");

        }
        int length = i;
        printf("The length is : %d\n", length);
        if((pid = fork()) < 0)
        {
            perror("ERROR in the FORK\n");
        }
        else if (pid == 0 && length == 1)
        {
            if((execlp(buffer, buffer, (char *) 0)) < 0)
            {
                //fprintf(stdout, "Command not found: %s \n", buffer);
                fprintf(stdout, "%s\n",strerror(errno));
                exit(1);
            }
            exit(1);
        }
        else if(pid == 0)
        {

            printf("Multiple Args Detected\n");
            exit(1);


        }
        if( (pid = waitpid(pid, &status, 0)) < 0)
        {
            strerror(errno);
        }
        fprintf(stdout, "myshell > ");
        


    }
    
    printf("Thank you for using the shell!\n");
    exit(1);


}

void sig_handler(int signo)
{
    if(signo == SIGINT && pid == 0)
    {
        printf("\nForceFul Quit\n");
        printf("Thank you for using the shell!\n");
        exit(0);
    }
}
void sig_handler2(int signo)
{
    if(signo == SIGTSTP)
    {
        printf("Thank you for using the shell\n");
        printf("HAVE A SHITTY DAY");
        exit(1);
    }
}