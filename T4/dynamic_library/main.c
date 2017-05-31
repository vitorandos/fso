#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "seno.h"

#define SINE 1
#define ARCSINE -1
#define INVALID 2

int main(int argc, char const *argv[]) {
  int option = (argv[1][1] == 's') ? SINE : (argv[1][1] == 'a') ? ARCSINE : INVALID;
  if (option != INVALID) {
    double angle = atof(argv[2]);
    double result = 0.0;
    if (option == SINE){
      result = sine(angle);
      printf("seno(%lf) = ", angle);
    }
    else {
      result = arcsine(angle);
      printf("arc_seno(%lf) = ", angle);
    }
    printf("%.8lf\n", result);
  } else {
    // do nothing, invalid option
  }

  return 0;
}

// sempre tem que apagar o .a e .o pra realmente mudar a biblioteca, pq?
