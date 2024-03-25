#include <stdio.h>
#include <tonc.h>
#include "efx.h"
#include "lcdiobuffer.h"
#include "menu.h"
#include "main.h"
#include "pillar.h"
#include "gfx/system.h"
#include "gfx/pil.h"
#include "puzzle.h"
#include "video.h"

EWRAM_DATA u8 puzStageMax;
EWRAM_DATA u8 puzStageCur;
EWRAM_DATA u8 puzLength;
EWRAM_DATA u8 puzBreadth;
EWRAM_DATA u8 puzHeight;
EWRAM_DATA u8 puzSolveStatus;
EWRAM_DATA s16 initPilCamY;
EWRAM_DATA s16 initBGOfsY;
EWRAM_DATA s16 initBGOfsY2;
EWRAM_DATA struct Pillar* selPil;
EWRAM_DATA u8 puzDispOptions;

u8* const puzDim[3] = {&puzLength, &puzBreadth, &puzHeight};

/* Sets upper nybble of all pillar->colour panes/elements in given pillar's
   row or column of puzzle array if pane is matched in row or column.
   Unsets otherwise.
   puzzleIsSolved can check these for each row and column to confirm
   if puzzle has been solved.
   
   id: id of pillar which' row or column will be matched.
   col: True if column, false if row.*/
const void puzzleMatchRowOrCol(int id, int col) {
  int i, j, pilID, paneID1, paneID2;
  
  if (col) {
    // Column.
    for (i = 0; i < PILLAR_HEIGHT_MAX; i++) {     // i = pane iterator.
      for (j = 0; j < puzLength - 1; j++) {       // j = row-element iterator.
        pilID = id + j * puzBreadth;
        paneID1 = i + pilArray[pilID].turned              * PILLAR_HEIGHT_MAX;
        paneID2 = i + pilArray[pilID + puzBreadth].turned * PILLAR_HEIGHT_MAX;
        if ((pilArray[pilID].colour[paneID1] & 0x3) != (pilArray[pilID + puzBreadth].colour[paneID2] & 0x3))
          break;
      }
      if (j == puzLength - 1) {
        // Column matches. Set match nybble in all of column's pillars.
        for (j = 0; j < puzLength; j++) {
          pilID = id + j * puzBreadth;
          paneID1 = i + pilArray[pilID].turned * PILLAR_HEIGHT_MAX;
          pilArray[pilID].colour[paneID1] |= 0x10;
        }
      } else {
        // Column doesn't match. Clear match nybble in all of column's pillars.
        for (j = 0; j < puzLength; j++) {
          pilID = id + j * puzBreadth;
          paneID1 = i + pilArray[pilID].turned * PILLAR_HEIGHT_MAX;
          pilArray[pilID].colour[paneID1] &= ~0x10;
        }
      }
    }
  } else {
    // Row.
    for (i = PILLAR_HEIGHT_MAX; i < PILLAR_HEIGHT_MAX * 2; i++) {     // i = pane iterator.
      for (j = 0; j < puzBreadth - 1; j++) {                          // j = row-element iterator.
        pilID = j + id * puzBreadth;
        paneID1 = i - pilArray[pilID].turned     * PILLAR_HEIGHT_MAX;
        paneID2 = i - pilArray[pilID + 1].turned * PILLAR_HEIGHT_MAX;
        if ((pilArray[pilID].colour[paneID1] & 0x3) != (pilArray[pilID + 1].colour[paneID2] & 0x3))
          break;
      }
      if (j == puzBreadth - 1) {
        // Row matches. Set match nybble in all of row's pillars.
        for (j = 0; j < puzBreadth; j++) {
          pilID = j + id * puzBreadth;
          paneID1 = i - pilArray[pilID].turned * PILLAR_HEIGHT_MAX;
          pilArray[pilID].colour[paneID1] |= 0x10;
        }
      } else {
        // Row doesn't match. Clear match nybble in all of row's pillars.
        for (j = 0; j < puzBreadth; j++) {
          pilID = j + id * puzBreadth;
          paneID1 = i - pilArray[pilID].turned * PILLAR_HEIGHT_MAX;
          pilArray[pilID].colour[paneID1] &= ~0x10;
        }
      }
    }
  }
}

