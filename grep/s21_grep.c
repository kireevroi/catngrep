#include "s21_grep.h"

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/s21_utils.h"

int main(int argc, char **argv) {
  params settings = parse(argc, argv);
  if (!settings.error) {
    grepFile(&settings);
  }
  freeEverything(&settings);
  return 0;
}
void pFileName(params *settings, int i) {
  if (settings->file_count > 1 && !settings->file_names)
    printf("%s:", settings->file_name[i]);
}
void pLineNum(params *settings, int line_num) {
  if (settings->number_lines) printf("%d:", line_num);
}
void pCountLines(params *settings, int count, int i) {
  if (settings->count_lines && !settings->match_files) {
    pFileName(settings, i);
    printf("%d\n", count);
  }
}
int pFileNameMatch_Files(params *settings, int i) {
  int res = 0;
  if (settings->match_files) {
    printf("%s\n", settings->file_name[i]);
    res = 1;
  }
  return res;
}
void pSuppress(params *settings) {
  if (!settings->suppress) errorMessage(ValueDoesntExist);
}
void regOutput(params *settings, char *line, regstruct reg, int i,
               int line_num) {
  int enter = 0;
  if (settings->output) {
    while (regexec(&(reg.regex), line, 1, reg.pmatch, 0) == 0) {
      enter = 0;
      pFileName(settings, i);
      pLineNum(settings, line_num);
      for (int k = (reg.pmatch)[0].rm_so; k < (reg.pmatch)[0].rm_eo; k++)
        putchar(line[k]);
      if (line[(reg.pmatch)[0].rm_eo] != '\0') {
        putchar('\n');
        enter = 1;
      }
      line += (reg.pmatch)[0].rm_eo;
    }
  } else {
    pFileName(settings, i);
    pLineNum(settings, line_num);
    printf("%s", line);
  }
  if (line[strlen(line) - 1] != '\n' && !enter &&
      (!settings->invert || !settings->output))
    printf("\n");  // add trailing \n for last line
}
void regFile(regex_t regex, params *settings, FILE *file, int i) {
  char *line = NULL;
  size_t len = 0;
  int status = regcomp(&regex, settings->compiled,
                       settings->ignore ? REG_ICASE | REG_EXTENDED | REG_NEWLINE
                                        : REG_EXTENDED | REG_NEWLINE);
  if (!status) {  // If regular expression is compiled
    regmatch_t *pmatch = malloc(sizeof(regmatch_t) * 2);
    regstruct reg = {.pmatch = pmatch, .regex = regex};
    ssize_t exit_code;
    int count = 0, line_num = 0;
    while ((exit_code = getline(&line, &len, file)) != -1) {
      line_num++;
      status = regexec(&regex, line, 1, pmatch, 0);  // find match
      if (!status && !settings->invert) {            // if = 0 e.g found
        if (pFileNameMatch_Files(settings, i)) break;
        if (!settings->count_lines) regOutput(settings, line, reg, i, line_num);
        count++;
      } else if (settings->invert && status == REG_NOMATCH) {  // if not found
        if (pFileNameMatch_Files(settings, i)) break;
        if (!settings->count_lines) regOutput(settings, line, reg, i, line_num);
        count++;
      }
    }
    pCountLines(settings, count, i);
    if (pmatch) free(pmatch);
    if (line) free(line);
  } else {
    errorMessage(WrongExpression);
  }
  regfree(&regex);
}

void grepFile(params *settings) {
  regex_t regex = {.re_nsub = 0};
  for (int i = 0; i < settings->file_count; i++) {
    FILE *file = fopen(settings->file_name[i], "r");
    if (file) {
      regFile(regex, settings, file, i);
      fclose(file);
    } else {
      pSuppress(settings);
    }
  }
  if (!settings->file_count) errorMessage(Usage);
}

/*
  PARSING
*/

