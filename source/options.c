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
  
  // Init text.
  tte_init_chr4c(0, BG_SBB(28), 7 << 12, bytes2word(3,2,0,0), CLR_WHITE, &verdana9_b4Font, (fnDrawg)chr4c_drawg_b4cts_fast);
  loadColours(systemPal, 7*16, systemPalLen>>1);
  
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
  /*
  const int lim[6] = {PUZ_LENGTH_MIN, PUZ_LENGTH_MAX,
                      PUZ_BREADTH_MIN, PUZ_BREADTH_MAX,
                      PUZ_HEIGHT_MIN, PUZ_HEIGHT_MAX};
  const int y = mi->id << 4;
  u8* dim = (&puzzle[0].length) + mi->id;
  int change = false;
  char dimGlyph[2] = {'0', '\0'};
  */
  
  // Draw additional cursor.
  drawMenuCursor(mi, 0, 0);
  drawMenuCursor(mi, 214, 0);
  drawMenuCursorFlip(mi, 180, 0);
  
  // Change display setting based on input.
  int keyTrib = KEY_EQ(key_hit, KEY_RIGHT) - KEY_EQ(key_hit, KEY_LEFT);
  if (keyTrib == 0)
    return;
  
  setColour(0x7C1F, 273);         // Magenta. Max red and blue, min green.
  setSyncPalFlagsByID(17);
  if (mi->id == 0) {
    if (keyTrib == -1) {
      if ((puzDispOptions & 0x07) == PUZDISP_CURSOR1)
        puzDispOptions = (puzDispOptions & 0xF8) | PUZDISP_CURSOR3;
      else
        puzDispOptions -= 1;
    } else if (keyTrib == 1) {
      if ((puzDispOptions & 0x07) == PUZDISP_CURSOR3)
        puzDispOptions = (puzDispOptions & 0xF8) | PUZDISP_CURSOR1;
      else
        puzDispOptions += 1;
    }
  } else if (mi->id == 1)
    puzDispOptions ^= (PUZDISP_MATCH1 | PUZDISP_MATCH2);

  
  /*
  // Redraw dimension value if it changed.
  if (change) {
    tte_erase_rect(64, y, 72, y+16);
    tte_set_pos(64, y);
    dimGlyph[0] = '0' + *dim;
    tte_write(dimGlyph);
  }
  */
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