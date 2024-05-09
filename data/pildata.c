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
const struct PillarSpriteData ptA1H0F0 = {{ATTR0_TALL | (-1 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 0}, &ptA1H0F0T0, NULL};

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

// PIL_ANIM_RAISE.
// HEIGHT 0.
// FRAME 4. Over-correct.
const struct PillarTileData ptA2H0F4T4 = {(void*)0x0E20, (void*)0x03C0, 1, {0, 0, 0, 6, 0, 0}, NULL};
const struct PillarTileData ptA2H0F4T3 = {(void*)0x0220, (void*)0x03A0, 1, {6, 0, 0, 0, 0, 0}, &ptA2H0F4T4};
const struct PillarTileData ptA2H0F4T2 = {(void*)0x0E00, (void*)0x0340, 1, {0, 0, 0, 6, 0, 0}, &ptA2H0F4T3};
const struct PillarTileData ptA2H0F4T1 = {(void*)0x0200, (void*)0x0320, 1, {6, 0, 0, 0, 0, 0}, &ptA2H0F4T2};
const struct PillarTileData ptA2H0F4T0 = {(void*)0x0000, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA2H0F4T1};
const struct PillarSpriteData ptA2H0F4 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 3}, &ptA2H0F4T0, NULL};

// FRAME 3. Jut out too far.
const struct PillarTileData ptA2H0F3T4 = {(void*)0x1000, (void*)0x03C0, 1, {0, 0, 0, 7, 5, 0}, NULL};
const struct PillarTileData ptA2H0F3T3 = {(void*)0x0400, (void*)0x03A0, 1, {7, 5, 0, 0, 0, 0}, &ptA2H0F3T4};
const struct PillarTileData ptA2H0F3T2 = {(void*)0x0F00, (void*)0x0340, 1, {0, 0, 0, 6, 0, 0}, &ptA2H0F3T3};
const struct PillarTileData ptA2H0F3T1 = {(void*)0x0300, (void*)0x0320, 1, {6, 0, 0, 0, 0, 0}, &ptA2H0F3T2};
const struct PillarTileData ptA2H0F3T0 = {(void*)0x0000, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA2H0F3T1};
const struct PillarSpriteData ptA2H0F3 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 6}, &ptA2H0F3T0, &ptA2H0F4};

// FRAME 2. Re-use height+1's PIL_ANIM_IDLE tiles this frame.
const struct PillarSpriteData ptA2H0F2 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 3}, &ptA1H1F0T0, &ptA2H0F3};

// FRAME 1.
const struct PillarTileData ptA2H0F1T4 = {(void*)0x0D20, (void*)0x03C0, 1, {0, 0, 0, 6, 0, 0}, NULL};
const struct PillarTileData ptA2H0F1T3 = {(void*)0x0120, (void*)0x03A0, 1, {6, 0, 0, 0, 0, 0}, &ptA2H0F1T4};
const struct PillarTileData ptA2H0F1T2 = {(void*)0x0D00, (void*)0x0340, 1, {0, 0, 0, 6, 0, 0}, &ptA2H0F1T3};
const struct PillarTileData ptA2H0F1T1 = {(void*)0x0100, (void*)0x0320, 1, {6, 0, 0, 0, 0, 0}, &ptA2H0F1T2};
const struct PillarTileData ptA2H0F1T0 = {(void*)0x0000, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA2H0F1T1};
const struct PillarSpriteData ptA2H0F1 = {{ATTR0_TALL | 4, ATTR1_SIZE(3), 0, 3}, &ptA2H0F1T0, &ptA2H0F2};

// FRAME 0.
const struct PillarTileData ptA2H0F0T2 = {(void*)0x0C80, (void*)0x03C0, 1, {0, 0, 0, 5, 0, 0}, NULL};
const struct PillarTileData ptA2H0F0T1 = {(void*)0x0080, (void*)0x03A0, 1, {5, 0, 0, 0, 0, 0}, &ptA2H0F0T2};
const struct PillarTileData ptA2H0F0T0 = {(void*)0x0000, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA2H0F0T1};
const struct PillarSpriteData ptA2H0F0 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 3}, &ptA2H0F0T0, &ptA2H0F1};

