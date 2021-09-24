#include "includes/utility.h"

/* Source: C Primer Plus p.591 Figure 13.5*/
char* S_Gets(char* st, int maxlen) {
  char* ret;
  char* find;

  ret = fgets(st, maxlen, stdin);
  if (ret) {
    find = strchr(st, '\n');

    /* newline found, swap the char with a str terminator */
    if (find) {
      *find = '\0';
    } else { /* no newline found, eat line */
      while (getchar() != '\n');
    }
  }

  return ret;
}

FILE* T_FOpen(const char* dest, const char* mode) {
  FILE* fp;
  if ((fp = fopen(dest, mode)) == NULL) {
    printf("FAILED TO OPEN %s, TERMINATING.", dest);
    exit(EXIT_FAILURE);
  }

  return fp;
}
