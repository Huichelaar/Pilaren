#include <stdio.h>
#include <tonc.h>
#include "main.h"
#include "lcdioBuffer.h"
#include "video.h"
#include "efx.h"
#include "pillar.h"
#include "puzzle.h"
#include "title.h"
#include "menu.h"
#include "gfx/system.h"
#include "gfx/pilTitle.h"
#include "gfx/guide/guide.h"
#include "guide.h"

EWRAM_DATA u8 guidePageID;

const void guideStart() {
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
  tte_init_chr4c(0, BG_SBB(28), 7 << 12, bytes2word(3,2,0,0), CLR_WHITE, &verdana9_b4Font, (fnDrawg)chr4c_drawg_b4cts_fast);
  loadColours(systemPal, 7*16, systemPalLen>>1);
  
  // Set BG to dark red.
  setColour(12, 0);
  setSyncPalFlagsByMask(0x81);
  
  setGameState(gGameState, GUIDE_IDLE);
  
  // Draw first page's text.
  guidePageFlip(guidePageID);
}

// Run page-specific init routines.
const void guidePageInit() {
  int row, col, id;
  struct Pillar pil;
  const u8 clrs1[2][2][6] = {{{PIL_CLRID_BLUE, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_WHITE}}, {{PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE}}};
  const u8 clrs2[3][3][6] = {{{PIL_CLRID_RED, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_RED, PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_WHITE}}, {{PIL_CLRID_BLUE, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_BLUE, PIL_CLRID_BLUE, PIL_CLRID_WHITE}, {PIL_CLRID_RED, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_BLUE, PIL_CLRID_BLUE, PIL_CLRID_WHITE}, {PIL_CLRID_WHITE, PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_BLUE, PIL_CLRID_WHITE}}, {{PIL_CLRID_WHITE, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_RED, PIL_CLRID_WHITE, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_WHITE}, {PIL_CLRID_WHITE, PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_YELLOW, PIL_CLRID_WHITE, PIL_CLRID_WHITE}}};
  
  switch (guidePageID) {
    case 0:
      // Setup row/column outline of example pillar matrix.
      lcdioBuffer.bg1hofs = -152;
      lcdioBuffer.bg1vofs = -81;
      addToCopyOnVBlankQueue(page0Tiles, &tile_mem[2], page0TilesLen>>2, BUFFER_COPY);
      CpuFastFill(0, &bgmap[1], 0x400);
      mapTilemap(page0Map, bgmap[1], 0, 0, 9, 9, 0x7000);
      mapTilemap(page0Map + (page0MapLen >> 2), bgmap[2], 0, 0, 9, 9, 0x7000);
      setSyncBGMapFlagsByMask(0x6);
      
      // Setup example pillar matrix.
      puzInitPil(1, -170, -50);
      for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
          pil = pilConstr((u8*)clrs1[row][col], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
          id = puzAddPil(1, &pil);
          puzzle[1].pil[id].height = 1;
          pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
        }
      }
      
      // Draw additional numbering to rows & columns.
      if (!GUIDE_PRERENDERTEXT) {
        tte_set_pos(167, 78);
        tte_write("#{ci:13}1");
        tte_set_pos(150, 95);
        tte_write("2");
        tte_set_pos(201, 78);
        tte_write("#{ci:15}1");
        tte_set_pos(218, 95);
        tte_write("2");
      }
      break;
    case 1:
      // Setup <-- turn --> arrows.
      lcdioBuffer.bg1hofs = 2;
      lcdioBuffer.bg1vofs = -2;
      addToCopyOnVBlankQueue(page1Tiles, &tile_mem[2], page1TilesLen>>2, BUFFER_COPY);
      CpuFastFill(0, &bgmap[1], 0x200);
      mapTilemap(page1Map, bgmap[1], 8, 6, 14, 1, 0x7000);
      setSyncBGMapFlagsByID(1);
      
      // Example turning pillar.
      puzInitPil(1, -187, 35);
      pil = pilConstr((u8*)clrs1[1][1], 6, -10);
      id = puzAddPil(1, &pil);
      puzzle[1].pil[id].height = 1;
      pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
      
      // Example unsolved puzzle.
      for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
          pil = pilConstr((u8*)clrs1[row][col], (col - 3) * PIL_TILE_WIDTH - 6, (row + 6) * PIL_TILE_HEIGHT - 6);
          id = puzAddPil(1, &pil);
          puzzle[1].pil[id].height = 1;
          pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
        }
      }
      puzzle[1].pil[1].turned = true;
      
      // Solved puzzle variant.
      for (row = 0; row < 2; row++) {
        for (col = 0; col < 2; col++) {
          pil = pilConstr((u8*)clrs1[row][col], (col + 1) * PIL_TILE_WIDTH - 6, (row + 2) * PIL_TILE_HEIGHT - 6);
          id = puzAddPil(1, &pil);
          puzzle[1].pil[id].height = 1;
          pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
        }
      }
      
      // Draw additional numbering to rows & columns.
      /* Commented out as it seemed to provide too much clutter.
      if (!GUIDE_PRERENDERTEXT) {
        tte_set_pos(35, 38);
        tte_write("#{ci:13}1");
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
      } */
      break;
    case 2:
      // Setup row/column outline of example pillar matrix.
      lcdioBuffer.bg1hofs = -132;
      lcdioBuffer.bg1vofs = -55;
      addToCopyOnVBlankQueue(page2Tiles, &tile_mem[2], page2TilesLen>>2, BUFFER_COPY);
      CpuFastFill(0, &bgmap[1], 0x400);
      mapTilemap(page2Map, bgmap[1], 0, 0, 13, 13, 0x7000);
      mapTilemap(page2Map + (page2MapLen >> 2), bgmap[2], 0, 0, 13, 13, 0x7000);
      setSyncBGMapFlagsByMask(0x6);
      
      // Example multistorey pillar.
      puzInitPil(1, -167, -24);
      pil = pilConstr((u8*)clrs1[0][0], -5, -41);
      id = puzAddPil(1, &pil);
      puzzle[1].pil[id].height = 3;
      pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
      
      // Draw additional numbering to storeys.
      if (!GUIDE_PRERENDERTEXT) {
        tte_set_pos(230, 8);
        tte_write("#{ci:12}3");
        tte_set_pos(230, 16);
        tte_write("2");
        tte_set_pos(230, 24);
        tte_write("1");
      }
      
      // Example pillar matrix.
      for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
          pil = pilConstr((u8*)clrs2[row][col], col * PIL_TILE_WIDTH, row * PIL_TILE_HEIGHT);
          id = puzAddPil(1, &pil);
          puzzle[1].pil[id].height = 2;
          pilSetAnim(&puzzle[1].pil[id], PIL_ANIM_IDLE);
        }
      }
      
      // Draw additional numbering to rows & columns.
      if (!GUIDE_PRERENDERTEXT) {
        tte_set_pos(164, 52);
        tte_write("#{ci:13}1");
        tte_set_pos(147, 69);
        tte_write("2");
        tte_set_pos(130, 86);
        tte_write("3");
        tte_set_pos(198, 52);
        tte_write("#{ci:15}1");
        tte_set_pos(215, 69);
        tte_write("2");
        tte_set_pos(232, 86);
        tte_write("3");
      }
      break;
    case 3:
      CpuFastFill(0, &bgmap[1], 0x400);
      setSyncBGMapFlagsByMask(0x6);
  }
}

