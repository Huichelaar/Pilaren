#include <stdio.h>
#include <tonc.h>
#include "efx.h"
#include "lcdiobuffer.h"
#include "menu.h"
#include "main.h"
#include "pillar.h"
#include "guide.h"
#include "gfx/system.h"
#include "gfx/pil.h"
#include "puzzle.h"
#include "video.h"

EWRAM_DATA s16 initPilCamY;
EWRAM_DATA s16 initBGOfsY;
EWRAM_DATA s16 initBGOfsY2;
EWRAM_DATA struct Puzzle puzzle[2];
EWRAM_DATA u8 puzDispOptions;

/* Sets upper nybble of all pillar->colour panes/elements in given pillar's
   row or column of puzzle array if pane is matched in row or column.
   Unsets otherwise.
   puzzleIsSolved can check these for each row and column to confirm
   if puzzle has been solved.
   
   id: id of pillar which' row or column will be matched.
   col: True if column, false if row.*/
const void puzMatchRowOrCol(int id, int col) {
  int i, j, pilID, paneID1, paneID2;
  
  if (col) {
    // Column.
    for (i = 0; i < PIL_HEIGHT_MAX; i++) {            // i = pane iterator.
      for (j = 0; j < puzzle[0].length - 1; j++) {    // j = row-element iterator.
        pilID = id + j * puzzle[0].breadth;
        paneID1 = i + puzzle[0].pil[pilID].turned                     * PIL_HEIGHT_MAX;
        paneID2 = i + puzzle[0].pil[pilID + puzzle[0].breadth].turned * PIL_HEIGHT_MAX;
        if ((puzzle[0].pil[pilID].colour[paneID1] & 0x3) != (puzzle[0].pil[pilID + puzzle[0].breadth].colour[paneID2] & 0x3))
          break;
      }
      if (j == puzzle[0].length - 1) {
        // Column matches. Set match nybble in all of column's pillars.
        for (j = 0; j < puzzle[0].length; j++) {
          pilID = id + j * puzzle[0].breadth;
          paneID1 = i + puzzle[0].pil[pilID].turned * PIL_HEIGHT_MAX;
          puzzle[0].pil[pilID].colour[paneID1] |= 0x10;
        }
      } else {
        // Column doesn't match. Clear match nybble in all of column's pillars.
        for (j = 0; j < puzzle[0].length; j++) {
          pilID = id + j * puzzle[0].breadth;
          paneID1 = i + puzzle[0].pil[pilID].turned * PIL_HEIGHT_MAX;
          puzzle[0].pil[pilID].colour[paneID1] &= ~0x10;
        }
      }
    }
  } else {
    // Row.
    for (i = PIL_HEIGHT_MAX; i < PIL_HEIGHT_MAX * 2; i++) {     // i = pane iterator.
      for (j = 0; j < puzzle[0].breadth - 1; j++) {                          // j = row-element iterator.
        pilID = j + id * puzzle[0].breadth;
        paneID1 = i - puzzle[0].pil[pilID].turned     * PIL_HEIGHT_MAX;
        paneID2 = i - puzzle[0].pil[pilID + 1].turned * PIL_HEIGHT_MAX;
        if ((puzzle[0].pil[pilID].colour[paneID1] & 0x3) != (puzzle[0].pil[pilID + 1].colour[paneID2] & 0x3))
          break;
      }
      if (j == puzzle[0].breadth - 1) {
        // Row matches. Set match nybble in all of row's pillars.
        for (j = 0; j < puzzle[0].breadth; j++) {
          pilID = j + id * puzzle[0].breadth;
          paneID1 = i - puzzle[0].pil[pilID].turned * PIL_HEIGHT_MAX;
          puzzle[0].pil[pilID].colour[paneID1] |= 0x10;
        }
      } else {
        // Row doesn't match. Clear match nybble in all of row's pillars.
        for (j = 0; j < puzzle[0].breadth; j++) {
          pilID = j + id * puzzle[0].breadth;
          paneID1 = i - puzzle[0].pil[pilID].turned * PIL_HEIGHT_MAX;
          puzzle[0].pil[pilID].colour[paneID1] &= ~0x10;
        }
      }
    }
  }
}

