#include <stdio.h>
#include <tonc.h>
#include "efx.h"
#include "videobuffer.h"
#include "lcdiobuffer.h"
#include "title.h"
#include "main.h"

EWRAM_DATA u32 gClock;
EWRAM_DATA u32 gStateClock;
EWRAM_DATA u32 gGameState;
EWRAM_DATA u32 gGenericState;

int main() {

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

  // Initialize game state, generic state & clocks.
  gClock = 0;
  gGameState = GAME_START;
  gGenericState = 0;
  setGameState(GAME_RESET, 0);

  // Set first colour of palette to white to avoid flashing.
  pal_bg_mem[0] = CLR_WHITE;

  // Enable vblank interrupt in lcdiobuffer.
  lcdioBuffer.dispstat |= DSTAT_VBL_IRQ;
  
  // Use objects as array.
  lcdioBuffer.dispcnt |= DCNT_OBJ_1D;

	// The vblank interrupt must be enabled for VBlankIntrWait() to work.
  IRQ_INIT();
  irq_add(II_VBLANK, VBlankHandler);

	while (1) {
		VBlankIntrWait();
    updateRoutine();
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
  clearOAMBuffer();
  
  // Copy data in copyOnVBlankQueue.
  flushCopyOnVBlankQueue();
  
  // Update keystates.
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
const void updateRoutine() {
  switch (gGameState) {
    case GAME_RESET:
      bootupUpdate();
      break;
    case GAME_TITLE:
      titleUpdate();
      break;
  }
}

// Updates game during bootup state.
const void bootupUpdate() {
  int timer;
  
  switch (gGenericState) {
    case BOOTUP_START:
      
      // Set BG-control.
      lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
      lcdioBuffer.bg1cnt = BG_BUILD(0, 29, 0, 0, 1, 0, 0);
      lcdioBuffer.bg2cnt = BG_BUILD(0, 30, 0, 0, 2, 0, 0);
      lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 0, 3, 0, 0);
    
      // Clear background tiles & screen entries.
      CBB_CLEAR(0);
      CBB_CLEAR(1);
      CBB_CLEAR(2);
      CBB_CLEAR(3);
      
      // Set first colour to white.
      setColour(CLR_WHITE, 0);
      setSyncPalFlagsByID(0);
      
      setGameState(gGameState, BOOTUP_FADE2BLACK);
    
    case BOOTUP_FADE2BLACK:
    
      gStateClock++;
      int duration = 20;
      const COLOR clrA = CLR_WHITE;
      const COLOR clrB = CLR_DEAD;
      COLOR clrC;
      
      if (gStateClock >= duration) {
        setColour(CLR_DEAD, 0);           // Set BG (transparant) colour.
        setColour(CLR_WHITE, 1);          // Set text colour to white.
        setSyncPalFlagsByID(0);
        
        lcdioBuffer.bg0hofs = -76;        // Move such that text is
        lcdioBuffer.bg0vofs = -76;        // in the centre of the screen.
        
        // Write text.
        tte_init_se(0,
                    BG_CBB(0)|BG_SBB(28),
                    0,
                    CLR_WHITE,
                    0,
                    NULL,
                    NULL);
        tte_write("Press START");
        
        setGameState(gGameState, BOOTUP_WAITFORINPUT);
        return;
      }
      
      clr_blend(&clrA, &clrB, &clrC, 1, ease(0, 0x1F, gStateClock, duration, EASE_SQUARED));
      setColour(clrC, 0);
      setSyncPalFlagsByID(0);
      break;
    
    case BOOTUP_WAITFORINPUT:
    
      // Flash text.
      timer = gStateClock % 30;
      if (timer < 10)
        lcdioBuffer.dispcnt &= ~DCNT_BG0;
      else
        lcdioBuffer.dispcnt |= DCNT_BG0;
      
      if (key_hit(1<<KI_START)) {
        
        // Disable & clear BG0.
        lcdioBuffer.dispcnt &= ~DCNT_BG0;
        clearBGMapBuffer(0);
        setSyncBGMapFlagsByID(0);
        
        // Seed RNG.
        sqran(gClock);
        
        setGameState(GAME_TITLE, TITLE_START);
        return;
      }
      
      gStateClock++;
      break;
  }
}