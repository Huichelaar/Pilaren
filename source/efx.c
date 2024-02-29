#include <stdio.h>
#include <tonc.h>
#include "efx.h"

IWRAM_DATA s16 sinusoidVCountTable[SCREEN_HEIGHT] = {0};    // Filled by generateSinusoid.
IWRAM_DATA u32 HBlankBGOFSCNT = {0};

// Ease values over an interval.
int ease(int start, int end, int curX, int endX, int easeFunc) {
  int num;
  
  // Don't divide by zero.
  if (!endX)
    return end;
  
  switch (easeFunc) {
    case EASE_LINEAR:
      num = (end - start) * curX;
      return start + num / endX;
      
    case EASE_SQUARED:
      num = (end - start) * curX * curX;
      return start + num / (endX * endX);
      
    case EASE_CUBIC:
      num = (end - start) * curX * curX * curX;
      return start + num / (endX * endX * endX);
    
    default:
      return 0;
  }
}

// Generate signed short values for horizontal/vertical scrolling on HBlank.
// Aims to replicate a sinusoid effect for the relevant background.
const void generateSinusoid(int frequency, int amplitude, int phase) {
  s32 step;
  
  for (int i = 0; i < SCREEN_HEIGHT; i++) {
    step = lu_sin((frequency * i + phase) & 0xFFFF);
    step = (amplitude * step) / 0x1000;
    sinusoidVCountTable[i] = step;
  }
  
}