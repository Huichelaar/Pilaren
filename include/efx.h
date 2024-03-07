#ifndef EFX
#define EFX

extern s16 sinusoidVCountTable[SCREEN_HEIGHT];
extern u32 HBlankBGOFSCNT;

// HBlankBGOFSCNT is a bitmask indicating how to scroll BG layers during HBlank.
// HBlankScrollBG uses these.
enum {
  HBLANK_BG0_HOFS_POS = 0x0001,
  HBLANK_BG0_HOFS_NEG = 0x0002,
  HBLANK_BG0_VOFS_POS = 0x0004,
  HBLANK_BG0_VOFS_NEG = 0x0008,
  HBLANK_BG1_HOFS_POS = 0x0010,
  HBLANK_BG1_HOFS_NEG = 0x0020,
  HBLANK_BG1_VOFS_POS = 0x0040,
  HBLANK_BG1_VOFS_NEG = 0x0080,
  HBLANK_BG2_HOFS_POS = 0x0100,
  HBLANK_BG2_HOFS_NEG = 0x0200,
  HBLANK_BG2_VOFS_POS = 0x0400,
  HBLANK_BG2_VOFS_NEG = 0x0800,
  HBLANK_BG3_HOFS_POS = 0x1000,
  HBLANK_BG3_HOFS_NEG = 0x2000,
  HBLANK_BG3_VOFS_POS = 0x4000,
  HBLANK_BG3_VOFS_NEG = 0x8000,
  
  HBLANK_BG0_HOFS_ALT = 0x0003,
  HBLANK_BG0_VOFS_ALT = 0x000C,
  HBLANK_BG1_HOFS_ALT = 0x0030,
  HBLANK_BG1_VOFS_ALT = 0x00C0,
  HBLANK_BG2_HOFS_ALT = 0x0300,
  HBLANK_BG2_VOFS_ALT = 0x0C00,
  HBLANK_BG3_HOFS_ALT = 0x3000,
  HBLANK_BG3_VOFS_ALT = 0xC000,
};

enum {
  // Easing function IDs.
  EASE_LINEAR = 0,
  EASE_IN_QUADRATIC = 1,
  EASE_OUT_QUADRATIC = 2,
  EASE_IN_CUBIC = 3,
  EASE_OUT_CUBIC = 4
};

int ease(int start, int end, int curX, int endX, int easeFunc);
const void generateSinusoid(int frequency, int amplitude, int phase);
IWRAM_CODE const void HBlankScrollBG();

#endif // EFX