// Checks if at least one pane matches in each row and column.
// If so, returns true; Puzzle is solved. Return false otherwise.
int puzzleIsSolved() {
  int i, j, pilID, paneID;
  
  // Row.
  for (i = 0; i < puzLength; i++) {
    for (j = PILLAR_HEIGHT_MAX; j < PILLAR_HEIGHT_MAX + puzHeight; j++) {
      pilID = i * puzBreadth;
      paneID = j - pilArray[pilID].turned * PILLAR_HEIGHT_MAX;
      if (pilArray[pilID].colour[paneID] & 0x10)
        break;
    }
    if (j == PILLAR_HEIGHT_MAX + puzHeight)
      return false;   // No matching colour for row i.
  }
  
  // Column.
  for (i = 0; i < puzBreadth; i++) {
    for (j = 0; j < puzHeight; j++) {
      paneID = j + pilArray[i].turned * PILLAR_HEIGHT_MAX;
      if (pilArray[i].colour[paneID] & 0x10)
        break;
    }
    if (j == puzHeight)
      return false;   // No matching colour for column i.
  }
  
  return true;
}

// Draws sprites highlighting matching panes in rows and columns.
const void puzzleDrawMatch(int timer) {
  int bld, i, j;
  const COLOR clrs[4] = {0x3E0, 0x3E0, 0x3E0, 0x3E0};   // Green. As green as it gets.
  COLOR bldClrs[4];
  s16 x, y, xPane, yPane;
  OBJ_ATTR obj = {0x8000, 0, 0, 0};
  
  if ((puzDispOptions & 0xF0) == PUZDISP_DISABLE)
    return;
  
  // Blend palettes.
  if ((puzDispOptions & 0xF0) == PUZDISP_MATCH1) {
    bld = ease(0, 0x1F, ABS(15 - (timer % 31)), 15, EASE_IN_QUADRATIC);
    clr_blend(&systemPal[3], clrs, bldClrs, 4, bld);
    loadColours(bldClrs, 291, 4);
    setSyncPalFlagsByID(18);
  }
  
  // Draw pane sprites.
  for (i = 0; i < puzLength * puzBreadth; i++) {
    if (!pilCalcCoords(&pilArray[i], &x, &y))
      continue;     // Offscreen; Don't draw.
    if (pilArray[i].animID != PIL_ANIM_IDLE)
      continue;     // Don't draw on moving or hidden pillars.
    for (j = 0; j < puzHeight; j++) {
      if (pilArray[i].colour[j] & 0x10) {
        // Match, draw highlight pane.
        // Adjust position.
        yPane = y + 56 + ((j - puzHeight) << 3);
        xPane = x + 8;
        
        // Adjust if turned.
        obj.attr1 &= 0xEFFF;
        if (pilArray[i].turned) {
          obj.attr1 |= ATTR1_HFLIP;
          xPane += 8;
        }
        
        // Add coordinates.
        obj.attr0 &= 0xFF00;
        obj.attr0 |= (yPane & ATTR0_Y_MASK);
        obj.attr1 &= 0xFE00;
        obj.attr1 |= (xPane & ATTR1_X_MASK);
        
        if ((puzDispOptions & 0xF0) == PUZDISP_MATCH1)
          obj.attr2 = 0x2382 + ((pilArray[i].colour[j] & 3) << 1);    // Grab correct colour.
        else //if ((puzDispOptions & 0xF0) == PUZDISP_MATCH2)
          obj.attr2 = 0x038A;
        
        // Draw to screen.
        // Pillars are 64 pixels tall, hence why we add 63.
        addToOAMBuffer(&obj, VERT_OBJ_LAYER(y + 63));
      }
      if (pilArray[i].colour[j + PILLAR_HEIGHT_MAX] & 0x10) {
        // Match, draw highlight pane.
        // Adjust position.
        yPane = y + 56 + ((j - puzHeight) << 3);
        xPane = x + 8;
        
        // Adjust if turned.
        obj.attr1 &= 0xEFFF;
        if (!pilArray[i].turned) {
          obj.attr1 |= ATTR1_HFLIP;
          xPane += 8;
        }
        
        // Add coordinates.
        obj.attr0 &= 0xFF00;
        obj.attr0 |= (yPane & ATTR0_Y_MASK);
        obj.attr1 &= 0xFE00;
        obj.attr1 |= (xPane & ATTR1_X_MASK);
        
        if ((puzDispOptions & 0xF0) == PUZDISP_MATCH1)
          obj.attr2 = 0x2382 + ((pilArray[i].colour[j + PILLAR_HEIGHT_MAX] & 3) << 1);  // Grab correct colour.
        else //if ((puzDispOptions & 0xF0) == PUZDISP_MATCH2)
          obj.attr2 = 0x038A;
        
        // Draw to screen.
        // Pillars are 64 pixels tall, hence why we add 63.
        addToOAMBuffer(&obj, VERT_OBJ_LAYER(y + 63));
      }
    }
  }
}

