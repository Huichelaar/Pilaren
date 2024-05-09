#include <stdio.h>
#include <tonc.h>
#include "video.h"
#include "lcdiobuffer.h"
#include "efx.h"
#include "gfx/system.h"
#include "gfx/pil.h"
#include "gfx/pilTitle.h"
#include "pillar.h"
#include "puzzle.h"
#include "guide.h"
#include "options.h"
#include "menu.h"
#include "main.h"
#include "title.h"

#define TITLE_PILLARCOUNT 20
#define TITLE_OPTIONCOUNT 10
#define TITLE_MENUCOUNT 10

#define ADR1 STATE_DATA
#define ADR2 ADR1+TITLE_PILLARCOUNT
#define ADR3 ADR2+sizeof(u8)
#define ADR4 ADR3+TITLE_OPTIONCOUNT

u8* hiddenPillars = (u8*)(ADR1);        // Tracks which pillars are still hidden. Size of TITLE_PILLARCOUNT bytes.
u8* titleLoadProgress = (u8*)(ADR2);    // Used by titleLoad to determine when to move to next state.
u8* cursorID = (u8*)(ADR3);             // Used to keep track of where cursor is in menu.
u8* menuID = (u8*)(ADR4);               // Used to keep track of which (sub)menu is active.

#undef ADR1
#undef ADR2
#undef ADR3
#undef ADR4

const void titleStart() {
  int pilID;
  
  // Set video mode to 0.
  lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_OBJ_1D;
  
  // Set BG-control.
  lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
  lcdioBuffer.bg1cnt = BG_BUILD(2, 29, 2, 0, 1, 0, 0);
  lcdioBuffer.bg2cnt = BG_BUILD(1, 30, 2, 0, 2, 0, 0);
  lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 8, 3, 0, 0);
  
  // Clear BGMaps.
  CpuFastFill(0, &bgmap[0], 0x800);
  
  // Clear background tiles & screen entries.
  CBB_CLEAR(0);
  CBB_CLEAR(1);
  CBB_CLEAR(2);
  CBB_CLEAR(3);
  
  // Set BG1 offset to 0,0.
  lcdioBuffer.bg1hofs = 0;
  lcdioBuffer.bg1vofs = 0;
  
  // Set default display options.
  puzDispOptions |= PUZDISP_MATCHDISABLE;
  puzDispOptions &= ~PUZDISP_CURSORDISABLE;
  
  // Used by titleLoad to determine when to move to next state.
  *titleLoadProgress = 0;
  
  // Add sinusoid HBlank effect to title BG.
  CpuFastFill(0, (void*)sinusoidVCountTable, SCREEN_HEIGHT >> 1);   // Empty offset array.
  HBlankBGOFSCNT = HBLANK_BG3_HOFS_POS;
  lcdioBuffer.dispstat |= DSTAT_HBL_IRQ;      // Enable hblank interrupt in lcdiobuffer.
  irq_add(II_HBLANK, HBlankScrollBG);
  
  // Setup alphablend for title BG.
  lcdioBuffer.bldcnt = BLD_BUILD(BLD_BG3, BLD_BACKDROP, 1);
  lcdioBuffer.bldalpha = BLDA_BUILD(0, 16);
  
  // Load Title palette. Randomize colours.
  loadColours((COLOR*)systemPal+1, 1, 2);
  for (int i = 3; i <= 83; i++)      // Title uses 83 different colourIDs.
    setColour(pilColourByID[qran_range(0, 4)], i);
  setSyncPalFlagsByMask(0x3F);
  
  // Load Title tiles, tilemap.
  CpuFastSet((void*)pilTitleTiles, (void*)tile8_mem, pilTitleTilesLen>>2);
  CpuFastSet((void*)pilTitleMap, (void*)bgmap[3], pilTitleMapLen>>2);
  setSyncBGMapFlagsByID(3);
  lcdioBuffer.dispcnt |= DCNT_BG3;
  
  // Setup window for main menu.
  lcdioBuffer.win0h = WIN_BUILD(168, 80);
  lcdioBuffer.win0v = WIN_BUILD(102, 102);
  lcdioBuffer.winin = WININ_BUILD((WIN_BG1 | WIN_BG2), (0));
  lcdioBuffer.winout = WINOUT_BUILD((WIN_BG3 | WIN_OBJ | WIN_BLD), (0));
  lcdioBuffer.dispcnt |= DCNT_WIN0 | DCNT_BG1 | DCNT_BG2;
  
  // Setup main menu.
  menuClear();
  addMenu(&titleMenu);
  
  // Enable sprites.
  lcdioBuffer.dispcnt |= DCNT_OBJ;
  loadColours((COLOR*)systemPal, 256, 16);
  setSyncPalFlagsByID(16);
  
  // Setup flavour pillars.
  puzInitPil(0, -104, 80);
  
  // Build pillars.
  const s16 titlePilCoords[TITLE_PILLARCOUNT][2] = {
    PIL_TILE(7, 0),
    PIL_TILE(6, 1),
    PIL_TILE(7, 1),
    PIL_TILE(8, 1),
    PIL_TILE(6, 2),
    PIL_TILE(7, 2),
    PIL_TILE(8, 2),
    PIL_TILE(9, 2),
    PIL_TILE(7, 3),
    PIL_TILE(8, 3),
    PIL_TILE(1, 6),
    PIL_TILE(2, 6),
    PIL_TILE(0, 7),
    PIL_TILE(1, 7),
    PIL_TILE(2, 7),
    PIL_TILE(3, 7),
    PIL_TILE(1, 8),
    PIL_TILE(2, 8),
    PIL_TILE(3, 8),
    PIL_TILE(2, 9)
  };
  
  struct Pillar pil;
  u8 clrs[6];
  for (int i = 0; i < TITLE_PILLARCOUNT; i++) {
    
    // Set random colours.
    pilGenerateRandColours(clrs);
    
    // Build pillar and add to pilArray.
    pil = pilConstr(clrs, titlePilCoords[i][0], titlePilCoords[i][1]);
    pilID = puzAddPil(0, &pil);
    if (pilID == -1)
      break;
    
    // Hide pillar and add to hiddenPillars array.
    pilSetAnim(&puzzle[0].pil[pilID], PIL_ANIM_HIDDEN);
    hiddenPillars[i] = pilID;
    puzzle[0].pil[pilID].height = 0;
  }
  
  setGameState(gGameState, TITLE_LOAD);
}