// HEIGHT 1.
// FRAME 4. Over-correct.
const struct PillarTileData ptA2H1F4T6 = {(void*)0x0E20, (void*)0x03C0, 1, {0, 0, 0, 0, 6, 0}, NULL};
const struct PillarTileData ptA2H1F4T5 = {(void*)0x0220, (void*)0x03A0, 1, {0, 6, 0, 0, 0, 0}, &ptA2H1F4T6};
const struct PillarTileData ptA2H1F4T4 = {(void*)0x1400, (void*)0x0340, 1, {0, 0, 0, 7, 5, 0}, &ptA2H1F4T5};
const struct PillarTileData ptA2H1F4T3 = {(void*)0x0800, (void*)0x0320, 1, {7, 5, 0, 0, 0, 0}, &ptA2H1F4T4};
const struct PillarTileData ptA2H1F4T2 = {(void*)0x0F00, (void*)0x02C0, 1, {0, 0, 0, 6, 0, 0}, &ptA2H1F4T3};
const struct PillarTileData ptA2H1F4T1 = {(void*)0x0300, (void*)0x02A0, 1, {6, 0, 0, 0, 0, 0}, &ptA2H1F4T2};
const struct PillarTileData ptA2H1F4T0 = {(void*)0x0000, (void*)0x0220, 2, {0, 0, 0, 0, 0, 0}, &ptA2H1F4T1};
const struct PillarSpriteData ptA2H1F4 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 3}, &ptA2H1F4T0, NULL};

// FRAME 3. Jut out too far.
const struct PillarTileData ptA2H1F3T6 = {(void*)0x1000, (void*)0x03C0, 1, {0, 0, 0, 0, 7, 5}, NULL};
const struct PillarTileData ptA2H1F3T5 = {(void*)0x0400, (void*)0x03A0, 1, {0, 7, 5, 0, 0, 0}, &ptA2H1F3T6};
const struct PillarTileData ptA2H1F3T4 = {(void*)0x1600, (void*)0x0340, 1, {0, 0, 0, 7, 5, 0}, &ptA2H1F3T5};
const struct PillarTileData ptA2H1F3T3 = {(void*)0x0A00, (void*)0x0320, 1, {7, 5, 0, 0, 0, 0}, &ptA2H1F3T4};
const struct PillarTileData ptA2H1F3T2 = {(void*)0x0F00, (void*)0x02C0, 1, {0, 0, 0, 6, 0, 0}, &ptA2H1F3T3};
const struct PillarTileData ptA2H1F3T1 = {(void*)0x0300, (void*)0x02A0, 1, {6, 0, 0, 0, 0, 0}, &ptA2H1F3T2};
const struct PillarTileData ptA2H1F3T0 = {(void*)0x0000, (void*)0x0220, 2, {0, 0, 0, 0, 0, 0}, &ptA2H1F3T1};
const struct PillarSpriteData ptA2H1F3 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 6}, &ptA2H1F3T0, &ptA2H1F4};

// FRAME 2. Re-use height+1's PIL_ANIM_IDLE tiles this frame.
const struct PillarSpriteData ptA2H1F2 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 3}, &ptA1H2F0T0, &ptA2H1F3};

