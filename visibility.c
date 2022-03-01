#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int NROWS = 25;
static int NCOLS = 100;

static int ROCK = ' ';
static int EMPTY = '.';
static int PASSAGE = '#';
static int HORIZONTAL = '-';
static int VERITAL = '|';
static int CORNER = '+';

static char** newGrid(void);
static void girdConvert(char** grid, FILE* fp);

int 
main(const int argc, char* argv[]) 
{

  char** grid = newGrid();

  FILE* mapf = fopen(argv[1], "r");

  girdConvert(grid, mapf);

  for (int y = 0; y < NROWS; y++) {
    for (int x = 0; x < NCOLS; x++) {
      printf("%c", grid[y][x]);
    }
    printf("\n");
  }
}

static char**
newGrid(void)
{
  // allocate a 2-dimensional array of NROWS x NCOLS
  char** grid = calloc(NROWS, sizeof(char*));
  char* contents = calloc(NROWS * NCOLS, sizeof(char));
  if (grid == NULL || contents == NULL) {
    fprintf(stderr, "cannot allocate memory for map\r\n");
    exit(1);
  }

  // set up the array of pointers, one for each row
  for (int y = 0; y < NROWS; y++) {
    grid[y] = contents + y * NCOLS;
  }

  // fill the board with empty cells
  for (int y = 0; y < NROWS; y++) {
    for (int x = 0; x < NCOLS; x++) {
      grid[y][x] = ' ';
    }
  }
  return grid;
}

static void
girdConvert(char** grid, FILE* fp)
{
  const int size = NCOLS+2;  // include room for \n\0
  char line[size];           // a line of input
  int y = 0;

  // read each line and copy it to the board
  while ( fgets(line, size, fp) != NULL && y < NROWS) {
    int len = strlen(line);
    if (line[len-1] == '\n') {
      // normal line
      len--; // don't copy the newline
    } else {
      // overly wide line
      len = NCOLS;
      fprintf(stderr, "board line %d too wide for screen; truncated.\r\n", y);
      for (char c = 0; c != '\n' && c != EOF; c = getc(fp))
        ; // scan off the excess part of the line
    }
    strncpy(grid[y++], line, len);
  }

  if (!feof(fp)) {
    fprintf(stderr, "board too big for screen; truncated to %d lines\r\n", y);
  }
}

