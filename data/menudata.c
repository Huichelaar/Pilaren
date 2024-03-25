#include <stdio.h>
#include <tonc.h>
#include "lang.h"
#include "title.h"
#include "puzzle.h"
#include "guide.h"
#include "gfx/mainMenu.h"
#include "menu.h"

const struct MenuItem titleMenuItemStGm;
const struct MenuItem titleMenuItemCuGm;
const struct MenuItem titleMenuItemGu;
const struct MenuItem* const titleMenuItems[4] = {&titleMenuItemStGm,
                                                  &titleMenuItemCuGm,
                                                  &titleMenuItemGu, NULL};

const struct Menu titleMenu = {
  1,
  2,
  80,
  56,
  11,
  12,
  6,
  7,
  0,
  {0, 0, 0},
  &drawMenu,
  NULL,
  (void*)mainMenuTiles,
  mainMenuTilesLen,
  (SCR_ENTRY*)mainMenuMap,
  (COLOR*)mainMenuPal,
  4,
  (const struct MenuItem** const)&titleMenuItems,
};

const struct MenuItem titleMenuItemStGm = {
  &titleMenu,
  {"   Standaard spel\n", "   Standard game\n"},
  0,
  4,
  0,
  {0, 0, 0},
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  &titleMenuItemCuGm,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem titleMenuItemCuGm = {
  &titleMenu,
  {"   Aangepast spel\n", "   Custom game\n"},
  0,
  20,
  1,
  {0, 0, 0},
  NULL,
  &selectCuGm,
  NULL,
  NULL,
  &titleMenuItemStGm,
  &titleMenuItemGu,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem titleMenuItemGu = {
  &titleMenu,
  {"   Uitleg\n", "   Guide\n"},
  0,
  36,
  2,
  {0, 0, 0},
  NULL,
  &selectGu,
  NULL,
  NULL,
  &titleMenuItemCuGm,
  &titleMenuItemStGm,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem customGameMenuItemL;
const struct MenuItem customGameMenuItemB;
const struct MenuItem customGameMenuItemH;
const struct MenuItem customGameMenuStart;
const struct MenuItem customGameMenuExit;
const struct MenuItem* const customGameMenuItems[6] = {&customGameMenuItemL, &customGameMenuItemB, &customGameMenuItemH, &customGameMenuStart, &customGameMenuExit, NULL};

const struct Menu customGameMenu = {
  1,
  2,
  80,
  56,
  11,
  12,
  6,
  7,
  0,
  {0, 0, 0},
  &drawMenu,
  &menuExit,
  (void*)mainMenuTiles,
  mainMenuTilesLen,
  (SCR_ENTRY*)mainMenuMap,
  (COLOR*)mainMenuPal+3,
  4,
  (const struct MenuItem** const)&customGameMenuItems,
};

const struct MenuItem customGameMenuItemL = {
  &customGameMenu,
  {"   Lengte:\n", "   Length:\n"},
  69,
  4,
  0,
  {0, 0, 0},
  &hoverCuGmItems,
  NULL,
  NULL,
  NULL,
  NULL,
  &customGameMenuItemB,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem customGameMenuItemB = {
  &customGameMenu,
  {"   Breedte:\n", "   Breadth:\n"},
  69,
  20,
  1,
  {0, 0, 0},
  &hoverCuGmItems,
  NULL,
  NULL,
  NULL,
  &customGameMenuItemL,
  &customGameMenuItemH,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem customGameMenuItemH = {
  &customGameMenu,
  {"   Hoogte:\n", "   Height:\n"},
  69,
  36,
  2,
  {0, 0, 0},
  &hoverCuGmItems,
  NULL,
  NULL,
  NULL,
  &customGameMenuItemB,
  &customGameMenuStart,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem customGameMenuStart = {
  &customGameMenu,
  {"   Start spel\n", "   Start game\n"},
  0,
  52,
  3,
  {0, 0, 0},
  NULL,
  &puzzleInit,
  NULL,
  NULL,
  &customGameMenuItemH,
  &customGameMenuExit,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem customGameMenuExit = {
  &customGameMenu,
  {"   Terug\n", "   Exit\n"},
  0,
  68,
  4,
  {0, 0, 0},
  NULL,
  &menuExit2,
  NULL,
  NULL,
  &customGameMenuStart,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};