// FRAME 1.
const struct PillarTileData ptA2H1F1T6 = {(void*)0x0D20, (void*)0x03C0, 1, {0, 0, 0, 0, 6, 0}, NULL};
const struct PillarTileData ptA2H1F1T5 = {(void*)0x0120, (void*)0x03A0, 1, {0, 6, 0, 0, 0, 0}, &ptA2H1F1T6};
const struct PillarTileData ptA2H1F1T4 = {(void*)0x1200, (void*)0x0340, 1, {0, 0, 0, 7, 5, 0}, &ptA2H1F1T5};
const struct PillarTileData ptA2H1F1T3 = {(void*)0x0600, (void*)0x0320, 1, {7, 5, 0, 0, 0, 0}, &ptA2H1F1T4};
const struct PillarTileData ptA2H1F1T2 = {(void*)0x0F00, (void*)0x02C0, 1, {0, 0, 0, 6, 0, 0}, &ptA2H1F1T3};
const struct PillarTileData ptA2H1F1T1 = {(void*)0x0300, (void*)0x02A0, 1, {6, 0, 0, 0, 0, 0}, &ptA2H1F1T2};
const struct PillarTileData ptA2H1F1T0 = {(void*)0x0000, (void*)0x0220, 2, {0, 0, 0, 0, 0, 0}, &ptA2H1F1T1};
const struct PillarSpriteData ptA2H1F1 = {{ATTR0_TALL | 4, ATTR1_SIZE(3), 0, 3}, &ptA2H1F1T0, &ptA2H1F2};

// FRAME 0. Re-use previous' height's jut-out-too-far frame.
const struct PillarSpriteData ptA2H1F0 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 3}, &ptA2H0F3T0, &ptA2H1F1};

// HEIGHT 2.
// FRAME 4. Over-correct.
const struct PillarTileData ptA2H2F4T8 = {(void*)0x0E20, (void*)0x03C0, 1, {0, 0, 0, 0, 0, 6}, NULL};
const struct PillarTileData ptA2H2F4T7 = {(void*)0x0220, (void*)0x03A0, 1, {0, 0, 6, 0, 0, 0}, &ptA2H2F4T8};
const struct PillarTileData ptA2H2F4T6 = {(void*)0x1400, (void*)0x0340, 1, {0, 0, 0, 0, 7, 5}, &ptA2H2F4T7};
const struct PillarTileData ptA2H2F4T5 = {(void*)0x0800, (void*)0x0320, 1, {0, 7, 5, 0, 0, 0}, &ptA2H2F4T6};
const struct PillarTileData ptA2H2F4T4 = {(void*)0x1600, (void*)0x02C0, 1, {0, 0, 0, 7, 5, 0}, &ptA2H2F4T5};
const struct PillarTileData ptA2H2F4T3 = {(void*)0x0A00, (void*)0x02A0, 1, {7, 5, 0, 0, 0, 0}, &ptA2H2F4T4};
const struct PillarTileData ptA2H2F4T2 = {(void*)0x0F00, (void*)0x0240, 1, {0, 0, 0, 6, 0, 0}, &ptA2H2F4T3};
const struct PillarTileData ptA2H2F4T1 = {(void*)0x0300, (void*)0x0220, 1, {6, 0, 0, 0, 0, 0}, &ptA2H2F4T2};
const struct PillarTileData ptA2H2F4T0 = {(void*)0x0000, (void*)0x01A0, 2, {0, 0, 0, 0, 0, 0}, &ptA2H2F4T1};
const struct PillarSpriteData ptA2H2F4 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 3}, &ptA2H2F4T0, NULL};

