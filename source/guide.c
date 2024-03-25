#include <stdio.h>
#include <tonc.h>
#include "main.h"
#include "lcdioBuffer.h"
#include "video.h"
#include "efx.h"
#include "pillar.h"
#include "gfx/system.h"
#include "gfx/guide/guide.h"
#include "guide.h"

EWRAM_DATA u8 guidePageID;

const void guideStart() {
  // Set dispcnt.
  lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;
  
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
  
  // Init text.
  tte_init_chr4c(0, BG_SBB(28), 0, bytes2word(3,2,0,0), CLR_WHITE, &verdana9_b4Font, (fnDrawg)chr4c_drawg_b4cts_fast);
  loadColours(systemPal, 0, systemPalLen>>1);
  setSyncPalFlagsByID(0);
  
  setGameState(gGameState, GUIDE_IDLE);
  
  // Draw first page's text.
  guidePageFlip(0);
}

// Run page-specific init routines.
const void guidePageInit() {
  int row, col, id;
  struct Pillar pil;
  const u8 clrs[2][2][6] = {{{PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_WHITE}}, {{PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE}}};
  
  switch (guidePageID) {
    case 0:
      // Setup row/column outline of example pillar matrix.
      lcdioBuffer.bg1hofs = -151;
      lcdioBuffer.bg1vofs = -80;
      CpuFastSet(page0Tiles, &tile_mem[2], page0TilesLen>>2);
      CpuFastFill(0, &bgmap[1], 0x400);
      mapTilemap(page0Map, bgmap[1], 0, 0, 9, 9, 0x0);
      mapTilemap(page0Map + (page0MapLen >> 2), bgmap[2], 0, 0, 9, 9, 0x0);
      setSyncBGMapFlagsByMask(0x6);
      lcdioBuffer.dispcnt |= DCNT_BG1;
      
      // Setup example pillar matrix.
      initPilArray(-170, -50);
      for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
          pil = pilConstr((u8*)clrs[row][col], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
          id = addPilToPilArray(&pil);
          pilArray[id].height = 1;
          pilSetAnim(&pilArray[id], PIL_ANIM_IDLE);
        }
      }
      
      // Draw additional numbering to rows & columns.
      tte_set_pos(167, 78);
      tte_write("#{ci:7}1");
      tte_set_pos(150, 95);
      tte_write("2");
      tte_set_pos(201, 78);
      tte_write("#{ci:15}1");
      tte_set_pos(218, 95);
      tte_write("2");
      break;
    case 1:
      // Setup <-- turn --> arrows.
      lcdioBuffer.bg1hofs = 2;
      lcdioBuffer.bg1vofs = -2;
      CpuFastSet(page1Tiles, &tile_mem[2], page1TilesLen>>2);
      CpuFastFill(0, &bgmap[1], 0x200);
      mapTilemap(page1Map, bgmap[1], 8, 6, 14, 1, 0x0);
      setSyncBGMapFlagsByID(1);
      lcdioBuffer.dispcnt |= DCNT_BG1;
      tte_set_pos(108, 48);
      tte_write("#{ci:3}Turn");
      
      // Setup example pillar.
      initPilArray(-187, 35);
      
      // Example turning pillar.
      pil = pilConstr((u8*)clrs[1][1], 6, -10);
      id = addPilToPilArray(&pil);
      pilArray[id].height = 1;
      pilSetAnim(&pilArray[id], PIL_ANIM_IDLE);
      
      // Example unsolved puzzle.
      for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
          pil = pilConstr((u8*)clrs[row][col], (col - 3) * PIL_TILE_WIDTH - 6, (row + 6) * PIL_TILE_HEIGHT - 6);
          id = addPilToPilArray(&pil);
          pilArray[id].height = 1;
          pilSetAnim(&pilArray[id], PIL_ANIM_IDLE);
        }
      }
      pilArray[1].turned = true;
      
      // Solved puzzle variant.
      for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
          pil = pilConstr((u8*)clrs[row][col], (col + 1) * PIL_TILE_WIDTH - 6, (row + 2) * PIL_TILE_HEIGHT - 6);
          id = addPilToPilArray(&pil);
          pilArray[id].height = 1;
          pilSetAnim(&pilArray[id], PIL_ANIM_IDLE);
        }
      }
      
      // Draw additional numbering to rows & columns.
      // Commented out as it seemed to provide too much clutter.
      /*
      tte_set_pos(35, 38);
      tte_write("#{ci:7}1");
      tte_set_pos(18, 55);
      tte_write("2");
      tte_set_pos(171, 38);
      tte_write("1");
      tte_set_pos(154, 55);
      tte_write("2");
      tte_set_pos(61, 38);
      tte_write("#{ci:15}1");
      tte_set_pos(78, 55);
      tte_write("2");
      tte_set_pos(197, 38);
      tte_write("1");
      tte_set_pos(214, 55);
      tte_write("2");
      */
      
      break;
  }
}

