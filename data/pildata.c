#include <stdio.h>
#include <tonc.h>
#include "gfx/pil.h"
#include "pillar.h"

/*
Pillar coloured pane order:

     /      \
    | 0 || 3 |
    | 1 || 4 |
    | 2 || 5 |

*/

const struct PillarTileData ptA0H0F0T0 = {(void*)0x0000, (void*)0x0000, 0, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarSpriteData ptA0H0F0 = {{0, 0, 0, 0}, &ptA0H0F0T0, NULL};

const struct PillarTileData ptA1H0F0T1 = {(void*)0x0040, (void*)0x00A0, 2, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA1H0F0T0 = {(void*)0x0000, (void*)0x0020, 2, {0, 0, 0, 0, 0, 0}, &ptA1H0F0T1};
const struct PillarSpriteData ptA1H0F0 = {{0, 0, 0, 0}, &ptA1H0F0T0, NULL};

const struct PillarTileData ptA1H1F0T4 = {(void*)0x0F20, (void*)0x0140, 1, {0, 0, 0, 6, 0, 0}, NULL};
const struct PillarTileData ptA1H1F0T3 = {(void*)0x0320, (void*)0x0120, 1, {6, 0, 0, 0, 0, 0}, &ptA1H1F0T4};
const struct PillarTileData ptA1H1F0T2 = {(void*)0x0F00, (void*)0x00C0, 1, {0, 0, 0, 6, 0, 0}, &ptA1H1F0T3};
const struct PillarTileData ptA1H1F0T1 = {(void*)0x0300, (void*)0x00A0, 1, {6, 0, 0, 0, 0, 0}, &ptA1H1F0T2};
const struct PillarTileData ptA1H1F0T0 = {(void*)0x0000, (void*)0x0020, 2, {0, 0, 0, 0, 0, 0}, &ptA1H1F0T1};
const struct PillarSpriteData ptA1H1F0 = {{0, 0, 0, 0}, &ptA1H1F0T0, NULL};

const struct PillarTileData ptA1H2F0T6 = {(void*)0x0F20, (void*)0x01C0, 1, {0, 0, 0, 0, 6, 0}, NULL};
const struct PillarTileData ptA1H2F0T5 = {(void*)0x0320, (void*)0x01A0, 1, {0, 6, 0, 0, 0, 0}, &ptA1H2F0T6};
const struct PillarTileData ptA1H2F0T4 = {(void*)0x1600, (void*)0x0140, 1, {0, 0, 0, 7, 5, 0}, &ptA1H2F0T5};
const struct PillarTileData ptA1H2F0T3 = {(void*)0x0A00, (void*)0x0120, 1, {7, 5, 0, 0, 0, 0}, &ptA1H2F0T4};
const struct PillarTileData ptA1H2F0T2 = {(void*)0x0F00, (void*)0x00C0, 1, {0, 0, 0, 6, 0, 0}, &ptA1H2F0T3};
const struct PillarTileData ptA1H2F0T1 = {(void*)0x0300, (void*)0x00A0, 1, {6, 0, 0, 0, 0, 0}, &ptA1H2F0T2};
const struct PillarTileData ptA1H2F0T0 = {(void*)0x0000, (void*)0x0020, 2, {0, 0, 0, 0, 0, 0}, &ptA1H2F0T1};
const struct PillarSpriteData ptA1H2F0 = {{0, 0, 0, 0}, &ptA1H2F0T0, NULL};

const struct PillarTileData ptA1H3F0T8 = {(void*)0x0F20, (void*)0x0240, 1, {0, 0, 0, 0, 0, 6}, NULL};
const struct PillarTileData ptA1H3F0T7 = {(void*)0x0320, (void*)0x0220, 1, {0, 0, 6, 0, 0, 0}, &ptA1H3F0T8};
const struct PillarTileData ptA1H3F0T6 = {(void*)0x1600, (void*)0x01C0, 1, {0, 0, 0, 0, 7, 5}, &ptA1H3F0T7};
const struct PillarTileData ptA1H3F0T5 = {(void*)0x0A00, (void*)0x01A0, 1, {0, 7, 5, 0, 0, 0}, &ptA1H3F0T6};
const struct PillarTileData ptA1H3F0T4 = {(void*)0x1600, (void*)0x0140, 1, {0, 0, 0, 7, 5, 0}, &ptA1H3F0T5};
const struct PillarTileData ptA1H3F0T3 = {(void*)0x0A00, (void*)0x0120, 1, {7, 5, 0, 0, 0, 0}, &ptA1H3F0T4};
const struct PillarTileData ptA1H3F0T2 = {(void*)0x0F00, (void*)0x00C0, 1, {0, 0, 0, 6, 0, 0}, &ptA1H3F0T3};
const struct PillarTileData ptA1H3F0T1 = {(void*)0x0300, (void*)0x00A0, 1, {6, 0, 0, 0, 0, 0}, &ptA1H3F0T2};
const struct PillarTileData ptA1H3F0T0 = {(void*)0x0000, (void*)0x0020, 2, {0, 0, 0, 0, 0, 0}, &ptA1H3F0T1};
const struct PillarSpriteData ptA1H3F0 = {{0, 0, 0, 0}, &ptA1H3F0T0, NULL};

// dim0 is animID, dim1 is height.
const struct PillarSpriteData* pilSpriteData[4][4] = {
  {&ptA0H0F0, &ptA0H0F0, &ptA0H0F0, &ptA0H0F0},
  {&ptA1H0F0, &ptA1H1F0, &ptA1H2F0, &ptA1H3F0},
  {NULL, NULL, NULL, NULL},
  {NULL, NULL, NULL, NULL}
};