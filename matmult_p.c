#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


// when testing use $cat matrices | ./matmult_p

int main(){
  int i;
  char stringtok[50]; 
  //First read in the two matricies
  fprintf(stdout,"User Input:\n\n");
  char string[100];
  while(fgets(string, 100, stdin)){
    if(strcmp(string, "")==0){
      fprintf(stdout,"You gave no Standard Input, try again.\n");
      exit(1);
    }
    else{
      printf("%s\n", string);
      //from here should tokenize string based on \n characters 
      }
    }
    // Start process of forking m*p times and writing result to parent

    
  return 0;
}
