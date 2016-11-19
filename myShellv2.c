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

#define RED "\x1B[31m" // Colors
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"


/* FUNCTION DEFINITIONS */
void pipeCreator(char *tokens[]);
void pipeHandler(char *tokens[], int length);
void redirStdin(char *tokens[], int argPtr);
void redirStdout(char *tokens[], int argPtr);
void redirStderr(char *tokens[], int argPtr);
void redirBoth(char * tokens[], int argPtr);
void cdHandler(char * tokens[]);
void sig_handler(int signo);
void sig_handler2(int signo);
void clear();


/* GLOBAL VARIABLES */
int pipes[MAXPIPES][2];
int semiTotal = 0;
int semiCounter = 0;
int pipeTotal = 0;
int pipeCounter = 0;
pid_t pid;


/* MAIN ROUTINE */
int main(void)
{
  fprintf(stdout, "Welcome to the MyShell Processd\n");
  fprintf(stdout, "Enter exit ot quit!\n");


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

  
  /* BEGIN COMMAND LINE PARSING */
  char* line = "";

  while (strcmp(buffer, "exit") != 0)
    {
      line = (char*) NULL;
      fprintf(stdout, "%sm%sy%ss%sh%se%sl%sl%s %s> ", RED, GRN, YEL, BLU, MAG, CYN, RED, GRN, YEL);
      buffer = readline("");

      //Exit case:
      if (strcmp(buffer, "exit") == 0)
	{
	  printf(stdout, "Thanks for using our Shell! Its better than the other ones...");
	  break;
	}
      
      //Tokenize the input string
      char *string = strtok(line, " ");
      char* tokens[20];
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
	  if(strcmp(tokens[j], "|") == 0)
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
	  createPipes(tokens, i);
	}


      //Begin character checking routine, calling corresponding functions to handle each control character
      for (j = 0; j < length; j++)
	{
	  //Do only fork we need
	  if ((pid = fork()) < 0)
	    {
	      perror("Error Forking:");
	    }


	  // Inside the child
	  else if(pid == 0)
	    {
	      
	    }//End child
	  
	}//End main for loop
      
    }//End main while loop
  
  
  
}//END MAIN


/* clear() -- clears the screen. */
void clear()
{
  printf("\033[H\033[J");
}



/* sig_handler() -- signal handler for SIGINT */
void sig_handler(int signo)
{
  if (signo == SIGINT && pid == 0)
    {
      fprintf(stdout, "Received a SIGINT signal... exiting.");
      exit(0);
    }
}


/* sig_handler2() -- signal handler for SIGTSTP */
void sig_handler2(int signo)
{
  if (signo == SIGTSTP)
    {
      fprintf(stdout, "Received a SIGTSTP signal... exiting.");
      exit(0);
    }
}




/* pipeCreator() -- creates pipes and stores them in global array pipe */
void pipeCreator(char *tokens[])
{
  int i;
  for (i = 0;  i < totalPipes; i++)
    {
      if ((pipe(pipes[i])) < 0)
	{
	  perror("Error creating pipes:");
	}
    }
}
