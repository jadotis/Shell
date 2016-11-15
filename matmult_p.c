#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define MAXLINE 200
#define MAX_TOKS 20
#define MAX_ROWS 20
#define MAX_COLS 20


int main(int argc, char *argv[])
{
  int matrixA[MAX_ROWS][MAX_COLS];
  int matrixB[MAX_ROWS][MAX_COLS];

  int j = 0;
  int i = 0;
  int temp;
  
  char* line = (char*)malloc(MAXLINE);
  char* string;

  
  
  while((fgets(line, MAXLINE, stdin)) && line[0] != '\n')
    {
      fprintf(stdout, "The line is: %s", line);
      string = strtok(line, " ");
      fprintf(stdout, "We make it here.\n");
      fflush(stdout);
      fprintf(stdout, "String: %s\n", string);
      fflush(stdout);
      
      matrixA[i][0] = atoi(string);

      while(string != NULL)
	{
	  //fprintf(stdout, "SECOND TEST\n");
	  string = strtok(NULL, " ");
	  //fprintf(stdout, "THIRD TEST\n");
	  //fflush(stdout);
	  matrixA[i][j++] = atoi(string);
	  fprintf(stdout, "%d ", matrixA[i][j]);
	  fflush(stdout);
	}
      fprintf(stdout, "\n");
      i++;
      j = 0;
    }

}

