#include <stdio.h>
#include <tonc.h>
#include "videobuffer.h"
#include "lcdiobuffer.h"
#include "efx.h"
#include "gfx/mainMenu.h"
#include "gfx/pil.h"
#include "gfx/pilTitle.h"
#include "pillar.h"
#include "main.h"
#include "title.h"

#define TITLE_PILLARCOUNT 20

#define ADR1 STATE_DATA
#define ADR2 ADR1+TITLE_PILLARCOUNT
//#define ADR3 ADR2+sizeof(struct TitleABCD2)


//struct TitleABCD2* title2 = (struct TitleABCD2*)(ADR2);
//struct TitleABCD* title3 = (struct TitleABCD*)(ADR3);


//#undefine ADR2 ADR1+sizeof(struct TitleABCD)
//#undefine ADR3 ADR2+sizeof(struct TitleABCD2)

u8* hiddenPillars = (u8*)(ADR1);        // Tracks which pillars are still hidden. Size of TITLE_PILLARCOUNT bytes.
u8* titleLoadProgress = (u8*)(ADR2);    // Used by titleLoad to determine when to move to next state.

#undef ADR1
#undef ADR2

const void titleStart() {
  int pilID;
  
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
  loadColours((COLOR*)pilPal+1, 1, 2);
  for (int i = 3; i <= 83; i++)      // Title uses 83 different colourIDs.
    setColour(pilColourByID[qran_range(0, 4)], i);
  setSyncPalFlagsByMask(0x3F);
  
  // Load Title tiles, tilemap and set BG3CNT.
  CpuFastSet((void*)pilTitleTiles, (void*)tile8_mem, pilTitleTilesLen>>2);
  CpuFastSet((void*)pilTitleMap, (void*)bgmap[3], pilTitleMapLen>>2);
  setSyncBGMapFlagsByID(3);
  lcdioBuffer.dispcnt |= DCNT_BG3;
  lcdioBuffer.bg3cnt |= BG_8BPP | BG_PRIO(3);
  lcdioBuffer.bg3vofs = -6;
  
  // Load main menu tiles, tilemap and palette.
  lcdioBuffer.bg2cnt &= ~BG_CBB_MASK;
  lcdioBuffer.bg2cnt |= BG_CBB(1);      // Avoid competing with title for tiles.
  lcdioBuffer.bg2cnt |= BG_PRIO(2);
  
  // Setup window for menu.
  lcdioBuffer.win0h = WIN_BUILD(168, 80);
  lcdioBuffer.win0v = WIN_BUILD(102, 102);
  lcdioBuffer.winin = WININ_BUILD((WIN_BG2), (0));
  lcdioBuffer.winout = WINOUT_BUILD((WIN_BG3 | WIN_OBJ | WIN_BLD), (0));
  lcdioBuffer.dispcnt |= DCNT_WIN0 | DCNT_BG2;
  
  CpuFastSet((void*)mainMenuTiles, (void*)(tile_mem[1]), mainMenuTilesLen>>2);
  mapTilemap((SCR_ENTRY*)mainMenuMap, bgmap[2], 10, 7, 11, 12, 0x6000);    // Use palette 6 onwards.
  setSyncBGMapFlagsByID(2);
  
  loadColours((COLOR*)mainMenuPal, 6*16, mainMenuPalLen>>1);
  setSyncPalFlagsByID(6);
  
  // Enable sprites.
  lcdioBuffer.dispcnt |= DCNT_OBJ;
  loadColours((COLOR*)pilPal, 256, 16);
  setSyncPalFlagsByID(16);
  
  initPilArray();
  pilCamX = -104;
  pilCamY = 80;
  
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
  
  //int mask = (qran_range(0, 0x200) << 12) | qran_range(0, 0x1000); // Randomize pillars.
  struct Pillar pil;
  u8 clrs[6] = {PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_RED, PIL_CLRID_YELLOW, PIL_CLRID_RED, PIL_CLRID_RED};
  int clrMask;
  u8 height;
  for (int i = 0; i < TITLE_PILLARCOUNT; i++) {
    
    // Set random colours.
    clrMask = qran_range(0, 0x1000);
    for (int j = 0; j < 6; j++) {
      clrs[j] = 3 & clrMask;
      clrMask >>= 2;
    }
    
    // Build pillar and add to pilArray.
    pil = pilConstr(clrs, titlePilCoords[i][0], titlePilCoords[i][1]);
    pilID = addPilToPilArray(&pil);
    if (pilID == -1)
      break;
    
    // Hide pillar and add to hiddenPillars array.
    pilSetAnim(&pilArray[pilID], PIL_ANIM_HIDDEN);
    hiddenPillars[i] = pilID;
    
    // FIXME, randomize height whilst animating raising & lowering pillars.
    // We favour height = 0 by mapping values > 3 to 0.
    // This way, nine out of twelve results will be zero,
    // assuming each value has the same chance of being generated.
    //height = qran_range(0, 16);
    //height >>= (height & 0xC);
    height = 0;
    
    pilArray[pilID].height = height;
    //pilArray[pilID].spriteData = pilSpriteData[pilArray[pilID].animID][pilArray[pilID].height];
    //pilArray[pilID].updateTiles = true;
  }
  
  //pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
  
  setGameState(gGameState, TITLE_LOAD);
}

