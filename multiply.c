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


int main(int argc, char *argv[]){

  int sum = 0;
  // Will read argv[1] and argv[2] as Matrix A and B
  char * matA; 
  matA = argv[1];
  char * matB; 
  matB = argv[2];
  // Something weird is going on not sure why I cannot grab argv 1/2


  //str tok both
  char * Atok, Btok;
  Atok = strtok(matA, " ");
  Btok = strtok(matB, " ");

  while( (Atok!=NULL) && (Btok !=NULL) ){
    fprintf(stdout, "%s\t%s\t%d\n", Atok, Btok, sum);
    sum += atoi(Atok) * atoi(Btok);
    Atok = strtok(matA, " ");
    Btok = strtok(matB, " ");
    fprintf(stdout, "%d\n", sum);
    }

  // Covert Mat A/B to int arrays then take the dot product
  fprintf(stdout, "%d\n", sum);

  // Write the value found to the shared memory for matmult_p.c

  return 0;

}
