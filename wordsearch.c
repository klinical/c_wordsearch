#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FNAMELEN 50
#define MAX_DICT_WLEN 50
#define ALPHABET_LEN 29

typedef struct scramble {
  char** data; /* multi dimensional dynamic array */
  int rows;
  int cols;
} Scramble;

typedef struct dictionary {
  char (*data)[MAX_DICT_WLEN]; /* dynamic array of static arrays */
  char chlist[ALPHABET_LEN]; /* make room for space, newline and a null terminator */
  int word_count;
  long int loc; /* starting locaiton of the dictionary */
} Dict;

void p_data(char **, int, int);
char* s_gets(char *, int);
Scramble *mk_scramble(FILE *, Scramble *);
Dict *mk_dict(FILE *, Dict *, long int);
FILE* t_fopen(const char *, const char *);

Scramble *mk_scramble(FILE *fp, Scramble *scramble) {
  int index;
  char ch;

  /* determine width */
  index = 1; /* last entry won't increment due to check */
  while ((ch = getc(fp)) != '\n')
    index++;

  scramble->cols = index; /* actual width, NOT including \n */

  scramble->rows = 1;
  index = 1;
  while ((ch = getc(fp)) != '\0') {
    if (ch == '\n') { /* a line */
      if (index == scramble->cols) { /* this line isnt as long as the known width */
        scramble->rows++;

        index = 1;
      }
      else
        break; /* this line is as long as the known width, increment scr height */
    } else
      index++;
  }

  /* allocate rows */
  scramble->data = (char**) malloc(scramble->rows * sizeof(char *));

  rewind(fp);
  for (index = 0; index < scramble->rows; index++) {
    /* allocate columns for each row */
    scramble->data[index] = (char *) malloc(scramble->cols * sizeof(char));
    /* read line from file into data[index] */
    fread(scramble->data[index], scramble->cols, 1, fp);
  }
  
  /* display */
  p_data(scramble->data, scramble->rows, scramble->cols);

  return scramble;
}

Dict *mk_dict(FILE *fp, Dict *dict, long int f_loc) {
  /* dynamic height, static width */
  char ch;
  int entries;
  int index;
  int inner_index;
  int chlist_index;

  dict->chlist[ALPHABET_LEN - 1] = '\n';

  entries = 0;
  while ((ch = fgetc(fp)) != EOF) {
    if (ch == '\n')
      entries++;
  }

  /* rewind to start of dict */
  fseek(fp, f_loc, SEEK_SET);

  dict->data = malloc(entries * sizeof(char[MAX_DICT_WLEN]));

  chlist_index = 0;
  inner_index = 0;
  for (index = 0; index < entries; index++) {
    while ((ch = getc(fp)) != '\n') {
      dict->data[index][inner_index] = ch;
      if (!strchr(dict->chlist, ch)) {
        /* not in chlist/alphabet, add it */
        dict->chlist[chlist_index] = ch;
        chlist_index++;
      }

      inner_index++;
    }

    dict->data[index][inner_index+1] = '\0'; /* replace newline with null terminator */
    inner_index = 0;
  }

  for (index = 0; index < entries; index++) {
    puts(dict->data[index]);
  }

  return dict;
}

void p_data(char **data, int rows, int cols) {
  int index, inner_index;
  putchar(' ');
  for (index = 0; index < rows; index++) {
    for (inner_index = 0; inner_index < cols; inner_index++) {
      printf("%-2c", data[index][inner_index]);
    }
  }

  putchar('\n');
}

/* Source: C Primer Plus p.591 Figure 13.5*/
char* s_gets(char* st, int maxlen) {
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

FILE* t_fopen(const char* dest, const char* mode) {
  FILE* fp;
  if ((fp = fopen(dest, mode)) == NULL) {
    printf("FAILED TO OPEN %s, TERMINATING.", dest);
    exit(EXIT_FAILURE);
  }

  return fp;
}

int main(int argc, char * argv[]) {
  Scramble scramble; /* word scramble to search through */
  Dict dict; /* dictionary */
	char input_file[FNAMELEN];
  char output_file[FNAMELEN];
  FILE* ifp = NULL;
  FILE* ofp = NULL;

  if (argc == 1) {
    /* ask user to enter some file names */
    fputs("Enter name of wordsearch input file: ", stdout);
    s_gets(input_file, FNAMELEN);
    ifp = t_fopen(input_file, "r");

    fputs("Enter name of desired output file to store the solution: ", stdout);
    s_gets(output_file, FNAMELEN);
    ofp = t_fopen(output_file, "w");
  } else if (argc == 3) {
    /* try to fopen argv[1] and argv[2] */
    ifp = t_fopen(argv[1], "r");
    ofp = t_fopen(argv[2], "w");

    printf("%s , %s \n", argv[1], argv[2]);
  } else {
    puts("INVALID NUMBER OF ARGUMENTS. EITHER RUN WITH TWO ARGUMENTS: SOURCE FILE AND OUTPUT FILE, OR WITH NO ARGUMENTS.");
    exit(EXIT_FAILURE);
  }

  puts("Allocating scramble data.");

  /* Partition the scramble / word search data */
  if (!mk_scramble(ifp, &scramble)) {
    puts("ALLOCATION FOR SCRAMBLE DATA FAILED. FATAL.");
    exit(EXIT_FAILURE);
  }

  puts("Allocated scramble data.");

  /* Determine the starting locaiton of the dictionary */
  rewind(ifp);
  fseek(ifp, (scramble.cols * scramble.rows), SEEK_SET);
  dict.loc = ftell(ifp);

  /* Partition the dictionary data */
  if (!mk_dict(ifp, &dict, dict.loc)) {
    puts("ALLOCATION FOR DICTIONARY FAILED. FATAL.");
    exit(EXIT_FAILURE);
  }

  puts("Allocated dictionary data.");

  clr_unused(&scramble, &dict);

  fclose(ifp);
  /* free_vec(scramble.data); */
  /* free_vec(dict.data); */
	return 0;
}
