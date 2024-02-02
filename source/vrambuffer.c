#include <stdio.h>
#include <tonc.h>
#include "vrambuffer.h"

EWRAM_DATA SCR_ENTRY bgmap[3][0x400] = {0};
EWRAM_DATA COLOR palBuffer[32][16] = {0};
EWRAM_DATA u32 syncPalFlags = 0;
EWRAM_DATA OBJ_ATTR oamBuffer[128] = {0};
EWRAM_DATA u8 syncBGMapFlags = 0;
EWRAM_DATA u8 oamBufferConsumed = 0;

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

const void loadPalette(COLOR* pal, int palSlot, int size) {
  CpuFastSet(pal, palBuffer[palSlot], size>>2);
}

const void setColour(COLOR col, int palSlot, int colSlot) {
  palBuffer[palSlot][colSlot] = col;
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
      src = (PALBANK*)palBuffer[i];
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

/*
const void testOAMBufferSort() {
  int size = (qran() & 0x7F) + 1;     // rand num in [1, 128].
  for (int i = 0; i < size; i++) {
    OBJ_ATTR object;
    object.attr0 = 0;
    object.attr1 = 0;
    object.attr2 = 0;
    addToOAMBuffer(&object, qran() & 0xFF);
  }
}
*/