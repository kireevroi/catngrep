#include <stdio.h>
#ifndef S21_CAT_H_
#define S21_CAT_H_

/*
        Structure that holds file descriptors and flag values
*/

typedef struct params {
  FILE *file;                    // file descriptor
  unsigned int num_empty_lines;  // -b
  unsigned int num_all_lines;    // -n
  unsigned int squeeze_lines;    // -s
  unsigned int replace_tabs;     // -t
  unsigned int mark_enter;       // -e
  unsigned int replace_hidden;   // -v
  unsigned int error;            // error
  unsigned int row;              // row number
  unsigned int end;              // end of squeeze
  int c;                         // current char
} params;
/*
        If no files were submitted loop indefinately
*/
void catLoop(params *settings);
/*
        Check long flags and set params
*/
int parseLongFlags(char *str, params *settings);
/*
        Check short flags and set params
*/
int parseShortFlags(char *str, params *settings);
/*
        Set initial structure state and allocate one sizeof(FILE*) to params
*/
void setNull(params *settings);
/*
        Allocate params and parse
*/
params parseInput(int count, char **var);
/*
        Add open file descriptor to array
*/
void numberLines(params *settings);
/*
  Find files in argv and apply flags
*/
void parseFiles(params *result, int count, char **var);
/*
        Print all lines of files
*/
int printFiles(params *settings);
/*
        Replace char in string at i position depending on flag
*/
void replaceChar(params *settings);
#endif  // S21_CAT_H_