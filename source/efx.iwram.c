#include <stdio.h>
#include <tonc.h>
#include "efx.h"

// HBlankHandler for changing BG_HOFS and BG_VOFS during HBlank.
const void HBlankScrollBG() {
  const int scanline = REG_VCOUNT;
  
  if (scanline >= SCREEN_HEIGHT)
    return;
  
  // Alternate; Odd scanlines will have offset multiplied by -1.
  // even scanlines will have offset multiplied by +1 (changes nothing).
  int alt = (((scanline & 1) << 1) - 1);
  
  // BG0.
  switch (HBlankBGOFSCNT & HBLANK_BG0_HOFS_ALT) {     // HOFS.
    case HBLANK_BG0_HOFS_ALT:
      REG_BG0HOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG0_HOFS_POS:
      REG_BG0HOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG0_HOFS_NEG:
      REG_BG0HOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
  switch (HBlankBGOFSCNT & HBLANK_BG0_VOFS_ALT) {     // VOFS.
    case HBLANK_BG0_VOFS_ALT:
      REG_BG0VOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG0_VOFS_POS:
      REG_BG0VOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG0_VOFS_NEG:
      REG_BG0VOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
  
  // BG1.
  switch (HBlankBGOFSCNT & HBLANK_BG1_HOFS_ALT) {     // HOFS.
    case HBLANK_BG1_HOFS_ALT:
      REG_BG1HOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG1_HOFS_POS:
      REG_BG1HOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG1_HOFS_NEG:
      REG_BG1HOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
  switch (HBlankBGOFSCNT & HBLANK_BG1_VOFS_ALT) {     // VOFS.
    case HBLANK_BG1_VOFS_ALT:
      REG_BG1VOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG1_VOFS_POS:
      REG_BG1VOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG1_VOFS_NEG:
      REG_BG1VOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
  
  // BG2.
  switch (HBlankBGOFSCNT & HBLANK_BG2_HOFS_ALT) {     // HOFS.
    case HBLANK_BG2_HOFS_ALT:
      REG_BG2HOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG2_HOFS_POS:
      REG_BG2HOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG1_HOFS_NEG:
      REG_BG2HOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
  switch (HBlankBGOFSCNT & HBLANK_BG2_VOFS_ALT) {     // VOFS.
    case HBLANK_BG2_VOFS_ALT:
      REG_BG2VOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG2_VOFS_POS:
      REG_BG2VOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG2_VOFS_NEG:
      REG_BG2VOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
  
  // BG3.
  switch (HBlankBGOFSCNT & HBLANK_BG3_HOFS_ALT) {     // HOFS.
    case HBLANK_BG3_HOFS_ALT:
      REG_BG3HOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG3_HOFS_POS:
      REG_BG3HOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG3_HOFS_NEG:
      REG_BG3HOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
  switch (HBlankBGOFSCNT & HBLANK_BG3_VOFS_ALT) {     // VOFS.
    case HBLANK_BG3_VOFS_ALT:
      REG_BG3VOFS = sinusoidVCountTable[scanline] * alt;
      break;
    case HBLANK_BG3_VOFS_POS:
      REG_BG3VOFS = sinusoidVCountTable[scanline];
      break;
    case HBLANK_BG3_VOFS_NEG:
      REG_BG3VOFS = sinusoidVCountTable[scanline] * -1;
    default:
      break;
  }
 
}