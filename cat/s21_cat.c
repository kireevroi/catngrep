#include "s21_cat.h"

#include <stdlib.h>
#include <string.h>

#include "../common/s21_utils.h"

int main(int argc, char **argv) {
  params settings = parseInput(argc, argv);
  if (!settings.error) {
    parseFiles(&settings, argc, argv);
    if (!settings.file && !settings.error) catLoop(&settings);
  }
  return 0;
}
/*
  Loop if no files specified
*/
void catLoop(params *settings) {
  settings->file = stdin;
  while (settings->c != EOF) {
    printFiles(settings);
  }
}
/*
  Replace char in string at i position depending on flag
*/
void replaceChar(params *settings) {
  if (settings->c == '\n' && settings->mark_enter) {
    putchar('$');
    putchar('\n');
  } else if (settings->c == '\t' && settings->replace_tabs) {
    putchar('^');
    putchar('I');
  } else {
    if (settings->replace_hidden && settings->c != '\n' &&
        settings->c != '\t') {
      if (settings->c >= 32 && settings->c <= 126) {
        putchar(settings->c);
      } else if ((settings->c < 32 && settings->c >= 0) || settings->c == 127) {
        putchar('^');
        putchar(settings->c == 127 ? '?' : settings->c + 64);
      } else if (settings->c > 127) {
        putchar('M');
        putchar('-');
        if (settings->c >= 128 + 32) {
          if (settings->c < 128 + 127) {
            putchar(settings->c - 128);
          } else {
            putchar('^');
            putchar('?');
          }
        } else {
          putchar('^');
          putchar(settings->c - 128 + 64);
        }
      }
    } else {
      putchar(settings->c);
    }
  }
}
/*
  Numbering the lines
*/
void numberLines(params *settings) {
  if (settings->num_empty_lines && settings->c != 10) {
    printf("%6u\t", settings->row);
    (settings->row)++;
  } else if (settings->num_all_lines && !settings->num_empty_lines &&
             settings->end < 2) {
    printf("%6u\t", settings->row);
    (settings->row)++;
  }
}
/*
  Print all lines of files
*/
int printFiles(params *settings) {
  while (1) {
    int old_c = settings->c;
    settings->c = fgetc(settings->file);
    if (settings->c == EOF) break;
    if (old_c == 10 && settings->c == 10 && settings->squeeze_lines) {
      (settings->end)++;
    } else {
      settings->end = 0;
    }
    if (old_c == 10) numberLines(settings);
    if (settings->end < 2) replaceChar(settings);
  }
  return settings->c;
}
/*
  Check short flags and set params
*/
int parseShortFlags(char *str, params *settings) {
  int error = NoError;
  str++;
  while (*str != '\0') {
    if (*str == 'b') {
      settings->num_empty_lines = 1;
    } else if (*str == 'e') {
      settings->mark_enter = 1;
      settings->replace_hidden = 1;
    } else if (*str == 'E') {
      settings->mark_enter = 1;
    } else if (*str == 'v') {
      settings->replace_hidden = 1;
    } else if (*str == 'n') {
      settings->num_all_lines = 1;
    } else if (*str == 's') {
      settings->squeeze_lines = 1;
    } else if (*str == 't') {
      settings->replace_tabs = 1;
      settings->replace_hidden = 1;
    } else if (*str == 'T') {
      settings->replace_tabs = 1;
    } else {
      error = FlagDoesntExist;
    }
    str++;
  }
  return error;
}
/*
  Check long flags and set params
*/
int parseLongFlags(char *str, params *settings) {
  int error = NoError;
  str += 2;
  if (strcmp(str, "number-nonblank") == 0) {
    settings->num_empty_lines = 1;
  } else if (strcmp(str, "number") == 0) {
    settings->num_all_lines = 1;
  } else if (strcmp(str, "squeeze-blank") == 0) {
    settings->squeeze_lines = 1;
  } else {
    error = FlagDoesntExist;
  }
  return error;
}
/*
  Set initial structure state and allocate one sizeof(FILE*) to params
*/
void setNull(params *settings) {
  settings->file = NULL;
  settings->num_all_lines = 0;
  settings->num_empty_lines = 0;
  settings->squeeze_lines = 0;
  settings->replace_hidden = 0;
  settings->replace_tabs = 0;
  settings->mark_enter = 0;
  settings->error = 0;
  settings->row = 1;
  settings->end = 0;
  settings->c = 10;
}
/*
  Allocate params and parse
*/
params parseInput(int count, char **var) {
  params result;
  int error = NoError;
  setNull(&result);
  for (int i = 1; i < count; i++) {
    if (checkArgs(var[i]) == 1) {
      error = parseShortFlags(var[i], &result);
    } else if (checkArgs(var[i]) == 2) {
      error = parseLongFlags(var[i], &result);
    }
    if (error) {
      errorMessage(error);
      result.error = 1;
      break;
    }
  }
  return result;
}
/*
  Find files in argv and apply flags
*/
void parseFiles(params *result, int count, char **var) {
  if (!result->error) {
    for (int i = 1; i < count; i++) {
      if (checkArgs(var[i]) == 0) {
        FILE *file = fopen(var[i], "r");
        if (file) {
          result->file = file;
          printFiles(result);
        } else {
          errorMessage(ValueDoesntExist);
          result->error = 1;
          break;
        }
        if (file) fclose(file);
      }
    }
  }
}