// FRAME 3. Jut out too far.
const struct PillarTileData ptA2H2F3T8 = {(void*)0x1800, (void*)0x03C0, 1, {0, 0, 0, 0, 0, 5}, NULL};
const struct PillarTileData ptA2H2F3T7 = {(void*)0x0C00, (void*)0x03A0, 1, {0, 0, 5, 0, 0, 0}, &ptA2H2F3T8};
const struct PillarTileData ptA2H2F3T6 = {(void*)0x1600, (void*)0x0340, 1, {0, 0, 0, 0, 7, 5}, &ptA2H2F3T7};
const struct PillarTileData ptA2H2F3T5 = {(void*)0x0A00, (void*)0x0320, 1, {0, 7, 5, 0, 0, 0}, &ptA2H2F3T6};
const struct PillarTileData ptA2H2F3T4 = {(void*)0x1600, (void*)0x02C0, 1, {0, 0, 0, 7, 5, 0}, &ptA2H2F3T5};
const struct PillarTileData ptA2H2F3T3 = {(void*)0x0A00, (void*)0x02A0, 1, {7, 5, 0, 0, 0, 0}, &ptA2H2F3T4};
const struct PillarTileData ptA2H2F3T2 = {(void*)0x0F00, (void*)0x0240, 1, {0, 0, 0, 6, 0, 0}, &ptA2H2F3T3};
const struct PillarTileData ptA2H2F3T1 = {(void*)0x0300, (void*)0x0220, 1, {6, 0, 0, 0, 0, 0}, &ptA2H2F3T2};
const struct PillarTileData ptA2H2F3T0 = {(void*)0x0000, (void*)0x01A0, 2, {0, 0, 0, 0, 0, 0}, &ptA2H2F3T1};
const struct PillarSpriteData ptA2H2F3 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 6}, &ptA2H2F3T0, &ptA2H2F4};

// FRAME 2. Re-use height+1's PIL_ANIM_IDLE tiles this frame.
const struct PillarSpriteData ptA2H2F2 = {{ATTR0_TALL, ATTR1_SIZE(3), 0, 3}, &ptA1H3F0T0, &ptA2H2F3};

// FRAME 1.
const struct PillarTileData ptA2H2F1T8 = {(void*)0x0D20, (void*)0x03C0, 1, {0, 0, 0, 0, 0, 6}, NULL};
const struct PillarTileData ptA2H2F1T7 = {(void*)0x0120, (void*)0x03A0, 1, {0, 0, 6, 0, 0, 0}, &ptA2H2F1T8};
const struct PillarTileData ptA2H2F1T6 = {(void*)0x1200, (void*)0x0340, 1, {0, 0, 0, 0, 7, 5}, &ptA2H2F1T7};
const struct PillarTileData ptA2H2F1T5 = {(void*)0x0600, (void*)0x0320, 1, {0, 7, 5, 0, 0, 0}, &ptA2H2F1T6};
const struct PillarTileData ptA2H2F1T4 = {(void*)0x1600, (void*)0x02C0, 1, {0, 0, 0, 7, 5, 0}, &ptA2H2F1T5};
const struct PillarTileData ptA2H2F1T3 = {(void*)0x0A00, (void*)0x02A0, 1, {7, 5, 0, 0, 0, 0}, &ptA2H2F1T4};
const struct PillarTileData ptA2H2F1T2 = {(void*)0x0F00, (void*)0x0240, 1, {0, 0, 0, 6, 0, 0}, &ptA2H2F1T3};
const struct PillarTileData ptA2H2F1T1 = {(void*)0x0300, (void*)0x0220, 1, {6, 0, 0, 0, 0, 0}, &ptA2H2F1T2};
const struct PillarTileData ptA2H2F1T0 = {(void*)0x0000, (void*)0x01A0, 2, {0, 0, 0, 0, 0, 0}, &ptA2H2F1T1};
const struct PillarSpriteData ptA2H2F1 = {{ATTR0_TALL | 4, ATTR1_SIZE(3), 0, 3}, &ptA2H2F1T0, &ptA2H2F2};

// FRAME 0. Re-use previous' height's jut-out-too-far frame.
const struct PillarSpriteData ptA2H2F0 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 3}, &ptA2H1F3T0, &ptA2H2F1};


// PIL_ANIM_LOWER. Re-use PIL_ANIM_RAISE frames.
// HEIGHT 1.
// FRAME 1.
const struct PillarSpriteData ptA3H1F1 = {{ATTR0_TALL | 4, ATTR1_SIZE(3), 0, 5}, &ptA2H0F1T0, NULL};

