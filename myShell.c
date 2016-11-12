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

#define MAXLINE 4096
#define MAXPIPES 10
void sig_handler(int signo);
void sig_handler2(int signo);

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"

//FUNCTION DEFINITIONS:
void clearArray(char *array[]);
void doRest(char *tokens[], int length);
void pipeCreator(char *tokens[], int length);
int pipes[MAXPIPES][2];
pid_t pid;
int pipeflag; 


//sudo code --user-data-dir="~/.vscode-root"
void clear()
{
    printf("\033[H\033[J");
}

int main(void)
{
    char *buffer = ""; //Potentially malloc
    int status;
    clear();
    fprintf(stdout, "Welcome to the MyShell Process\n");
    fprintf(stdout, "Enter exit to quit!\n");

    if (signal(SIGINT, sig_handler) == SIG_ERR)
    {
        perror("There was a signal ERROR\n");
        exit(-1);
    }
    if (signal(SIGTSTP, sig_handler2) == SIG_ERR)
    {
        perror("Error killing the process");
    }
    //bgin commandLine Parsing
    while (strcmp(buffer, "exit") != 0)
    {
        buffer = (char*) NULL;
        fprintf(stdout, "%sm%sy%ss%sh%se%sl%sl%s %s> ", RED, GRN, YEL, BLU, MAG, CYN, RED, GRN, YEL);
        buffer = readline("");
        if (strcmp(buffer, "exit") == 0)
        {
            fprintf(stdout, "WELL GO FUCK YOURSELF\n");
            break;
        }
        char *string = strtok(buffer, " ");
        char *tokens[20];
        int i = 0;
        while (string != NULL)
        {
            tokens[i++] = string;
            string = strtok(NULL, " ");
        }
        pipeCreator(tokens, i);
        if(pipeflag == 0)
        {
            doRest(tokens, i);
        }
        else
        {
            //handle the pipes.
        }
    }
}

