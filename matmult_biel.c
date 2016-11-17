#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>


/* MACRO DEFINITIONS */
#define MAXLINE 200
#define MAX_TOKS 20
#define MAX_ROWS 20
#define MAX_COLS 20
#define MAX_CHILDREN 20



/* GLOBAL VARIABLES */
int firstArrayKey;
int secondArrayKey;


/* FUNCTION DEFS */
void printArray(int * arr, int len);
void printMat(int r, int c, int matrixp[r][c]);
int multiply(int * rowA, int * colB, int len);
char * itoaArray(int * array, int len);


/* MAIN ROUTINE */
int main(int argc, char * argv[])
{
  int matrixA[MAX_ROWS][MAX_COLS];
  int matrixB[MAX_ROWS][MAX_COLS];

  /* Fill matrices with 0's */
  memset(matrixA, 0, 4*MAX_COLS*MAX_ROWS);
  memset(matrixB, 0, 4*MAX_COLS*MAX_ROWS);

  int cols = 0;
  int rows = 0;
  
}
