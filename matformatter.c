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

int main(int argc, char * argv[])
{
  fflush(stdout);
  int matrixA[MAX_ROWS][MAX_COLS];
  memset(matrixA, 0, 4*MAX_COLS*MAX_ROWS);
  int cols = 0;
  int rows = 0;
  int MatArow;
  int MatAcol;
  char * readInfo = malloc(1000);
  char * string = malloc(100);
  int fd = dup(STDOUT_FILENO);
  close(STDOUT_FILENO);
  readInfo = readline("");
  int tempcols = 0;
  while(readInfo != NULL)
  {
    string = strtok(readInfo, " ");
    //fprintf(stdout,"After string tok %d value instered is:%d\n", rows*cols, matrixA[rows][cols]);
    matrixA[rows][cols] = atoi(string);
    string = strtok(NULL, " ");
    while(string != NULL)
    {
      cols++;
      matrixA[rows][cols] = atoi(string);
      string = strtok(NULL, " ");
    }
    rows++;
    tempcols = cols;
    cols = 0;
    readInfo = readline("");
  }

  //fprintf(stdout, "We get here with rows: %d, cols: %d\n", rows, tempcols);
  fflush(stdout);
  MatArow = rows;
  MatAcol = tempcols + 1;
  //Step 2: Determine dimmensions of read array
  //Step 3: Create new array with flipped dimensions ie 2 X 3 -> 3 X 2
  int matrixTrans[MatAcol][MatArow];
  memset(matrixTrans, 0, 4*MatAcol*MatArow);



  //Step 4: Loop through rows / columns of original array [i][j] and fill new array with at positoin [j][i]
  int i, j;
  for(i = 0; i < MatArow; i++){
    for(j = 0; j < MatAcol; j++){
      //fprintf(stdout,"Values inserted: %d\n", matrixA[i][j]);
      matrixTrans[j][i] = matrixA[i][j];
    }
  }
  dup2(fd, STDOUT_FILENO);
  //Step 5: write new transposed matrix to stdout
  fprintf(stdout,"Matrix:\n");
  for(i = 0; i < MatAcol; i++){
    for(j =0; j < MatArow; j++){
      fprintf(stdout, "%d ", matrixTrans[i][j]);
    }
    fprintf(stdout, "\n");
  }
  fprintf(stdout, "\n");
  return 0;
}