const void titleLoad() {
  int pilID, count, id, freq, bld, dist;
  
  // Unhide one of the pillars every few frames.
  if (!(*titleLoadProgress & TITLELOAD_PILLARS)) {
    freq = 2;
    if (gStateClock >= freq * TITLE_PILLARCOUNT) {
      *titleLoadProgress |= TITLELOAD_PILLARS;        // Finished unhiding pillars.
    } else if (gStateClock % freq == 0) {
      count = gStateClock / freq;
      
      id = qran_range(0, TITLE_PILLARCOUNT - count);
      pilID = hiddenPillars[id];
      
      pilSetAnim(&pilArray[pilID], PIL_ANIM_IDLE);
      
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
      bld = ease(0, 16, freq / 100, 40, EASE_SQUARED);
      lcdioBuffer.bldalpha = BLDA_BUILD(16 - bld, bld);
    } else {
      lcdioBuffer.bldcnt = 0;
      lcdioBuffer.dispstat &= ~DSTAT_HBL_IRQ;         // Disable hblank interrupt in lcdiobuffer.
      irq_delete(II_HBLANK);
      *titleLoadProgress |= TITLELOAD_TITLE;          // Finished sinusoiding title.
    }
  }
  
  // Expand menu window.
  if (!(*titleLoadProgress & TITLELOAD_MENU)) {
    if (gStateClock < 40) {
      dist = ease(0, 50, gStateClock, 40, EASE_CUBIC);
      lcdioBuffer.win0v = WIN_BUILD(102+dist, 102-dist);
    } else {
      lcdioBuffer.dispcnt &= ~DCNT_WIN0;
      *titleLoadProgress |= TITLELOAD_MENU;           // Finished opening menu window.
    }
  }
  
  /*
  switch (gStateClock % 360) {
    case 0:
      pilArray[13].height = 1;
      pilSetAnim(&pilArray[13], PIL_ANIM_LOWER);
      break;
    case 60:
      pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
      break;
    case 120:
      pilSetAnim(&pilArray[13], PIL_ANIM_RAISE);
      break;
    case 180:
      pilSetAnim(&pilArray[13], PIL_ANIM_RAISE);
      break;
    case 240:
      pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
      break;
    case 300:
      pilSetAnim(&pilArray[13], PIL_ANIM_LOWER);
      break;
  }
  */
  
  pilRunAnims();
  pilDrawAll();
  
  gStateClock++;
}

const void titleUpdate() {
  switch(gGenericState) {
    case TITLE_START:
      titleStart();
      break;
    case TITLE_LOAD:
      titleLoad();
      break;
  }
}