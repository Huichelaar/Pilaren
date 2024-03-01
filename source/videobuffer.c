#include <stdio.h>
#include <tonc.h>
#include "videobuffer.h"

EWRAM_DATA SCR_ENTRY bgmap[3][0x400] = {0};
EWRAM_DATA COLOR palBuffer[32*16] = {0};
EWRAM_DATA u32 syncPalFlags = 0;
EWRAM_DATA OBJ_ATTR oamBuffer[128] = {0};
EWRAM_DATA s16 oamAffBuffer[32][4] = {0};
EWRAM_DATA u8 syncBGMapFlags = 0;
EWRAM_DATA u8 oamBufferConsumed = 0;
EWRAM_DATA u8 oamAffBufferConsumed = 0;
EWRAM_DATA u8 copyOnVBlankQueueConsumed = 0;
EWRAM_DATA u8 ewramPad1_8 = 0;
EWRAM_DATA struct copyOnVBlankEntry copyOnVBlankQueue[1000] = {0};

const void setSyncBGMapFlagsByID(int bg) {
  syncBGMapFlags |= 1 << bg;
}

const void setSyncBGMapFlagsByMask(int bg) {
  syncBGMapFlags |= bg;
}

const void setSyncPalFlagsByID(int pal) {
  syncPalFlags |= 1 << pal;
}

const void setSyncPalFlagsByMask(int pal) {
  syncPalFlags |= pal;
}

// load clrs into palette buffer.
// palslot: offset at which to start copying to.
// size: number of colours.
const void loadColours(COLOR* clrs, int colSlot, int size) {
  // CpuFastSet if size is multiple of 8 bytes and large enough.
  if ((size >= 0x20) && ((size & 0x3) == 0))
    CpuFastSet(clrs, &palBuffer[colSlot], size>>1);
  else
    CpuSet(clrs, &palBuffer[colSlot], size);
}

// For setting individual colours.
const void setColour(COLOR col, int colSlot) {
  palBuffer[colSlot] = col;
}

// copy tilemaps from buffers to screen blocks
// as indicated by syncBGMapFlags.
const void flushBGBuffer() {
  SCR_ENTRY* src;
  void* dest;
  
  for (int i = 0; i < 4; i++) {
    if (syncBGMapFlags & (1 << i)) {
      src = bgmap[i];
      dest = (void*)(MEM_VRAM | ((*(REG_BGCNT+i) & BG_SBB_MASK) << 3));
      CpuFastSet(src, dest, 0x200);
    }
  }
  
  syncBGMapFlags = 0;
}

const void flushPalBuffer() {
  PALBANK* src;
  PALBANK* dest;
  
  for (int i = 0; i < 32; i++) {
    if (syncPalFlags & (1 << i)) {
      src = (PALBANK*)&palBuffer[i<<4];
      dest = (PALBANK*)pal_bg_bank[i];
      CpuFastSet(src, dest, 0x8);
    }
  }
  
  syncPalFlags = 0;
}

const void clearBGMapBuffer(int bg) {
  CpuFastFill(0, bgmap[bg], 0x200);
}

// This function is separate so we avoid allocating
// 128 OBJ_ATTR on the stack repeatedly in recursive calls.
const void mergeOAMBuffer(OBJ_ATTR* oamArr, OBJ_ATTR* arrL, OBJ_ATTR* arrR, int lSize, int rSize) {
  OBJ_ATTR arrLCopy[64];
  OBJ_ATTR arrRCopy[64];
  for (int i = 0; i < lSize; i++)
    arrLCopy[i] = arrL[i];
  for (int i = 0; i < rSize; i++)
    arrRCopy[i] = arrR[i];
  
  // Sort the two sub-arrays into the original array (merge).
  int l = 0;
  int r = 0;
  int i = 0;
  while (true) {
    
    // Bounds.
    if (l >= lSize && r >= rSize)
      break;
    else if (l >= lSize) {
      oamArr[i] = arrRCopy[r];
      i++;
      r++;
      continue;
    } else if (r >= rSize) {
      oamArr[i] = arrLCopy[l];
      i++;
      l++;
      continue;
    }
    
    // Regular case.
    if (arrLCopy[l].fill <= arrRCopy[r].fill) {
      oamArr[i] = arrLCopy[l];
      l++;
    } else {
      oamArr[i] = arrRCopy[r];
      r++;
    }
    i++;
  }
}