// Highlight given pillar using alternate palette.
const void puzzleDrawHighlight(struct Pillar* pil, int timer) {
  int palID, bld, layer;
  const COLOR clrs1[1] = {0x7C1F};        // Magenta. Max red and blue, min green.
  const COLOR clrs2[7] = {CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK};
  COLOR bldClrs[7];
  s16 x, y;
  OBJ_ATTR obj = {0, 0, 0, 0};
  
  if ((puzDispOptions & 0xF) == PUZDISP_DISABLE)
    return;
  
  // Only draw on idle pillars.
  if (pil->animID != PIL_ANIM_IDLE)
    return;
  
  if (!pilCalcCoords(pil, &x, &y))
    return;       // Offscreen; Don't draw.
  
  layer = VERT_OBJ_LAYER(y + 63);   // Pillars are 64 pixels tall, hence why we add 63.
  
  // Build object based on spriteData member.
  obj_copy(&obj, &pil->spriteData->obj, 1);
  
  // Blend palette.
  bld = ease(0, 0x1F, ABS(15 - (timer % 31)), 15, EASE_IN_QUADRATIC);
  if ((puzDispOptions & 0xF) == PUZDISP_HIGHLIGHT1) {             // Blend entire pillar periodically.
    
    // Set attributes.
    obj.attr0 = (obj.attr0 & 0xFF00) | ((obj.attr0 + y) & ATTR0_Y_MASK);
    obj.attr1 = (obj.attr1 & 0xFE00) | ((obj.attr1 + x) & ATTR1_X_MASK);
    obj.attr2 = 0x1000 | (((pil->id - 1) << 5) & ATTR2_ID_MASK);
    
    // Mask with pillar specific attributes (no coordinates!)
    obj.attr0 |= pil->objMask.attr0;
    obj.attr1 |= pil->objMask.attr1;
    obj.attr2 |= pil->objMask.attr2;
    
    // Blend entire pillar.
    clr_blend(systemPal, clrs2, bldClrs, 7, bld);
    loadColours(bldClrs, 17 << 4, 7);
    
  } else if ((puzDispOptions & 0xF) == PUZDISP_HIGHLIGHT2 ||      // Blend top of pillar periodically.
             (puzDispOptions & 0xF) == PUZDISP_HIGHLIGHT3) {      // Set top of pillar to highlight colour.
    
    // Calculate coordinates.
    y += 48 + (obj.attr0 & ATTR0_Y_MASK) - (pil->height << 3);
    x += 8;
    
    // Set attributes.
    obj.attr0 = y & ATTR0_Y_MASK;
    obj.attr1 = ATTR1_SIZE_16 | (x & ATTR1_X_MASK);
    obj.attr2 = 0x138C;
    
    // Blend top of pillar.
    clr_blend(systemPal+1, clrs1, bldClrs, 1, bld);
    setColour(bldClrs[0], (17 << 4) + 1);
    
  }
  
  // Draw to screen.
  addToOAMBuffer(&obj, layer);
  
  // Update blended palette, unless it's PUZDISP_HIGHLIGHT3.
  if ((puzDispOptions & 0xF) != PUZDISP_HIGHLIGHT3)
    setSyncPalFlagsByID(17);
}

