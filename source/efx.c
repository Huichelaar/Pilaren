#include <stdio.h>
#include <tonc.h>
#include "efx.h"

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