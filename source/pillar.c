#include <stdio.h>
#include <tonc.h>
#include "efx.h"
#include "video.h"
#include "lcdiobuffer.h"
#include "gfx/system.h"
#include "gfx/pil.h"
#include "main.h"
#include "pillar.h"

const COLOR pilColourByID[4] = {PIL_CLR_WHITE, PIL_CLR_BLUE, PIL_CLR_RED, PIL_CLR_YELLOW};

// Assumes colours is a six element array.
// Fills each element with a colour.
const void pilGenerateRandColours(u8* clrs) {
  int clrMask = qran_range(0, 0x1000);
    for (int i = 0; i < 6; i++) {
      clrs[i] = 3 & clrMask;
      clrMask >>= 2;
    }
}

struct Pillar pilConstr(u8* colours, s16 x, s16 y) {
  struct Pillar pil;
  OBJ_ATTR objMask;
  obj_set_attr(&objMask, 0, 0, 0);
  
  pil.id = -1;
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
  pil.leftUp = NULL;
  pil.leftDown = NULL;
  pil.rightUp = NULL;
  pil.rightDown = NULL;
  
  return pil;
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
        srcOffs += (pil->colour[i] & 3) << td->paneInfluence[j];
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

// Calculates coordinates where pillar would be drawn,
// based on pilCamY and pilCamX. Returns false if pillar
// would be drawn offscreen, true otherwise.
// layer, x and y args aren't used, but are used to return
// layer, x and y coordinates and of where pillar would be drawn.
int pilCalcCoords(struct Pillar* pil, s16* x, s16* y, s16 camX, s16 camY) {
  *x = (pil->x - pil->y) - camX;
  *y = (pil->x + pil->y) - camY;
  
  // Pillars are four tiles (32 pixels) wide
  // and eight tiles (64 pixels) tall.
  if (*x <= -32 || *x >= SCREEN_WIDTH ||
      *y <= -64 || *y >= SCREEN_HEIGHT)
    return false;       // Offscreen.
  return true;
}