// Checks if at least one pane matches in each row and column.
// If so, returns true; Puzzle is solved. Return false otherwise.
int puzIsSolved() {
  int i, j, pilID, paneID;
  
  // Row.
  for (i = 0; i < puzzle[0].length; i++) {
    for (j = PIL_HEIGHT_MAX; j < PIL_HEIGHT_MAX + puzzle[0].height; j++) {
      pilID = i * puzzle[0].breadth;
      paneID = j - puzzle[0].pil[pilID].turned * PIL_HEIGHT_MAX;
      if (puzzle[0].pil[pilID].colour[paneID] & 0x10)
        break;
    }
    if (j == PIL_HEIGHT_MAX + puzzle[0].height)
      return false;   // No matching colour for row i.
  }
  
  // Column.
  for (i = 0; i < puzzle[0].breadth; i++) {
    for (j = 0; j < puzzle[0].height; j++) {
      paneID = j + puzzle[0].pil[i].turned * PIL_HEIGHT_MAX;
      if (puzzle[0].pil[i].colour[paneID] & 0x10)
        break;
    }
    if (j == puzzle[0].height)
      return false;   // No matching colour for column i.
  }
  
  return true;
}

// Draw all pillars in pillar array.
const void puzDrawAll(int puzID) {
  struct Pillar* pil;
  s16 x, y;
  OBJ_ATTR obj = {0, 0, 0, 0};
  
  for (int i = 0; i < PUZ_ARRAY_MAX; i++) {
    if (!puzzle[puzID].pil[i].id)
      continue;
    
    pil = &puzzle[puzID].pil[i];
    
    // Don't draw hidden pillars.
    if (pil->animID == PIL_ANIM_HIDDEN)
      continue;
    
    if (!pilCalcCoords(pil, &x, &y, puzzle[puzID].camX, puzzle[puzID].camY))
      continue;     // Offscreen; Don't draw.
    
    // Build object based on spriteData member.
    obj_copy(&obj, &pil->spriteData->obj, 1);
    
    // Add coordinates.
    obj.attr0 = (obj.attr0 & 0xFF00) | ((obj.attr0 + y) & ATTR0_Y_MASK);
    obj.attr1 = (obj.attr1 & 0xFE00) | ((obj.attr1 + x) & ATTR1_X_MASK);
    
    // Set tile.
    obj.attr2 |= ((i << 5) & ATTR2_ID_MASK);
    
    // Mask with pillar specific attributes (no coordinates!)
    obj.attr0 |= pil->objMask.attr0;
    obj.attr1 |= pil->objMask.attr1;
    obj.attr2 |= pil->objMask.attr2;
    
    // Draw to screen.
    // Pillars are 64 pixels tall, hence why we add 63.
    addToOAMBuffer(&obj, VERT_OBJ_LAYER(y + 63));
  }
}

