#ifndef PUZZLE
#define PUZZLE
#include "pillar.h"

#define PUZZLE_LENGTH_MIN 2
#define PUZZLE_LENGTH_MAX 4
#define PUZZLE_BREADTH_MIN 2
#define PUZZLE_BREADTH_MAX 4
#define PUZZLE_HEIGHT_MIN 1
#define PUZZLE_HEIGHT_MAX PILLAR_HEIGHT_MAX

extern u8 puzStageMax;
extern u8 puzStageCur;
extern u8 puzLength;
extern u8 puzBreadth;
extern u8 puzHeight;
extern u8 puzSolveStatus;
extern s16 initPilCamY;
extern s16 initBGOfsY;
extern s16 initBGOfsY2;
extern struct Pillar* selPil;

extern u8* const puzDim[3];

enum {
  // puzzle states.
  PUZZLE_TRANSITION1 = 0,
  PUZZLE_TRANSITION2 = 1,
  PUZZLE_IDLE = 2,
};

enum {
  // puzzle display options.
  PUZDISP_DISABLE = 0x00,
  PUZDISP_HIGHLIGHT1 = 0x01,
  PUZDISP_HIGHLIGHT2 = 0x02,
  PUZDISP_HIGHLIGHT3 = 0x03,
  //...
  PUZDISP_MATCH1 = 0x10,
  PUZDISP_MATCH2 = 0x20
};
extern u8 puzDispOptions;

const void puzzleMatchRowOrCol(int id, int col);
int puzzleIsSolved();
const void puzzleDrawMatch(int timer);
const void puzzleDrawHighlight(struct Pillar* pil, int timer);
const void puzzleGenerate();
const void puzzleInit();
const void puzzleTransition1();
const void puzzleTransition2();
const void puzzleIdle();
const void puzzleUpdate();

#endif // PUZZLE