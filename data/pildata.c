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

// PIL_ANIM_HIDDEN.
const struct PillarTileData ptA0H0F0T0 = {(void*)0x0000, (void*)0x0000, 0, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarSpriteData ptA0H0F0 = {{0, 0, 0, 0}, &ptA0H0F0T0, NULL};

// PIL_ANIM_IDLE.
// HEIGHT 0.
const struct PillarTileData ptA1H0F0T1 = {(void*)0x0040, (void*)0x03A0, 2, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA1H0F0T0 = {(void*)0x0000, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA1H0F0T1};
const struct PillarSpriteData ptA1H0F0 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 0}, &ptA1H0F0T0, NULL};

// HEIGHT 1.
const struct PillarTileData ptA1H1F0T4 = {(void*)0x0F20, (void*)0x03C0, 1, {0, 0, 0, 6, 0, 0}, NULL};
const struct PillarTileData ptA1H1F0T3 = {(void*)0x0320, (void*)0x03A0, 1, {6, 0, 0, 0, 0, 0}, &ptA1H1F0T4};
const struct PillarTileData ptA1H1F0T2 = {(void*)0x0F00, (void*)0x0340, 1, {0, 0, 0, 6, 0, 0}, &ptA1H1F0T3};
const struct PillarTileData ptA1H1F0T1 = {(void*)0x0300, (void*)0x0320, 1, {6, 0, 0, 0, 0, 0}, &ptA1H1F0T2};
const struct PillarTileData ptA1H1F0T0 = {(void*)0x0000, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA1H1F0T1};
const struct PillarSpriteData ptA1H1F0 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 0}, &ptA1H1F0T0, NULL};

// HEIGHT 2.
const struct PillarTileData ptA1H2F0T6 = {(void*)0x0F20, (void*)0x03C0, 1, {0, 0, 0, 0, 6, 0}, NULL};
const struct PillarTileData ptA1H2F0T5 = {(void*)0x0320, (void*)0x03A0, 1, {0, 6, 0, 0, 0, 0}, &ptA1H2F0T6};
const struct PillarTileData ptA1H2F0T4 = {(void*)0x1600, (void*)0x0340, 1, {0, 0, 0, 7, 5, 0}, &ptA1H2F0T5};
const struct PillarTileData ptA1H2F0T3 = {(void*)0x0A00, (void*)0x0320, 1, {7, 5, 0, 0, 0, 0}, &ptA1H2F0T4};
const struct PillarTileData ptA1H2F0T2 = {(void*)0x0F00, (void*)0x02C0, 1, {0, 0, 0, 6, 0, 0}, &ptA1H2F0T3};
const struct PillarTileData ptA1H2F0T1 = {(void*)0x0300, (void*)0x02A0, 1, {6, 0, 0, 0, 0, 0}, &ptA1H2F0T2};
const struct PillarTileData ptA1H2F0T0 = {(void*)0x0000, (void*)0x0220, 2, {0, 0, 0, 0, 0, 0}, &ptA1H2F0T1};
const struct PillarSpriteData ptA1H2F0 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 0}, &ptA1H2F0T0, NULL};

// HEIGHT 3.
const struct PillarTileData ptA1H3F0T8 = {(void*)0x0F20, (void*)0x03C0, 1, {0, 0, 0, 0, 0, 6}, NULL};
const struct PillarTileData ptA1H3F0T7 = {(void*)0x0320, (void*)0x03A0, 1, {0, 0, 6, 0, 0, 0}, &ptA1H3F0T8};
const struct PillarTileData ptA1H3F0T6 = {(void*)0x1600, (void*)0x0340, 1, {0, 0, 0, 0, 7, 5}, &ptA1H3F0T7};
const struct PillarTileData ptA1H3F0T5 = {(void*)0x0A00, (void*)0x0320, 1, {0, 7, 5, 0, 0, 0}, &ptA1H3F0T6};
const struct PillarTileData ptA1H3F0T4 = {(void*)0x1600, (void*)0x02C0, 1, {0, 0, 0, 7, 5, 0}, &ptA1H3F0T5};
const struct PillarTileData ptA1H3F0T3 = {(void*)0x0A00, (void*)0x02A0, 1, {7, 5, 0, 0, 0, 0}, &ptA1H3F0T4};
const struct PillarTileData ptA1H3F0T2 = {(void*)0x0F00, (void*)0x0240, 1, {0, 0, 0, 6, 0, 0}, &ptA1H3F0T3};
const struct PillarTileData ptA1H3F0T1 = {(void*)0x0300, (void*)0x0220, 1, {6, 0, 0, 0, 0, 0}, &ptA1H3F0T2};
const struct PillarTileData ptA1H3F0T0 = {(void*)0x0000, (void*)0x01A0, 2, {0, 0, 0, 0, 0, 0}, &ptA1H3F0T1};
const struct PillarSpriteData ptA1H3F0 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 0}, &ptA1H3F0T0, NULL};