// Draws sprites highlighting matching panes in rows and columns.
const void puzDrawMatch(int puzID, int timer) {
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
  for (i = 0; i < puzzle[puzID].length * puzzle[puzID].breadth; i++) {
    if (!pilCalcCoords(&puzzle[puzID].pil[i], &x, &y, puzzle[puzID].camX, puzzle[puzID].camY))
      continue;     // Offscreen; Don't draw.
    if (puzzle[puzID].pil[i].animID != PIL_ANIM_IDLE)
      continue;     // Don't draw on moving or hidden pillars.
    for (j = 0; j < puzzle[puzID].height; j++) {
      if (puzzle[puzID].pil[i].colour[j] & 0x10) {
        // Match, draw highlight pane.
        // Adjust position.
        yPane = y + 56 + ((j - puzzle[puzID].height) << 3);
        xPane = x + 8;
        
        // Adjust if turned.
        obj.attr1 &= 0xEFFF;
        if (puzzle[puzID].pil[i].turned) {
          obj.attr1 |= ATTR1_HFLIP;
          xPane += 8;
        }
        
        // Add coordinates.
        obj.attr0 &= 0xFF00;
        obj.attr0 |= (yPane & ATTR0_Y_MASK);
        obj.attr1 &= 0xFE00;
        obj.attr1 |= (xPane & ATTR1_X_MASK);
        
        if ((puzDispOptions & 0xF0) == PUZDISP_MATCH1)
          obj.attr2 = 0x2382 + ((puzzle[puzID].pil[i].colour[j] & 3) << 1);    // Grab correct colour.
        else //if ((puzDispOptions & 0xF0) == PUZDISP_MATCH2)
          obj.attr2 = 0x038A;
        
        // Draw to screen.
        // Pillars are 64 pixels tall, hence why we add 63.
        addToOAMBuffer(&obj, VERT_OBJ_LAYER(y + 63));
      }
      if (puzzle[puzID].pil[i].colour[j + PIL_HEIGHT_MAX] & 0x10) {
        // Match, draw highlight pane.
        // Adjust position.
        yPane = y + 56 + ((j - puzzle[puzID].height) << 3);
        xPane = x + 8;
        
        // Adjust if turned.
        obj.attr1 &= 0xEFFF;
        if (!puzzle[puzID].pil[i].turned) {
          obj.attr1 |= ATTR1_HFLIP;
          xPane += 8;
        }
        
        // Add coordinates.
        obj.attr0 &= 0xFF00;
        obj.attr0 |= (yPane & ATTR0_Y_MASK);
        obj.attr1 &= 0xFE00;
        obj.attr1 |= (xPane & ATTR1_X_MASK);
        
        if ((puzDispOptions & 0xF0) == PUZDISP_MATCH1)
          obj.attr2 = 0x2382 + ((puzzle[puzID].pil[i].colour[j + PIL_HEIGHT_MAX] & 3) << 1);  // Grab correct colour.
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
const void puzDrawHighlight(int puzID, struct Pillar* pil, int timer) {
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
  
  if (!pilCalcCoords(pil, &x, &y, puzzle[puzID].camX, puzzle[puzID].camY))
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

// Add pillar to puzzle. Return pilID in puzzle if success.
// Return -1 if pillar could not be added to puzzle.
int puzAddPil(int puzID, struct Pillar* pil) {
  if (puzzle[puzID].pilCounter >= PUZ_ARRAY_MAX)
    return -1;
  
  for (int i = 0; i < PUZ_ARRAY_MAX; i++) {
    if (puzzle[puzID].pil[i].id != 0)
      continue;
    
    // Copy pillar to array entry.
    puzzle[puzID].pil[i].id = i+1;
    
    for (int j = 0; j < 6; j++) { puzzle[puzID].pil[i].colour[j] = pil->colour[j]; }
    puzzle[puzID].pil[i].turned = pil->turned;
    puzzle[puzID].pil[i].animID = pil->animID;
    puzzle[puzID].pil[i].animTimer = pil->animTimer;
    puzzle[puzID].pil[i].height = pil->height;
    puzzle[puzID].pil[i].updateTiles = pil->updateTiles;
    
    puzzle[puzID].pil[i].x = pil->x;
    puzzle[puzID].pil[i].y = pil->y;
    puzzle[puzID].pil[i].spriteData = pil->spriteData;
    puzzle[puzID].pil[i].objMask = pil->objMask;
    
    puzzle[puzID].pil[i].left = pil->left;
    puzzle[puzID].pil[i].right = pil->right;
    puzzle[puzID].pil[i].up = pil->up;
    puzzle[puzID].pil[i].down = pil->down;
    
    puzzle[puzID].pilCounter++;
    
    return i;
  }
  
  return -1;              // return -1 if no empty slot in array was found.
}

// Creates a puzzle of given dimensions.
const void puzGenerate() {
  struct Pillar pil;
  int row, col, i, j, matchHeight, matchColour, id;
  int size = puzzle[0].length * puzzle[0].breadth;
  u8 clrs[puzzle[0].length][puzzle[0].breadth][6];
  u8 turnpillars[size];
  const int sampleSizeRange[PUZ_LENGTH_MAX * PUZ_BREADTH_MAX][2] = {
    {0, 0}, {0, 0}, {0, 0}, {2, 2},
    {0, 0}, {2, 4}, {0, 0}, {2, 6},
    {2, 7}, {0, 0}, {0, 0}, {3, 9},
    {0, 0}, {0, 0}, {0, 0}, {4, 12}
  };
  int sampleSize = qran_range(sampleSizeRange[size-1][0], sampleSizeRange[size-1][1]+1);
  
  // Repeat until unsolved puzzle is generated.
  while (true) {
  
    // Randomize pillar colours.
    for (row = 0; row < puzzle[0].length; row++)
      for (col = 0; col < puzzle[0].breadth; col++)
        pilGenerateRandColours(clrs[row][col]);
    
    // Set specific colours to match for rows.
    for (row = 0; row < puzzle[0].length; row++) {
      matchHeight = qran_range(PUZ_HEIGHT_MIN, puzzle[0].height+1);
      matchColour = qran_range(0, 4);
      
      for (col = 0; col < puzzle[0].breadth; col++)
        clrs[row][col][matchHeight + 2] = matchColour;
    }
    
    // Repeat for columns.
    for (col = 0; col < puzzle[0].breadth; col++) {
      matchHeight = qran_range(PUZ_HEIGHT_MIN, puzzle[0].height+1);
      matchColour = qran_range(0, 4);
      
      for (row = 0; row < puzzle[0].length; row++)
        clrs[row][col][matchHeight - 1] = matchColour;
    }
    
    // Create pillars.
    puzInitPil(0, 0, 0);
    for (row = 0; row < puzzle[0].length; row++) {
      for (col = 0; col < puzzle[0].breadth; col++) {
        pil = pilConstr(clrs[row][col], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
        id = puzAddPil(0, &pil);
        puzzle[0].pil[id].height = puzzle[0].height;
        pilSetAnim(&puzzle[0].pil[id], PIL_ANIM_IDLE);
        
        // Set neighbouring pillar pointers.
        puzzle[0].pil[id].left = (col == 0 && row == 0) ? &puzzle[0].pil[puzzle[0].length * puzzle[0].breadth - 1] : &puzzle[0].pil[id - 1];
        puzzle[0].pil[id].right = (col == puzzle[0].breadth - 1 && row == puzzle[0].length - 1) ? &puzzle[0].pil[0] : &puzzle[0].pil[id + 1];
        puzzle[0].pil[id].up = (id < puzzle[0].breadth) ? &puzzle[0].pil[puzzle[0].breadth * puzzle[0].length - ((puzzle[0].breadth - id) % puzzle[0].breadth) - 1] : &puzzle[0].pil[id - puzzle[0].breadth];
        puzzle[0].pil[id].down = (id >= puzzle[0].breadth * (puzzle[0].length - 1)) ? &puzzle[0].pil[(id + 1 - puzzle[0].breadth * (puzzle[0].length - 1)) % puzzle[0].breadth] : &puzzle[0].pil[id + puzzle[0].breadth];
        
        // LeftUp.
        if (row == puzzle[0].length - 1 && col == 0)
          puzzle[0].pil[id].leftUp = &puzzle[0].pil[puzzle[0].breadth - 1];
        else if (col == 0)
          puzzle[0].pil[id].leftUp = &puzzle[0].pil[id + puzzle[0].breadth + min(puzzle[0].breadth - 1, puzzle[0].length - row - 2) * (puzzle[0].breadth + 1)];
        else if (row == 0)
          puzzle[0].pil[id].leftUp = &puzzle[0].pil[id - 1 +       min(puzzle[0].length - 1,  puzzle[0].breadth - col)    * (puzzle[0].breadth + 1)];
        else
          puzzle[0].pil[id].leftUp = &puzzle[0].pil[id - puzzle[0].breadth - 1];
        
        // LeftDown.
        if (row == 0 && col == 0)
          puzzle[0].pil[id].leftDown = &puzzle[0].pil[puzzle[0].breadth * puzzle[0].length - 1];
        else if (col == 0)
          puzzle[0].pil[id].leftDown = &puzzle[0].pil[id - puzzle[0].breadth - min(puzzle[0].breadth - 1, row - 1)  * (puzzle[0].breadth - 1)];
        else if (row == puzzle[0].length - 1)
          puzzle[0].pil[id].leftDown = &puzzle[0].pil[id - puzzle[0].breadth - min(puzzle[0].length - 2,  puzzle[0].breadth - col - 1) * (puzzle[0].breadth - 1)];
        else
          puzzle[0].pil[id].leftDown = &puzzle[0].pil[id + puzzle[0].breadth - 1];
        
        // RightUp.
        if (col == puzzle[0].breadth - 1 && row == puzzle[0].length - 1)
          puzzle[0].pil[id].rightUp = &puzzle[0].pil[0];
        else if (col == puzzle[0].breadth - 1)
          puzzle[0].pil[id].rightUp = &puzzle[0].pil[id + puzzle[0].breadth + min(puzzle[0].breadth - 1, puzzle[0].length - row - 2) * (puzzle[0].breadth - 1)];
        else if (row == 0)
          puzzle[0].pil[id].rightUp = &puzzle[0].pil[id + puzzle[0].breadth + min(puzzle[0].length - 2,  col)              * (puzzle[0].breadth - 1)];
        else
          puzzle[0].pil[id].rightUp = &puzzle[0].pil[id - puzzle[0].breadth + 1];
        
        // RightDown.
        if (col == puzzle[0].breadth - 1 && row == 0)
          puzzle[0].pil[id].rightDown = &puzzle[0].pil[puzzle[0].breadth * (puzzle[0].length - 1)];
        else if (row == puzzle[0].length - 1)
          puzzle[0].pil[id].rightDown = &puzzle[0].pil[id - puzzle[0].breadth - min(puzzle[0].length - 2,  col)     * (puzzle[0].breadth + 1)];
        else if (col == puzzle[0].breadth - 1)
          puzzle[0].pil[id].rightDown = &puzzle[0].pil[id - puzzle[0].breadth - min(puzzle[0].breadth - 1, row - 1) * (puzzle[0].breadth + 1)];
        else
          puzzle[0].pil[id].rightDown = &puzzle[0].pil[id + puzzle[0].breadth + 1];
      }
    }
    
    // Turn a sample of pillars.
    /*
    for (i = 0; i < size; i++)
      turnpillars[i] = i;
    j = sampleSize;
    for (i = 0; i < sampleSize; i++) {
      id = turnpillars[qran_range(0, j)];
      puzzle[0].pil[id].turned = true;
      turnpillars[i] = turnpillars[j];
      j--;
    }
    */
    
    // Create a new puzzle if puzzle is already solved.
    for (row = 0; row < puzzle[0].length; row++)
      puzMatchRowOrCol(row, false);
    for (col = 0; col < puzzle[0].breadth; col++)
      puzMatchRowOrCol(col, true);
    //if (!puzIsSolved())
    return;
  }
}

const void puzInit() {
  menuClear();
  
  initPilCamY = puzzle[0].camY;
  initBGOfsY = lcdioBuffer.bg1vofs;
  initBGOfsY2 = lcdioBuffer.bg3vofs;
  
  setGameState(GAME_PUZZLE, PUZZLE_TRANSITION1);
}

const void puzInitPil(int puzID, int x, int y) {
  for (int i = 0; i < PUZ_ARRAY_MAX; i++) {
    puzzle[puzID].pil[i].id = 0;
  }
  puzzle[puzID].pilCounter = 0;
  puzzle[puzID].camX = x;
  puzzle[puzID].camY = y;
}

// Randomly pick a pillar and animate it.
const void puzAnimRand(int puzID, int freq) {
  
  // Only animate every freq frames.
  if (gClock % freq)
    return;
  
  const int id = qran_range(0, PUZ_ARRAY_MAX);
  int animID = qran_range(PIL_ANIM_RAISE, PIL_ANIM_TURN+1);
  
  // Don't animate pillars that are already animating.
  if (puzzle[puzID].pil[id].animID != PIL_ANIM_IDLE)
    return;
  
  if (animID == PIL_ANIM_RAISE && puzzle[puzID].pil[id].height == PIL_HEIGHT_MAX)
    animID = PIL_ANIM_LOWER;
  else if (animID == PIL_ANIM_LOWER && puzzle[puzID].pil[id].height == 0)
    animID = PIL_ANIM_RAISE;
  
  pilSetAnim(&puzzle[puzID].pil[id], animID);
}

// Reload tiles in case pillars had been replaced.
const void puzReloadTiles(int puzID) {
  for (int i = 0; i < PUZ_ARRAY_MAX; i++) {
    if (!puzzle[puzID].pil[i].id)
      continue;
    
    puzzle[puzID].pil[i].updateTiles = true;
  }
}

// Continues animations of all pillars in puzzle.
// If TURN or RAISE animations finish, set to IDLE.
const void puzRunAnims(int puzID) {
  struct Pillar* pil;
  for (int i = 0; i < PUZ_ARRAY_MAX; i++) {
    if (!puzzle[puzID].pil[i].id)
      continue;
    
    pil = &puzzle[puzID].pil[i];
    
    switch (pil->animID) {
        
      case PIL_ANIM_RAISE:
        if (pil->animTimer >= pil->spriteData->obj.fill) {
          if (pil->spriteData->nextFrame != NULL) {
            // Move to next animation frame.
            pil->spriteData = pil->spriteData->nextFrame;
            pil->animTimer = 0;
            pil->updateTiles = true;
          } else {
            // If that was the last frame, return to idle.
            // Also increment height.
            pil->height += 1;
            pilSetAnim(&puzzle[puzID].pil[i], PIL_ANIM_IDLE);
          }
        } else {
          pil->animTimer++;
        }
        break;
      
      case PIL_ANIM_LOWER:
        if (pil->animTimer >= pil->spriteData->obj.fill) {
          if (pil->spriteData->nextFrame != NULL) {
            // Move to next animation frame.
            pil->spriteData = pil->spriteData->nextFrame;
            pil->animTimer = 0;
            pil->updateTiles = true;
          } else {
            // If that was the last frame, return to idle.
            // Also decrement height.
            pil->height -= 1;
            pilSetAnim(&puzzle[puzID].pil[i], PIL_ANIM_IDLE);
          }
        } else {
          pil->animTimer++;
        }
        break;
      
      case PIL_ANIM_TURN:
        if (pil->animTimer >= pil->spriteData->obj.fill) {
          if (pil->spriteData->nextFrame != NULL) {
            // Move to next animation frame.
            pil->spriteData = pil->spriteData->nextFrame;
            pil->animTimer = 0;
            pil->updateTiles = true;
          } else {
            // If that was the last frame, return to idle.
            // Also flip turn parameter.
            pil->turned = !pil->turned;
            pilSetAnim(&puzzle[puzID].pil[i], PIL_ANIM_IDLE);
          }
        } else {
          pil->animTimer++;
        }
      case PIL_ANIM_HIDDEN:
      case PIL_ANIM_IDLE:
      default:
        break;
    }
    
    // Load new tiles.
    if (pil->updateTiles) {
      pilLoadTiles(pil, (int)(MEM_VRAM_OBJ + (i << 10)));
      pil->updateTiles = false;
    }
  }
}

const void puzTransition1() {
  int yOffs, pilID, i, j;
  int loBndX, hiBndX, loBndY, hiBndY;
  u8 clrs[6];
  struct Pillar pil;
  const int pilCount = PUZ_ARRAY_MAX;
  
  if (gStateClock < 60) {
    // Move camera away from title screen.
    yOffs = ease(0, 160, gStateClock, 60, EASE_IN_QUADRATIC);
    puzzle[0].camY = initPilCamY - yOffs;
    lcdioBuffer.bg1vofs = initBGOfsY - yOffs;
    lcdioBuffer.bg2vofs = initBGOfsY - yOffs;
    lcdioBuffer.bg3vofs = initBGOfsY2 - yOffs;
    
    puzAnimRand(0, 60);
    puzRunAnims(0);
    puzDrawAll(0);
    
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
    puzInitPil(1, -104, 160);
    
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
      pilID = puzAddPil(1, &pil);
      puzzle[1].pil[pilID].height = qran_range(0, 4);
      
      // Unhide pillar if it doesn't overlap with any other pillars.
      for (j = 0; j < i; j++) {
        if (puzzle[1].pil[pilID].x == puzzle[1].pil[j].x &&
            puzzle[1].pil[pilID].y == puzzle[1].pil[j].y)
          break;
      }
      if (j == i)
        pilSetAnim(&puzzle[1].pil[pilID], PIL_ANIM_IDLE);      // Unhide pillar.
    }
    
  } else {
    puzzle[1].camY -= 4;
    
    // Move to next state if all flavour pillars have passed.
    for (i = 0; i < pilCount; i++) {
      if (puzzle[1].pil[i].id != 0) {
        yOffs = (puzzle[1].pil[i].x + puzzle[1].pil[i].y) - puzzle[1].camY;
        if (yOffs < SCREEN_HEIGHT)
          break;
      }
    }
    if (i == pilCount) {
      setGameState(gGameState, PUZZLE_TRANSITION2);
      return;
    }
    
    puzAnimRand(1, 60);
    puzRunAnims(1);
    puzDrawAll(1);
  }
  gStateClock++;
}

const void puzTransition2() {
  if (gStateClock == 0) {
    loadColours((COLOR*)systemPal, 272, systemPalLen>>1);
    if ((puzDispOptions & 0xF) == PUZDISP_HIGHLIGHT3)
      setColour(0x7C1F, 273);         // Magenta. Max red and blue, min green.
    setSyncPalFlagsByID(17);
    puzGenerate();
    puzzle[0].camX = -104;
    puzzle[0].camY = 160;
  } else if (gStateClock < 45) {
    puzzle[0].camY = ease(160, 14, gStateClock, 45, EASE_OUT_QUADRATIC);
  } else {
    puzzle[0].camY = 14;
    
    puzzle[0].selPil = &puzzle[0].pil[0];
    
    setGameState(gGameState, PUZZLE_IDLE);
    gStateClock = 14;
    puzzle[0].solveStatus = false;
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  gStateClock++;
}

const void puzIdle() {
  int moved;
  
  // Only handle input if selected pillar is not still moving.
  if (puzzle[0].selPil->animID == PIL_ANIM_IDLE) {
    
    if (puzzle[0].solveStatus) {
      puzMatchRowOrCol((puzzle[0].selPil->id - 1) / puzzle[0].breadth, false);
      puzMatchRowOrCol((puzzle[0].selPil->id - 1) % puzzle[0].breadth, true);
      puzzle[0].solveStatus = false;
    }
  
    // Handle fire button input.
    if (key_hit(KEY_A)) {
      pilSetAnim(puzzle[0].selPil, PIL_ANIM_TURN);
      puzzle[0].solveStatus = true;         // Update solved status after pillar finished turning.
    } else if (key_hit(KEY_B)) {
      ; // TODO
    } else if (key_hit(KEY_START)) {
      lcdioBuffer.dispcnt = 0;      // Disable all display.
      setGameState(GAME_GUIDE, GUIDE_START);
    } else {
    
      // Handle D-pad input.
      moved = moveCursorByInput((u32*)&puzzle[0].selPil,
                                (u32)puzzle[0].selPil->leftDown,
                                (u32)puzzle[0].selPil->rightUp,
                                (u32)puzzle[0].selPil->leftUp,
                                (u32)puzzle[0].selPil->rightDown,
                                (u32)puzzle[0].selPil->left,
                                (u32)puzzle[0].selPil->down,
                                (u32)puzzle[0].selPil->up,
                                (u32)puzzle[0].selPil->right);
      if (moved)
        gStateClock = 0;
        ; // TODO
    }
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawMatch(0, gStateClock);
  puzDrawHighlight(0, puzzle[0].selPil, gStateClock);
  
  gStateClock++;
}

const void puzUpdate() {
  switch(gGenericState) {
    case PUZZLE_TRANSITION1:
      puzTransition1();
      break;
    case PUZZLE_TRANSITION2:
      puzTransition2();
      break;
    case PUZZLE_IDLE:
      puzIdle();
      break;
  }
}