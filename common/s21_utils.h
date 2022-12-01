#include <stdio.h>
#include <string.h>
#ifndef S21_UTILS_H_
#define S21_UTILS_H_

/*
        Error Handling Messages
*/

enum errors {
  NoError,
  Allocation,
  FlagDoesntExist,
  ValueDoesntExist,
  WrongExpression,
  Usage
};

/*
        Throw Error Message to stderr
*/

void errorMessage(int error);

/*
  Return 2 if longflag
  Return 1 if shortflag
  Return 0 if value
*/

int checkArgs(char *arg);

#endif  // S21_UTILS_H_