// Mergesort.
// We're using attr3/fill as additional priority,
// and sorting sprites accordingly.
OBJ_ATTR* sortOAMBuffer(OBJ_ATTR* oamArr, int arrSize) {
  
  // If only one element, return that element.
  if (arrSize == 1 || arrSize == 0)
    return oamArr;
  
  // Split array and sort both sub-arrays.
  int m = arrSize >> 1;
  int lSize = m;
  int rSize = arrSize - m;
  OBJ_ATTR* arrL = sortOAMBuffer(oamArr, lSize);
  OBJ_ATTR* arrR = sortOAMBuffer(oamArr + lSize, rSize);
  
  // Separate function.
  mergeOAMBuffer(oamArr, arrL, arrR, lSize, rSize);

  return oamArr;
}

// move objects in buffer to OAM.
const void flushOAMBuffer() {
  sortOAMBuffer(oamBuffer, oamBufferConsumed);
  obj_copy(oam_mem, oamBuffer, oamBufferConsumed);
}

// move affine matrices in buffer to OAM affine.
const void flushAffOAMBuffer() {
  for (int i = 0; i < oamAffBufferConsumed; i++) {
    obj_aff_mem[i].pa = oamAffBuffer[i][0];
    obj_aff_mem[i].pb = oamAffBuffer[i][1];
    obj_aff_mem[i].pc = oamAffBuffer[i][2];
    obj_aff_mem[i].pd = oamAffBuffer[i][3];
  }
}

const void clearOAMBuffer() {
  for (int i = 0; i < 128; i++) {
    oamBuffer[i].attr0 |= ATTR0_HIDE;
  }
  oamBufferConsumed = 0;
}

// If buffer is full, returns false.
// Otherwise, adds object to OAMBuffer,
// to be displayed next frame and returns true.
int addToOAMBuffer(OBJ_ATTR* object, int priority) {
  if (oamBufferConsumed >= 128)
    return false;
  
  oamBuffer[oamBufferConsumed] = *object;
  oamBuffer[oamBufferConsumed].fill = (s16)priority;
  oamBufferConsumed++;
  
  return true;
}

// If either OAM or affine buffer is full, returns -1.
// Otherwise, add object to OAMBuffer and affine matrix
// to affine OAM buffer and return ID of affine matrix.
int addAffToOAMBuffer(OBJ_AFFINE* affMatr, OBJ_ATTR* object, int priority) {
  if (oamAffBufferConsumed >= 32)
    return -1;
  
  // Set affine matrix params.
  oamAffBuffer[oamAffBufferConsumed][0] = affMatr->pa;
  oamAffBuffer[oamAffBufferConsumed][1] = affMatr->pb;
  oamAffBuffer[oamAffBufferConsumed][2] = affMatr->pc;
  oamAffBuffer[oamAffBufferConsumed][3] = affMatr->pd;
  
  // Set affine matrix ID.
  object->attr1 &= ~ATTR1_AFF_ID_MASK;
  object->attr1 |= (oamAffBufferConsumed << ATTR1_AFF_ID_SHIFT);
  
  if (!addToOAMBuffer(object, priority))
    return -1;

  oamAffBufferConsumed++;
  return oamAffBufferConsumed - 1;
}

// Copy data as instructed in entries in queue.
// This allows buffering data until immediately after VDraw.
// TODO, allow sizes smaller than 8 words!
const void flushCopyOnVBlankQueue() {
  for (int i = 0; i < copyOnVBlankQueueConsumed; i++) {
    
    if (copyOnVBlankQueue[i].mode == BUFFER_FILL)
      CpuFastFill((u32)copyOnVBlankQueue[i].src, copyOnVBlankQueue[i].dest, copyOnVBlankQueue[i].size);
    
    else if (copyOnVBlankQueue[i].mode == BUFFER_COPY)
      CpuFastSet(copyOnVBlankQueue[i].src, copyOnVBlankQueue[i].dest, copyOnVBlankQueue[i].size);
  }
  
  copyOnVBlankQueueConsumed = 0;
}

// Add instruction, to copy size data from src to dest,
// to copyOnVBlankQueue. Returns true if succesful,
// false if queue is full.
int addToCopyOnVBlankQueue(void* src, void* dest, int size, int mode) {
  if (copyOnVBlankQueueConsumed >= 1000)
    return false;
  
  copyOnVBlankQueue[copyOnVBlankQueueConsumed].src = src;
  copyOnVBlankQueue[copyOnVBlankQueueConsumed].dest = dest;
  copyOnVBlankQueue[copyOnVBlankQueueConsumed].size = size;
  copyOnVBlankQueue[copyOnVBlankQueueConsumed].mode = mode;
  copyOnVBlankQueueConsumed++;
  return true;
}

// Map tileMap in src to dest.
// mask can contain palette and additional tileoffset data.
const void mapTilemap(SCR_ENTRY* src, SCR_ENTRY* dest, int xOffs, int yOffs, int width, int height, SCR_ENTRY mask) {
  int x, y;
  
  dest = dest + yOffs * 32 + xOffs;
  
  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
      dest[y * 32 + x] = src[y * width + x] | mask;
}