// FRAME 0.
const struct PillarSpriteData ptA3H1F0 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 6}, &ptA2H0F4T0, &ptA3H1F1};

// HEIGHT 2.
// FRAME 2.
const struct PillarSpriteData ptA3H2F2 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 4}, &ptA2H0F4T0, NULL};

// FRAME 1.
const struct PillarSpriteData ptA3H2F1 = {{ATTR0_TALL | 4, ATTR1_SIZE(3), 0, 5}, &ptA2H1F1T0, &ptA3H2F2};

// FRAME 0.
const struct PillarSpriteData ptA3H2F0 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 6}, &ptA2H1F4T0, &ptA3H2F1};

// HEIGHT 3.
// FRAME 2.
const struct PillarSpriteData ptA3H3F2 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 4}, &ptA2H1F4T0, NULL};

// FRAME 1.
const struct PillarSpriteData ptA3H3F1 = {{ATTR0_TALL | 4, ATTR1_SIZE(3), 0, 5}, &ptA2H2F1T0, &ptA3H3F2};

// FRAME 0.
const struct PillarSpriteData ptA3H3F0 = {{ATTR0_TALL | 1, ATTR1_SIZE(3), 0, 6}, &ptA2H2F4T0, &ptA3H3F1};


// PIL_ANIM_TURN.
// HEIGHT 0.
// FRAME 2.
const struct PillarTileData ptA4H0F2T1 = {(void*)0x18C0, (void*)0x03A0, 2, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA4H0F2T0 = {(void*)0x1880, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA4H0F2T1};
const struct PillarSpriteData ptA4H0F2 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | ATTR1_HFLIP, 0, 5}, &ptA4H0F2T0, NULL};

// FRAME 1.
const struct PillarTileData ptA4H0F1T1 = {(void*)0x2740, (void*)0x03A0, 2, {0, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA4H0F1T0 = {(void*)0x2700, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA4H0F1T1};
const struct PillarSpriteData ptA4H0F1 = {{ATTR0_TALL | (-3 & ATTR0_Y_MASK), ATTR1_SIZE(3) | 2, 0, 5}, &ptA4H0F1T0, &ptA4H0F2};

// FRAME 0.
const struct PillarSpriteData ptA4H0F0 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 5}, &ptA4H0F2T0, &ptA4H0F1};

// HEIGHT 1.
// FRAME 2.
const struct PillarTileData ptA4H1F2T4 = {(void*)0x1A80, (void*)0x03C0, 1, {5, 0, 0, 7, 0, 0}, NULL};
const struct PillarTileData ptA4H1F2T3 = {(void*)0x1A00, (void*)0x03A0, 1, {5, 0, 0, 0, 0, 0}, &ptA4H1F2T4};
const struct PillarTileData ptA4H1F2T2 = {(void*)0x1980, (void*)0x0340, 1, {0, 0, 0, 5, 0, 0}, &ptA4H1F2T3};
const struct PillarTileData ptA4H1F2T1 = {(void*)0x1900, (void*)0x0320, 1, {5, 0, 0, 0, 0, 0}, &ptA4H1F2T2};
const struct PillarTileData ptA4H1F2T0 = {(void*)0x1880, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA4H1F2T1};
const struct PillarSpriteData ptA4H1F2 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | ATTR1_HFLIP, 0, 5}, &ptA4H1F2T0, NULL};

// FRAME 1.
const struct PillarTileData ptA4H1F1T3 = {(void*)0x2800, (void*)0x03C0, 1, {5, 0, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA4H1F1T2 = {(void*)0x2780, (void*)0x03A0, 1, {5, 0, 0, 0, 0, 0}, &ptA4H1F1T3};
const struct PillarTileData ptA4H1F1T1 = {(void*)0x2740, (void*)0x0320, 2, {0, 0, 0, 0, 0, 0}, &ptA4H1F1T2};
const struct PillarTileData ptA4H1F1T0 = {(void*)0x2700, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA4H1F1T1};
const struct PillarSpriteData ptA4H1F1 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | 2, 0, 5}, &ptA4H1F1T0, &ptA4H1F2};

