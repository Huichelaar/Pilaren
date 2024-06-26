#include <stdio.h>
#include <tonc.h>
#include "gfx/system.h"
#include "gfx/gmenu.h"
#include "lang.h"
#include "efx.h"
#include "lcdiobuffer.h"
#include "video.h"
#include "main.h"
#include "menu.h"

// Last non-NULL entry of this list is the currently active menu.
EWRAM_DATA const struct MenuItem* gMenus[MENU_MAXDEPTH];

// Returns active menu.
// Returns NULL if no active menu.
const struct Menu* activeMenu() {
  int i;
  
  for (i = 0; i < MENU_MAXDEPTH; i++)
    if (gMenus[i] == NULL) {
      if (i <= 0)
        return NULL;
      break;
    }
  return gMenus[i-1]->menu;
}

// Clear all menus.
const void menuClear() {
  for (int i = 0; i < MENU_MAXDEPTH; i++)
    gMenus[i] = NULL;
}

const void menuClearTiles(const struct Menu* menu) {
  u16 bgcnt;
  u8 cbb;
  
  // Clear menu text.
  bgcnt = *(&lcdioBuffer.bg0cnt + menu->bgIDText);
  cbb = (bgcnt & BG_CBB_MASK) >> BG_CBB_SHIFT;
  CpuFastFill(0, (void*)tile_mem[cbb], 0x12C0);
  
  // Clear menu box.
  if (menu->mapFunc != NULL) {
    bgcnt = *(&lcdioBuffer.bg0cnt + menu->bgIDBox);
    CpuFastFill(0, &bgmap[menu->bgIDBox], 0x200);
    setSyncBGMapFlagsByID(menu->bgIDBox);
  }
}

// Clear last-in menu.
// Re-open previous menu if there is one.
const void menuExit() {
  int i;
  const struct Menu* menu;
  
  // Clear last-in menu.
  for (i = MENU_MAXDEPTH-1; i > 0; i--)
    if (gMenus[i] != NULL) {
      break;
    }
  if (gMenus[i] != NULL) {
    menuClearTiles(gMenus[i]->menu);
    gMenus[i] = NULL;
  }
  
  // Re-open previous menu if there is one.
  menu = activeMenu();
  if (menu != NULL)
    menu->onOpen(menu);
}

// Draw menu box and text.
const void drawMenu(const struct Menu* menu) {
  u16 bgcnt;
  u8 sbb, cbb, i;
  
  // Clear menu tiles.
  menuClearTiles(menu);
  
  // Load cursor palette & tiles.
  CpuFastSet((void*)systemTiles, (void*)(&tile_mem_obj[1][384]), systemTilesLen>>2);
  loadColours((COLOR*)systemPal, (menu->palIDCursor+16)*16, systemPalLen>>1);
  setSyncPalFlagsByID(menu->palIDCursor+16);
  
  // Box tiles.
  if (menu->mapFunc != NULL)
    menu->mapFunc(menu);
  
  // Init menu item text.
  bgcnt = *(&lcdioBuffer.bg0cnt + menu->bgIDText);
  cbb = (bgcnt & BG_CBB_MASK) >> BG_CBB_SHIFT;
  sbb = (bgcnt & BG_SBB_MASK) >> BG_SBB_SHIFT;
  tte_init_chr4c(menu->bgIDText, BG_CBB(cbb)|BG_SBB(sbb), menu->palIDText << 12, bytes2word(3,2,0,0), CLR_WHITE, &verdana9_b4Font, (fnDrawg)chr4c_drawg_b4cts_fast);
  loadColours((COLOR*)systemPal, menu->palIDText*16, systemPalLen>>1);
  setSyncPalFlagsByID(menu->palIDText);
  
  // Draw menu item text.
  i = 0;
  while (menu->items[i] != NULL) {
    tte_set_pos(menu->items[i]->x, menu->items[i]->y);
    tte_write(menu->items[i]->name[gLang]);
    i++;
  }
}

