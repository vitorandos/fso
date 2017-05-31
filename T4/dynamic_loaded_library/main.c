#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "seno.h"

#define SINE 1
#define ARCSINE -1
#define INVALID 2

int main(int argc, char const *argv[]) {
  void *library_handle;
  char *error;
  double (*sine)(double);
  double (*arcsine)(double);

  library_handle = dlopen("./libseno.so", RTLD_LAZY);
  if (!library_handle){
    fprintf(stderr, "%s\n", dlerror());
    exit(1);
  } else {
    // do nothing
  }
  int option = (argv[1][1] == 's') ? SINE : (argv[1][1] == 'a') ? ARCSINE : INVALID;
  if (option != INVALID) {
    double angle = atof(argv[2]);
    double result = 0.0;
    if (option == SINE){
      sine = dlsym(library_handle, "sine");
      if ((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
      } else {
        // do nothing
      }

      result = (*sine)(angle);
      printf("seno(%lf) = ", angle);
    }
    else {
      arcsine = dlsym(library_handle, "arcsine");
      if ((error = dlerror()) != NULL){
        fprintf(stderr, "%s\n", error);
        exit(1);
      } else {
        // do nothing
      }

      result = (*arcsine)(angle);
      printf("arc_seno(%lf) = ", angle);
    }
    printf("%.8lf\n", result);
  } else {
    // do nothing, invalid option
  }

  dlclose(library_handle);
  return 0;
}

// sempre tem que apagar o .a e .o pra realmente mudar a biblioteca, pq?
