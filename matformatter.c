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

void printMat(int r, int c, int matrix[r][c]);

int main(int argc, char *argv[])
{
  //Step 1: Read Matrix from STDIn (not sure exactly how he wants this done)
  int matrixA[MAX_ROWS][MAX_COLS];
  int cols = 0;
  int rows = 0;
  int MatArow;
  int MatAcol;

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

  //Step 2: Determine dimmensions of read array
  printf("The matrix dimensions: %d X %d\n", MatArow, MatAcol);

  //Step 3: Create new array with flipped dimensions ie 2 X 3 -> 3 X 2
  int matrixTrans[MatAcol][MatArow];

  //Step 4: Loop through rows / columns of original array [i][j] and fill new array with at positoin [j][i]
  int i, j;
  for(i = 0; i < MatArow; i++){
    for(j = 0; j < MatAcol; j++){
      matrixTrans[j][i] = matrixA[i][j];
    }
  }

  //Step 5: write new transposed matrix to stdout
  printMat(MatAcol, MatArow, matrixTrans[MatAcol][MatArow]);
  

  return 0;



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
