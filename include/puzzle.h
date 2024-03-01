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
extern u8 puzPad[3];

extern u8* const puzDim[3];

const void puzzleInit();

#endif // PUZZLE