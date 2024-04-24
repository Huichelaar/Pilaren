#include <stdio.h>
#include <tonc.h>
#include "lang.h"
#include "title.h"
#include "puzzle.h"
#include "guide.h"
#include "options.h"
#include "gfx/mainMenu.h"
#include "gfx/exitMenu.h"
#include "menu.h"

const struct MenuItem titleMenuItemStGm;
const struct MenuItem titleMenuItemCuGm;
const struct MenuItem titleMenuItemGu;
const struct MenuItem titleMenuItemOp;
const struct MenuItem* const titleMenuItems[5] = {&titleMenuItemStGm,
                                                  &titleMenuItemCuGm,
                                                  &titleMenuItemGu,
                                                  &titleMenuItemOp, NULL};

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
  {"   Standaard spel", "   Standard game"},
  80,
  58,
  0,
  {0, 0, 0},
  NULL,
  NULL,
  NULL,
  NULL,
  &titleMenuItemGu,
  &titleMenuItemCuGm,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem titleMenuItemCuGm = {
  &titleMenu,
  {"   Aangepast spel", "   Custom game"},
  80,
  74,
  1,
  {0, 0, 0},
  NULL,
  &selectCuGm,
  NULL,
  NULL,
  &titleMenuItemStGm,
  &titleMenuItemOp,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem titleMenuItemOp = {
  &titleMenu,
  {"   Opties", "   Settings"},
  80,
  90,
  2,
  {0, 0, 0},
  NULL,
  &selectOp,
  NULL,
  NULL,
  &titleMenuItemCuGm,
  &titleMenuItemGu,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem titleMenuItemGu = {
  &titleMenu,
  {"   Uitleg", "   Guide"},
  80,
  106,
  2,
  {0, 0, 0},
  NULL,
  &selectGu,
  NULL,
  NULL,
  &titleMenuItemOp,
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
  {"   Lengte:", "   Length:"},
  80,
  58,
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
  {"   Breedte:", "   Breadth:"},
  80,
  74,
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
  {"   Hoogte:", "   Height:"},
  80,
  90,
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
  {"   Start spel", "   Start game"},
  80,
  106,
  3,
  {0, 0, 0},
  NULL,
  &puzInit,
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
  {"   Terug", "   Exit"},
  80,
  122,
  4,
  {0, 0, 0},
  NULL,
  &menuExit,
  NULL,
  NULL,
  &customGameMenuStart,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem optionsMenuItemHighlight;
const struct MenuItem optionsMenuItemMatch;
const struct MenuItem optionsMenuItemMusic;
const struct MenuItem optionsMenuItemSfx;
const struct MenuItem* const optionsMenuItems[5] = {&optionsMenuItemHighlight, &optionsMenuItemMatch, &optionsMenuItemMusic, &optionsMenuItemSfx, NULL};

const struct Menu optionsMenu = {
  0,
  0,
  0,
  0,
  30,
  20,
  0,
  7,
  0,
  {0, 0, 0},
  &drawMenu,
  &exitOp,
  NULL,
  0,
  NULL,
  NULL,
  0,
  (const struct MenuItem** const)&optionsMenuItems,
};

const struct MenuItem optionsMenuItemHighlight = {
  &optionsMenu,
  {"   Weergave geselecteerde pilaar:", "   Selected pillar display:"},
  0,
  16,
  0,
  {0, 0, 0},
  &hoverOpItems,
  NULL,
  NULL,
  NULL,
  NULL,
  &optionsMenuItemMatch,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem optionsMenuItemMatch = {
  &optionsMenu,
  {"   Weergave gelijke vlakken:", "   Matching pane display:"},
  0,
  48,
  1,
  {0, 0, 0},
  &hoverOpItems,
  NULL,
  NULL,
  NULL,
  &optionsMenuItemHighlight,
  &optionsMenuItemMusic,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem optionsMenuItemMusic = {
  &optionsMenu,
  {"   Niveau muziek:", "   Music:"},
  0,
  80,
  2,
  {0, 0, 0},
  &hoverOpItems,
  NULL,
  NULL,
  NULL,
  &optionsMenuItemMatch,
  &optionsMenuItemSfx,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem optionsMenuItemSfx = {
  &optionsMenu,
  {"   Niveau effecten", "   Sound effects"},
  0,
  112,
  3,
  {0, 0, 0},
  &hoverOpItems,
  NULL,
  NULL,
  NULL,
  &optionsMenuItemMusic,
  &optionsMenuItemHighlight,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem exitMenuItemNo;
const struct MenuItem exitMenuItemYes;
const struct MenuItem* const exitMenuItems[3] = {&exitMenuItemNo,
                                                 &exitMenuItemYes, NULL};

const struct Menu exitMenu = {
  0,
  1,
  42,
  58,
  20,
  6,
  6,
  7,
  0,
  {0, 0, 0},
  &puzDrawExitMenu,
  &puzCancelExit,
  (void*)mainMenuTiles,
  mainMenuTilesLen,
  (SCR_ENTRY*)exitMenuMap,
  (COLOR*)mainMenuPal,
  4,
  (const struct MenuItem** const)&exitMenuItems,
};

const struct MenuItem exitMenuItemNo = {
  &exitMenu,
  {"   Nee", "   No"},
  70,
  82,
  0,
  {0, 0, 0},
  NULL,
  &puzCancelExit,
  NULL,
  &exitMenuItemYes,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem exitMenuItemYes = {
  &exitMenu,
  {"   Ja", "   Yes"},
  134,
  82,
  0,
  {0, 0, 0},
  NULL,
  &puzExit,
  &exitMenuItemNo,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};