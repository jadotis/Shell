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
#define MAX_CHILDREN 20

int firstArrayKey;
int secondArrayKey;
typedef struct {
  int numrows;
  int numcolumns;
  int rowvals[MAX_ROWS];
  int colvals[MAX_COLS];

}matrix;

//helper functions below main function
int* getCol(int matrix[MAX_ROWS][MAX_COLS], intpos, int length);
int* getRow(int matrix[MAX_ROWS][MAX_COLS], int pos, int length);
void printArray(int * arr, int len);
int multiply(int * rowA, int * colB, int len)

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

  int sharedMemory = shmget(IPC_PRIVATE, MatArow*MatBcol*4, IPC_CREAT | S_IRUSR | S_IWUSR);
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
	int * tempCol = getCol(matrixB, colSpace, MatBrow);
	int * tempRow = getRow(matrixA, rowSpace, MatAcol);
	fprintf(stdout,"Matrix B column: %d", colSpace);
	printArray(tempCol, MatBrow);
	fprintf(stdout,"Matrix A row: %d", rowSpace);
	printArray(tempRow, MatAcol);
	fprintf(stdout, "First two digits in col: %d,%d\n", tempCol[0], tempCol[1]);
	pids[children++] = getpid(); //store the process ID of the child to be waited on.
        //execlp("./multiply",matrixA[rowSpace], tempCol, result+(NumbersProcessed++));
        //We pass in the row of A, Col of B and the return addess of the shared Memory object.
      }
      else
      {
        continue;
      }
      }

    }
  int statflag;
  int i = 0;
  for(i; i < children; i++)
  {
    if((pids[i] = waitpid(pids[i], &statflag, 0)) < 0)
    {
      perror(NULL);
      fprintf(stderr, "Error waiting on the child: %d\n", pids[i]);
    }
    else
    {

    }
  }
//Handle shared Memory;
//Optional pass to matformatter/
//shmaddr

}


int* getCol(int matrix[MAX_ROWS][MAX_COLS], int pos, int length){
  int tempArray[length];
  int i;
  for(i = 0; i < length; i++){
    tempArray[i] = matrix[i][pos];
    //fprintf(stdout, "%d\n", tempArray[i]); 
  }
  return tempArray;

}

int* getRow(int matrix[MAX_ROWS][MAX_COLS], int pos, int length){
  int tempArray[length];
  int i; 
  for(i = 0; i < length; i++){
    tempArray[i] = matrix[pos][i];
  }
  return tempArray;
    

}


void printArray(int * arr, int len){
  int i;
  for (i=0;i < len;i++) {
    printf("%d",arr[i]);
  }
  printf("\n");
}


// This is what multiply should do after converting the agument types

int multiply(int * rowA, int * colB, int len){
  int i;
  int total = 0; 
  for(i = 0; i < len; i++){
    total += rowA[i]*colB[i];
  }
  return total;
}
