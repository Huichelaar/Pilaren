#include <stdio.h>
#include <tonc.h>
#include "lcdiobuffer.h"
#include "videobuffer.h"
#include "efx.h"
#include "title.h"
#include "main.h"
#include "gfx/lang/lang.h"
#include "lang.h"

// FIXME? I wanted to put the address of langCursor at STATE_DATA,
// that way RAM can be re-used, but for the life of me,
// I can't figure out the syntax to setting that up.

EWRAM_DATA u8 gLang;
EWRAM_DATA const struct LanguageWidget* langCursor;

const void langStart() {
  // Set BG-control.
  lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
  lcdioBuffer.bg1cnt = BG_BUILD(0, 29, 0, 0, 1, 0, 0);
  lcdioBuffer.bg2cnt = BG_BUILD(0, 30, 0, 0, 2, 0, 0);
  lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 0, 3, 0, 0);
  lcdioBuffer.dispcnt |= DCNT_OBJ;

  // Clear background tiles & screen entries.
  CBB_CLEAR(0);
  CBB_CLEAR(1);
  CBB_CLEAR(2);
  CBB_CLEAR(3);
  
  // Set first colour to white.
  setColour(CLR_WHITE, 0);
  setSyncPalFlagsByID(0);
  
  // Load language tiles & palettes.
  for (int i = 0; i < LANG_LIMIT; i++) {
    CpuFastSet(langWidget[i].tiles, (void*)(MEM_VRAM_OBJ + 0x800 * i), 0x200);
    loadColours((COLOR*)langWidget[i].pal, 256 + (i << 4), 16);
    setSyncPalFlagsByID(i+16);
  }
  
  // Initialize langCursor to first widget.
  langCursor = &langWidget[0];
  
  setGameState(gGameState, LANG_LOAD);
}

const void langLoad() {
  gStateClock++;
  int duration = 20;
  OBJ_ATTR langObj = {0, 0, 0, 0};
  const COLOR clrA = CLR_WHITE;
  const COLOR clrB = CLR_DEAD;
  COLOR clrC;
  
  // Display language icons.
  for (int i = 0; i<<1 < gStateClock; i++) {
    langObj.attr0 = langWidget[i].y | ATTR0_SQUARE;
    langObj.attr1 = langWidget[i].x | ATTR1_SIZE_64x64;
    langObj.attr2 = (i << ATTR2_PALBANK_SHIFT) | (i << 6);
    addToOAMBuffer(&langObj, i);
  }
  
  if (gStateClock >= duration) {
    setColour(CLR_DEAD, 0);           // Set BG (transparant) colour.
    setColour(CLR_WHITE, 1);          // Set text colour to white.
    setSyncPalFlagsByID(0);
    
    setGameState(gGameState, LANG_WAITFORINPUT);
    return;
  }
  
  clr_blend(&clrA, &clrB, &clrC, 1, ease(0, 0x1F, gStateClock, duration, EASE_SQUARED));
  setColour(clrC, 0);
  setSyncPalFlagsByID(0);
}

const void langInput() {
  OBJ_ATTR langObj = {0, 0, 0, 0};
  int moved, id;
  const COLOR clrA = CLR_WHITE;
  const COLOR clrB = CLR_BLACK;
  COLOR clrC;
  
  // Display language icons.
  for (int i = 0; i < LANG_LIMIT; i++) {
    langObj.attr0 = langWidget[i].y | ATTR0_SQUARE;
    langObj.attr1 = langWidget[i].x | ATTR1_SIZE_64x64;
    langObj.attr2 = (i << ATTR2_PALBANK_SHIFT) | (i << 6);
    addToOAMBuffer(&langObj, i);
  }
  
  // Move cursor if applicable.
  id = langCursor->id;
  moved = moveCursorByInput((u32*)&langCursor,
                            (u32)langCursor->left,
                            (u32)langCursor->right,
                            (u32)langCursor->up,
                            (u32)langCursor->down,
                            (u32)langCursor->leftUp,
                            (u32)langCursor->leftDown,
                            (u32)langCursor->rightUp,
                            (u32)langCursor->rightDown);
  if (moved) {
    // Set previously selected grid-element's glowing colour back to default.
    setColour(clrB, (id << 4) + 258);
    setSyncPalFlagsByID(id + 16);
    gStateClock = 15;
  }
  
  // Highlight selected language.
  clr_blend(&clrA, &clrB, &clrC, 1, ease(0, 0x1F, ABS(15 - (gStateClock % 31)), 15, EASE_SQUARED));
  setColour(clrC, (langCursor->id << 4) + 258);
  setSyncPalFlagsByID(langCursor->id + 16);
  
  // Confirm language setting and move to next state.
  if (KEY_EQ(key_hit, KEY_START) || KEY_EQ(key_hit, KEY_A)) {
    if (langCursor->id >= LANG_COUNT)
      // TODO disallowed sfx.
      ;
    else {
      gLang = langCursor->id;
      
      // Disable & clear BG0.
      lcdioBuffer.dispcnt &= ~DCNT_BG0;
      clearBGMapBuffer(0);
      setSyncBGMapFlagsByID(0);
      
      // Seed RNG.
      sqran(gClock);
      
      setGameState(GAME_TITLE, TITLE_START);
      return;
    }
  }
  gStateClock++;
}

// Updates game during language selection.
const void langUpdate() {
  switch (gGenericState) {
    case LANG_START:
      langStart();
      break;
    case LANG_LOAD:
      langLoad();
      break;
    case LANG_WAITFORINPUT:
      langInput();
      break;
  }
}