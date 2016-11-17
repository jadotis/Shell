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



#define MAXLINE 200
#define MAX_TOKS 20
#define MAX_ROWS 20
#define MAX_COLS 20
#define MAX_CHILDREN 200
char *grabRow(int *array[], int index, int size);
char *grabCol(int *array[], int index, int size);
void reverse(char s[]);
void itoa(int n, char s[]);




int firstArrayKey;
int secondArrayKey;
int numChildren;
pid_t pids[MAX_CHILDREN];
//helper functions below main function
/* Global variables */

int main(int argc, char *argv[])
{
  int matrixA[MAX_ROWS][MAX_COLS];
  int matrixB[MAX_ROWS][MAX_COLS];
  memset(matrixA, 0, 4*MAX_COLS*MAX_ROWS);
  memset(matrixB, 0, 4*MAX_COLS*MAX_ROWS);


  int cols = 0;
  int rows = 0;
  int MatArow;
  int MatAcol;
  int MatBrow;
  int MatBcol;

  char * readInfo;
  char * string;
  readInfo = readline("");
  while(readInfo != NULL)
  {
    string = strtok(readInfo," ");
    matrixA[rows][cols] = atoi(string);
    while(string != NULL)
    {
      string = strtok(NULL, " ");
      if(string == NULL)
      {
        break;
      }
      cols++;
      matrixA[rows][cols] = atoi(string);
    }
    readInfo = readline("");
    if(readInfo[0] == '\0')
    {

      break;
    }
    rows++;
    MatAcol = cols+1;
    cols = 0;
  }
  MatArow = rows+1;
  rows = 0;
  readInfo = readline("");
  cols = 0;
  while(readInfo != NULL)
    {
    string = strtok(readInfo," ");
    matrixB[rows][cols] = atoi(string);
    while(string != NULL)
      {
      string = strtok(NULL, " ");
      if(string == NULL)
	{
        break;
      }
      cols++;
      matrixB[rows][cols] = atoi(string);
    }
    readInfo = readline("");
    if(readInfo == NULL)
      {
	break;
      }
    rows++;
    MatBcol = cols+1;
    cols = 0;
  }
  MatBrow = rows+1;
  printf("The matrices at dimensions: %d X %d and %d X %d\n", MatArow, MatAcol,MatBrow,MatBcol);
  //creat matrix C
  printf("The answer to this multiplication will have dimensions: %d X %d\n", MatArow, MatBcol);
  
  
  //We have now read all the info for the matrices.
  //Error handling for the matrices.
  if(MatAcol != MatBrow)
  {
    perror("Invalid Matrices: Please Enter proper Dimensions\n");
    exit(-1);
  }
  
  int sharedMemory = shmget(IPC_PRIVATE, MatArow*MatBcol*4, IPC_CREAT | S_IRUSR | S_IWUSR);
  char *result = (char*)shmat(sharedMemory, NULL, 0); // points to the place where the processes will write their results
  //Initialize shared memory.
  int memkeys[3];
  memkeys[0] = firstArrayKey;
  memkeys[1] = secondArrayKey;
  memkeys[2] = sharedMemory;
  numChildren = MatArow * MatBcol;
  int i = 0;
  char * arg1;
  char * size;
  itoa(MatArow, &size);

  for(i; i < numChildren; i++)
  {
    if((pids[i] = fork()) < 0)
    {
      perror(NULL);
      fprintf(stdout,"Child %d failed to fork\n", getpid());
    }
    if(pids[i] == 0)
    { 
      char *argv[4];
      argv[0] = "./multiply";
      argv[1] = grabRow(matrixA, i, MatArow);
      argv[2] = grabCol(matrixB, i, MatArow);
      argv[3] = size;
      argv[4] = result;

      fprintf(stdout, "int i is: %d\n", i);
      fflush(stdout);
      execvp(argv[0], argv);
      
      
    }
    else if(pids[i] > 0) //The parent
    {
      int j = 0;
      int dickflag;
      for(j; j < numChildren; j++)
      {
        if((pids[j] = waitpid(pids[j], &dickflag, 0)) < 0)
        {
          perror(NULL);
          fprintf(stderr, "There was an error with teh FUCKING children\n");
        }
        else
        {
         fprintf(stdout,"successfuly waited on the child pids[%d]: %d\n", j, pids[j]); 
        }
      }
    }
  }
  //End
  
}

/* 
************************************************************************************
*************************************************************************************
*/

void itoa(int n, char s[])
 {
   
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

  void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }


char *grabRow(int *array[], int index, int size)
{
    char * buffer;
    char * string;
    int i = 0;
    for(i; i < size; i++)
    {
      itoa(array[index][i], &buffer);
      strcat(string, buffer);
      strcat(string, " "); //This should be fine.
    }
    return string;
}

char *grabCol(int *array[], int index, int size)
{
  char * buffer;
  char * string;
  int i = 0;
  for(i; i < size; i++)
  {
    itoa(array[i][index], &buffer);
    strcat(string, buffer);
    strcat(string, " ");
  }
  return string;
}