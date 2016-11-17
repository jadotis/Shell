#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(int argc, char *argv[])
{
  long sum = 0;
  // Will read argv[1] and argv[2] as Matrix A and B
  char * rowA = argv[1];
  char * colB = argv[2];

  int length = (int)atoi(argv[3]);

  int * shMem = (int*)argv[4];

  fprintf(stdout, "shMem: %s", argv[4]);
  fflush(stdout);

  int aToks[3];
  int bToks[3];
  
  //fprintf (stdout, "argv[1]: %s\n", argv[1]);
  //fprintf(stdout, "%s\n", colB);
  // Argv[3] will be length of co
  //fprintf(stdout, "argv[1]: %s\nargv[2]: %s\n", argv[1], argv[2]);
  //fflush(stdout);
  char * aString = "";
  char * bString = "";
  //str tok both

	      
  aString = strtok(rowA, " ");
  aToks[0] = (int)atoi(aString);

  int i;
  for (i = 1; i < length; i++)
    {
      aString = strtok(NULL, " ");
      aToks[i] = (int)atoi(aString);
    }
  
  bString = strtok(colB, " ");
  bToks[0] = (int)atoi(bString);

  for (i = 1; i < length; i++)
    {
      bString = strtok(NULL, " ");
      bToks[i] = (int)atoi(bString);
    }

  
 
  for (i = 0; i < length; i++)
    {
      sum += aToks[i] * bToks[i];

      fprintf(stdout, "Sum: %d\n", sum);
      fflush(stdout);
    }

  *shMem = sum;  
  
  return 0;
  
}
