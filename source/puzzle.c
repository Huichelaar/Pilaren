#include <stdio.h>
#include <tonc.h>
#include "efx.h"
#include "lcdiobuffer.h"
#include "menu.h"
#include "main.h"
#include "lang.h"
#include "pillar.h"
#include "guide.h"
#include "options.h"
#include "video.h"
#include "title.h"
#include "gfx/system.h"
#include "gfx/pil.h"
#include "puzzle.h"


EWRAM_DATA s16 initCam;
EWRAM_DATA s16 initBGOfsY[3];
EWRAM_DATA u8 puzDispOptions;
EWRAM_DATA u8 puzPad5[3];
EWRAM_DATA struct Puzzle puzzle[2];


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
    obj.attr2 |= ((i << 5) & ATTR2_ID_MASK) | ATTR2_PRIO(1);
    
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
const void puzDrawMatch(int puzID, u8 puzDisp, int timer) {
  int bld, i, j, lMatch, rMatch;
  const COLOR clrs[4] = {0x3E0, 0x3E0, 0x3E0, 0x3E0};   // Green. As green as it gets.
  COLOR bldClrs[4];
  s16 x, y, xPane, yPane;
  OBJ_ATTR obj = {0x8000, 0, 0, 0};
  
  if (puzDisp & PUZDISP_MATCHDISABLE)
    return;
  
  // Blend palettes.
  if ((puzDisp & 0xF0) == PUZDISP_MATCH1) {
    bld = ease(0, 0x1F, ABS(31 - (timer % 63)), 31, EASE_IN_QUADRATIC);
    clr_blend(&systemPal[3], clrs, bldClrs, 4, bld);
    loadColours(bldClrs, 291, 4);
    setSyncPalFlagsByID(18);
  }
  
  // Draw pane sprites.
  for (i = 0; i < puzzle[puzID].length * puzzle[puzID].breadth; i++) {
    if (!pilCalcCoords(&puzzle[puzID].pil[i], &x, &y, puzzle[puzID].camX, puzzle[puzID].camY))
      continue;         // Offscreen; Don't draw.
    if (!puzIsPilValid(&puzzle[puzID].pil[i]))
      continue;         // Don't draw panes on invalid pillars.
    if (puzzle[puzID].pil[i].animID != PIL_ANIM_IDLE)
      continue;         // Don't draw on moving or hidden pillars.
    if (puzzle[puzID].pil[i].id == puzzle[puzID].selPil->id && puzzle[puzID].solveStatus)
      continue;         // Don't draw before matching row & column of selected pillar.
    lMatch = 0x2000;    // These are used to only show
    rMatch = 0x2000;    // the first matching pane of each side.
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
        
        if ((puzDisp & 0xF0) == PUZDISP_MATCH1) {
          obj.attr2 = 0x2382 + ((puzzle[puzID].pil[i].colour[j] & 3) << 1);    // Grab correct colour.
        } else if ((puzDisp & 0xF0) == PUZDISP_MATCH3) {
          obj.attr2 = 0x0382 + ((puzzle[puzID].pil[i].colour[j] & 3) << 1) + lMatch;
          lMatch = 0;
        } else if ((puzDisp & 0xF0) == PUZDISP_MATCH2)
          obj.attr2 = 0x038A;
        obj.attr2 |= ATTR2_PRIO(1);
        
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
        
        if ((puzDisp & 0xF0) == PUZDISP_MATCH1) {
          obj.attr2 = 0x2382 + ((puzzle[puzID].pil[i].colour[j + PIL_HEIGHT_MAX] & 3) << 1);
        } else if ((puzDisp & 0xF0) == PUZDISP_MATCH3) {
          obj.attr2 = 0x0382 + ((puzzle[puzID].pil[i].colour[j + PIL_HEIGHT_MAX] & 3) << 1) + rMatch;
          rMatch = 0;
        } else if ((puzDisp & 0xF0) == PUZDISP_MATCH2)
          obj.attr2 = 0x038A;
        obj.attr2 |= ATTR2_PRIO(1);
        
        // Draw to screen.
        // Pillars are 64 pixels tall, hence why we add 63.
        addToOAMBuffer(&obj, VERT_OBJ_LAYER(y + 63));
      }
    }
  }
}

