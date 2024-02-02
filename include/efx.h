#ifndef EFX
#define EFX

enum {
  // Easing function IDs.
  EASE_LINEAR = 0,
  EASE_SQUARED = 1,
  EASE_CUBIC = 2
};

int ease(int start, int end, int curX, int endX, int easeFunc);

#endif // EFX