#include <stdio.h>
#include <tonc.h>
#include "efx.h"
#include "lcdiobuffer.h"
#include "menu.h"
#include "main.h"
#include "pillar.h"
#include "puzzle.h"
#include "video.h"

EWRAM_DATA u8 puzStageMax;
EWRAM_DATA u8 puzStageCur;
EWRAM_DATA u8 puzLength;
EWRAM_DATA u8 puzBreadth;
EWRAM_DATA u8 puzHeight;
EWRAM_DATA u8 puzPad;
EWRAM_DATA s16 initPilCamY;
EWRAM_DATA s16 initBGOfsY;
EWRAM_DATA s16 initBGOfsY2;
EWRAM_DATA struct Pillar* selPil;

u8* const puzDim[3] = {&puzLength, &puzBreadth, &puzHeight};

// Creates a puzzle of given dimensions.
const void puzzleGenerate(int length, int breadth, int height) {
  struct Pillar pil;
  int row, col, i, j, matchHeight, matchColour, id;
  int size = length * breadth;
  u8 clrs[length][breadth][6];
  u8 turnpillars[size];
  const int sampleSizeRange[PUZZLE_LENGTH_MAX * PUZZLE_BREADTH_MAX][2] = {
    {0, 0}, {0, 0}, {0, 0}, {1, 3},
    {0, 0}, {2, 4}, {0, 0}, {2, 6},
    {2, 7}, {0, 0}, {0, 0}, {3, 9},
    {0, 0}, {0, 0}, {0, 0}, {4, 12}
  };
  int sampleSize = qran_range(sampleSizeRange[size-1][0], sampleSizeRange[size-1][1]+1);
  
  // Randomize pillar colours.
  for (row = 0; row < length; row++)
    for (col = 0; col < breadth; col++)
      pilGenerateRandColours(clrs[row][col]);
  
  // Set specific colours to match for rows.
  for (row = 0; row < length; row++) {
    matchHeight = qran_range(PUZZLE_HEIGHT_MIN, height+1);
    matchColour = qran_range(0, 4);
    
    for (col = 0; col < breadth; col++)
      clrs[row][col][matchHeight + 2] = matchColour;
  }
  
  // Repeat for columns.
  for (col = 0; col < breadth; col++) {
    matchHeight = qran_range(PUZZLE_HEIGHT_MIN, height+1);
    matchColour = qran_range(0, 4);
    
    for (row = 0; row < length; row++)
      clrs[row][col][matchHeight - 1] = matchColour;
  }
  
  // Create pillars.
  initPilArray(0, 0);
  for (row = 0; row < length; row++) {
    for (col = 0; col < breadth; col++) {
      pil = pilConstr(clrs[row][col], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
      id = addPilToPilArray(&pil);
      pilArray[id].height = height;
      pilSetAnim(&pilArray[id], PIL_ANIM_IDLE);
      
      // Set neighbouring pillar pointers.
      pilArray[id].left = (col == 0 && row == 0) ? &pilArray[length * breadth - 1] : &pilArray[id - 1];
      pilArray[id].right = (col == breadth - 1 && row == length - 1) ? &pilArray[0] : &pilArray[id + 1];
      pilArray[id].up = (id < breadth) ? &pilArray[breadth * length - ((breadth - id) % breadth) - 1] : &pilArray[id - breadth];
      pilArray[id].down = (id >= breadth * (length - 1)) ? &pilArray[(id + 1 - breadth * (length - 1)) % breadth] : &pilArray[id + breadth];
      
      // LeftUp.
      if (row == length - 1 && col == 0)
        pilArray[id].leftUp = &pilArray[breadth - 1];
      else if (col == 0)
        pilArray[id].leftUp = &pilArray[id + breadth + min(breadth - 1, length - row - 2) * (breadth + 1)];
      else if (row == 0)
        pilArray[id].leftUp = &pilArray[id - 1 +       min(length - 1,  breadth - col)    * (breadth + 1)];
      else
        pilArray[id].leftUp = &pilArray[id - breadth - 1];
      
      // LeftDown.
      if (row == 0 && col == 0)
        pilArray[id].leftDown = &pilArray[breadth * length - 1];
      else if (col == 0)
        pilArray[id].leftDown = &pilArray[id - breadth - min(breadth - 1, row - 1)  * (breadth - 1)];
      else if (row == length - 1)
        pilArray[id].leftDown = &pilArray[id - breadth - min(length - 2,  breadth - col - 1) * (breadth - 1)];
      else
        pilArray[id].leftDown = &pilArray[id + breadth - 1];
      
      // RightUp.
      if (col == breadth - 1 && row == length - 1)
        pilArray[id].rightUp = &pilArray[0];
      else if (col == breadth - 1)
        pilArray[id].rightUp = &pilArray[id + breadth + min(breadth - 1, length - row - 2) * (breadth - 1)];
      else if (row == 0)
        pilArray[id].rightUp = &pilArray[id + breadth + min(length - 2,  col)              * (breadth - 1)];
      else
        pilArray[id].rightUp = &pilArray[id - breadth + 1];
      
      // RightDown.
      if (col == breadth - 1 && row == 0)
        pilArray[id].rightDown = &pilArray[breadth * (length - 1)];
      else if (row == length - 1)
        pilArray[id].rightDown = &pilArray[id - breadth - min(length - 2,  col)     * (breadth + 1)];
      else if (col == breadth - 1)
        pilArray[id].rightDown = &pilArray[id - breadth - min(breadth - 1, row - 1) * (breadth + 1)];
      else
        pilArray[id].rightDown = &pilArray[id + breadth + 1];
    }
  }
  
  // Turn a sample of pillars.
  for (i = 0; i < size; i++)
    turnpillars[i] = i;
  j = sampleSize;
  for (i = 0; i < sampleSize; i++) {
    id = turnpillars[qran_range(0, j)];
    pilArray[id].turned = true;
    turnpillars[i] = turnpillars[j];
    j--;
  }
}

const void puzzleInit() {
  menuClear();
  
  initPilCamY = pilCamY;
  initBGOfsY = lcdioBuffer.bg1vofs;
  initBGOfsY2 = lcdioBuffer.bg3vofs;
  
  setGameState(GAME_PUZZLE, PUZZLE_TRANSITION1);
}

const void puzzleTransition1() {
  int yOffs, pilID, i, j;
  int loBndX, hiBndX, loBndY, hiBndY;
  u8 clrs[6];
  struct Pillar pil;
  const int pilCount = PIL_ARRAY_MAX;
  
  if (gStateClock < 60) {
    // Move camera away from title screen.
    yOffs = ease(0, 160, gStateClock, 60, EASE_IN_QUADRATIC);
    pilCamY = initPilCamY - yOffs;
    lcdioBuffer.bg1vofs = initBGOfsY - yOffs;
    lcdioBuffer.bg2vofs = initBGOfsY - yOffs;
    lcdioBuffer.bg3vofs = initBGOfsY2 - yOffs;
    
  } else if (gStateClock == 60) {
    // Init LCDIO.
    lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_OBJ_1D | DCNT_OBJ;
    lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
    lcdioBuffer.bg1cnt = BG_BUILD(0, 29, 0, 0, 1, 0, 0);
    lcdioBuffer.bg2cnt = BG_BUILD(0, 30, 0, 0, 2, 0, 0);
    lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 0, 3, 0, 0);
    
    // Clear background tiles & screen entries.
    CBB_CLEAR(0);
    CBB_CLEAR(1);
    CBB_CLEAR(2);
    CBB_CLEAR(3);
    CpuFastFill(0, &bgmap[0], 0x800);
    
    // Reset BG HOFS and VOFS.
    lcdioBuffer.bg0hofs = 0;
    lcdioBuffer.bg0vofs = 0;
    lcdioBuffer.bg1hofs = 0;
    lcdioBuffer.bg1vofs = 0;
    lcdioBuffer.bg2hofs = 0;
    lcdioBuffer.bg2vofs = 0;
    lcdioBuffer.bg3hofs = 0;
    lcdioBuffer.bg3vofs = 0;
    
    // Create temporary flavour pillars.
    initPilArray(-104, 160);
    
    for (i = 0; i < pilCount; i++) {
      // Set random colours.
      pilGenerateRandColours(clrs);
      
      // Set randomish (less skewed towards centre) coordinates.
      if (i & 1) {
        loBndX = -(i>>1);
        hiBndX = -(i>>1)+3;
        loBndY = -3-(i>>1);
        hiBndY = -(i>>1);
      } else {
        loBndX = -3-(i>>1);
        hiBndX = -(i>>1);
        loBndY = -(i>>1);
        hiBndY = -(i>>1)+3;
      }
      int coords[2] = PIL_TILE(qran_range(loBndX, hiBndX), qran_range(loBndY, hiBndY));
      
      pil = pilConstr(clrs, coords[0], coords[1]);
      pilID = addPilToPilArray(&pil);
      pilArray[pilID].height = qran_range(0, 4);
      
      // Unhide pillar if it doesn't overlap with any other pillars.
      for (j = 0; j < i; j++) {
        if (pilArray[pilID].x == pilArray[j].x &&
            pilArray[pilID].y == pilArray[j].y)
          break;
      }
      if (j == i)
        pilSetAnim(&pilArray[pilID], PIL_ANIM_IDLE);      // Unhide pillar.
    }
    
  } else {
    pilCamY -= 4;
    
    // Move to next state if all flavour pillars have passed.
    for (i = 0; i < pilCount; i++) {
      if (pilArray[i].id != 0) {
        yOffs = (pilArray[i].x + pilArray[i].y) - pilCamY;
        if (yOffs < SCREEN_HEIGHT)
          break;
      }
    }
    if (i == pilCount) {
      setGameState(gGameState, PUZZLE_TRANSITION2);
      return;
    }
  }
  
  pilAnimRand(60);
  pilRunAnims();
  pilDrawAll();
  gStateClock++;
}