void setNull(params *settings) {
  settings->file_name = NULL;
  settings->file_count = 0;
  settings->strPattern = NULL;
  settings->pattern = 0;
  settings->pattern_files = NULL;
  settings->compiled = NULL;
  settings->files = 0;
  settings->ignore = 0;
  settings->invert = 0;
  settings->count_lines = 0;
  settings->match_files = 0;
  settings->number_lines = 0;
  settings->file_names = 0;
  settings->suppress = 0;
  settings->output = 0;
  settings->error = 0;
}

int parseShortFlags(char *str, params *settings, int *flag) {
  int error = NoError;
  str++;  // shift past -
  while (*str != '\0') {
    if (*str == 'e') {
      (settings->pattern)++;
      *flag = 1;
      break;
    } else if (*str == 'i') {
      settings->ignore = 1;
    } else if (*str == 'v') {
      settings->invert = 1;
    } else if (*str == 'c') {
      settings->count_lines = 1;
    } else if (*str == 'l') {
      settings->match_files = 1;
    } else if (*str == 'n') {
      settings->number_lines = 1;
    } else if (*str == 'h') {
      settings->file_names = 1;
    } else if (*str == 's') {
      settings->suppress = 1;
    } else if (*str == 'f') {
      settings->files++;
      *flag = 1;
      break;
    } else if (*str == 'o') {
      settings->output = 1;
    } else {
      error = FlagDoesntExist;
    }
    str++;
  }
  return error;
}

int parseStrFlag(params *settings, char *str, int *flag) {
  int error = 0;
  *flag = 0;
  while (*str != 'e' && *str != 'f') str++;  // move to e or f
  if (*str == 'f') {  // if f add element to pattern-files
    str++;
    if (*str != '\0') {
      addString(&(settings->pattern_files), str, settings->files);
    } else {
      *flag = 2;
    }
  } else if (*str == 'e') {  // if e add element to pattern-str
    str++;
    if (*str != '\0') {
      addString(&(settings->strPattern), str, settings->pattern);
    } else {
      *flag = 1;
    }
  }
  return error;
}

params parse(int argc, char **argv) {
  params settings;     // initialize settings
  setNull(&settings);  // set them to null
  int strFlag = 0, error = 0;
  for (int i = 1; i < argc; i++) {
    if (strFlag) {  // If -e or -f flags were used and there is something after
                    // them
      if (strFlag == 1)
        error = addString(&(settings.strPattern), argv[i], settings.pattern);
      if (strFlag == 2)
        error = addString(&(settings.pattern_files), argv[i], settings.files);
      strFlag = 0;
    } else {                          // Else normal parse
      if (checkArgs(argv[i]) == 1) {  // If flag
        error = parseShortFlags(argv[i], &settings, &strFlag);  // parse em
        if (strFlag && !error)
          error = parseStrFlag(
              &settings, argv[i],
              &strFlag);  // if -e or -f, we need to know where they are
      } else if (checkArgs(argv[i]) == 0) {
        (settings.file_count)++;
        error = addString(&(settings.file_name), argv[i],
                          settings.file_count);  // Just read normal file
      }
      if (error) break;
    }
  }
  if (!settings.file_name) {
    error = Usage;
    errorMessage(error);
  }
  settings.error = error;
  if (!error) cleanUp(&settings);
  return settings;
}

void cleanUp(params *settings) {
  settings->error = appendFilePatternToString(settings);
  if (!settings->error) settings->error = noFlag(settings);
  if (!settings->error) {
    removeDuplicates(settings);
    sortPatterns(settings);
    settings->error = compileRegExp(settings);
  }
}
int noFlag(params *settings) {
  int error = 0;
  if (!settings->strPattern && !settings->pattern_files &&
      settings->file_name) {  // if there are no patterns, but there is a file
                              // to read
    (settings->pattern)++;
    error = addString(&(settings->strPattern), settings->file_name[0],
                      settings->pattern);
    if (!error) shiftFiles(settings);
  }
  return error;
}
void shiftFiles(params *settings) {
  free(settings->file_name[0]);
  for (int i = 1; i < settings->file_count; i++) {
    settings->file_name[i - 1] = settings->file_name[i];
  }
  (settings->file_count)--;
}

