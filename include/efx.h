#ifndef EFX
#define EFX

extern s16 sinusoidVCountTable[SCREEN_HEIGHT];
extern u32 HBlankBGOFSCNT;

enum {
  // Easing function IDs.
  EASE_LINEAR = 0,
  EASE_SQUARED = 1,
  EASE_CUBIC = 2
};

int ease(int start, int end, int curX, int endX, int easeFunc);
const void HBlankScrollBG();
const void generateSinusoid(int frequency, int amplitude);

#endif // EFX