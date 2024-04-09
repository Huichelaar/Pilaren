#ifndef PUZZLE
#define PUZZLE
#include "pillar.h"

#define PUZ_LENGTH_MIN 2
#define PUZ_LENGTH_MAX 4
#define PUZ_BREADTH_MIN 2
#define PUZ_BREADTH_MAX 4
#define PUZ_HEIGHT_MIN 1
#define PUZ_HEIGHT_MAX PIL_HEIGHT_MAX
#define PUZ_ARRAY_MAX 20

extern s16 initPilCamY;
extern s16 initBGOfsY;
extern s16 initBGOfsY2;

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

struct Puzzle {
  u8 stageMax;
  u8 stageCur;
  u8 length;
  u8 breadth;
  u8 height;
  u8 solveStatus;
  s16 camX;
  s16 camY;
  
  u16 pilCounter;
  struct Pillar* selPil;
  struct Pillar pil[PUZ_ARRAY_MAX];
};
extern struct Puzzle puzzle[2];       // puzzle[0] uses actual logic, puzzle[1] is for display-only.

const void puzMatchRowOrCol(int id, int col);
int puzIsSolved();
const void puzDrawAll(int puzID);
const void puzDrawMatch(int puzID, int timer);
const void puzDrawHighlight(int puzID, struct Pillar* pil, int timer);
int puzAddPil(int puzID, struct Pillar* pil);
const void puzGenerate();
const void puzInit();
const void puzInitPil(int puzID, int x, int y);
const void puzAnimRand(int puzID, int freq);
const void puzReloadTiles(int puzID);
const void puzRunAnims(int puzID);
const void puzTransition1();
const void puzTransition2();
const void puzIdle();
const void puzUpdate();

#endif // PUZZLE