// Highlight given pillar using alternate palette.
const void puzDrawCursor(int puzID, u8 puzDisp, struct Pillar* pil, int timer) {
  int bld, layer;
  const COLOR clrs1[1] = {0x7C1F};        // Magenta. Max red and blue, min green.
  const COLOR clrs2[7] = {CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK};
  COLOR bldClrs[7];
  s16 x, y;
  OBJ_ATTR obj = {0, 0, 0, 0};
  
  if (puzDisp & PUZDISP_CURSORDISABLE)
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
  bld = ease(0, 0x1F, ABS(31 - (timer % 63)), 31, EASE_IN_QUADRATIC);
  if ((puzDisp & 0xF) == PUZDISP_CURSOR1) {             // Blend entire pillar periodically.
    
    // Set attributes.
    obj.attr0 = (obj.attr0 & 0xFF00) | ((obj.attr0 + y) & ATTR0_Y_MASK);
    obj.attr1 = (obj.attr1 & 0xFE00) | ((obj.attr1 + x) & ATTR1_X_MASK);
    obj.attr2 = 0x1000 | ATTR2_PRIO(1) | (((pil->id - 1) << 5) & ATTR2_ID_MASK);
    
    // Mask with pillar specific attributes (no coordinates!)
    obj.attr0 |= pil->objMask.attr0;
    obj.attr1 |= pil->objMask.attr1;
    obj.attr2 |= pil->objMask.attr2;
    
    // Blend entire pillar.
    clr_blend(systemPal, clrs2, bldClrs, 7, bld);
    loadColours(bldClrs, 17 << 4, 7);
    
  } else if ((puzDisp & 0xF) == PUZDISP_CURSOR2 ||      // Blend top of pillar periodically.
             (puzDisp & 0xF) == PUZDISP_CURSOR3) {      // Set top of pillar to highlight colour.
    
    // Calculate coordinates.
    y += 48 + (obj.attr0 & ATTR0_Y_MASK) - (pil->height << 3);
    x += 8;
    
    // Set attributes.
    obj.attr0 = y & ATTR0_Y_MASK;
    obj.attr1 = ATTR1_SIZE_16 | (x & ATTR1_X_MASK);
    obj.attr2 = 0x138C | ATTR2_PRIO(1);
    
    // Blend top of pillar.
    clr_blend(systemPal+1, clrs1, bldClrs, 1, bld);
    setColour(bldClrs[0], (17 << 4) + 1);
    
  }
  
  // Draw to screen.
  addToOAMBuffer(&obj, layer);
  
  // Update blended palette, unless it's PUZDISP_CURSOR3.
  if ((puzDisp & 0xF) != PUZDISP_CURSOR3)
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
        puzzle[0].pil[id].height = 0;
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
    
    /* Add flavour pillars if we're not in the final stage of a multi-stage puzzle.
       We use two loops to iterate over all open spots (*).
       
            / col
       row / 
              o o * *       Loop 1
              o o * *       Loop 1
              * * * *       Loop 2
              * * * *       Loop 2
    */
    if (puzzle[0].stageCur != puzzle[0].stageMax) {
      for (row = 0; row < puzzle[0].length; row++) {
        for (col = puzzle[0].breadth; col < PUZ_BREADTH_MAX; col++) { 
          pil = pilConstr(clrs[0][0], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
          id = puzAddPil(0, &pil);
          puzzle[0].pil[id].height = 0;
          pilSetAnim(&puzzle[0].pil[id], PIL_ANIM_IDLE);
        }
      }
      for (row = puzzle[0].length; row < PUZ_LENGTH_MAX; row++) {
        for (col = 0; col < PUZ_BREADTH_MAX; col++) { 
          pil = pilConstr(clrs[0][0], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
          id = puzAddPil(0, &pil);
          puzzle[0].pil[id].height = 0;
          pilSetAnim(&puzzle[0].pil[id], PIL_ANIM_IDLE);
        }
      }
    }
    
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
  
  initCam = puzzle[0].camY;
  initBGOfsY[0] = lcdioBuffer.bg1vofs;
  initBGOfsY[1] = lcdioBuffer.bg2vofs;
  initBGOfsY[2] = lcdioBuffer.bg3vofs;
  
  setGameState(GAME_PUZZLE, PUZZLE_TRANSITION1);
}

const void puzInitDefault() {
  puzzle[0].stageMax = 3;
  puzzle[0].stageCur = 1;
  
  puzzle[0].length = 2;
  puzzle[0].breadth = 2;
  puzzle[0].height = 1;
  
  puzInit();
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

// Loads button prompt text.
const void puzLoadButtonPrompts() {
  const char* promptText[3][LANG_COUNT] = {
    {": Exit.", ": Exit."},
    {": Menu.", ": Menu."},
    {": Los op.", ": Solve."}
  };
  
  // Init.
  tte_init_chr4c(0, BG_SBB(28), 0x7000, bytes2word(2,1,0,0), CLR_BLACK, &verdana9_b4Font, (fnDrawg)chr4c_drawg_b4cts_fast);
  
  // Draw text.
  tte_set_pos(12, 145);
  tte_write(promptText[0][gLang]);
  tte_set_pos(190, 133);
  tte_write(promptText[1][gLang]);
  tte_set_pos(190, 145);
  tte_write(promptText[2][gLang]);
  
  // Palette.
  loadColours(systemPal, 7, systemPalLen>>1);
  setSyncPalFlagsByID(7);
}

// Draws button prompt sprites.
const void puzDrawButtonPrompts() {
  OBJ_ATTR obj = {0, 0, 0, 0};
  
  // B.
  obj.attr0 = ATTR0_BLEND | 148;
  obj.attr1 = 2;
  obj.attr2 = ATTR2_PALBANK(0) | 0x39B;
  addToOAMBuffer(&obj, 0);
  
  // SELECT.
  obj.attr0 = ATTR0_WIDE | ATTR0_BLEND | 137;
  obj.attr1 = ATTR1_SIZE_8x32 | 156;
  obj.attr2 = ATTR2_PALBANK(0) | 0x394;
  addToOAMBuffer(&obj, 0);
  
  // START.
  obj.attr0 = ATTR0_WIDE | ATTR0_BLEND | 149;
  obj.attr1 = ATTR1_SIZE_8x32 | 156;
  obj.attr2 = ATTR2_PALBANK(0) | 0x390;
  addToOAMBuffer(&obj, 0);
}

const void puzStartExitMenu() {
  addMenu(&puzExitMenu);
}

// Default exit menu text.
const void puzDrawExitMenu(const struct Menu* menu) {
  const char* question[LANG_COUNT] = {
    "Keer terug naar titelscherm?",
    "Return to title?"
  };
  
  drawMenu(menu);
  
  tte_set_pos(menu->xBox + 3, menu->yBox + 2);
  tte_write(question[gLang]);
}

// Menu text when a stage of the puzzle is solved.
const void puzDrawStageSolvedMenu(const struct Menu* menu) {
  const char* question[LANG_COUNT][2] = {
    {"Level behaald! Door", "naar volgende level?"},
    {"Stage complete!", "Load next stage?"}
  };
  
  drawMenu(menu);
  
  tte_set_pos(menu->xBox + 3, menu->yBox + 2);
  tte_write(question[gLang][0]);
  tte_set_pos(menu->xBox + 3, menu->yBox + 14);
  tte_write(question[gLang][1]);
}

// Menu text when puzzle is solved.
const void puzDrawSolvedMenu(const struct Menu* menu) {
  const char* question[LANG_COUNT][2] = {
    {"Puzzel opgelost! Keer", "terug naar titelscherm?"},
    {"Puzzle solved!", "Return to title?"}
  };
  
  drawMenu(menu);
  
  tte_set_pos(menu->xBox + 3, menu->yBox + 2);
  tte_write(question[gLang][0]);
  tte_set_pos(menu->xBox + 3, menu->yBox + 14);
  tte_write(question[gLang][1]);
}

// If no more active menus, reload button prompts.
const void puzMenuExit() {

  menuExit();
  
  // Re-load button prompt text.
  if (activeMenu() == NULL)
    puzLoadButtonPrompts();
}

// Return from puzzle to title screen.
const void puzExit() {
  lcdioBuffer.dispcnt = 0;
  setGameState(GAME_TITLE, TITLE_START);
}

// Run when confirming next stage is to be loaded.
// Sets the puzzle state to PUZZLE_SOLVETRANSITIONRETURN.
const void puzNextStage() {
  
  menuExit();
  setGameState(gGameState, PUZZLE_SOLVETRANSITIONRETURN);
  gStateClock++;
  
  if (puzzle[0].height == 1) {
    
    // Skip non-matching colours blending to original colours,
    // as there shouldn't be any non-matching colours.
    gStateClock = 31;
    
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
    puzzle[0].camY = initCam - yOffs;
    lcdioBuffer.bg1vofs = initBGOfsY[0] - yOffs;
    lcdioBuffer.bg2vofs = initBGOfsY[1] - yOffs;
    lcdioBuffer.bg3vofs = initBGOfsY[2] - yOffs;
    
    puzAnimRand(0, 60);
    puzRunAnims(0);
    puzDrawAll(0);
    
  } else if (gStateClock == 60) {
    // Init LCDIO.
    lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_OBJ_1D | DCNT_OBJ;
    lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
    lcdioBuffer.bg1cnt = BG_BUILD(2, 29, 0, 0, 0, 0, 0);
    lcdioBuffer.bg2cnt = BG_BUILD(0, 30, 0, 0, 2, 0, 0);
    lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 0, 3, 0, 0);
    
    // Clear background tiles & screen entries.
    CBB_CLEAR(0);
    CBB_CLEAR(1);
    CBB_CLEAR(2);
    CBB_CLEAR(3);
    
    // Draw button prompts on BG0.
    puzLoadButtonPrompts();
    
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
  int bld;
  
  if (gStateClock == 0) {
    
    // Setup blend for button prompt blending in.
    lcdioBuffer.dispcnt |= DCNT_BG0 | DCNT_BG1;
    lcdioBuffer.bldcnt = BLD_BUILD(BLD_BG0, BLD_BACKDROP, 1);
    lcdioBuffer.bldalpha = BLDA_BUILD(0, 16);
    
    // Load system palette.
    loadColours((COLOR*)systemPal, 272, systemPalLen>>1);
    
    // Load pal for solid colour option.
    if ((puzDispOptions & 0x0F) == PUZDISP_CURSOR3)
      setColour(CLR_MAGENTA, 273);
    setSyncPalFlagsByID(17);
    
    // Initialize puzzle.
    puzGenerate();
    puzzle[0].camX = -104;
    puzzle[0].camY = 160;
  } else if (gStateClock < 45) {
    
    // Blend in button prompts.
    bld = ease(0, 16, gStateClock, 45, EASE_OUT_QUADRATIC);
    lcdioBuffer.bldalpha = BLDA_BUILD(bld, 16 - bld);
    
    // Scroll puzzle in.
    puzzle[0].camY = ease(160, 14, gStateClock, 45, EASE_OUT_QUADRATIC);
  } else {
    lcdioBuffer.bldcnt = 0;
    
    puzzle[0].camY = 14;
    
    puzzle[0].selPil = &puzzle[0].pil[0];
    
    setGameState(gGameState, PUZZLE_RAISE);
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawButtonPrompts();
  gStateClock++;
}

// Turn non-matching panes dark.
// If puzzle is solved,
// move game state to solveTransitionTrue.
// otherwise,
// move game state to solveTransitionFalse.
//
// If puzzle is solved also move puzzle to the right,
// and open window prompt on the left. Otherwise,
// brighten palette again.
const void puzSolveTransition() {
  int bld;
  const COLOR clrs[4] = {CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK};
  COLOR bldClrs[4];
  
  if (gStateClock == 1) {
    
    // Set pane-matching colours to pre-blend colours.
    // We will re-use these panes.
    loadColours(&systemPal[3], 291, 4);
    setSyncPalFlagsByID(18);
    
  } else if (gStateClock < 32) {
    
    // Blend non-matching colours with black.
    bld = ease(0, 0x1F, gStateClock, 31, EASE_IN_QUADRATIC);
    clr_blend(&systemPal[3], clrs, bldClrs, 4, bld);
    loadColours(bldClrs, 259, 4);
    setSyncPalFlagsByID(16);
    
  } else if (gStateClock == 32 ) {
    
    // Set non-matching colours to black.
    loadColours(clrs, 259, 4);
    setSyncPalFlagsByID(16);
  
  } else if (gStateClock == 40) {
    
    if (puzIsSolved()) {
      CpuFastFill(0, &tile_mem[0], 0x12C0);     // Clear button prompt text.
      initCam = puzzle[0].camX;
      setGameState(gGameState, PUZZLE_SOLVETRANSITIONTRUE);
    } else
      setGameState(gGameState, PUZZLE_SOLVETRANSITIONFALSE);
    
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawButtonPrompts();
  puzDrawMatch(0, PUZDISP_MATCH3, 0);
  gStateClock++;
}

// Move puzzle to the right, and
// open window prompt on the left.
const void puzSolveTransitionTrue() {
  int scrl;
  
  if (gStateClock < 32) {
    scrl = ease(0, 45, gStateClock, 31, EASE_IN_QUADRATIC);
    puzzle[0].camX = initCam - scrl;
  } else if (gStateClock == 32) {
    if (puzzle[0].stageCur == puzzle[0].stageMax)
      addMenu(&puzSolvedMenu);
    else
      addMenu(&puzStageSolvedMenu);
  }
  
  runMenus();
  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawMatch(0, PUZDISP_MATCH3, 0);
  gStateClock++;
}

// Brighten palettes again before returning to Idle.
const void puzSolveTransitionFalse() {
  int bld;
  const COLOR clrs[4] = {CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK};
  COLOR bldClrs[4];
  
  if (gStateClock < 32) {
    
    // Transition non-matching colours back to original colours.
    bld = ease(0x1F, 0, gStateClock, 31, EASE_OUT_QUADRATIC);
    clr_blend(&systemPal[3], clrs, bldClrs, 4, bld);
    loadColours(bldClrs, 259, 4);
    setSyncPalFlagsByID(16);
  
  } else if (gStateClock == 32) {
    
    // Set non-matching colours back to original colours.
    loadColours(&systemPal[3], 259, 4);
    setSyncPalFlagsByID(16);
    
    // Return to idle.
    setGameState(gGameState, PUZZLE_IDLE);
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawButtonPrompts();
  puzDrawMatch(0, PUZDISP_MATCH3, 0);
  gStateClock++;
}

// This function executes after confirming
// the next stage should be loaded.
// Move pillars back to center,
// brighten pane colours again,
// lower pillars.
const void puzSolveTransitionReturn() {
  int bld, scrl;
  const COLOR clrs[4] = {CLR_BLACK, CLR_BLACK, CLR_BLACK, CLR_BLACK};
  COLOR bldClrs[4];
    
  if (gStateClock < 32) {
    
    // Transition non-matching colours back to original colours.
    bld = ease(0x1F, 0, gStateClock, 31, EASE_OUT_QUADRATIC);
    clr_blend(&systemPal[3], clrs, bldClrs, 4, bld);
    loadColours(bldClrs, 259, 4);
    setSyncPalFlagsByID(16);
  
  } else if (gStateClock < 63) {
    
    // Move puzzle back to centre.
    scrl = ease(45, 0, gStateClock - 31, 31, EASE_IN_QUADRATIC);
    puzzle[0].camX = initCam - scrl;
    
  } else {
    
    // Set puzzle to centre, enable button prompts and lower pillars.
    puzzle[0].camX = initCam;
    puzLoadButtonPrompts();
    setGameState(gGameState, PUZZLE_LOWER);
    
  }

  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawMatch(0, PUZDISP_MATCH3, 0);
  gStateClock++;
}

// Raise all valid pillars at the start of stage.
const void puzRaise() {
  int id, lim;
  
  // Do nothing if pillars are still raising.
  if (puzzle[0].selPil->animID == PIL_ANIM_IDLE) {
    
    // Raise pillars if they're not tall enough and not currently raising.
    if (puzzle[0].selPil->height < puzzle[0].height) {
      lim = puzzle[0].length * puzzle[0].breadth;
      for (id = 0; id < lim; id++)
        pilSetAnim(&puzzle[0].pil[id], PIL_ANIM_RAISE);
      
    // If finished raising, return to idle.
    } else {
      setGameState(gGameState, PUZZLE_IDLE);
      gStateClock = 30;
      puzzle[0].solveStatus = false;
    }
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawButtonPrompts();
  gStateClock++;
}

// Lower all valid pillars at the end of stage.
const void puzLower() {
  int id, lim;
  s16 camX, camY;
  
  // Do nothing if pillars are still lowering.
  if (puzzle[0].selPil->animID == PIL_ANIM_IDLE) {
    
    // Lower pillars if they're not buried and not currently lowering.
    if (puzzle[0].selPil->height > 0) {
      lim = puzzle[0].length * puzzle[0].breadth;
      for (id = 0; id < lim; id++)
        pilSetAnim(&puzzle[0].pil[id], PIL_ANIM_LOWER);
      
    // If finished lowering, generate puzzle before raising again.
    } else {
      
      // Increase params.
      puzzle[0].stageCur++;
      puzzle[0].length++;
      puzzle[0].breadth++;
      puzzle[0].height++;
      
      // Generate puzzle, but keep camera values.
      camX = puzzle[0].camX;
      camY = puzzle[0].camY;
      puzGenerate();
      puzzle[0].camX = camX;
      puzzle[0].camY = camY;
      
      // Next state.
      setGameState(gGameState, PUZZLE_RAISE);
    }
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  puzDrawButtonPrompts();
  gStateClock++;
}

const void puzIdle() {
  int moved;
  
  // Handle menu.
  if (activeMenu() != NULL)
    runMenus();
  else if (puzzle[0].selPil->animID == PIL_ANIM_IDLE) {
    // Only handle input if selected pillar is not still moving.
    
    // Match panes again after pillar was turned.
    if (puzzle[0].solveStatus) {
      puzMatchRowOrCol((puzzle[0].selPil->id - 1) / puzzle[0].breadth, false);
      puzMatchRowOrCol((puzzle[0].selPil->id - 1) % puzzle[0].breadth, true);
      puzzle[0].solveStatus = false;
      
      // Move to solved transition if puzzle has been solved.
      //if (puzIsSolved())
        //setGameState(gGameState, PUZZLE_SOLVEDTRANSITION);
      
    } else if (key_held(KEY_L)) {           // Handle button input.
      if (key_hit(KEY_A))
        puzDispOptions ^= PUZDISP_MATCHDISABLE;
      if (key_hit(KEY_B))
        puzDispOptions ^= PUZDISP_CURSORDISABLE;
    } else if (key_hit(KEY_A)) {
      pilSetAnim(puzzle[0].selPil, PIL_ANIM_TURN);
      puzzle[0].solveStatus = true;         // Re-solve puzzle after pillar finished turning.
    } else if (key_hit(KEY_B)) {
      puzStartExitMenu();
    } else if (key_hit(KEY_START)) {        // Confirm puzzle solution.
      
      // Skip darkening panes when no panes can be darkened.
      if (puzzle[0].height == 1) {
        if (puzIsSolved()) {
          loadColours(&systemPal[3], 291, 4);
          setSyncPalFlagsByID(18);
          CpuFastFill(0, &tile_mem[0], 0x12C0);     // Clear button prompt text.
          initCam = puzzle[0].camX;
          setGameState(gGameState, PUZZLE_SOLVETRANSITIONTRUE);
        } else
          ; // Play "Incorrect" sfx.
      } else
        setGameState(gGameState, PUZZLE_SOLVETRANSITION);
      
    } else if (key_hit(KEY_SELECT)) {
      addMenu(&puzMainMenu);
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
    }
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  
  // Draw these only if there's no active menu.
  if (activeMenu() == NULL) {
    puzDrawButtonPrompts();
    puzDrawMatch(0, puzDispOptions, gStateClock);
    puzDrawCursor(0, puzDispOptions, puzzle[0].selPil, gStateClock);
  }
  
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
    case PUZZLE_SOLVETRANSITION:
      puzSolveTransition();
      break;
    case PUZZLE_SOLVETRANSITIONTRUE:
      puzSolveTransitionTrue();
      break;
    case PUZZLE_SOLVETRANSITIONFALSE:
      puzSolveTransitionFalse();
      break;
    case PUZZLE_SOLVETRANSITIONRETURN:
      puzSolveTransitionReturn();
      break;
    case PUZZLE_RAISE:
      puzRaise();
      break;
    case PUZZLE_LOWER:
      puzLower();
      break;
    case PUZZLE_IDLE:
      puzIdle();
      break;
  }
}