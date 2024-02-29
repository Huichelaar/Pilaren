#ifndef MENU
#define MENU
#include "lang.h"

#define MENU_MAXDEPTH 10

struct Menu {
  u8 bgIDText;                                      // Background layer which displays text.
  u8 bgIDBox;                                       // Background layer which displays the box.
  u8 x;
  u8 y;
  u8 width;
  u8 height;
  u8 palIDBox;
  u8 palIDText;
  u8 palIDCursor;
  u8 pad[3];
  
  const void (*onOpen)(const struct Menu* menu);    // Run routine when opening menu.
  const void (*onCancel)(const struct Menu* menu);  // Run routine on B-press.
  
  void* tiles;                                      // These params refer to the box.
  int tileLen;
  SCR_ENTRY* map;
  COLOR* pal;
  int palLen;
  
  const struct MenuItem** const items;
};

struct MenuItem {
  const struct Menu* menu;                            // Menu this item is part of.
  char* name[LANG_COUNT];
  s16 xOffs;
  s16 yOffs;
  u8 id;
  u8 pad[3];
  const void (*onHover)(const struct MenuItem* mi);   // Run this routine every frame.
  const void (*onSelect)(const struct MenuItem* mi);  // Run routine on A-press.
  const struct MenuItem* left;
  const struct MenuItem* right;
  const struct MenuItem* up;
  const struct MenuItem* down;
  const struct MenuItem* leftUp;
  const struct MenuItem* leftDown;
  const struct MenuItem* rightUp;
  const struct MenuItem* rightDown;
};
extern const struct MenuItem* gMenus[MENU_MAXDEPTH];

extern const struct Menu titleMenu;
extern const struct Menu customGameMenu;

const void menuClear();
const void menuExit(const struct Menu* menu);
const void menuExit2(const struct MenuItem* mi);
const void drawMenu(const struct Menu* menu);
int addMenu(const struct Menu* menu);
const void drawMenuCursor(const struct MenuItem* mi);
const void drawMenuCursorFlip(const struct MenuItem* mi);
int runMenus();


#endif // MENU