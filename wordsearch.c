#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/utility.h"

#define FNAMELEN 50
#define MAX_DICT_WLEN 50
#define ALPHABET_LEN 29

typedef struct scramble {
  char **data_sheet; /* multi dimensional dynamic array */
  char **comp_sheet; /* sheet for comparison, 0 = no matching char 1 = matching char, use char bc only need 1 bit (though we use a byte) */
  int rows;
  int cols;
} Scramble;

typedef struct dictionary {
  char (*data)[MAX_DICT_WLEN]; /* dynamic array of static arrays */
  char *f_chars; /* dynamically allocated list of each first char in dict word */
  int entries;
  long int loc; /* starting locaiton of the dictionary */
} Dict;

void solve(Scramble *, Dict *);
void p_data(char **, int, int);
Scramble *mk_scramble(FILE *, Scramble *);
Dict *mk_dict(FILE *, Dict *, long int);
void search(int, int, const Scramble *, direction, const char[MAX_DICT_WLEN]);

int main(int argc, char * argv[]) {
  int i, j;
  Scramble scramble; /* word scramble to search through */
  Dict dict; /* dictionary */
	char input_file[FNAMELEN];
  char output_file[FNAMELEN];
  FILE* ifp = NULL;
  FILE* ofp = NULL;

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

  solve(&scramble, &dict);

  for (i = 0; i < scramble.rows; i++) {
    for (j = 0; j < scramble.cols; j++) {
      fprintf(ofp, "%c", (scramble.data_sheet[i][j]));
    }
  }

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

void solve(Scramble *scramble, Dict *dict) {
  int i, j, k, l;
  char ch;
  char **sbl = scramble->data_sheet; /* short hand for the scramble data sheet */
  char **cmp = scramble->comp_sheet; /* short hand for comparison sheet */

  /* For each row */
  for (i = 0; i < scramble->rows; i++) {
    /* For each column */
    for (j = 0; j < scramble->cols; j++) {
      /* For each first char in each dictionary entry
          Searching for a match to a first char */
      for (k = 0; k < dict->entries; k++) {
        if ((ch = sbl[i][j]) == dict->f_chars[k]) {
          /* A first char from dict word was found 
              Note that f_char index (k) corresponds to the dict entry index */
          
          /* NW is the last element in the directions enum */
          for (l = 0; l <= NW; l++) {
            search(i, j, scramble, (direction) l, dict->data[k]);
          } 
        }        
      }
    }
  }

  for (i = 0; i < scramble->rows; i++) {
    /* skip spaces, new line and final space */
    for (j = 0; j <= (scramble->cols-3); j+=2) {
      if (cmp[i][j] != '1') {
        sbl[i][j] = ' ';
      }
    }
  }
}

void search(int row, int col, const Scramble *scramble, direction dir, const char dict_word[MAX_DICT_WLEN]) {
  int i, j, k;
  char **sbl = scramble->data_sheet;
  char **cmp = scramble->comp_sheet;
  int start_row = row, start_col = col;
  int wordlen = strlen(dict_word);

  /* We have the word we are trying to match for, search until we either find a
  non match char, or, if we find the entire word, mark the comp_sheet with 1s in 
  'dir' direction */
  switch (dir) {
    case N:
      /* increment and check by row until 0 */
      for (i = 0; row >= 0; row--, i++) {
        if (sbl[row][start_col] != dict_word[i]) {
          /* no longer matches, break out and do nothing */
          break;
        } else {
          /* matches */
          if ((i+1) == wordlen) {
            /* found a full word! Note that we know how long the word is, and our starting pos */
            for (j = start_row; j >= row; j--)
              cmp[j][start_col] = '1';

            break;
          }
        }
      }
      break;
    case NE:
      for (i = 0; col <= (scramble->cols-3) && row >= 0; col+=2, row--, i++) {
        /* skipping spaces by inc col by 2 - scramble->cols-1 is a newline char, -2 is a space */
        if(sbl[row][col] != dict_word[i]) {
          break;
        } else {
          if ((i+1) == wordlen) {
            for (j = start_row, k = start_col; j >= row && k <= col; j--, k+=2) {
              cmp[j][k] = '1';
            }

            break;
          }
        }
      }
      break;
    case E:
      for (i = 0; col < scramble->cols; col++, i++) {
        if (sbl[start_row][col] == ' ') {
          i-=1;
          continue;
        }

        if(sbl[start_row][col] != dict_word[i]) {
          break;
        } else {
          if ((i+1) == wordlen) {
            for (j = start_col; j <= col; j++) {
              if (sbl[start_row][j] == ' ') {
                continue;
              } else {
                cmp[start_row][j] = '1';
              }
            }

            break;
          }
        }
      }
      break;
    case SE:
      for (i = 0; col <= (scramble->cols-3) && row < scramble->rows; col+=2, row++, i++) {
        /* skipping spaces by inc col by 2 - scramble->cols-1 is a newline char, -2 is a space */
        if(sbl[row][col] != dict_word[i]) {
          break;
        } else {
          if ((i+1) == wordlen) {
            for (j = start_row, k = start_col; j <= row && k <= col; j++, k+=2) {
              cmp[j][k] = '1';
            }

            break;
          }
        }
      }
      break;
    case S:
      for (i = 0; row < scramble->rows; row++, i++) {
        if (sbl[row][start_col] != dict_word[i]) {
          break;
        } else {
          /* matches */
          if ((i+1) == wordlen) {
            /* found a full word! Note that we know how long the word is, and our starting pos */
            for (j = start_row; j <= row; j++)
              cmp[j][start_col] = '1';

            break;
          }
        }
      }
      break;
    case SW:
      for (i = 0; col >= 0 && row < scramble->rows; col-=2, row++, i++) {
        /* skipping spaces by inc col by 2 - scramble->cols-1 is a newline char, -2 is a space */
        if(sbl[row][col] != dict_word[i]) {
          break;
        } else {
          if ((i+1) == wordlen) {
            for (j = start_row, k = start_col; j <= row && k >= col; j++, k-=2) {
              cmp[j][k] = '1';
            }

            break;
          }
        }
      }
      break;
    case W:
      for (i = 0; col >= 0; col--, i++) {
        if (sbl[start_row][col] == ' ') {
          i-=1;
          continue;
        }

        if(sbl[start_row][col] != dict_word[i]) {
          break;
        } else {
          if ((i+1) == wordlen) {
            for (j = start_col; j >= col; j--) {
              if (sbl[start_row][j] == ' ') {
                continue;
              } else {
                cmp[start_row][j] = '1';
              }
            }

            break;
          }
        }
      }
      break;
    case NW:
      for (i = 0; col >= 0 && row >= 0; col-=2, row--, i++) {
        /* skipping spaces by inc col by 2 - scramble->cols-1 is a newline char, -2 is a space */
        if(sbl[row][col] != dict_word[i]) {
          break;
        } else {
          if ((i+1) == wordlen) {
            for (j = start_row, k = start_col; j >= row && k >= col; j--, k-=2) {
              cmp[j][k] = '1';
            }

            break;
          }
        }
      }
      break;
    default:
      puts("Invalid search direction. This is a bug.");
  }
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

  /* rewind to start of dict */
  fseek(fp, f_loc, SEEK_SET);

  dict->data = malloc(entries * sizeof(char[MAX_DICT_WLEN]));

  inner_index = 0;
  for (index = 0; index < entries; index++) {
    ch = getc(fp);
    dict->f_chars[index] = ch;
    dict->data[index][inner_index] = ch;

    inner_index = 1;
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
      printf("%c", data[index][inner_index]);
    }
  }

  putchar('\n');
}
