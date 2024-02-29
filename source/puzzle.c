#include <stdio.h>
#include <tonc.h>
#include "puzzle.h"

EWRAM_DATA u8 puzStageMax;
EWRAM_DATA u8 puzStageCur;
EWRAM_DATA u8 puzLength;
EWRAM_DATA u8 puzBreadth;
EWRAM_DATA u8 puzHeight;
EWRAM_DATA u8 puzPad[3];

u8* const puzDim[3] = {&puzLength, &puzBreadth, &puzHeight};

const void puzzleInit() {
  // TODO
}