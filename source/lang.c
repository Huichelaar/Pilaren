#include <stdio.h>
#include <tonc.h>
#include "lcdiobuffer.h"
#include "video.h"
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

// Display language icons.
const void displayLang(int count) {
  OBJ_ATTR langObj = {0, 0, 0, 0};
  
  for (int i = 0; i < count; i++) {
    langObj.attr0 = langWidget[i].y | ATTR0_SQUARE;
    langObj.attr0 |= langCursor->id == i ? ATTR0_BLEND : 0;
    langObj.attr1 = langWidget[i].x | ATTR1_SIZE_64x64;
    langObj.attr2 = (i << ATTR2_PALBANK_SHIFT) | (i << 6);
    langObj.attr2 |= langCursor->id == i ? 0 : ATTR2_PRIO(1);
    addToOAMBuffer(&langObj, i);
  }
}

const void langStart() {
  // Set BG-control.
  lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
  lcdioBuffer.bg1cnt = BG_BUILD(0, 29, 0, 0, 0, 0, 0);
  lcdioBuffer.bg2cnt = BG_BUILD(0, 30, 2, 0, 0, 0, 0);
  lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 0, 0, 0, 0);
  
  // Set BG2 params.
  lcdioBuffer.bg2pa = 0;
  lcdioBuffer.bg2pd = 0;
  lcdioBuffer.bg2x = 0;
  lcdioBuffer.bg2y = 0;
  
  // Set BG2 as affine background.
  lcdioBuffer.dispcnt = DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D | DCNT_MODE1;
  
  // Clear background tiles & screen entries.
  CBB_CLEAR(0);
  CBB_CLEAR(1);
  CBB_CLEAR(2);
  CBB_CLEAR(3);
  
  // Setup blend for selected language object.
  lcdioBuffer.bldcnt = BLD_BUILD(0, BLD_BG2 | BLD_BACKDROP, 1);
  lcdioBuffer.bldalpha = BLDA_BUILD(16, 0);
  
  // Set second colour to CLR_DEAD.
  // This will give BG2 the appearance of a window.
  COLOR clrs[2] = {CLR_WHITE, CLR_DEAD};
  loadColours(clrs, 0, 2);
  setSyncPalFlagsByID(0);
  
  // Load language tiles & palettes for OBJ.
  for (int i = 0; i < LANG_LIMIT; i++) {
    CpuFastSet(langWidget[i].tiles, (void*)(MEM_VRAM_OBJ + 0x800 * i), 0x200);
    loadColours((COLOR*)langWidget[i].pal, 256 + (i << 4), 16);
    setSyncPalFlagsByID(i+16);
  }
  // Load language mask tiles & map for BG.
  CpuFastSet((void*)flag_maskTiles, &tile_mem[0][0], flag_maskTilesLen>>2);
  CpuFastSet((void*)flag_maskMap, &bgmap[2], flag_maskMapLen>>2);
  setSyncBGMapFlagsByMask(0xC);
  
  // Initialize langCursor to first widget.
  langCursor = &langWidget[0];
  
  setGameState(gGameState, LANG_LOAD);
}

const void langLoad() {
  gStateClock++;
  int duration = 20;
  const COLOR clrA = CLR_WHITE;
  const COLOR clrB = CLR_DEAD;
  COLOR clrC;
  
  // Display language icons.
  displayLang(gStateClock >> 1);
  
  if (gStateClock >= duration) {
    setColour(CLR_DEAD, 0);           // Set BG (transparant) colour.
    setSyncPalFlagsByID(0);
    
    setGameState(gGameState, LANG_WAITFORINPUT);
    return;
  }
  
  clr_blend(&clrA, &clrB, &clrC, 1, ease(0, 0x1F, gStateClock, duration, EASE_IN_QUADRATIC));
  setColour(clrC, 0);
  setSyncPalFlagsByID(0);
}

const void langInput() {
  int moved, id;
  const COLOR clrA = CLR_WHITE;
  const COLOR clrB = CLR_BLACK;
  COLOR clrC;
  
  // Display language icons.
  displayLang(LANG_LIMIT);
  
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
  clr_blend(&clrA, &clrB, &clrC, 1, ease(0, 0x1F, ABS(15 - (gStateClock % 31)), 15, EASE_IN_QUADRATIC));
  setColour(clrC, (langCursor->id << 4) + 258);
  setSyncPalFlagsByID(langCursor->id + 16);
  
  // Confirm language setting and move to next state.
  if (KEY_EQ(key_hit, KEY_START) || KEY_EQ(key_hit, KEY_A)) {
    if (langCursor->id >= LANG_COUNT)
      // TODO play disallowed sfx.
      ;
    else {
      // Highlight selected language.
      setColour(CLR_WHITE, (langCursor->id << 4) + 258);
      setSyncPalFlagsByID(langCursor->id + 16);
      
      // Set language.
      gLang = langCursor->id;
      
      // Seed RNG.
      sqran(gClock);
      
      setGameState(gGameState, LANG_CLEAR);
      return;
    }
  }
  gStateClock++;
}

// Clear language icons gradually.
const void langClear() {
  
  // Display language icons.
  displayLang(LANG_LIMIT);
  
  // Expand BG2 to cover unselected language objects.
  // Also blend out selected language object.
  const int blend = ease(0, 16, gStateClock, 32, EASE_LINEAR);
  lcdioBuffer.bldalpha = BLDA_BUILD(16-blend, blend);
  const int size = ease(0x600, 0x100, gStateClock, 32, EASE_LINEAR);
  const BgAffineSource asx = {208<<8, 208<<8, langCursor->x+25, langCursor->y+25, size, size, 0};
  bg_rotscale_ex((BG_AFFINE*)&lcdioBuffer.bg2pa, &asx);
  
  if (gStateClock > 32) {
    lcdioBuffer.dispcnt = 0;
    setGameState(GAME_TITLE, TITLE_START);
    return;
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
    case LANG_CLEAR:
      langClear();
      break;
  }
}