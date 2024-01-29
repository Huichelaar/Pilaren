#include <stdio.h>
#include <tonc.h>
#include "main.h"

int main() {

	// The vblank interrupt must be enabled for VBlankIntrWait() to work.
  IRQ_INIT();
  irq_add(II_VBLANK, VBlankHandler);

	while (1) {
		VBlankIntrWait();
	}
}

// Flush gfx buffers to VRAM.
const void VBlankHandler() {
  // TODO.
  ((unsigned short*)0x06000000)[120+80*240] = 0x001F;   // This is merely here for testing.
}