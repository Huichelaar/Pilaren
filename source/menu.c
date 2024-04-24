#include <stdio.h>
#include <tonc.h>
#include "gfx/system.h"
#include "lang.h"
#include "efx.h"
#include "lcdiobuffer.h"
#include "video.h"
#include "main.h"
#include "menu.h"

// Last non-NULL entry of this list is the currently active menu.
EWRAM_DATA const struct MenuItem* gMenus[MENU_MAXDEPTH];

// Clear all menus.
const void menuClear() {
  for (int i = 0; i < MENU_MAXDEPTH; i++)
    gMenus[i] = NULL;
}

// Clear last-in menu.
// Re-open previous menu if there is one.
const void menuExit() {
  int i;
  
  // Clear last-in menu.
  for (i = 0; i < MENU_MAXDEPTH; i++)
    if (gMenus[i] == NULL) {
      if (i <= 0)
        return;
      break;
    }
  gMenus[i-1] = NULL;
  
  // Re-open previous menu if there is one.
  if (i >= 2)
    gMenus[i-2]->menu->onOpen(gMenus[i-2]->menu);
}

// Draw menu box and text.
const void drawMenu(const struct Menu* menu) {
  u16* bgofs;
  u16 bgcnt;
  u8 sbb, cbb, i;
  
  // Load cursor palette & tiles.
  CpuFastSet((void*)systemTiles, (void*)(&tile_mem_obj[1][384]), systemTilesLen>>2);
  loadColours((COLOR*)systemPal, (menu->palIDCursor+16)*16, systemPalLen>>1);
  setSyncPalFlagsByID(menu->palIDCursor+16);
  
  // Box.
  bgcnt = *(&lcdioBuffer.bg0cnt + menu->bgIDBox);
  cbb = (bgcnt & BG_CBB_MASK) >> BG_CBB_SHIFT;
  if (menu->tiles) {
    
    // Move box BG HOFS and VOFS.
    bgofs = (&lcdioBuffer.bg0hofs + (menu->bgIDBox<<1));
    *bgofs = -menu->xBox;
    bgofs = (&lcdioBuffer.bg0vofs + (menu->bgIDBox<<1));
    *bgofs = -menu->yBox;
    
    // Load box tiles.
    CpuFastSet(menu->tiles, (void*)tile_mem[cbb], menu->tileLen>>2);
  
    // Map box.
    mapTilemap(menu->map, bgmap[menu->bgIDBox], 0, 0, menu->wBox, menu->hBox, menu->palIDBox << 12);
    setSyncBGMapFlagsByID(menu->bgIDBox);
  
    // Load box palette.
    loadColours(menu->pal, menu->palIDBox*16, menu->palLen);
    setSyncPalFlagsByID(menu->palIDBox);
  }
  
  // Clear menu item tiles.
  bgcnt = *(&lcdioBuffer.bg0cnt + menu->bgIDText);
  cbb = (bgcnt & BG_CBB_MASK) >> BG_CBB_SHIFT;
  sbb = (bgcnt & BG_SBB_MASK) >> BG_SBB_SHIFT;
  CpuFastFill(0, (void*)tile_mem[cbb], 0x12C0);
  
  // Init menu item text.
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

// Returns true if there's at least one active menu.
// Returns false otherwise.
int isThereAnActiveMenu() {
  return (gMenus[0] != NULL);
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