const void titleLoad() {
  int pilID, count, id, freq, bld, scrl, dist;
  
  // Unhide one of the pillars every few frames.
  if (!(*titleLoadProgress & TITLELOAD_PILLARS)) {
    freq = 2;
    if (gStateClock >= freq * TITLE_PILLARCOUNT) {
      *titleLoadProgress |= TITLELOAD_PILLARS;        // Finished unhiding pillars.
    } else if (gStateClock % freq == 0) {
      count = gStateClock / freq;
      
      id = qran_range(0, TITLE_PILLARCOUNT - count);
      pilID = hiddenPillars[id];
      
      pilSetAnim(&puzzle[0].pil[pilID], PIL_ANIM_IDLE);
      
      // Remove unhidden pillar, and shift rest of hiddenPillars array.
      for (int i = id; i < (TITLE_PILLARCOUNT - 1) - count; i++)
        hiddenPillars[i] = hiddenPillars[i+1];
    }
  }
  
  // Sinusoid and blend title.
  if (!(*titleLoadProgress & TITLELOAD_TITLE)) {
    freq = 4000 - gStateClock * 100;
    if (freq > 0) {
      generateSinusoid(freq, 8, 100);
      bld = ease(0, 16, freq / 100, 40, EASE_IN_QUADRATIC);
      scrl = ease(-6, 0, freq / 100, 40, EASE_IN_QUADRATIC);
      lcdioBuffer.bldalpha = BLDA_BUILD(16 - bld, bld);
      lcdioBuffer.bg3vofs = scrl;
    } else {
      lcdioBuffer.bldcnt = 0;
      lcdioBuffer.bg3vofs = -6;
      lcdioBuffer.dispstat &= ~DSTAT_HBL_IRQ;         // Disable hblank interrupt in lcdiobuffer.
      irq_delete(II_HBLANK);
      *titleLoadProgress |= TITLELOAD_TITLE;          // Finished sinusoiding title.
    }
  }
  
  // Expand menu window.
  if (!(*titleLoadProgress & TITLELOAD_MENU)) {
    if (gStateClock < 40) {
      dist = ease(0, 50, gStateClock, 40, EASE_IN_CUBIC);
      lcdioBuffer.win0v = WIN_BUILD(102+dist, 102-dist);
    } else {
      lcdioBuffer.dispcnt &= ~DCNT_WIN0;
      *titleLoadProgress |= TITLELOAD_MENU;           // Finished opening menu window.
    }
  }
  
  puzRunAnims(0);
  puzDrawAll(0);
  
  gStateClock++;
  
  if ((*titleLoadProgress & TITLELOAD_ALL) == TITLELOAD_ALL)
    setGameState(gGameState, TITLE_INPUT);
}

