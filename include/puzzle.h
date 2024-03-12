#ifndef PUZZLE
#define PUZZLE

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
extern u8 puzPad;
extern s16 initPilCamY;
extern s16 initBGOfsY;
extern s16 initBGOfsY2;

extern u8* const puzDim[3];

enum {
  // puzzle states.
  PUZZLE_TRANSITION1 = 0,
  PUZZLE_TRANSITION2 = 1,
  PUZZLE_IDLE = 2,
};

const void puzzleGenerate(int length, int breadth, int height);
const void puzzleInit();
const void puzzleTransition1();
const void puzzleTransition2();
const void puzzleIdle();
const void puzzleUpdate();

#endif // PUZZLE