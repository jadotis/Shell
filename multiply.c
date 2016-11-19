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
  close(1);

  long sum = 0;
  // Will read argv[1] and argv[2] as Matrix A and B
  char * rowA = argv[1];
  char * colB = argv[2];


  int length = (int)atoi(argv[3]);
  int memKey = (int)atoi(argv[4]); // look up
  int * writeLocation = (int*)shmat(memKey, NULL, 0);
  writeLocation += atoi(argv[5]);
   //atoi hex

  fflush(stdout);

  int aToks[length];
  int bToks[length];
  
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

  //aToks[4] is ALWAYS wrong
  //fprintf(stdout, "aToks[4]: %d\n", aToks[4]);

  bString = strtok(colB, " ");
  bToks[0] = (int)atoi(bString);

  for (i = 1; i < length; i++)
    {
      bString = strtok(NULL, " ");
      bToks[i] = (int)atoi(bString);
    }

  
 
  for (i = 0; i < length; i++)
    {
      fprintf(stdout, "aToks[%d]: %d\nbToks[%d]: %d\n\n", i, aToks[i], i, bToks[i]);
      sum += aToks[i] * bToks[i];
      fprintf(stdout, "Sum: %d\n", sum);
      fflush(stdout);
    }
  *writeLocation = sum;
  exit(1);
  
}

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */

//Provided by Rich West
void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
     
  /* If %d is specified and D is minus, put `-' in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;
     
  /* Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
     
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);
     
  /* Terminate BUF. */
  *p = 0;
     
  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}
