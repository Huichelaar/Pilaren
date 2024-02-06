#include <stdio.h>
#include <tonc.h>
#include "gfx/pil.h"
#include "pillar.h"

EWRAM_DATA u32 pilCounter;
EWRAM_DATA s16 pilCamX;
EWRAM_DATA s16 pilCamY;
EWRAM_DATA struct Pillar pilArray[PIL_ARRAY_SIZE];

const void initPilArray() {
  for (int i = 0; i < PIL_ARRAY_SIZE; i++) {
    pilArray[i].pilID = 0;
  }
  pilCounter = 0;
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

// Sets pillar anim to idle so it will be drawn.
const void pilUnhide(struct Pillar* pil) {
  pil->animID = PIL_ANIM_IDLE;
  pil->animTimer = 0;
  pil->updateTiles = true;
  pil->spriteData = pilSpriteData[PIL_ANIM_IDLE][pil->height];
}

// Load relevant tiles of pillar into VRAM.
const void pilLoadTiles(struct Pillar* pil, int offsVRAM) {
  const struct PillarTileData* td;
  int src, dest;
  int srcOffs, size;
  
  td = pil->spriteData->tileData;
  while (td != NULL) {
    
    // Correct tile offset on colour.
    srcOffs = (int)td->tileOffs;
    for (int i = 0; i < 6; i++) {
      if (td->paneInfluence[i] != 0)
        srcOffs += pil->colour[i] << td->paneInfluence[i];
    }
    
    src = (int)&pilTiles + srcOffs;
    dest = (int)offsVRAM + (int)td->VRAMOffs;
    size = td->size << 3;
    
    CpuFastSet((void*)src, (void*)dest, size);
    
    td = td->nextTiles;
  }
}

// Continues animations of all pillars in pillar array.
// TODO
const void pilRunAnims() {
  struct Pillar* pil;
  for (int i = 0; i < PIL_ARRAY_SIZE; i++) {
    if (!pilArray[i].pilID)
      continue;
    
    pil = &pilArray[i];
    
    switch (pil->animID) {
        
      case PIL_ANIM_RAISE:
        if (pil->animTimer >= pil->spriteData->obj.fill) {
          if (pil->spriteData != NULL) {
            // Move to next animation frame.
            pil->spriteData = pil->spriteData->nextFrame;
          } else {
            // If that was the last frame, return to idle.
            // Also increment height.
            pil->height += 1;
            pil->animID = PIL_ANIM_IDLE;
            pil->spriteData = pilSpriteData[pil->animID][pil->height];
          }
          pil->animTimer = 0;
          pil->updateTiles = true;
        } else {
          pil->animTimer++;
        }
        break;
        
      case PIL_ANIM_TURN:
        if (pil->animTimer >= pil->spriteData->obj.fill) {
          if (pil->spriteData != NULL) {
            // Move to next animation frame.
            pil->spriteData = pil->spriteData->nextFrame;
          } else {
            // If that was the last frame, return to idle.
            // Also flip turn parameter.
            pil->turned = !pil->turned;
            pil->animID = PIL_ANIM_IDLE;
            pil->spriteData = pilSpriteData[pil->animID][pil->height];
          }
          pil->animTimer = 0;
          pil->updateTiles = true;
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
  struct Pillar pil;
  for (int i = 0; i < PIL_ARRAY_SIZE; i++) {
    if (!pilArray[i].pilID)
      continue;
    
    pil = pilArray[i];
    
    // TODO
    
  }
}

//const void pilRaise(