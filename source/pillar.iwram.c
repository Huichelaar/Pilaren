#include <stdio.h>
#include <tonc.h>
#include "video.h"
#include "gfx/pil.h"
#include "pillar.h"

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