// Creates a puzzle of given dimensions.
const void puzzleGenerate() {
  struct Pillar pil;
  int row, col, i, j, matchHeight, matchColour, id;
  int size = puzLength * puzBreadth;
  u8 clrs[puzLength][puzBreadth][6];
  u8 turnpillars[size];
  const int sampleSizeRange[PUZZLE_LENGTH_MAX * PUZZLE_BREADTH_MAX][2] = {
    {0, 0}, {0, 0}, {0, 0}, {2, 2},
    {0, 0}, {2, 4}, {0, 0}, {2, 6},
    {2, 7}, {0, 0}, {0, 0}, {3, 9},
    {0, 0}, {0, 0}, {0, 0}, {4, 12}
  };
  int sampleSize = qran_range(sampleSizeRange[size-1][0], sampleSizeRange[size-1][1]+1);
  
  // Repeat until unsolved puzzle is generated.
  while (true) {
  
    // Randomize pillar colours.
    for (row = 0; row < puzLength; row++)
      for (col = 0; col < puzBreadth; col++)
        pilGenerateRandColours(clrs[row][col]);
    
    // Set specific colours to match for rows.
    for (row = 0; row < puzLength; row++) {
      matchHeight = qran_range(PUZZLE_HEIGHT_MIN, puzHeight+1);
      matchColour = qran_range(0, 4);
      
      for (col = 0; col < puzBreadth; col++)
        clrs[row][col][matchHeight + 2] = matchColour;
    }
    
    // Repeat for columns.
    for (col = 0; col < puzBreadth; col++) {
      matchHeight = qran_range(PUZZLE_HEIGHT_MIN, puzHeight+1);
      matchColour = qran_range(0, 4);
      
      for (row = 0; row < puzLength; row++)
        clrs[row][col][matchHeight - 1] = matchColour;
    }
    
    // Create pillars.
    initPilArray(0, 0);
    for (row = 0; row < puzLength; row++) {
      for (col = 0; col < puzBreadth; col++) {
        pil = pilConstr(clrs[row][col], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
        id = addPilToPilArray(&pil);
        pilArray[id].height = puzHeight;
        pilSetAnim(&pilArray[id], PIL_ANIM_IDLE);
        
        // Set neighbouring pillar pointers.
        pilArray[id].left = (col == 0 && row == 0) ? &pilArray[puzLength * puzBreadth - 1] : &pilArray[id - 1];
        pilArray[id].right = (col == puzBreadth - 1 && row == puzLength - 1) ? &pilArray[0] : &pilArray[id + 1];
        pilArray[id].up = (id < puzBreadth) ? &pilArray[puzBreadth * puzLength - ((puzBreadth - id) % puzBreadth) - 1] : &pilArray[id - puzBreadth];
        pilArray[id].down = (id >= puzBreadth * (puzLength - 1)) ? &pilArray[(id + 1 - puzBreadth * (puzLength - 1)) % puzBreadth] : &pilArray[id + puzBreadth];
        
        // LeftUp.
        if (row == puzLength - 1 && col == 0)
          pilArray[id].leftUp = &pilArray[puzBreadth - 1];
        else if (col == 0)
          pilArray[id].leftUp = &pilArray[id + puzBreadth + min(puzBreadth - 1, puzLength - row - 2) * (puzBreadth + 1)];
        else if (row == 0)
          pilArray[id].leftUp = &pilArray[id - 1 +       min(puzLength - 1,  puzBreadth - col)    * (puzBreadth + 1)];
        else
          pilArray[id].leftUp = &pilArray[id - puzBreadth - 1];
        
        // LeftDown.
        if (row == 0 && col == 0)
          pilArray[id].leftDown = &pilArray[puzBreadth * puzLength - 1];
        else if (col == 0)
          pilArray[id].leftDown = &pilArray[id - puzBreadth - min(puzBreadth - 1, row - 1)  * (puzBreadth - 1)];
        else if (row == puzLength - 1)
          pilArray[id].leftDown = &pilArray[id - puzBreadth - min(puzLength - 2,  puzBreadth - col - 1) * (puzBreadth - 1)];
        else
          pilArray[id].leftDown = &pilArray[id + puzBreadth - 1];
        
        // RightUp.
        if (col == puzBreadth - 1 && row == puzLength - 1)
          pilArray[id].rightUp = &pilArray[0];
        else if (col == puzBreadth - 1)
          pilArray[id].rightUp = &pilArray[id + puzBreadth + min(puzBreadth - 1, puzLength - row - 2) * (puzBreadth - 1)];
        else if (row == 0)
          pilArray[id].rightUp = &pilArray[id + puzBreadth + min(puzLength - 2,  col)              * (puzBreadth - 1)];
        else
          pilArray[id].rightUp = &pilArray[id - puzBreadth + 1];
        
        // RightDown.
        if (col == puzBreadth - 1 && row == 0)
          pilArray[id].rightDown = &pilArray[puzBreadth * (puzLength - 1)];
        else if (row == puzLength - 1)
          pilArray[id].rightDown = &pilArray[id - puzBreadth - min(puzLength - 2,  col)     * (puzBreadth + 1)];
        else if (col == puzBreadth - 1)
          pilArray[id].rightDown = &pilArray[id - puzBreadth - min(puzBreadth - 1, row - 1) * (puzBreadth + 1)];
        else
          pilArray[id].rightDown = &pilArray[id + puzBreadth + 1];
      }
    }
    
    // Turn a sample of pillars.
    /*
    for (i = 0; i < size; i++)
      turnpillars[i] = i;
    j = sampleSize;
    for (i = 0; i < sampleSize; i++) {
      id = turnpillars[qran_range(0, j)];
      pilArray[id].turned = true;
      turnpillars[i] = turnpillars[j];
      j--;
    }
    */
    
    // Create a new puzzle if puzzle is already solved.
    for (row = 0; row < puzLength; row++)
      puzzleMatchRowOrCol(row, false);
    for (col = 0; col < puzBreadth; col++)
      puzzleMatchRowOrCol(col, true);
    //if (!puzzleIsSolved())
    return;
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
    loadColours((COLOR*)systemPal, 272, systemPalLen>>1);
    if ((puzDispOptions & 0xF) == PUZDISP_HIGHLIGHT3)
      setColour(0x7C1F, 273);         // Magenta. Max red and blue, min green.
    setSyncPalFlagsByID(17);
    puzzleGenerate();
    pilCamX = -104;
    pilCamY = 160;
  } else if (gStateClock < 45) {
    pilCamY = ease(160, 14, gStateClock, 45, EASE_OUT_QUADRATIC);
  } else {
    pilCamY = 14;
    
    selPil = &pilArray[0];
    
    setGameState(gGameState, PUZZLE_IDLE);
    gStateClock = 14;
    puzSolveStatus = false;
  }
  
  pilRunAnims();
  pilDrawAll();
  gStateClock++;
}

const void puzzleIdle() {
  int moved;
  
  // Only handle input if selected pillar is not still moving.
  if (selPil->animID == PIL_ANIM_IDLE) {
    
    if (puzSolveStatus) {
      puzzleMatchRowOrCol((selPil->id - 1) / puzBreadth, false);
      puzzleMatchRowOrCol((selPil->id - 1) % puzBreadth, true);
      puzSolveStatus = false;
    }
  
    // Handle fire button input.
    if (key_hit(KEY_A)) {
      pilSetAnim(selPil, PIL_ANIM_TURN);
      puzSolveStatus = true;        // Update solved status after pillar finished turning.
    } else if (key_hit(KEY_B)) {
      ; // TODO
    } else {
    
      // Handle D-pad input.
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
    }
  }
  
  pilRunAnims();
  pilDrawAll();
  puzzleDrawMatch(gStateClock);
  puzzleDrawHighlight(selPil, gStateClock);
  
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