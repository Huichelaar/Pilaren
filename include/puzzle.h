#ifndef PUZZLE
#define PUZZLE
#include "menu.h"
#include "lang.h"
#include "pillar.h"

#define PUZ_LENGTH_MIN 2
#define PUZ_LENGTH_MAX 4
#define PUZ_BREADTH_MIN 2
#define PUZ_BREADTH_MAX 4
#define PUZ_HEIGHT_MIN 1
#define PUZ_HEIGHT_MAX PIL_HEIGHT_MAX
#define PUZ_ARRAY_MAX 20

extern s16 initCam;
extern s16 initBGOfsY[3];

enum {
  // puzzle states.
  PUZZLE_TRANSITION1 = 0,
  PUZZLE_TRANSITION2 = 1,
  PUZZLE_SOLVETRANSITION = 2,
  PUZZLE_SOLVETRANSITIONTRUE = 3,
  PUZZLE_SOLVETRANSITIONFALSE = 4,
  PUZZLE_SOLVETRANSITIONRETURN = 5,
  PUZZLE_RAISE = 6,
  PUZZLE_LOWER = 7,
  PUZZLE_IDLE = 8,
};

enum {
  // puzzle display options.
  PUZDISP_CURSOR1 = 0x01,       // Blend entire pillar periodically.
  PUZDISP_CURSOR2 = 0x02,       // Blend top of pillar periodically.
  PUZDISP_CURSOR3 = 0x03,       // Set top of pillar to highlight colour.
  //...
  PUZDISP_CURSORDISABLE = 0x08,
  
  PUZDISP_MATCH1 = 0x10,        // Blend matching panes with green.
  PUZDISP_MATCH2 = 0x20,        // Set matching panes green.
  PUZDISP_MATCH3 = 0x40,        // Display matching panes in original colour. Not available in options.
  //...
  PUZDISP_MATCHDISABLE = 0x80
};
extern u8 puzDispOptions;
extern u8 puzPad5[3];

struct Puzzle {
  u8 stageMax;
  u8 stageCur;
  u8 length;
  u8 breadth;
  u8 height;
  u8 solveStatus;     // If true, re-solve puzzle. If false, do nothing.
  s16 camX;
  s16 camY;
  
  u16 pilCounter;
  struct Pillar* selPil;
  struct Pillar pil[PUZ_ARRAY_MAX];
};
extern struct Puzzle puzzle[2];                     // puzzle[0] uses actual logic, puzzle[1] is for display-only.
extern const char* promptText[8][LANG_COUNT][2];    // In data/promptData.c

INLINE int puzIsPilValid(struct Pillar* pil) {
  return (pil->height != 0) && (pil->animID != PIL_ANIM_HIDDEN);
}

const void puzMatchRowOrCol(int id, int col);
int puzIsSolved();
const void puzDrawAll(int puzID);
const void puzDrawMatch(int puzID, u8 puzDisp, int timer);
const void puzDrawCursor(int puzID, u8 puzDisp, struct Pillar* pil, int timer);
int puzAddPil(int puzID, struct Pillar* pil);
const void puzGenerate();
const void puzInit();
const void puzInitDefault();
const void puzInitPil(int puzID, int x, int y);
const void puzAnimRand(int puzID, int freq);
const void puzReloadTiles(int puzID);
const void puzRunAnims(int puzID);
const void puzLoadButtonPrompts();
const void puzDrawButtonPrompts();
const void puzStartExitMenu();
const void puzDrawExitMenu(const struct Menu* menu);
const void puzDrawStageSolvedMenu(const struct Menu* menu);
const void puzDrawSolvedMenu(const struct Menu* menu);
const void puzMenuExit();
const void puzExit();
const void puzNextStage();
const void puzTransition1();
const void puzTransition2();
const void puzRaise();
const void puzSolveTransition();
const void puzSolveTransitionTrue();
const void puzSolveTransitionFalse();
const void puzSolveTransitionReturn();
const void puzLower();
const void puzIdle();
const void puzUpdate();

#endif // PUZZLE