// Run page-specific idle routines.
const void guidePageIdle() {
  OBJ_ATTR obj = {0, 0, 0, 0};
  
  switch (guidePageID) {
    case 0:
      if (gStateClock % 180 == 0)
        lcdioBuffer.bg1vofs += 256;
    
      puzRunAnims(1);
      puzDrawAll(1);
      break;
    case 1:
      if (gStateClock % 120 == 0)
        pilSetAnim(&puzzle[1].pil[0], PIL_ANIM_TURN);
      
      puzRunAnims(1);
      puzDrawAll(1);
      break;
    case 2:
      if (gStateClock % 180 == 0)
        lcdioBuffer.bg1vofs += 256;
    
      puzRunAnims(1);
      puzDrawAll(1);
      break;
    case 3:
      
      // A.
      obj.attr0 = 0x1B;
      obj.attr1 = 0xE;
      obj.attr2 = 0x39A;        // A-Button.
      addToOAMBuffer(&obj, 0);
      
      // B.
      obj.attr0 = 0x27;
      obj.attr1 = 0xE;
      obj.attr2 = 0x39B;        // B-Button.
      addToOAMBuffer(&obj, 0);
      
      // D-Pad.
      obj.attr0 = 0x33;
      obj.attr1 = 0xE | ((gStateClock & 0x80) << 5) | ((gStateClock & 0x80) << 6);
      obj.attr2 = 0x39C + ((gStateClock & 0x40) >> 6);        // D-Pad.
      addToOAMBuffer(&obj, 0);
      
      // R + D-Pad.
      obj.attr0 = 0x3F;
      obj.attr1 = 0x6;
      obj.attr2 = 0x399;        // R-button.
      addToOAMBuffer(&obj, 0);
      obj.attr0 = 0x42;
      obj.attr1 = 0x10;
      obj.attr2 = 0x3A1;        // +.
      addToOAMBuffer(&obj, 0);
      obj.attr0 = 0x3F;
      obj.attr1 = 0x16 | (((gStateClock + 0x40) & 0x80) << 5) | ((gStateClock & 0x80) << 6);
      obj.attr2 = 0x39E;        // D-Pad.
      addToOAMBuffer(&obj, 0);
      
      // L + A.
      obj.attr0 = 0x4B;
      obj.attr1 = 0x6;
      obj.attr2 = 0x398;        // L-button.
      addToOAMBuffer(&obj, 0);
      obj.attr0 = 0x4E;
      obj.attr1 = 0x10;
      obj.attr2 = 0x3A1;        // +.
      addToOAMBuffer(&obj, 0);
      obj.attr0 = 0x4B;
      obj.attr1 = 0x16;
      obj.attr2 = 0x39A;        // A-Button.
      addToOAMBuffer(&obj, 0);
      
      // L + B.
      obj.attr0 = 0x57;
      obj.attr1 = 0x6;
      obj.attr2 = 0x398;        // L-button.
      addToOAMBuffer(&obj, 0);
      obj.attr0 = 0x5A;
      obj.attr1 = 0x10;
      obj.attr2 = 0x3A1;        // +.
      addToOAMBuffer(&obj, 0);
      obj.attr0 = 0x57;
      obj.attr1 = 0x16;
      obj.attr2 = 0x39B;        // B-Button.
      addToOAMBuffer(&obj, 0);
      
      // Select.
      obj.attr0 = 0x64 | ATTR0_WIDE;
      obj.attr1 = 0x2  | ATTR1_SIZE_32x8;
      obj.attr2 = 0x394;        // Select-Button.
      addToOAMBuffer(&obj, 0);
      
      // Start.
      obj.attr0 = 0x70 | ATTR0_WIDE;
      obj.attr1 = 0x2  | ATTR1_SIZE_32x8;
      obj.attr2 = 0x390;        // Start-Button.
      addToOAMBuffer(&obj, 0);
      
  }
}

