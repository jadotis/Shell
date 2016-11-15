#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ipc.h>
#include <sys/shm.h>



#define MAXLINE 200
#define MAX_TOKS 20
#define MAX_ROWS 20
#define MAX_COLS 20
#define MAX_CHILDREN 20
int firstArrayKey;
int secondArrayKey;

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
  //We have now read all the info for the matrices.
  //Error handling for the matrices.
  if(MatAcol != MatBrow)
  {
    perror("Invalid Matrices: Please Enter proper Dimensions\n");
    exit(-1);
  }

  int sharedMemory = shmget(IPC_PRIVATE, MatArow*MatBrow*4, IPC_CREAT | S_IRUSR | S_IWUSR);
  int *result = (int*)shmat(sharedMemory, NULL, 0); // points to the place where the processes will write their results
  //Initialize shared memory.
  int memkeys[3];
  memkeys[0] = firstArrayKey;
  memkeys[1] = secondArrayKey;
  memkeys[2] = sharedMemory;

  int pids[MAX_CHILDREN];
  pid_t pid;
  int rowSpace;
  int colSpace;
  int children;
  int NumbersProcessed;
  for(rowSpace = 0; rowSpace < MatArow; rowSpace++)
  {
    for(colSpace = 0; colSpace < MatBcol; colSpace++)
    {
      if((pid = fork()) < 0) //We make the child
      {
        fprintf(stdout, "Our children have problems\n");
        perror(NULL);
      }
      if(pid == 0)
      {
        pids[children++] = getpid(); //store the process ID of the child to be waited on.
        execlp("./multiply",MatrixA[rowSpace], MatrixB[colSpace, result*NumbersProcess++])
        //We pass in the row of A, Col of B and the return addess of the shared Memory object.
      }

    }
  }






}