// FRAME 0.
const struct PillarSpriteData ptA4H1F0 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 5}, &ptA4H1F2T0, &ptA4H1F1};

// HEIGHT 2.
// FRAME 2.
const struct PillarTileData ptA4H2F2T6 = {(void*)0x1A80, (void*)0x03C0, 1, {0, 5, 0, 0, 7, 0}, NULL};
const struct PillarTileData ptA4H2F2T5 = {(void*)0x1A00, (void*)0x03A0, 1, {0, 5, 0, 0, 0, 0}, &ptA4H2F2T6};
const struct PillarTileData ptA4H2F2T4 = {(void*)0x1F00, (void*)0x0340, 1, {7, 0, 0, 9, 5, 0}, &ptA4H2F2T5};
const struct PillarTileData ptA4H2F2T3 = {(void*)0x1D00, (void*)0x0320, 1, {7, 5, 0, 0, 0, 0}, &ptA4H2F2T4};
const struct PillarTileData ptA4H2F2T2 = {(void*)0x1C80, (void*)0x02C0, 1, {0, 0, 0, 5, 0, 0}, &ptA4H2F2T3};
const struct PillarTileData ptA4H2F2T1 = {(void*)0x1900, (void*)0x02A0, 1, {5, 0, 0, 0, 0, 0}, &ptA4H2F2T2};
const struct PillarTileData ptA4H2F2T0 = {(void*)0x1880, (void*)0x0220, 2, {0, 0, 0, 0, 0, 0}, &ptA4H2F2T1};
const struct PillarSpriteData ptA4H2F2 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | ATTR1_HFLIP, 0, 5}, &ptA4H2F2T0, NULL};

// FRAME 1.
const struct PillarTileData ptA4H2F1T5 = {(void*)0x2800, (void*)0x03C0, 1, {0, 5, 0, 0, 0, 0}, NULL};
const struct PillarTileData ptA4H2F1T4 = {(void*)0x2780, (void*)0x03A0, 1, {0, 5, 0, 0, 0, 0}, &ptA4H2F1T5};
const struct PillarTileData ptA4H2F1T3 = {(void*)0x2800, (void*)0x0340, 1, {5, 0, 0, 0, 0, 0}, &ptA4H2F1T4};
const struct PillarTileData ptA4H2F1T2 = {(void*)0x2780, (void*)0x0320, 1, {5, 0, 0, 0, 0, 0}, &ptA4H2F1T3};
const struct PillarTileData ptA4H2F1T1 = {(void*)0x2740, (void*)0x02A0, 2, {0, 0, 0, 0, 0, 0}, &ptA4H2F1T2};
const struct PillarTileData ptA4H2F1T0 = {(void*)0x2700, (void*)0x0220, 2, {0, 0, 0, 0, 0, 0}, &ptA4H2F1T1};
const struct PillarSpriteData ptA4H2F1 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | 2, 0, 5}, &ptA4H2F1T0, &ptA4H2F2};

// FRAME 0.
const struct PillarSpriteData ptA4H2F0 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 5}, &ptA4H2F2T0, &ptA4H2F1};