void doRest(char *tokens[], int length)
{
    int basecase = 1;
    if (length == 1)
    {
      if (strcmp(tokens[0], "cd") == 0)
	{
	  return;
	}
      
        if ((pid = fork()) < 0)
        {
            perror("Fork Error");
        }
	
        else if (pid == 0)
        {
            if ((execlp(tokens[0], tokens[0], (char *)NULL)) < 0)
            {
                perror("EXEC ERROR FOR BASECASE");
            }
            exit(1);
        }
        else
        {
            int statflag;
            if ((pid = waitpid(pid, &statflag, 0)) < 0)
            {
                perror("Could not wait on the pid child");
            }
        }
    }
    /*
    FOR A NORMAL COMMAND without basecase.
*/
    else if (length > 1)
    {
        int j = 0;
        for (j; j < length; j++)
        {
            if (strcmp(tokens[j], "|") == 0 || strcmp(tokens[j], ">") == 0 || strcmp(tokens[j], "<") == 0 ||
                strcmp(tokens[j], "1>") == 0 || strcmp(tokens[j], "2>") == 0 || strcmp(tokens[j], "&>") == 0 ||
                strcmp(tokens[j], "&") == 0 || strcmp(tokens[j], ";") == 0)
            {
                basecase = 0;
            }
        }
        //This is a single command with Arguments
        if (basecase == 1)
        {
            tokens[length] = (char *)NULL;
            pid_t child;

	    if (strcmp(tokens[0], "cd") == 0)//BEGIN "cd" case
	      {
		char * buffer = (char*) malloc(100);
		buffer = getcwd(buffer, 100);

		char *newEnv = buffer;
		
		int n = strlen(buffer)-1;

		if (strcmp(tokens[1], "..") == 0)
		  {
		    while (newEnv[n] != '/')
		      {
			newEnv[n] = '\0';
			n--;
		      }
		  }

		if (tokens[1][0] == '/')
		  {
		    newEnv = tokens[1];
		  }


		if(tokens[1][0] == '.' && tokens[1][1] == '/')
		  {
		    int i = 2;
		    char * partial = tokens[1]+1;

		    strcat(newEnv, partial);
		  }
		
		if((setenv("PWD", newEnv, 1)) < 0)
		  {
		    perror("");
		  }

		if((chdir(newEnv)) < 0)
		  {
		    perror("");
		  }
		return;
	      }//END "cd" case
	    
            if ((child = fork()) < 0)
            {
                perror("fork error in the basecase");
            }
            else if (child == 0)
            {
                if ((execvp(tokens[0], tokens)) < 0)
                {
		  perror("");
                }
                exit(1);
            }
            else
            {
                int statflag;
                if ((child = waitpid(child, &statflag, 0)) < 0)
                {
                    perror("Error waiting on the child in the basecase");
                }
            }
        }
        //end basecase

        //Start the regular expressions.
        else
        {

            int argPtr;
            int semicolon_counter = 0;
            for (argPtr = 0; argPtr < length; argPtr++)
            {
	      if (strcmp(tokens[argPtr], "cd") == 0) //BEGIN "cd" case
		{
		  fprintf(stdout, "HIT IT\n");
		  char *value = getenv("PATH");
		  fprintf(stdout, "Path: %s", value);
		  fflush(stdout);
		}//END "cd" case
	      
	      
                if (strcmp(tokens[argPtr], ";") == 0)
                {
                    semicolon_counter++;
                    pid_t child2;

                    if ((child2 = fork()) < 0)
                    {
                        perror("Fork error in Child2");
                    }
                    if (child2 == 0 && semicolon_counter < 2) // in the child
                    {
                        int argsBefore = 0;
                        int n = argPtr - 1;
                        while (n > -1 && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
                               strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 &&
                               strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
                        {
                            argsBefore++;
                            n--;
                        }
                        //Counts the number of arguments before

                        if (argsBefore == 1) //Case that we only have a single cmd and no arguments.
                        {
                            if ((execlp(tokens[argPtr - 1], tokens[argPtr - 1], (char *)NULL)) < 0)
                            {
                                perror("Exex error in the single cmd \n");
                            }
                            exit(1);
                        }
                        else // The case of the fact that we have a cmd with arguments.
                        {
                            char *argv[argsBefore + 1];
                            for (n = 0; n <= argsBefore; n++)
                            {
                                argv[n] = tokens[argPtr - argsBefore--];
                            }
                            argv[n] = (char *)NULL;
                            if ((execvp(argv[0], argv)) < 0)
                            {
                                perror("Error in multiple argument exec");
                            }
                            exit(1);
                        }
                    }
                    else if(child2 == 0)
                    {
                        exit(1);
                    }
                    //What to do if we have more than one semicolon
                    else if (child2 > 0)
                    {
                        if (semicolon_counter)
                        {
                            int statflag;
                            if ((child2 = waitpid(child2, &statflag, 0)) < 0)
                            {
                                perror("Could not wait for the child in the parent");
                            }
                        }
                        pid_t child3;
                        if ((child3 = fork()) < 0)
                        {
                            perror("Forking error in the third child\n");
                        }
                        if (child3 == 0)
                        {
                            int argsAfter = 0;
                            int n = argPtr + 1;
                            while (tokens[n] != NULL && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
                                   strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 &&
                                   strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
                            {
                                argsAfter++;
                                n++;
                            }
                            fflush(stdout);
                            if (argsAfter == 1)
                            {
                                if ((execlp(tokens[argPtr + 1], tokens[argPtr + 1], (char *)NULL)) < 0)
                                {
                                    perror("Error Execing the Second SingleArg\n");
                                }
                                fflush(stdout);
                                exit(10);
                            }
                            if (argsAfter > 1)
                            {
                                char *argv[argsAfter + 1];
                                for (n = 0; n < argsAfter; n++)
                                {
                                    argv[n] = tokens[argPtr + n + 1];
                                }
                                argv[n] = (char *)NULL; //We Should Check this out.
                                if ((execvp(argv[0], argv)) < 0)
                                {
                                    perror("We had an exec error in the multiarg cse\n");
                                }
                                exit(0);
                            }
                        }
                        else if (child3 != 0)
                        {
                            int statflag;
                            if ((child3 = waitpid(child3, &statflag, 0)) < 0)
                            {
                                perror("We had an error waiting on the third child\n");
                            }
                        }
                        //NOW WE ARE BACK IN THE PARENT

                    }

                } //End the semicolon case
                if (strcmp(tokens[argPtr], ">") == 0 || strcmp(tokens[argPtr], "1>") == 0){
                    pid_t child;
                    if((child = fork()) < 0)
                    {
                        perror("Could not fork for redirection char\n");
                    }
                    if(child == 0)
                    {
                        int fd = 0;
                        fd = open(tokens[argPtr + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        dup2(fd,1);

                        int argsBefore = 0;
                        int n = argPtr - 1;

                        while (tokens[n] != NULL && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
                                strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 &&
                                strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
                            {
			      argsBefore++;
                                n--;
                            }

                        if (argsBefore == 1)
                        {
                            if((execlp(tokens[argPtr - 1], tokens[argPtr - 1], (char * )NULL)) < 0)
                            {
                                perror("EXEC ERROR");
                            }
                            exit(1);
                        }

                        else
                        {
                            char *argv[argsBefore+1];
                            for (n=0; n <= argsBefore; n++)
                            {
                                argv[n] = tokens[argPtr - argsBefore];
                                argsBefore--;
                            }
                            argv[n] = (char*) NULL;
                            if((execvp(argv[0], argv)) < 0)
                            {
                                perror("EXECVP ERROR IN < CASE");
                            }
                            exit(1);
                        }
                    }
                    else
                    {
                        int statflag;
                        if((child = waitpid(child, &statflag, 0)) < 0)
                        {
                            perror("error waiting on the child\n");
                        }
                    }



                }//END the direction case;

		// BEGIN "<" direction case;
		if (strcmp(tokens[argPtr], "<") == 0)
		  {
		    pid_t child;
		    if ((child = fork()) < 0)
		      {	 	
			perror("FORKING ERROR in '<' case");
		      }

		    else if (child == 0)
		      {
			int fd = 0;
			fd = open(tokens[argPtr + 1], O_RDONLY, S_IRUSR | S_IWUSR);
			dup2(fd, 0);

			int argsBefore = 0;
			int n = argPtr-1;

			 while (tokens[n] != NULL && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
                                strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 &&
                                strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
			   {
			     argsBefore++;
			     n--;
			   }

			 if (argsBefore == 1)
			   {
			     if ((execlp(tokens[argPtr-1], tokens[argPtr-1], (char*)NULL)) < 0)
			       {
				 perror("EXEC ERROR with single command in '<' case");
			       }
			     
			     exit(1);
			   }

			 else
			   {
			     char *argv[argsBefore+1];
			     for (n=0; n <= argsBefore; n++)
			       {
				 argv[n] = tokens[argPtr - argsBefore];
				 argsBefore--;
			       }
			     argv[n] = (char*) NULL;
			     if((execvp(argv[0], argv)) < 0)
			       {
				 perror("EXECVP ERROR IN < CASE");
			       }
			     exit(1);
			   }
		      }


		    else
		      {
			int statflag;
			if((child = waitpid(child, &statflag, 0)) < 0)
			  {
			    perror("ERROR waiting on child in '<' case");
			  }
		      }
		    
		  }//END "<" direction case;

		if (strcmp(tokens[argPtr], "2>") == 0) //BEGIN "2>" direction case
		  {
		    pid_t child;
		    if ((child = fork()) < 0)
		      {
			perror("FORK ERROR in '2>' case");
		      }

		    else if (child == 0)
		      {
			int fd = 0;
			fd = open(tokens[argPtr + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			dup2(fd, STDERR_FILENO);
			
			int argsBefore = 0;
			int n = argPtr-1;

			while (tokens[n] != NULL && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
			       strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 &&
			       strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
			  {
			    argsBefore++;
			    n--;
			  }
			
			if (argsBefore == 1)
			  {
			    if ((execlp(tokens[argPtr-1], tokens[argPtr-1], (char*)NULL)) < 0)
			      {
				perror("EXEC ERROR with single command in '2>' case");
			      }
			    
			    exit(1);
			  }
			
			else
			  {
			    char *argv[argsBefore+1];
			    for (n=0; n <= argsBefore; n++)
			      {
				argv[n] = tokens[argPtr - argsBefore];
				argsBefore--;
			      }
			    argv[n] = (char*) NULL;
			    if((execvp(argv[0], argv)) < 0)
			      {
				perror("EXECVP ERROR IN < CASE");
			      }
			    exit(1);
			    
			  }
		      }
		    else
		      {
			int statflag;
			if ((child = waitpid(child, &statflag, 0)) < 0)
			  {
			    perror("ERROR waiting on child in '2>' case");
			  }
		      }
		    
		  }//END "2>" direction case

		if(strcmp(tokens[argPtr], "&") == 0) //BEGIN "&" case
		  {
		    pid_t child;
		    if ((child = fork()) < 0)
		      {
			perror("FORK ERROR in '&' case");
		      }

		    else if (child == 0)
		      {
			int argsBefore = 0;
			int n = argPtr-1;

			while (tokens[n] != NULL && strcmp(tokens[n], "|") != 0 && strcmp(tokens[n], ">") != 0 && strcmp(tokens[n], "<") != 0 &&
			       strcmp(tokens[n], "1>") != 0 && strcmp(tokens[n], "2>") != 0 && strcmp(tokens[n], "&>") != 0 &&
			       strcmp(tokens[n], "&") != 0 && strcmp(tokens[n], ";") != 0)
			  {
			    argsBefore++;
			    n--;
			  }
			
			if (argsBefore == 1)
			  {
			    sleep(2);
			    if ((execlp(tokens[argPtr-1], tokens[argPtr-1], (char*)NULL)) < 0)
			      {
				perror("EXEC ERROR with single command in '2>' case");
			      }
			    
			    exit(1);
			  }
			
			else
			  {
			    char *argv[argsBefore+1];
			    for (n=0; n <= argsBefore; n++)
			      {
				argv[n] = tokens[argPtr - argsBefore];
				argsBefore--;
			      }
			    argv[n] = (char*) NULL;
			    sleep(2);
			    if((execvp(argv[0], argv)) < 0)
			      {
				perror("EXECVP ERROR IN < CASE");
			      }
			    exit(1);
			  }
			
		      }

		    else
		      {
			fprintf(stdout, "Executing process number [%d].\n", child);
			fflush(stdout);
			sleep(3);
		      }
		    
		    
		  }//END "&" case
	    }
            
	    




        }

        //End the case of anything but single arg.
    }

    //End the do Rest();
    else if (length > 1)
    {
        int statflag;
        if ((pid = waitpid(pid, &statflag, 0)) < 0)
        {
            perror("ERROR WAITING ON THE MAIN PROCESS\n");
        }
    }
}

/*


    THIS IS THE LINE DO NOT CROSS IT



*/

//Makes the filedescriptor array for the global variable
void pipeCreator(char *tokens[], int length)
{
    int numPipes = 0;
    int i = 0;
    for(i; i < length ; i++)
    {
        if(tokens[i][0] == '|')
        {
            numPipes++;
        }
    }
    for (i = 0; i < numPipes; i++)
    {
        if ((pipe(pipes[i])) < 0)
        {
            perror("PIPE CREATION ERROR");
        }
    }
    pipeflag = numPipes;
}






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
