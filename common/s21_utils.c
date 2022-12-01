#include "s21_utils.h"

/*
        Throw Error Message to stderr
*/
void errorMessage(int error) {
  if (error == 0) {
    fprintf(stderr, "Log: No Errors\n");
  } else if (error == 1) {
    fprintf(stderr, "Error: Allocation Error\n");
  } else if (error == 2) {
    fprintf(stderr, "Error: Flag does not exist\n");
  } else if (error == 3) {
    fprintf(stderr, "Error: Path does not exist\n");
  } else if (error == 4) {
    fprintf(stderr, "Error: Wrong expression\n");
  } else if (error == 5) {
    fprintf(stderr, "Learn the usage before hand!!!\n");
  }
}

/*
  Return 2 if longflag
  Return 1 if shortflag
  Return 0 if value
  Return -1 if error
*/
int checkArgs(char *arg) {
  int result = 0;
  int len = strlen(arg);
  if (arg[0] == '-') {
    if (arg[1] != '-' && len > 1) {
      result = 1;
    } else if (arg[1] == '-' && len > 2) {
      result = 2;
    } else {
      result = -1;
    }
  }
  return result;
}