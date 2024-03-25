#ifndef MAIN
#define MAIN

#define STATE_DATA 0x02038000   // Used differently depending on what state game is in. Room for 0x8000 bytes.

extern u32 keyHeld[10];         // Timer per key indicating how many frames it's been held down for.
extern u32 gClock;
extern u32 gStateClock;
enum {
  // game states.
  GAME_START = 0,
	GAME_RESET = 1,
  GAME_TITLE = 2,
  GAME_PUZZLE = 3,
  GAME_GUIDE = 4,
};
extern u32 gGameState;
extern u32 gGenericState;

const void VBlankHandler();
const int setGameState(u32 gameState, u32 genericState);
const void update();
const void reset(int forceReset);

// Moves cursor diagonally
// if both hori and vert are true and next tile is available.
INLINE int moveDiagonal(int hori, int vert, u32* cursor, u32 leftUp, u32 leftDown, u32 rightUp, u32 rightDown) {
  
  if (hori == -1 && vert == -1 && leftUp) {
    *cursor = leftUp;
    return true;
  }
  if (hori == -1 && vert == 1 && leftDown) {
    *cursor = leftDown;
    return true;
  }
  if (hori == 1 && vert == -1 && rightUp) {
    *cursor = rightUp;
    return true;
  }
  if (hori == 1 && vert == 1 && rightDown) {
    *cursor = rightDown;
    return true;
  }
  
  return false;
}

// Moves cursor laterally (either horizontally or vertically)
// if dir is true and next tile is available.
INLINE int moveLateral(int dir, u32* cursor, u32 neg, u32 pos) {
  if ((dir == -1) && neg) {
    *cursor = neg;
    return true;
  } else if ((dir == 1) && pos) {
    *cursor = pos;
    return true;
  }
  return false;
}

int moveCursorByInput(u32* cursor, u32 left, u32 right, u32 up, u32 down,
                      u32 leftUp, u32 leftDown, u32 rightUp, u32 rightDown);

#endif // MAIN