// Change page to id page.
const void guidePageFlip(int id) {
  // Set global pageID var.
  guidePageID = id;
  
  if (GUIDE_PRERENDERTEXT) {
    addToCopyOnVBlankQueue(guidePageTextTiles[id][gLang], (void*)tile8_mem, 19456>>2, BUFFER_COPY);
    CpuFastSet(&guidePageTextMap, bgmap[0], 320);
    setSyncBGMapFlagsByID(0);
  } else {
    // Clear text.
    chr4c_erase(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    tte_set_pos(0, 0);
    
    // Draw page's text.
    tte_write(guidePageText[guidePageID][gLang]);
    
    // Draw page number.
    tte_set_pos(112, 148);
    tte_putc('1' + guidePageID);
    tte_putc('/');
    tte_putc('0' + GUIDE_PAGECOUNT);
  }
  
  gStateClock = 1;
  guidePageInit();
}

// Listen for input and draw sprites.
const void guideIdle() {
  int xOffs;
  OBJ_ATTR obj1 = {0, 0, 0, 0};
  OBJ_ATTR obj2 = {0, 0, 0, 0};
  int newPageID;
  const int horiHit = KEY_EQ(key_hit, KEY_RIGHT) - KEY_EQ(key_hit, KEY_LEFT);
  const int shoulderHit = KEY_EQ(key_hit, KEY_R) - KEY_EQ(key_hit, KEY_L);
  
  lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
  
  // Listen for input.
  if (horiHit) {                // Flip page.
    if (!GUIDE_PRERENDERTEXT)
      lcdioBuffer.dispcnt = 0;
    newPageID = guidePageID + horiHit;
    if (newPageID >= 0 && newPageID < GUIDE_PAGECOUNT)
      guidePageFlip(newPageID);
  } else if (shoulderHit) {     // Flip page.
    if (!GUIDE_PRERENDERTEXT)
      lcdioBuffer.dispcnt = 0;
    newPageID = guidePageID + shoulderHit;
    if (newPageID >= 0 && newPageID < GUIDE_PAGECOUNT)
      guidePageFlip(newPageID);
  } else if (key_hit(KEY_B) || key_hit(KEY_START)) {      // Exit guide.
    lcdioBuffer.dispcnt = 0;
    setGameState(gGameState, GUIDE_RETURN);
    return;
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

const void guideReturn() {
  const struct Menu* menu;
  
  if (gPrevGameState == GAME_TITLE) {
    
    // Set BG-control.
    lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
    lcdioBuffer.bg1cnt = BG_BUILD(2, 29, 2, 0, 1, 0, 0);
    lcdioBuffer.bg2cnt = BG_BUILD(1, 30, 2, 0, 2, 0, 0);
    lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 8, 3, 0, 0);
    lcdioBuffer.bg1hofs = 0;
    lcdioBuffer.bg1vofs = 0;
    lcdioBuffer.bg3vofs = -6;
    
    // Setup dispcnt.
    lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_OBJ_1D | DCNT_BG1 | DCNT_BG2 | DCNT_BG3 | DCNT_OBJ;
    
    // Set BG to 0xDEAD.
    setColour(CLR_DEAD, 0);
    setSyncPalFlagsByID(0);
    
    // Re-draw menu.
    menu = activeMenu();
    if (menu != NULL)
      menu->onOpen(menu);
    
    // Load Title tiles, tilemap.
    CpuFastSet((void*)pilTitleTiles, (void*)tile8_mem, pilTitleTilesLen>>2);
    CpuFastSet((void*)pilTitleMap, (void*)bgmap[3], pilTitleMapLen>>2);
    setSyncBGMapFlagsByID(3);
    
    // Re-load flavour pillar tiles.
    puzReloadTiles(0);
    
    setGameState(GAME_TITLE, TITLE_INPUT);
    
  } else if (gPrevGameState == GAME_PUZZLE)  {
    
    // Set BG-control.
    lcdioBuffer.bg0cnt = BG_BUILD(0, 28, 0, 0, 0, 0, 0);
    lcdioBuffer.bg1cnt = BG_BUILD(2, 29, 0, 0, 0, 0, 0);
    lcdioBuffer.bg2cnt = BG_BUILD(0, 30, 0, 0, 2, 0, 0);
    lcdioBuffer.bg3cnt = BG_BUILD(0, 31, 0, 0, 3, 0, 0);
    
    // Setup dispcnt.
    lcdioBuffer.dispcnt = DCNT_MODE0 | DCNT_OBJ_1D | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;
    
    // Clear background tiles & screen entries.
    CBB_CLEAR(0);
    CBB_CLEAR(1);
    CBB_CLEAR(2);
    CBB_CLEAR(3);
    CpuFastFill(0, &bgmap[0], 0x800);
    
    // Re-load button prompt text.
    puzLoadButtonPrompts();
    
    // Set BG to 0xDEAD.
    setColour(CLR_DEAD, 0);
    setSyncPalFlagsByID(0);
    
    // Load pal for solid colour option.
    if ((puzDispOptions & 0x0F) == PUZDISP_CURSOR3) {  
      setColour(CLR_MAGENTA, 273);
      setSyncPalFlagsByID(17);
    }
    
    // Re-load pillar tiles.
    puzReloadTiles(0);
    
    // Re-draw menu.
    menu = activeMenu();
    if (menu != NULL)
      menu->onOpen(menu);
    
    setGameState(GAME_PUZZLE, PUZZLE_IDLE);
  }
}

const void guideUpdate() {
  switch(gGenericState) {
    case GUIDE_START:
      guideStart();
      break;
    case GUIDE_IDLE:
      guideIdle();
      break;
    case GUIDE_RETURN:
      guideReturn();
      break;
  }
}