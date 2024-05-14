#include <stdio.h>
#include <tonc.h>
#include "sound.h"

#define YODA_FREQ 48000
#define YODA_LEN  61440

// Basic routine for playing a sqwav note.
// Taken from tonc's sound demo.
const void playNote(int note, int octave) {
    REG_SND1FREQ = SFREQ_RESET | SND_RATE(note, octave);
}

// Adapted from Deku's tree of Art's tutorial for sound mixer.
// The mirror I used is at https://stuij.github.io/deku-sound-tutorial/sound1.html
const void startSoundMixer() {
  
  // Setup Direct Sound A (mono).
  REG_SNDDSCNT = SDS_DMG100 | SDS_AL | SDS_AR | SDS_ARESET;     // 0x0604
  REG_SNDSTAT = SSTAT_ENABLE;
  
  // Setup timers.
  REG_TM0CNT = 0;
  REG_TM0D = 65536 - (16777216 / YODA_FREQ);
  REG_TM0CNT = TM_ENABLE;
  
  // Interrupt and disable DMA when sample is finished.
  REG_TM1CNT = 0;
  REG_TM1D = 65536 - YODA_LEN;
  REG_TM1CNT = TM_CASCADE | TM_IRQ | TM_ENABLE;
  irq_add(II_TIMER1, stopSample);
  
  // Setup DMA.
  REG_DMA1CNT = 0;
  REG_DMA1SAD = (u32) YODADEATH2;
  REG_DMA1DAD = (u32) &(REG_FIFO_A);
  REG_DMA1CNT = DMA_DST_FIXED | DMA_REPEAT | DMA_32 | DMA_AT_FIFO | DMA_ENABLE;   // 0xB640
}