// HEIGHT 3.
// FRAME 2.
const struct PillarTileData ptA4H3F2T8 = {(void*)0x1A80, (void*)0x03C0, 1, {0, 0, 5, 0, 0, 7}, NULL};
const struct PillarTileData ptA4H3F2T7 = {(void*)0x1A00, (void*)0x03A0, 1, {0, 0, 5, 0, 0, 0}, &ptA4H3F2T8};
const struct PillarTileData ptA4H3F2T6 = {(void*)0x1F00, (void*)0x0340, 1, {0, 7, 0, 0, 9, 5}, &ptA4H3F2T7};
const struct PillarTileData ptA4H3F2T5 = {(void*)0x1D00, (void*)0x0320, 1, {0, 7, 5, 0, 0, 0}, &ptA4H3F2T6};
const struct PillarTileData ptA4H3F2T4 = {(void*)0x1F00, (void*)0x02C0, 1, {7, 0, 0, 9, 5, 0}, &ptA4H3F2T5};
const struct PillarTileData ptA4H3F2T3 = {(void*)0x1D00, (void*)0x02A0, 1, {7, 5, 0, 0, 0, 0}, &ptA4H3F2T4};
const struct PillarTileData ptA4H3F2T2 = {(void*)0x1C80, (void*)0x0240, 1, {0, 0, 0, 5, 0, 0}, &ptA4H3F2T3};
const struct PillarTileData ptA4H3F2T1 = {(void*)0x1900, (void*)0x0220, 1, {5, 0, 0, 0, 0, 0}, &ptA4H3F2T2};
const struct PillarTileData ptA4H3F2T0 = {(void*)0x1880, (void*)0x01A0, 2, {0, 0, 0, 0, 0, 0}, &ptA4H3F2T1};
const struct PillarSpriteData ptA4H3F2 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | ATTR1_HFLIP, 0, 5}, &ptA4H3F2T0, NULL};

// FRAME 1.
const struct PillarTileData ptA4H3F1T7 = {(void*)0x2800, (void*)0x03C0, 1, {0, 0, 5, 0, 0, 0}, NULL};
const struct PillarTileData ptA4H3F1T6 = {(void*)0x2780, (void*)0x03A0, 1, {0, 0, 5, 0, 0, 0}, &ptA4H3F1T7};
const struct PillarTileData ptA4H3F1T5 = {(void*)0x2800, (void*)0x0340, 1, {0, 5, 0, 0, 0, 0}, &ptA4H3F1T6};
const struct PillarTileData ptA4H3F1T4 = {(void*)0x2780, (void*)0x0320, 1, {0, 5, 0, 0, 0, 0}, &ptA4H3F1T5};
const struct PillarTileData ptA4H3F1T3 = {(void*)0x2800, (void*)0x02C0, 1, {5, 0, 0, 0, 0, 0}, &ptA4H3F1T4};
const struct PillarTileData ptA4H3F1T2 = {(void*)0x2780, (void*)0x02A0, 1, {5, 0, 0, 0, 0, 0}, &ptA4H3F1T3};
const struct PillarTileData ptA4H3F1T1 = {(void*)0x2740, (void*)0x0220, 2, {0, 0, 0, 0, 0, 0}, &ptA4H3F1T2};
const struct PillarTileData ptA4H3F1T0 = {(void*)0x2700, (void*)0x01A0, 2, {0, 0, 0, 0, 0, 0}, &ptA4H3F1T1};
const struct PillarSpriteData ptA4H3F1 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3) | 2, 0, 5}, &ptA4H3F1T0, &ptA4H3F2};

// FRAME 0.
const struct PillarSpriteData ptA4H3F0 = {{ATTR0_TALL | (-2 & ATTR0_Y_MASK), ATTR1_SIZE(3), 0, 5}, &ptA4H3F2T0, &ptA4H3F1};

// dim0 is animID, dim1 is height.
const struct PillarSpriteData* pilSpriteData[5][4] = {
  {&ptA0H0F0, &ptA0H0F0, &ptA0H0F0, &ptA0H0F0},
  {&ptA1H0F0, &ptA1H1F0, &ptA1H2F0, &ptA1H3F0},
  {&ptA2H0F0, &ptA2H1F0, &ptA2H2F0, NULL},
  {NULL, &ptA3H1F0, &ptA3H2F0, &ptA3H3F0},
  {&ptA4H0F0, &ptA4H1F0, &ptA4H2F0, &ptA4H3F0}
};