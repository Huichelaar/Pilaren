#include <stdio.h>
#include <tonc.h>
#include "video.h"

// Add instruction, to copy size data from src to dest,
// to copyOnVBlankQueue. Returns true if succesful,
// false if queue is full.
int addToCopyOnVBlankQueue(const void* src, void* dest, int size, int mode) {
  if (copyOnVBlankIndex >= 1000)
    return false;
  
  copyOnVBlankQueue[copyOnVBlankIndex].src = src;
  copyOnVBlankQueue[copyOnVBlankIndex].dest = dest;
  copyOnVBlankQueue[copyOnVBlankIndex].size = size;
  copyOnVBlankQueue[copyOnVBlankIndex].mode = mode;
  copyOnVBlankIndex++;
  return true;
}

// Copy data as instructed in entries in queue.
// This allows buffering data until immediately after VDraw.
// TODO, allow sizes smaller than 8 words!
const void flushCopyOnVBlankQueue() {
  for (int i = 0; i < copyOnVBlankIndex; i++) {
    
    if (copyOnVBlankQueue[i].mode == BUFFER_FILL)
      CpuFastFill((u32)copyOnVBlankQueue[i].src, copyOnVBlankQueue[i].dest, copyOnVBlankQueue[i].size);
    
    else if (copyOnVBlankQueue[i].mode == BUFFER_COPY)
      CpuFastSet(copyOnVBlankQueue[i].src, copyOnVBlankQueue[i].dest, copyOnVBlankQueue[i].size);
  }
  
  copyOnVBlankIndex = 0;
}