const void puzzleTransition2() {
  if (gStateClock == 0) {
    puzzleGenerate(puzLength, puzBreadth, puzHeight);
    pilCamX = -104;
    pilCamY = 160;
  } else if (gStateClock < 45) {
    pilCamY = ease(160, 14, gStateClock, 45, EASE_OUT_QUADRATIC);
  } else {
    pilCamY = 14;
    
    selPil = &pilArray[0];
    
    setGameState(gGameState, PUZZLE_IDLE);
    gStateClock = 14;
  }
  
  pilRunAnims();
  pilDrawAll();
  gStateClock++;
}

const void puzzleIdle() {
  struct Pillar* prev;
  int moved;
  
  // Handle fire button input.
  // TODO
  
  // Handle D-pad input.
  prev = selPil;
  moved = moveCursorByInput((u32*)&selPil,
                            (u32)selPil->leftDown,
                            (u32)selPil->rightUp,
                            (u32)selPil->leftUp,
                            (u32)selPil->rightDown,
                            (u32)selPil->left,
                            (u32)selPil->down,
                            (u32)selPil->up,
                            (u32)selPil->right);
  if (moved)
    ; // TODO
  
  pilRunAnims();
  pilDrawHighlight(selPil, gStateClock);
  pilDrawAll();
  
  gStateClock++;
}

const void puzzleUpdate() {
  switch(gGenericState) {
    case PUZZLE_TRANSITION1:
      puzzleTransition1();
      break;
    case PUZZLE_TRANSITION2:
      puzzleTransition2();
      break;
    case PUZZLE_IDLE:
      puzzleIdle();
      break;
  }
}