void sortPatterns(params *settings) {
  for (int i = 0; i < settings->pattern; i++) {
    for (int j = 0; j < settings->pattern - i - 1; j++) {
      if (strcmp(settings->strPattern[j], settings->strPattern[j + 1]) > 0) {
        char *tmp = settings->strPattern[j];
        settings->strPattern[j] = settings->strPattern[j + 1];
        settings->strPattern[j + 1] = tmp;
      }
    }
  }
}
void removeDuplicates(params *settings) {
  // Set duplicates as empty strings;
  for (int i = 0; i < settings->pattern - 1; i++) {
    for (int j = i + 1; j < settings->pattern; j++) {
      if (strcmp(settings->strPattern[i], settings->strPattern[j]) == 0) {
        *(settings->strPattern[j]) = '\0';
      }
    }
  }
  // Remove duplicates and shift
  for (int i = 0; i < settings->pattern; i++) {
    if (*(settings->strPattern[i]) == '\0') {
      free(settings->strPattern[i]);
      for (int j = i; j < settings->pattern - 1; j++) {
        settings->strPattern[j] = settings->strPattern[j + 1];
      }
      (settings->pattern)--;
    }
  }
}
int compileRegExp(params *settings) {
  int error = 0;
  int size = 1;
  for (int i = 0; i < settings->pattern; i++) {
    size += strlen(settings->strPattern[i]) + 4;
  }
  settings->compiled =
      malloc(sizeof(char) * (size)*3);  // 3 for meta chars just in case
  if (settings->compiled) {
    settings->compiled[0] = '\0';
    for (int i = 0; i < settings->pattern; i++) {
      strcat(settings->compiled, settings->strPattern[i]);
      if (i < settings->pattern - 1) strcat(settings->compiled, "|");
    }
  } else {
    error = 1;
  }
  return error;
}
int appendFilePatternToString(params *settings) {  // returns 1 if error
  int error = 0;
  if (settings->files) {  // If files were even initialized
    for (int i = 0; i < settings->files; i++) {  // Go through all files
      FILE *file = fopen(settings->pattern_files[i], "r");  // try to open them
      if (file) {                                           // if file exists
        char *line = NULL;
        size_t len = 0;
        ssize_t exit_code;
        while ((exit_code = getline(&line, &len, file)) !=
               -1) {  // read every line
          if (line[strlen(line) - 1] == '\n' && line[0] != '\n')
            line[strlen(line) - 1] = '\0';  // replace \n with \0
          settings->pattern++;              // invrements pattern array
          error = addString(&(settings->strPattern), line,
                            settings->pattern);  // add the pattern to array
          if (error) break;
        }
        if (error) break;
        if (line) free(line);
      } else {  // Throw error and break
        error = ValueDoesntExist;
        errorMessage(error);
        break;
      }
      if (file) fclose(file);  // Close file
    }
  }
  return error;
}

int addString(char ***pointer, char *str, int size) {
  int error = 0;
  char ***tmp = pointer;
  *pointer = realloc(*pointer, sizeof(char *) * size);
  if (*pointer) {
    (*pointer)[size - 1] = malloc(sizeof(char) * (strlen(str) + 1));
    if ((*pointer)[size - 1]) {
      strcpy((*pointer)[size - 1], str);
    } else {
      error = 1;
    }
  } else {
    error = 1;
    *pointer = *tmp;
  }
  return error;
}
void freePointer(char ***pointer, int size) {
  if (*pointer) {
    for (int i = 0; i < size; i++) {
      if ((*pointer)[i]) free((*pointer)[i]);
    }
    free(*pointer);
  }
}
void freeEverything(params *settings) {
  freePointer(&(settings->file_name), settings->file_count);
  freePointer(&(settings->strPattern), settings->pattern);
  freePointer(&(settings->pattern_files), settings->files);
  if (settings->compiled) free(settings->compiled);
}