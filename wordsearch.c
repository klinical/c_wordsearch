#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/utility.h"

#define FNAMELEN 50
#define MAX_DICT_WLEN 50
#define ALPHABET_LEN 29

typedef struct scramble {
  char **data_sheet; /* multi dimensional dynamic array */
  char **comp_sheet; /* sheet for comparison, 0 = no matching charm 1 = matching char */
  int rows;
  int cols;
} Scramble;

typedef struct dictionary {
  char (*data)[MAX_DICT_WLEN]; /* dynamic array of static arrays */
  char *f_chars; /* dynamically allocated list of each first char in dict word */
  int entries;
  long int loc; /* starting locaiton of the dictionary */
} Dict;

char **solve(Scramble *, Dict *);
void p_data(char **, int, int);
Scramble *mk_scramble(FILE *, Scramble *);
Dict *mk_dict(FILE *, Dict *, long int);

int main(int argc, char * argv[]) {
  int i;
  Scramble scramble; /* word scramble to search through */
  Dict dict; /* dictionary */
	char input_file[FNAMELEN];
  char output_file[FNAMELEN];
  FILE* ifp = NULL;
  FILE* ofp = NULL;
  char **solution = NULL;

  if (argc == 1) {
    /* ask user to enter some file names */
    fputs("Enter name of wordsearch input file: ", stdout);
    S_Gets(input_file, FNAMELEN);
    ifp = T_FOpen(input_file, "r");

    fputs("Enter name of desired output file to store the solution: ", stdout);
    S_Gets(output_file, FNAMELEN);
    ofp = T_FOpen(output_file, "w");
  } else if (argc == 3) {
    /* try to fopen argv[1] and argv[2] */
    ifp = T_FOpen(argv[1], "r");
    ofp = T_FOpen(argv[2], "w");
  } else {
    puts("INVALID NUMBER OF ARGUMENTS. EITHER RUN WITH TWO ARGUMENTS: SOURCE FILE AND OUTPUT FILE, OR WITH NO ARGUMENTS.");
    exit(EXIT_FAILURE);
  }

  /* Partition the scramble / word search data */
  if (!mk_scramble(ifp, &scramble)) {
    puts("ALLOCATION FOR SCRAMBLE DATA FAILED. FATAL.");
    exit(EXIT_FAILURE);
  }

  /* Determine the starting location of the dictionary */
  rewind(ifp);
  fseek(ifp, (scramble.cols * scramble.rows), SEEK_SET);
  dict.loc = ftell(ifp);

  /* Partition the dictionary data */
  if (!mk_dict(ifp, &dict, dict.loc)) {
    puts("ALLOCATION FOR DICTIONARY FAILED. FATAL.");
    exit(EXIT_FAILURE);
  }

  solution = solve(&scramble, &dict);

  fclose(ifp);
  fclose(ofp);

  /* Free all dynamically allocated memory */
  for (i = 0; i < scramble.rows; i++) {
    free(scramble.data_sheet[i]);
  }

  free(scramble.data_sheet);
  free(dict.data);
  free(dict.f_chars);
  /*                                        */

	return 0;
}

char **solve(Scramble *scramble, Dict *dict) {
  int i, j, k;
  char ch;
  char **sbl = scramble->data_sheet; /* short hand for the scramble data sheet */
  char **cmp = scramble->comp_sheet; /* short hand for comparison sheet */
  char **solution = NULL;

  /* For each row */
  for (i = 0; i < scramble->rows; i++) {
    /* For each column */
    for (j = 0; j < scramble->cols; j++) {
      /* For each first char in each dictionary entry */
      for (k = 0; k < dict->entries; k++) {
        if ((ch = sbl[i][j]) == dict->f_chars[k]) {
          printf("First char found at %d,%d word num %d\n", i, j, k);
        }        
      }
    }
  }

  return solution;
}

int search(char **data, enum direction dir) {

}

Scramble *mk_scramble(FILE *fp, Scramble *scramble) {
  int index, inner_index;
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
  scramble->data_sheet = (char**) malloc(scramble->rows * sizeof(char *));
  scramble->comp_sheet = (char**) malloc(scramble->rows * sizeof(char *));

  rewind(fp);
  for (index = 0; index < scramble->rows; index++) {
    /* allocate columns for each row */
    scramble->data_sheet[index] = (char *) malloc(scramble->cols * sizeof(char));
    scramble->comp_sheet[index] = (char *) malloc(scramble->cols * sizeof(char));

    /* read line from file into data[index] */
    fread(scramble->data_sheet[index], scramble->cols, 1, fp);
    
    for (inner_index = 0; inner_index < scramble->cols-2; inner_index += 2) {
      scramble->comp_sheet[index][inner_index] = '0';
    }

    scramble->comp_sheet[index][scramble->cols-1] = '\n';
  }
  
  /* display so the scramble for funsies */
  p_data(scramble->data_sheet, scramble->rows, scramble->cols);
  p_data(scramble->comp_sheet, scramble->rows, scramble->cols);

  return scramble;
}

Dict *mk_dict(FILE *fp, Dict *dict, long int f_loc) {
  /* dynamic height, static width */
  char ch;
  int entries;
  int index;
  int inner_index;

  entries = 0;
  while ((ch = fgetc(fp)) != EOF) {
    if (ch == '\n')
      entries++;
  }

  /* allocate space for the first characters in each word */
  dict->f_chars = malloc(entries * sizeof(char));

  printf("%d entries.", entries);

  /* rewind to start of dict */
  fseek(fp, f_loc, SEEK_SET);

  dict->data = malloc(entries * sizeof(char[MAX_DICT_WLEN]));

  inner_index = 0;
  for (index = 0; index < entries; index++) {
    ch = getc(fp);
    dict->f_chars[index] = ch;

    while ((ch = getc(fp)) != '\n') {
      dict->data[index][inner_index] = ch;

      inner_index++;
    }

    /* we want each dictionary entry to be a string, a single 'word' */
    dict->data[index][inner_index+1] = '\0'; /* replace newline with null terminator */
    inner_index = 0;
  }

  dict->entries = entries;

  return dict;
}

void p_data(char **data, int rows, int cols) {
  int index, inner_index;
  for (index = 0; index < rows; index++) {
    for (inner_index = 0; inner_index < cols; inner_index++) {
      printf("%2c", data[index][inner_index]);
    }
  }

  putchar('\n');
}
