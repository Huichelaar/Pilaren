#include <stdio.h>
#include <tonc.h>
#include "efx.h"
#include "videobuffer.h"
#include "lcdiobuffer.h"
#include "lang.h"
#include "title.h"
#include "main.h"

#define CURSORLOOP_INIT_DELAY 30  // After CURSORLOOP_INIT_DELAY frames,
#define CURSORLOOP_DELAY 6        // move to next element every CURSORLOOP_DELAY frames.

EWRAM_DATA u32 keyHeld[10];
EWRAM_DATA u32 gClock;
EWRAM_DATA u32 gStateClock;
EWRAM_DATA u32 gGameState;
EWRAM_DATA u32 gGenericState;

int main() {
  reset(true);

	while (1) {
		VBlankIntrWait();
    update();
    reset(false);
	}
}

// Resets game if START+SELECT+L+R is held.
const void reset(int forceReset) {
  int resetKeys = KEY_SELECT | KEY_START | KEY_R | KEY_L;
  int keyHit = key_hit(KEY_SELECT | KEY_START | KEY_R | KEY_L);
  
  // Reset only if forceReset, or reset key combination was given.
  if (forceReset || (KEY_EQ(key_is_down, resetKeys) && keyHit)) {
  
    // Clear IRQ.
    IRQ_INIT();
    
    // Clear buffers.
    clearBGMapBuffer(0);
    clearBGMapBuffer(1);
    clearBGMapBuffer(2);
    clearBGMapBuffer(3);
    CpuFastFill(0, palBuffer, 0x200);
    syncPalFlags = 0;
    oam_init(oamBuffer, 128);
    syncBGMapFlags = 0;
    oamBufferConsumed = 0;
    
    for (int i = 0; i < 10; i++)
      keyHeld[i] = 0;

    // Initialize game state, generic state & clocks.
    gClock = 0;
    gGameState = GAME_START;
    gGenericState = 0;
    setGameState(GAME_RESET, 0);

    // Set first colour of palette to white to avoid flashing.
    pal_bg_mem[0] = CLR_WHITE;

    // Enable vblank interrupt in lcdiobuffer.
    lcdioBuffer.dispstat |= DSTAT_VBL_IRQ;

    // The vblank interrupt must be enabled for VBlankIntrWait() to work.
    irq_add(II_VBLANK, VBlankHandler);
  }
}

// Various things that need to happen at VBlank.
// Increment clock, assumption is made that VBlank
// is reached every second, which is not entirely true.
const void VBlankHandler() {
  flushLCDIOBuffer();
  flushBGBuffer();
  flushPalBuffer();
  
  // Clear OAM, Flush buffer to OAM, clear buffer.
  oam_init(oam_mem, 128);
  flushOAMBuffer();
  flushAffOAMBuffer();
  clearOAMBuffer();
  oamAffBufferConsumed = 0;
  
  // Copy data in copyOnVBlankQueue.
  flushCopyOnVBlankQueue();
  
  // Update keystates.
  for (int i = 0; i < 10; i++)
    keyHeld[i] += (REG_KEYINPUT & (1 << i)) ? -keyHeld[i] : 1;
  key_poll();
  
  gClock++;
}

// Set different gameState and genericState and reset state clock.
// Does nothing and returns false if
// gameState is the same as active gameState
// and genericState is the same as active genericState.
const int setGameState(u32 gameState, u32 genericState) {
  if (gameState == gGameState && genericState == gGenericState)
    return false;
  
  gGameState = gameState;
  gGenericState = genericState;
  gStateClock = 0;
  return true;
}

// Updates game.
const void update() {
  switch (gGameState) {
    case GAME_RESET:
      langUpdate();
      break;
    case GAME_TITLE:
      titleUpdate();
      break;
  }
}

// Move cursor from one grid element to the next,
// based on key input. Holding R enables diagonal movement.
// Returns true if cursor moved, false otherwise.
// Allows for looping every CURSORLOOP_DELAY frames to
// next element if directional keys have been held for
// CURSORLOOP_INIT_DELAY frames.
int moveCursorByInput(u32* cursor, u32 left, u32 right, u32 up, u32 down,
                      u32 leftUp, u32 leftDown, u32 rightUp, u32 rightDown) {
  int horiLoop = 0;
  int vertLoop = 0;
  int keyDelay = 0;
  const int horiHit = KEY_EQ(key_hit, KEY_RIGHT) - KEY_EQ(key_hit, KEY_LEFT);
  const int horiHeld = KEY_EQ(key_is_down, KEY_RIGHT) - KEY_EQ(key_is_down, KEY_LEFT);
  const int vertHit = KEY_EQ(key_hit, KEY_DOWN) - KEY_EQ(key_hit, KEY_UP);
  const int vertHeld = KEY_EQ(key_is_down, KEY_DOWN) - KEY_EQ(key_is_down, KEY_UP);
  
  // Determine cursor loop delay. Shared between keys to avoid triggering
  // loops more often if more keys are held starting at different frames.
  for (int i = KI_RIGHT; i <= KI_DOWN; i++) {
    if (keyHeld[i] >= CURSORLOOP_INIT_DELAY) {
      keyDelay = keyHeld[i];
      break;
    }
  }
  
  // Check if keys have been held long enough for auto-move.
  if (((keyHeld[KI_RIGHT] >= CURSORLOOP_INIT_DELAY) || 
       (keyHeld[KI_LEFT] >= CURSORLOOP_INIT_DELAY)) &&
       (!(keyDelay % CURSORLOOP_DELAY)))
    horiLoop = horiHeld;
  if (((keyHeld[KI_DOWN] >= CURSORLOOP_INIT_DELAY) || 
       (keyHeld[KI_UP] >= CURSORLOOP_INIT_DELAY)) &&
       (!(keyDelay % CURSORLOOP_DELAY)))
    vertLoop = vertHeld;
  
  if (KEY_EQ(key_is_down, KEY_R)) {
    // Diagonal input.
    if (moveDiagonal(horiHit, vertHit, cursor, leftUp, leftDown, rightUp, rightDown))
      return true;
    if (moveDiagonal(horiHit, vertHeld, cursor, leftUp, leftDown, rightUp, rightDown))
      return true;
    if (moveDiagonal(horiHeld, vertHit, cursor, leftUp, leftDown, rightUp, rightDown))
      return true;
    if (moveDiagonal(horiLoop, vertLoop, cursor, leftUp, leftDown, rightUp, rightDown))
      return true;
  } else {
    // Lateral input.
    if (moveLateral(horiHit, cursor, left, right))
      return true;
    if (moveLateral(vertHit, cursor, up, down))
      return true;
    if (moveLateral(horiLoop, cursor, left, right))
      return true;
    if (moveLateral(vertLoop, cursor, up, down))
      return true;
  }
  
  return false;
}