// PIL_ANIM_RAISE. TODO

// PIL_ANIM_TURN.
// HEIGHT 0.
// FRAME 3.
const struct PillarTileData ptA3H0F3T1 = {(void*)0x18C0, (void*)0x03A0, 2, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA3H0F3T0 = {(void*)0x1880, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA3H0F3T1};
const struct PillarSpriteData ptA3H0F3 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | ATTR1_HFLIP, 0, 5}, &ptA3H0F3T0, NULL};

// FRAME 2.
const struct PillarTileData ptA3H0F2T1 = {(void*)0x2740, (void*)0x03A0, 2, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA3H0F2T0 = {(void*)0x2700, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA3H0F2T1};
const struct PillarSpriteData ptA3H0F2 = {{ATTR0_TALL | (-3 & ATTR0_Y_MASK), ATTR1_SIZE(3) | 2, 0, 5}, &ptA3H0F2T0, &ptA3H0F3};

// FRAME 1.
const struct PillarSpriteData ptA3H0F1 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 5}, &ptA3H0F3T0, &ptA3H0F2};

// FRAME 0. We re-use PIL_ANIM_IDLE tiles for the first frame.
const struct PillarSpriteData ptA3H0F0 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 1}, &ptA1H0F0T0, &ptA3H0F1};

// HEIGHT 1.
// FRAME 3.
const struct PillarTileData ptA3H1F3T4 = {(void*)0x1A80, (void*)0x03C0, 1, {5, 0, 0, 7, 0, 0}, NULL};
const struct PillarTileData ptA3H1F3T3 = {(void*)0x1A00, (void*)0x03A0, 1, {5, 0, 0, 0, 0, 0}, &ptA3H1F3T4};
const struct PillarTileData ptA3H1F3T2 = {(void*)0x1980, (void*)0x0340, 1, {0, 0, 0, 5, 0, 0}, &ptA3H1F3T3};
const struct PillarTileData ptA3H1F3T1 = {(void*)0x1900, (void*)0x0320, 1, {5, 0, 0, 0, 0, 0}, &ptA3H1F3T2};
const struct PillarTileData ptA3H1F3T0 = {(void*)0x1880, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA3H1F3T1};
const struct PillarSpriteData ptA3H1F3 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | ATTR1_HFLIP, 0, 5}, &ptA3H1F3T0, NULL};

// FRAME 2.
const struct PillarTileData ptA3H1F2T3 = {(void*)0x2800, (void*)0x03C0, 1, {5, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA3H1F2T2 = {(void*)0x2780, (void*)0x03A0, 1, {5, 0, 0, 0, 0, 0}, &ptA3H1F2T3};
const struct PillarTileData ptA3H1F2T1 = {(void*)0x2740, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA3H1F2T2};
const struct PillarTileData ptA3H1F2T0 = {(void*)0x2700, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA3H1F2T1};
const struct PillarSpriteData ptA3H1F2 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | 2, 0, 5}, &ptA3H1F2T0, &ptA3H1F3};

// FRAME 1.
const struct PillarSpriteData ptA3H1F1 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 5}, &ptA3H1F3T0, &ptA3H1F2};

// FRAME 0.
const struct PillarSpriteData ptA3H1F0 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 1}, &ptA1H1F0T0, &ptA3H1F1};


// dim0 is animID, dim1 is height.
const struct PillarSpriteData* pilSpriteData[4][4] = {
  {&ptA0H0F0, &ptA0H0F0, &ptA0H0F0, &ptA0H0F0},
  {&ptA1H0F0, &ptA1H1F0, &ptA1H2F0, &ptA1H3F0},
  {NULL, NULL, NULL, NULL},
  {&ptA3H0F0, &ptA3H1F0, NULL, NULL}
};