// Run page-specific idle routines.
const void guidePageIdle() {
  switch (guidePageID) {
    case 0:
      if (gStateClock % 180 == 0)
        lcdioBuffer.bg1vofs += 256;
    
      pilRunAnims();
      pilDrawAll();
      break;
    case 1:
      if (gStateClock % 120 == 0)
        pilSetAnim(&pilArray[0], PIL_ANIM_TURN);
      
      pilRunAnims();
      pilDrawAll();
      break;
  }
}

// Change page to id page.
const void guidePageFlip(int id) {
  // Set global pageID var.
  guidePageID = id;
  
  // Clear text.
  chr4c_erase(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  
  // Draw page's text.
  tte_set_pos(0, 0);
  tte_write(guidePageText[guidePageID][gLang]);
  
  // Draw page number.
  tte_set_pos(112, 148);
  tte_putc('1' + guidePageID);
  tte_putc('/');
  tte_putc('0' + GUIDE_PAGECOUNT);
  
  gStateClock = 1;
  guidePageInit();
}

// TODO, listen for input and draw sprites.
const void guideIdle() {
  int xOffs;
  OBJ_ATTR obj1 = {0, 0, 0, 0};
  OBJ_ATTR obj2 = {0, 0, 0, 0};
  int newPageID;
  const int horiHit = KEY_EQ(key_hit, KEY_RIGHT) - KEY_EQ(key_hit, KEY_LEFT);
  const int shoulderHit = KEY_EQ(key_hit, KEY_R) - KEY_EQ(key_hit, KEY_L);
  
  // Listen for input.
  if (horiHit) {                // Flip page.
    newPageID = guidePageID + horiHit;
    if (newPageID >= 0 && newPageID < GUIDE_PAGECOUNT)
      guidePageFlip(newPageID);
  } else if (shoulderHit) {     // Flip page.
    newPageID = guidePageID + shoulderHit;
    if (newPageID >= 0 && newPageID < GUIDE_PAGECOUNT)
      guidePageFlip(newPageID);
  } else if (key_hit(KEY_B) || key_hit(KEY_START)) {      // Exit guide.
    ; // TODO
  }
  
  // Run idle routines for page.
  guidePageIdle();
  
  // Draw arrow sprites & related button prompts.
  xOffs = ease(1, 3, ABS(16 - (gClock % 33)), 16, EASE_IN_QUADRATIC);
  obj1.attr0 = ATTR0_TALL | 148;
  obj1.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | 0x380;
  obj2.attr0 = ATTR0_SQUARE | 148;
  obj2.attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(0) | 0x39C;
  
  if (guidePageID > 0) {
    obj1.attr1 = ATTR1_SIZE_8x16 | ATTR1_HFLIP | (8 - xOffs);
    addToOAMBuffer(&obj1, 0);          // Left arrow sprite.
    obj2.attr1 = ATTR1_SIZE_8x8 | 18;
    addToOAMBuffer(&obj2, 0);          // Left button prompt.
  }
  if (guidePageID < GUIDE_PAGECOUNT - 1) {
    obj1.attr1 = ATTR1_SIZE_8x16 | (SCREEN_WIDTH - 16 + xOffs);
    addToOAMBuffer(&obj1, 0);          // Right arrow sprite.
    obj2.attr1 = ATTR1_SIZE_8x8 | ATTR1_HFLIP | (SCREEN_WIDTH - 26);
    addToOAMBuffer(&obj2, 0);          // Right button prompt.
  }

  gStateClock++;
}

const void guideUpdate() {
  switch(gGenericState) {
    case GUIDE_START:
      guideStart();
      break;
    case GUIDE_IDLE:
      guideIdle();
      break;
  }
}