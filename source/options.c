#include <stdio.h>
#include <tonc.h>
#include "lcdiobuffer.h"
#include "video.h"
#include "guide.h"
#include "menu.h"
#include "puzzle.h"
#include "main.h"
#include "gfx/system.h"
#include "options.h"

const void optionsStart() {
  struct Pillar pil;
  int id;
  const u8 clrs[2][6] = {{PIL_CLRID_BLUE, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_BLUE, PIL_CLRID_RED}, {PIL_CLRID_RED | 0x10, PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE}};
  
  // Set BG-control.
  lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
  lcdioBuffer.bg1cnt = BG_BUILD(2, 29, 2, 0, 1, 0, 0);
  lcdioBuffer.bg2cnt = BG_BUILD(0, 30, 0, 0, 2, 0, 0);
  lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 0, 3, 0, 0);
  
  // Clear BGMaps.
  CpuFastFill(0, &bgmap[0], 0x800);
  setSyncBGMapFlagsByMask(0xE);
  
  // Clear background tiles & screen entries.
  CBB_CLEAR(0);
  CBB_CLEAR(1);
  CBB_CLEAR(2);
  CBB_CLEAR(3);
  
  // Reset BGoffs.
  lcdioBuffer.bg0hofs = 0;
  lcdioBuffer.bg0vofs = 0;
  
  // Init example pillars.
  puzzle[1].length = 2;
  puzzle[1].breadth = 1;
  puzzle[1].height = 1;
  puzInitPil(1, 0, 0);
  
  // Selected pillar example.
  pil = pilConstr((u8*)clrs[0], 5 * PIL_TILE_WIDTH - 7, -6 * PIL_TILE_HEIGHT - 7);
  id = puzAddPil(1, &pil);
  puzzle[1].pil[id].height = 1;
  pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
  puzzle[1].selPil = &puzzle[1].pil[id];
  
  // Pillar with pane match example.
  pil = pilConstr((u8*)clrs[1], 5 * PIL_TILE_WIDTH + 9, -6 * PIL_TILE_HEIGHT + 9);
  id = puzAddPil(1, &pil);
  puzzle[1].pil[id].height = 1;
  pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
  
  // Set BG to dark green.
  setColour(12<<5, 0);
  setSyncPalFlagsByMask(0x81);
  
  if (puzDispOptions == PUZDISP_CURSOR3) {  // Load pal for this option.
    setColour(0x7C1F, 273);                 // Magenta. Max red and blue, min green.
    setSyncPalFlagsByID(17);
  }
  
  // Enable menu & BGs
  addMenu(&optionsMenu);
  
  setGameState(gGameState, OPTIONS_IDLE);
}

const void optionsIdle() {
  lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
  
  // Pillar display.
  puzRunAnims(1);
  puzDrawAll(1);
  puzDrawMatch(1, puzDispOptions & 0x77, gStateClock);
  puzDrawCursor(1, puzDispOptions & 0x77, puzzle[1].selPil, gStateClock);
  
  // Exit when pressing select.
  if (key_hit(KEY_SELECT))
    exitOp();
  
  // Menu.
  runMenus();
  
  gStateClock++;
}

const void optionsReturn() {
  menuExit();
  guideReturn();
}

// Routine run when pressing B on options menu.
const void exitOp() {
  lcdioBuffer.dispcnt = 0;
  setGameState(gGameState, OPTIONS_RETURN);
}

// Routine run when hovering over options.
// Display extra menu cursor and change
// option values based on directional input.
const void hoverOpItems(const struct MenuItem* mi) {
  
  // Draw additional cursor.
  drawMenuCursor(mi, 0, 0);
  drawMenuCursor(mi, 214, 0);
  drawMenuCursorFlip(mi, 180, 0);
  
  // Change display setting based on input.
  int keyTrib = KEY_EQ(key_hit, KEY_RIGHT) - KEY_EQ(key_hit, KEY_LEFT);
  if (keyTrib == 0)
    return;
  
  // Change puzDispOptions on input.
  // Cursor display.
  if (mi->id == 0) {
    if (keyTrib == -1) {
      
      // Change displayed option.
      if ((puzDispOptions & 0x07) == PUZDISP_CURSOR1)
        puzDispOptions = (puzDispOptions & 0xF8) | PUZDISP_CURSOR3;
      else
        puzDispOptions -= 1;
      
      // Load pal for solid colour option.
      if ((puzDispOptions & 0x07) == PUZDISP_CURSOR3) {  
        setColour(CLR_MAGENTA, 273);
        setSyncPalFlagsByID(17);
      }
      
    } else if (keyTrib == 1) {
      
      // Change displayed option.
      if ((puzDispOptions & 0x07) == PUZDISP_CURSOR3)
        puzDispOptions = (puzDispOptions & 0xF8) | PUZDISP_CURSOR1;
      else
        puzDispOptions += 1;
      
      // Load pal for solid colour option.
      if ((puzDispOptions & 0x07) == PUZDISP_CURSOR3) {  
        setColour(CLR_MAGENTA, 273);
        setSyncPalFlagsByID(17);
      }
      
    }
  // Match display.
  } else if (mi->id == 1)
    puzDispOptions ^= (PUZDISP_MATCH1 | PUZDISP_MATCH2);
}

const void optionsUpdate() {
  switch(gGenericState) {
    case OPTIONS_START:
      optionsStart();
      break;
    case OPTIONS_IDLE:
      optionsIdle();
      break;
    case OPTIONS_RETURN:
      optionsReturn();
      break;
  }
}