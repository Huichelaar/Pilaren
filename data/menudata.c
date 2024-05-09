#include <stdio.h>
#include <tonc.h>
#include "lang.h"
#include "title.h"
#include "puzzle.h"
#include "guide.h"
#include "options.h"
#include "gfx/gmenu.h"
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
  &mapDefaultMenu,
  (COLOR*)menuPal,
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
  &puzInitDefault,
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
  &mapDefaultMenu,
  (COLOR*)menuPal+3,
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

const struct MenuItem puzMainMenuItemOptions;
const struct MenuItem puzMainMenuItemGuide;
const struct MenuItem puzMainMenuItemExit;
const struct MenuItem puzMainMenuItemBack;
const struct MenuItem* const puzMainMenuItems[5] = {&puzMainMenuItemOptions,
                                                    &puzMainMenuItemGuide,
                                                    &puzMainMenuItemExit,
                                                    &puzMainMenuItemBack, NULL};

const struct Menu puzMainMenu = {
  0,
  1,
  86,
  40,
  9,
  10,
  6,
  7,
  0,
  {0, 0, 0},
  &drawMenu,
  &puzMenuExit,
  &mapDefaultMenu,
  (COLOR*)menuPal,
  4,
  (const struct MenuItem** const)&puzMainMenuItems,
};

const struct MenuItem puzMainMenuItemOptions = {
  &puzMainMenu,
  {"   Opties", "   Settings"},
  86,
  42,
  0,
  {0, 0, 0},
  NULL,
  &selectOp,
  NULL,
  NULL,
  &puzMainMenuItemBack,
  &puzMainMenuItemGuide,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzMainMenuItemGuide = {
  &puzMainMenu,
  {"   Uitleg", "   Guide"},
  86,
  58,
  1,
  {0, 0, 0},
  NULL,
  &selectGu,
  NULL,
  NULL,
  &puzMainMenuItemOptions,
  &puzMainMenuItemExit,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzMainMenuItemExit = {
  &puzMainMenu,
  {"   Exit", "   Exit"},
  86,
  74,
  2,
  {0, 0, 0},
  NULL,
  &puzStartExitMenu,
  NULL,
  NULL,
  &puzMainMenuItemGuide,
  &puzMainMenuItemBack,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzMainMenuItemBack = {
  &puzMainMenu,
  {"   Terug", "   Back"},
  86,
  90,
  3,
  {0, 0, 0},
  NULL,
  &puzMenuExit,
  NULL,
  NULL,
  &puzMainMenuItemExit,
  &puzMainMenuItemOptions,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzExitMenuItemNo;
const struct MenuItem puzExitMenuItemYes;
const struct MenuItem* const puzExitMenuItems[3] = {&puzExitMenuItemNo,
                                                    &puzExitMenuItemYes, NULL};

const struct Menu puzExitMenu = {
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
  &puzMenuExit,
  &mapDefaultMenu,
  (COLOR*)menuPal,
  4,
  (const struct MenuItem** const)&puzExitMenuItems,
};

const struct MenuItem puzExitMenuItemNo = {
  &puzExitMenu,
  {"   Nee", "   No"},
  70,
  82,
  0,
  {0, 0, 0},
  NULL,
  &puzMenuExit,
  NULL,
  &puzExitMenuItemYes,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzExitMenuItemYes = {
  &puzExitMenu,
  {"   Ja", "   Yes"},
  134,
  82,
  1,
  {0, 0, 0},
  NULL,
  &puzExit,
  &puzExitMenuItemNo,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzSolvedMenuItemYes;
const struct MenuItem* const puzSolvedMenuItems[2] = {&puzSolvedMenuItemYes, NULL};

const struct Menu puzSolvedMenu = {
  0,
  1,
  6,
  6,
  15,
  8,
  6,
  7,
  1,
  {0, 0, 0},
  &puzDrawSolvedMenu,
  NULL,
  &mapDefaultMenu,
  (COLOR*)menuPal+6,
  4,
  (const struct MenuItem** const)&puzSolvedMenuItems,
};

const struct MenuItem puzSolvedMenuItemYes = {
  &puzSolvedMenu,
  {"   Ja", "   Yes"},
  14,
  48,
  0,
  {0, 0, 0},
  NULL,
  &puzExit,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzStageSolvedMenuItemYes;
const struct MenuItem puzStageSolvedMenuItemNo;
const struct MenuItem* const puzStageSolvedMenuItems[3] = {&puzStageSolvedMenuItemYes,
                                                           &puzStageSolvedMenuItemNo, NULL};

const struct Menu puzStageSolvedMenu = {
  0,
  1,
  6,
  6,
  15,
  8,
  6,
  7,
  1,
  {0, 0, 0},
  &puzDrawStageSolvedMenu,
  NULL,
  &mapDefaultMenu,
  (COLOR*)menuPal+6,
  4,
  (const struct MenuItem** const)&puzStageSolvedMenuItems,
};

const struct MenuItem puzStageSolvedMenuItemYes = {
  &puzStageSolvedMenu,
  {"   Ja", "   Yes"},
  14,
  48,
  0,
  {0, 0, 0},
  NULL,
  &puzNextStage,
  NULL,
  &puzStageSolvedMenuItemNo,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

const struct MenuItem puzStageSolvedMenuItemNo = {
  &puzStageSolvedMenu,
  {"   Nee", "   No"},
  46,
  48,
  0,
  {0, 0, 0},
  NULL,
  &puzStartExitMenu,
  &puzStageSolvedMenuItemYes,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};