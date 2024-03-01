#include <stdio.h>
#include <tonc.h>
#include "videobuffer.h"
#include "gfx/pil.h"
#include "main.h"
#include "pillar.h"

EWRAM_DATA u32 pilCounter;
EWRAM_DATA s16 pilCamX;
EWRAM_DATA s16 pilCamY;
EWRAM_DATA struct Pillar pilArray[PIL_ARRAY_SIZE];
const COLOR pilColourByID[4] = {PIL_CLR_WHITE, PIL_CLR_BLUE, PIL_CLR_RED, PIL_CLR_YELLOW};

const void initPilArray() {
  for (int i = 0; i < PIL_ARRAY_SIZE; i++) {
    pilArray[i].pilID = 0;
  }
  pilCounter = 0;
  pilCamX = 0;
  pilCamY = 0;
}

struct Pillar pilConstr(u8* colours, s16 x, s16 y) {
  struct Pillar pil;
  OBJ_ATTR objMask;
  obj_set_attr(&objMask, 0, 0, 0);
  
  pil.pilID = -1;
  for (int i = 0; i < 6; i++) { pil.colour[i] = colours[i]; }
  pil.turned = false;
  pil.animID = PIL_ANIM_HIDDEN;
  pil.animTimer = 0;
  pil.height = 0;
  pil.updateTiles = false;
  
  pil.x = x;
  pil.y = y;
  pil.spriteData = pilSpriteData[PIL_ANIM_HIDDEN][0];
  pil.objMask = objMask;
  
  pil.left = NULL;
  pil.right = NULL;
  pil.up = NULL;
  pil.down = NULL;
  
  return pil;
}

// Add Pillar to array. Return pilID in array if success.
// Return -1 if pillar could not be added to array.
int addPilToPilArray(struct Pillar* pil) {
  if (pilCounter >= PIL_ARRAY_SIZE)
    return false;
  
  for (int i = 0; i < PIL_ARRAY_SIZE; i++) {
    if (pilArray[i].pilID != 0)
      continue;
    
    // Copy pillar to array entry.
    pilArray[i].pilID = i+1;
    
    for (int j = 0; j < 6; j++) { pilArray[i].colour[j] = pil->colour[j]; }
    pilArray[i].turned = pil->turned;
    pilArray[i].animID = pil->animID;
    pilArray[i].animTimer = pil->animTimer;
    pilArray[i].height = pil->height;
    pilArray[i].updateTiles = pil->updateTiles;
    
    pilArray[i].x = pil->x;
    pilArray[i].y = pil->y;
    pilArray[i].spriteData = pil->spriteData;
    pilArray[i].objMask = pil->objMask;
    
    pilArray[i].left = pil->left;
    pilArray[i].right = pil->right;
    pilArray[i].up = pil->up;
    pilArray[i].down = pil->down;
    
    pilCounter++;
    
    return i;
  }
  
  return -1;              // return -1 if no empty slot in array was found.
}

// Sets pillar anim to given anim.
const void pilSetAnim(struct Pillar* pil, u8 animID) {
  pil->animID = animID;
  pil->animTimer = 0;
  pil->spriteData = pilSpriteData[animID][pil->height];
  
  if (animID == PIL_ANIM_HIDDEN) {
    pil->updateTiles = false;
  } else {
    pil->updateTiles = true;
  }
}

// Load relevant tiles of pillar into VRAM.
const void pilLoadTiles(struct Pillar* pil, int offsVRAM) {
  const struct PillarTileData* td;
  int src, dest;
  int srcOffs, size;
  
  // Clear tiles first. Load immediately if buffer is full.
  // FIXME, if this fills buffer, next tile updates will happen before tiles get cleared!
  if (!addToCopyOnVBlankQueue(0, (void*)offsVRAM, 0x100, BUFFER_FILL))
    CpuFastFill(0, (void*)offsVRAM, 0x100);
  
  td = pil->spriteData->tileData;
  while (td != NULL) {
    
    // Correct tile offset on colour.
    srcOffs = (int)td->tileOffs;
    int j;
    for (int i = 0; i < 6; i++) {
      j = (i + 3 * pil->turned) % 6;    // Invert pane influence if pillar is turned.
      if (td->paneInfluence[j] != 0)
        srcOffs += pil->colour[i] << td->paneInfluence[j];
    }
    
    src = (int)&pilTiles + srcOffs;
    dest = (int)offsVRAM + (int)td->VRAMOffs;
    size = td->size << 3;
    
    // Load tiles. Load immediately if buffer is full.
    if (!addToCopyOnVBlankQueue((void*)src, (void*)dest, size, BUFFER_COPY))
      CpuFastSet((void*)src, (void*)dest, size);
    
    td = td->nextTiles;
  }
}

// Randomly pick a pillar and animate it.
const void pilAnimRand(int freq) {
  
  // Only animate every freq frames.
  if (gClock % freq)
    return;
  
  const int id = qran_range(0, PIL_ARRAY_SIZE);
  int animID = qran_range(PIL_ANIM_RAISE, PIL_ANIM_TURN+1);
  
  // Don't animate pillars that are already animating.
  if (pilArray[id].animID != PIL_ANIM_IDLE)
    return;
  
  if (animID == PIL_ANIM_RAISE && pilArray[id].height == PILLAR_HEIGHT_MAX)
    animID = PIL_ANIM_LOWER;
  else if (animID == PIL_ANIM_LOWER && pilArray[id].height == 0)
    animID = PIL_ANIM_RAISE;
  
  pilSetAnim(&pilArray[id], animID);
}

// Continues animations of all pillars in pillar array.
// If TURN or RAISE animations finish, set to IDLE.
// FIXME, do we need &pilArray[i] when calling pilSetAnim? Can't we just give pil variable?
const void pilRunAnims() {
  struct Pillar* pil;
  for (int i = 0; i < PIL_ARRAY_SIZE; i++) {
    if (!pilArray[i].pilID)
      continue;
    
    pil = &pilArray[i];
    
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
            pilSetAnim(&pilArray[i], PIL_ANIM_IDLE);
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
            pilSetAnim(&pilArray[i], PIL_ANIM_IDLE);
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
            pilSetAnim(&pilArray[i], PIL_ANIM_IDLE);
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

// Draw all pillars in pillar array.
const void pilDrawAll() {
  struct Pillar* pil;
  s16 x, y;
  OBJ_ATTR obj = {0, 0, 0, 0};;
  
  for (int i = 0; i < PIL_ARRAY_SIZE; i++) {
    if (!pilArray[i].pilID)
      continue;
    
    pil = &pilArray[i];
    
    // Don't draw hidden pillars.
    if (pil->animID == PIL_ANIM_HIDDEN)
      continue;
    
    // Pillars are displayed at a 45 degree angle.
    x = (pil->x - pil->y) - pilCamX;
    y = (pil->x + pil->y) - pilCamY;
    
    // Pillars are four tiles (32 pixels) wide
    // and eight tiles (64 pixels) tall.
    if (x <= -32 || x >= SCREEN_WIDTH ||
        y <= -64 || y >= SCREEN_HEIGHT)
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
    // Priority determined by Y-value. Higher pillars should be drawn before lower pillars.
    // Therefore, we flip the importance of Y, by multiplying it by -1.
    addToOAMBuffer(&obj, -y);
  }
}

//const void pilRaise(