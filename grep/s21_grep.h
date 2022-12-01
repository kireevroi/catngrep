#include <regex.h>
#include <stdio.h>
#ifndef S21_GREP_H_
#define S21_GREP_H_

/*
        Structure that holds file descriptors and flag values
*/

typedef struct params {
  char **file_name;
  int file_count;  // file descriptor
  char **strPattern;
  int pattern;  // -e
  char **pattern_files;
  int files;         // -f
  int ignore;        // -i
  int invert;        // -v
  int count_lines;   // -c
  int match_files;   // -l
  int number_lines;  // -n
  int file_names;    // -h
  int suppress;      // -s
  int output;        // -o
  int error;         // error
  char *compiled;
} params;
typedef struct regstruct {
  regex_t regex;
  regmatch_t *pmatch;
} regstruct;
/*
        Set initial structure state and allocate one sizeof(FILE*) to params
*/
void setNull(params *settings);

void printstruct(params settings);

int parseStrFlag(params *settings, char *str, int *flag);
void shiftFiles(params *settings);
int noFlag(params *settings);
params parse(int argc, char **argv);

int pFileNameMatch_Files(params *settings, int i);
void pCountLines(params *settings, int count, int i);
void regOutput(params *settings, char *line, regstruct reg, int i,
               int line_num);
void pLineNum(params *settings, int line_num);
void pFileName(params *settings, int i);
void regFile(regex_t regex, params *settings, FILE *file, int i);
void cleanUp(params *settings);
int addString(char ***pointer, char *str, int size);
void freePointer(char ***pointer, int size);
int compileRegExp(params *settings);
void sortPatterns(params *settings);
void removeDuplicates(params *settings);
int appendFilePatternToString(params *settings);
void grepFile(params *settings);
void freeEverything(params *settings);
#endif  // S21_GREP_H_