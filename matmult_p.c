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
void printArray(int * arr, int len);
void printMat(int r, int c, int matrix[r][c]);
int multiply(int * rowA, int * colB, int len);
char * intarraytostring(int *array, int len);
void reverse(char s[]);
char * my_itoa(int n, char * s);



/* Global variables */
int children;
int pids[MAX_CHILDREN];

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
  int matrixC[MatArow][MatBcol];
  printf("The answer to this multiplication will have dimensions: %d X %d\n", MatArow, MatBcol);
  
  
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

  pid_t pid;
  int rowSpace;
  int colSpace;
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

      if (pid > 0)
	{
	  pids[children++] = pid;
	}
      
      if(pid == 0)
	{
	  fflush(stdout);
	  int tempCol[MatBrow];
	  int a;
	  for(a = 0; a < MatBrow; a++)
	    {
	      tempCol[a] = matrixB[a][colSpace];
	    }
	  fprintf(stdout,"Matrix B column: %d: ", colSpace);
	  printArray(tempCol, MatBrow);
	  //char * stringArray = intarraytostring(tempCol, MatBrow);
	  
	  int tempRow[MatAcol];
	  int b; 
	  for(b = 0; b < MatAcol; b++)
	    {
	      tempRow[b] = matrixA[rowSpace][b];
	    }


	  
	  fprintf(stdout,"Matrix A row: %d: ", rowSpace);
	  printArray(tempRow, MatAcol);
	  
	  //all this shit needs to be done with exec passing tempCol and tempRow as arguments not sure how to do this tbh
	  int Cab = multiply(tempRow, tempCol, MatAcol);
	  fprintf(stdout, ". product of A row %d and B col %d = %d\n", rowSpace, colSpace, Cab);
	  // need to write this part to parent
	  matrixC[rowSpace][colSpace] = Cab;
	  //execlp("./multiply",matrixA[rowSpace], matrixB[colSpace], result+(NumbersProcessed++));
	  //We pass in the row of A, Col of B and the return addess of the shared Memory object.
	  exit(1);
	}
      else
	{
	  continue;
	}
    }
    
  }
  if (pid > 0)
    {
      fprintf(stdout, "Value of children: %d\n", children);
      fflush(stdout);
      int statflag;
      int i = 0;
      for(i; i < children; i++)
	{
	  if((pids[i] = waitpid(pids[i], &statflag, 0)) < 0)
	    {
	      fprintf(stdout, "ERRRORRRRRRR\n");
	      fflush(stdout);
	      perror(NULL);
	      fprintf(stderr, "Error waiting on the child: %d\n", pids[i]);
	    }
	  else
	    {
	      fprintf(stdout, "PID of child we just waited on: %d\n", pids[i]);
	      //fflush(stdout);
	    }
	}
    }
  //Handle shared Memory;
  //Optional pass to matformatter/
  //shmaddr
  
  printMat(MatArow, MatBcol, matrixC);
}

//Helper functions


void printArray(int * arr, int len){
  int i;
  for (i=0;i < len;i++) {
    printf("%d ",arr[i]);
  }
  printf("\n");
}

void printMat(int r, int c, int matrix[r][c]){
  int row, columns;
  printf("Matrix:\n");
  for (int row=0; row<r; row++)
    {
      for(int columns=0; columns<c; columns++)
	printf("%d     ", matrix[row][columns]);
      printf("\n");
    }
  printf("\n");
}

//this one is a work in progress
char * intarraytostring(int *array, int len){
  int i; 
  char * string[50]; 
  for(i = 0; i < len; i++){
    my_itoa(array[i], string[i]);
    fprintf(stdout, "String[%d], %s\n", i, string[i]);
    fflush(stdout);
  }
  
  return string;
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

char * my_itoa(int n, char * s)
{
  int i, sign;
  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do {
    s[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);

  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
  return s;
}


void reverse(char s[]) {
  int i, j;
  char c;

  for (i = 0, j = strlen(s)-1; i < j; i++, j--)
    {
      c = s[i];
      s[i] = s[j];
      s[j] = c;
    }
}