const void titleInput() {
  
  runMenus();
  puzAnimRand(0, 60);
  puzRunAnims(0);
  puzDrawAll(0);
  
  gStateClock++;
};

const void titleUpdate() {
  switch(gGenericState) {
    case TITLE_START:
      titleStart();
      break;
    case TITLE_LOAD:
      titleLoad();
      break;
    case TITLE_INPUT:
      titleInput();
      break;
  }
}

// Routine run when selecting "Custom game" on titlemenu.
const void selectCuGm(const struct MenuItem* mi) {
  addMenu(&customGameMenu);
  
  puzzle[0].stageMax = 1;
  puzzle[0].stageCur = 1;
  
  tte_set_pos(144, 58);
  tte_write("2");
  puzzle[0].length = 2;
  tte_set_pos(144, 74);
  tte_write("2");
  puzzle[0].breadth = 2;
  tte_set_pos(144, 90);
  tte_write("1");
  puzzle[0].height = 1;
}

// Routine run when hovering over puzzle dimensions
// in custom game submenu. Change dimension value
// based on input.
const void hoverCuGmItems(const struct MenuItem* mi) {
  const int lim[6] = {PUZ_LENGTH_MIN, PUZ_LENGTH_MAX,
                      PUZ_BREADTH_MIN, PUZ_BREADTH_MAX,
                      PUZ_HEIGHT_MIN, PUZ_HEIGHT_MAX};
  u8* dim = (&puzzle[0].length) + mi->id;
  int change = false;
  char dimGlyph[2] = {'0', '\0'};
  
  // Draw two cursors.
  drawMenuCursor(mi, 69, 0);
  drawMenuCursorFlip(mi, 52, 0);
  
  // Change dimension value based on input.
  int keyTrib = KEY_EQ(key_hit, KEY_RIGHT) - KEY_EQ(key_hit, KEY_LEFT);
  if (keyTrib) {
    *dim += keyTrib;
    change = true;
    if (*dim < lim[mi->id << 1]) {
      *dim = lim[mi->id << 1];
      change = false;
    } else if (*dim > lim[(mi->id << 1) + 1]) {
      *dim = lim[(mi->id << 1) + 1];
      change = false;
    }
  }
  
  // Redraw dimension value if it changed.
  if (change) {
    tte_erase_rect(144, mi->y, 152, mi->y + 16);
    tte_set_pos(144, mi->y);
    dimGlyph[0] = '0' + *dim;
    tte_write(dimGlyph);
  }
}

// Routine run when selecting "Guide" on titlemenu.
const void selectGu(const struct MenuItem* mi) {
  
  lcdioBuffer.dispcnt = 0;      // Disable all display.
  setGameState(GAME_GUIDE, GUIDE_START);
}

// Routine run when selecting "Options" on titlemenu.
const void selectOp(const struct MenuItem* mi) {
  
  lcdioBuffer.dispcnt = 0;      // Disable all display.
  setGameState(GAME_OPTIONS, OPTIONS_START);
}