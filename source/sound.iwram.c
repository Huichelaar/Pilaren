#include <stdio.h>
#include <tonc.h>
#include "sound.h"

// Stop sample once it's finished playing.
const void stopSample() {
  REG_DMA1CNT = 0;
  irq_delete(II_TIMER1);
};