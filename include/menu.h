#ifndef MENU
#define MENU
#include "lang.h"

#define MENU_MAXDEPTH 10

struct Menu {
  u8 bgIDText;                                      // Background layer which displays text.
  u8 bgIDBox;                                       // Background layer which displays the box.
  u8 xBox;                                          // Menubox dimensions.
  u8 yBox;                                          // ^.
  u8 wBox;                                          // ^.
  u8 hBox;                                          // ^.
  u8 palIDBox;
  u8 palIDText;
  u8 palIDCursor;
  u8 pad[3];
  
  const void (*onOpen)(const struct Menu* menu);    // Run routine when opening menu.
  const void (*onCancel)(const struct Menu* menu);  // Run routine on B-press.
  
  const void (*mapFunc)(const struct Menu* menu);   // Function that draws tilemap. If NULL, don't draw.
  COLOR* pal;
  int palLen;
  
  const struct MenuItem** const items;
};

struct MenuItem {
  const struct Menu* menu;                            // Menu this item is part of.
  char* name[LANG_COUNT];
  s16 x;                                              // Text offset.
  s16 y;                                              // ^.
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
extern const struct Menu optionsMenu;
extern const struct Menu puzMainMenu;
extern const struct Menu puzExitMenu;
extern const struct Menu puzSolvedMenu;
extern const struct Menu puzStageSolvedMenu;

const struct Menu* activeMenu();
const void menuClear();
const void menuClearTiles(const struct Menu* menu);
const void menuExit();
const void drawMenu(const struct Menu* menu);
const void mapDefaultMenu(const struct Menu* menu);
int addMenu(const struct Menu* menu);
const void drawMenuCursor(const struct MenuItem* mi, int xOffs, int yOffs);
const void drawMenuCursorFlip(const struct MenuItem* mi, int xOffs, int yOffs);
int runMenus();

#endif // MENU