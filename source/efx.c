#include <stdio.h>
#include <tonc.h>
#include "efx.h"

IWRAM_DATA s16 sinusoidVCountTable[SCREEN_HEIGHT] = {0};    // Filled by generateSinusoid.
IWRAM_DATA u32 HBlankBGOFSCNT = {0};

// Ease values over an interval.
int ease(int start, int end, int curX, int endX, int easeFunc) {
  const int distRange = end - start;
  const int inteRange = endX - curX;
  int num;
  
  // Don't divide by zero.
  if (!endX)
    return end;
  
  switch (easeFunc) {
    case EASE_LINEAR:
      num = distRange * curX;
      return start + num / endX;
      
    case EASE_IN_QUADRATIC:
      num = distRange * curX * curX;
      return start + num / (endX * endX);
      
    case EASE_OUT_QUADRATIC:
      num = distRange * inteRange * inteRange;
      return start + distRange - num / (endX * endX);
      
    case EASE_IN_CUBIC:
      num = distRange * curX * curX * curX;
      return start + num / (endX * endX * endX);
    
    case EASE_OUT_CUBIC:
      num = distRange * inteRange * inteRange * inteRange;
      return start + distRange - num / (endX * endX * endX);
    
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