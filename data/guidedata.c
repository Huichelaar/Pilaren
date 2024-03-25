#include <stdio.h>
#include <tonc.h>
#include "guide.h"
#include "lang.h"

const char* guidePageText[GUIDE_PAGECOUNT][LANG_COUNT] = {
  {"#{ci:3}Pilaren is a puzzle which presents you with a matrix\nof pillars. Pillars have two sides; The left faces a\n#{ci:15}column#{ci:3} and the right faces a #{ci:7}row#{ci:3}. In a stage-1\nmatrix, each side contains a pane which can be white,\n#{ci:4}blue#{ci:3}, #{ci:5}red#{ci:3} or #{ci:6}yellow#{ci:3}. If, for each #{ci:15}column#{ci:3} and #{ci:7}row#{ci:3},\nall panes' colours match, the puzzle is solved.\n\nThe matrix to the right is solved;\n#{ci:7}Row 1#{ci:3} is all #{ci:6}yellow#{ci:3}.\n#{ci:7}Row 2#{ci:3} is all white.\n#{ci:15}Column 1#{ci:3} is all #{ci:4}blue#{ci:3}.\n#{ci:15}Column 2#{ci:3} is all #{ci:5}red#{ci:3}.",
  
  "Gilaren is a puzzle game which presents you with\na matrix of pillars. A stage-1 matrix consists of two\nrows, two columns and each pillar consists of two\ncoloured panes. One pane represents the colour of\nthat pillar's row, and the other represents the colour\nof that pillar's column.\n\n"},
  
  {"#{ci:3}As the player, you are able to turn pillars,\nswapping which pane faces the #{ci:15}column#{ci:3}, and\nwhich pane faces the #{ci:7}row#{ci:3}. Note the below matrices:\n\n\n\n\n\n\nNeither #{ci:7}row 1#{ci:3} nor #{ci:15}column 1#{ci:3} of the matrix to the\nleft match. However, turning the top pillar will\nproduce the matrix on the right which is solved.",
  "Gorem ipsum"}};