// Draws tiles of menu based on width & height.
const void mapDefaultMenu(const struct Menu* menu) {
  u16* bgofs;
  u16 bgcnt;
  u8 cbb;
  const int bg = menu->bgIDBox;
  const int pal = SE_PALBANK(menu->palIDBox);
  int i, j;
  
  bgcnt = *(&lcdioBuffer.bg0cnt + bg);
  cbb = (bgcnt & BG_CBB_MASK) >> BG_CBB_SHIFT;
  
  // Move box BG HOFS and VOFS.
  bgofs = (&lcdioBuffer.bg0hofs + (bg << 1));
  *bgofs = -menu->xBox;
  bgofs = (&lcdioBuffer.bg0vofs + (bg << 1));
  *bgofs = -menu->yBox;
  
  // Load box tiles.
  CpuFastSet(menuTiles, (void*)tile_mem[cbb], menuTilesLen>>2);

  // Clear previous tiles.
  CpuFastFill(0, &bgmap[bg], 0x200);

  // Map box.
  // Top edge.
  bgmap[bg][0] = pal | 1;
  for (i = 1; i < (menu->wBox - 2); i++)
    bgmap[bg][i] = pal | 2;
  bgmap[bg][i] = pal | SE_HFLIP | 1;
  bgmap[bg][i + 1] = pal | 3;
  
  // Middle.
  for (j = 32; j < ((menu->hBox - 1) * 32); j += 32) {
    bgmap[bg][j] = pal | 4;
    for (i = 1; i < (menu->wBox - 2); i++)
      bgmap[bg][j + i] = pal | 5;
    bgmap[bg][j + i] = pal | SE_HFLIP | 4;
    bgmap[bg][j + i + 1] = pal | 6;
  }
  
  // Bottom edge.
  bgmap[bg][j] = pal | 7;
  for (i = 1; i < (menu->wBox - 1); i++)
    bgmap[bg][j + i] = pal | 8;
  bgmap[bg][j + i] = pal | 9;
  
  setSyncBGMapFlagsByID(bg);

  // Load box palette.
  loadColours(menu->pal, menu->palIDBox*16, menu->palLen);
  setSyncPalFlagsByID(menu->palIDBox);
}

// Add menu to end of menu list.
// Return false if menu could not be added.
// Return true otherwise.
int addMenu(const struct Menu* menu) {
  for (int i = 0; i < MENU_MAXDEPTH; i++) {
    if (gMenus[i] == NULL) {
      gMenus[i] = menu->items[0];
      if (menu->onOpen != NULL)
        menu->onOpen(menu);
      return true;
    }
  }
  return false;
}

const void drawMenuCursor(const struct MenuItem* mi, int xOffs, int yOffs) {
  OBJ_ATTR obj = {0, 0, 0, 0};
  
  int xWiggle = ease(1, 3, ABS(16 - (gClock % 33)), 16, EASE_IN_QUADRATIC);
  
  obj.attr0 = (mi->y + yOffs + 2) | ATTR0_TALL;
  obj.attr1 = (mi->x + xOffs + xWiggle) | ATTR1_SIZE_8x16;
  obj.attr2 = (mi->menu->palIDCursor << ATTR2_PALBANK_SHIFT) | 0x380;
  
  addToOAMBuffer(&obj, LAYER_COUNT - 1);
}

const void drawMenuCursorFlip(const struct MenuItem* mi, int xOffs, int yOffs) {
  OBJ_ATTR obj = {0, 0, 0, 0};
  
  int xWiggle = ease(3, 1, ABS(16 - (gClock % 33)), 16, EASE_IN_QUADRATIC);
  
  obj.attr0 = (mi->y + yOffs + 2) | ATTR0_TALL;
  obj.attr1 = (mi->x + xOffs + xWiggle) | ATTR1_HFLIP | ATTR1_SIZE_8x16;
  obj.attr2 = (mi->menu->palIDCursor << ATTR2_PALBANK_SHIFT) | 0x380;
  
  addToOAMBuffer(&obj, LAYER_COUNT - 1);
}

// Returns true if there is a menu and some input was registered.
// Return false if no menus present or no input was registered.
int runMenus() {
  int i, moved;
  const struct MenuItem** mi = NULL;
  
  // Find active menu.
  for (i = 0; i < MENU_MAXDEPTH; i++) {
    if (gMenus[i] == NULL) {
      if (i == 0)
        return false;     // No menus present.
      mi = &gMenus[i-1];
      break;
    }
  }
  if (i == MENU_MAXDEPTH)
    mi = &gMenus[i-1];
  
  // Run hover routine.
  if ((*mi)->onHover != NULL)
    (*mi)->onHover(*mi);
  else
    drawMenuCursor(*mi, 0, 0);
  
  // Read input. Directional pad.
  moved = moveCursorByInput((u32*)&(*mi),
                            (u32)(*mi)->left,
                            (u32)(*mi)->right,
                            (u32)(*mi)->up,
                            (u32)(*mi)->down,
                            (u32)(*mi)->leftUp,
                            (u32)(*mi)->leftDown,
                            (u32)(*mi)->rightUp,
                            (u32)(*mi)->rightDown);
  if (moved)
    return true;
  
  // Read input. Fire buttons.
  if (KEY_EQ(key_hit, KEY_A) && ((*mi)->onSelect != NULL)) {
    (*mi)->onSelect(*mi);
    return true;
  }
  if (KEY_EQ(key_hit, KEY_B) && ((*mi)->menu->onCancel != NULL)) {
    (*mi)->menu->onCancel((*mi)->menu);
    return true;
  }
  
  return false;
  
}