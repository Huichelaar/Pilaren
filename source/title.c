#include <stdio.h>
#include <tonc.h>
#include "videobuffer.h"
#include "lcdiobuffer.h"
#include "efx.h"
#include "gfx/pil.h"
#include "gfx/pilTitle.h"
#include "pillar.h"
#include "main.h"
#include "title.h"

//#define ADR1 STATE_DATA
//#define ADR2 ADR1+sizeof(struct TitleABCD)
//#define ADR3 ADR2+sizeof(struct TitleABCD2)

//struct TitleABCD* title = (struct TitleABCD*)(ADR1);
//struct TitleABCD2* title2 = (struct TitleABCD2*)(ADR2);
//struct TitleABCD* title3 = (struct TitleABCD*)(ADR3);

u8* hiddenPillars = (u8*)(STATE_DATA);    // Tracks which pillars are still hidden. Size of 20 bytes.

const void titleUpdate() {
  int pilID;
  
  switch(gGenericState) {
    case TITLE_START:
      
      // Prepare sinusoid stuff. TODO, clarify!
      HBlankBGOFSCNT = 0x40;      // TODO, clarify what this means with an enum!
      lcdioBuffer.dispstat |= DSTAT_HBL_IRQ;      // Enable hblank interrupt in lcdiobuffer.
      irq_add(II_HBLANK, HBlankScrollBG);
      
      // Load Title palette. Randomize colours.
      loadColours((COLOR*)pilPal+1, 1, 2);
      for (int i = 3; i <= 83; i++)      // Title uses 83 different colourIDs.
        setColour(pilColourByID[qran_range(0, 4)], i);
      setSyncPalFlagsByMask(0x3F);
      
      // Load Title tiles, tilemap and set BG3CNT.
      CpuFastSet((void*)pilTitleTiles, (void*)tile8_mem, pilTitleTilesLen>>2);
      CpuFastSet((void*)pilTitleMap, (void*)bgmap[3], pilTitleMapLen>>2);
      setSyncBGMapFlagsByID(3);
      lcdioBuffer.dispcnt |= DCNT_BG3;
      lcdioBuffer.bg3cnt |= BG_8BPP | BG_PRIO(3);
      lcdioBuffer.bg3vofs = -6;
      
      // Enable sprites.
      lcdioBuffer.dispcnt |= DCNT_OBJ;
      loadColours((COLOR*)pilPal, 256, 16);
      setSyncPalFlagsByID(16);
      
      initPilArray();
      pilCamX = -104;
      pilCamY = 80;
      
      // Build pillars.
      const s16 titlePilCoords[20][2] = {
        PIL_TILE(7, 0),
        PIL_TILE(6, 1),
        PIL_TILE(7, 1),
        PIL_TILE(8, 1),
        PIL_TILE(6, 2),
        PIL_TILE(7, 2),
        PIL_TILE(8, 2),
        PIL_TILE(9, 2),
        PIL_TILE(7, 3),
        PIL_TILE(8, 3),
        PIL_TILE(1, 6),
        PIL_TILE(2, 6),
        PIL_TILE(0, 7),
        PIL_TILE(1, 7),
        PIL_TILE(2, 7),
        PIL_TILE(3, 7),
        PIL_TILE(1, 8),
        PIL_TILE(2, 8),
        PIL_TILE(3, 8),
        PIL_TILE(2, 9)
      };
      
      //int mask = (qran_range(0, 0x200) << 12) | qran_range(0, 0x1000); // Randomize pillars.
      struct Pillar pil;
      u8 clrs[6] = {PIL_CLRID_BLUE, PIL_CLRID_WHITE, PIL_CLRID_RED, PIL_CLRID_YELLOW, PIL_CLRID_RED, PIL_CLRID_RED};
      int clrMask;
      u8 height;
      for (int i = 0; i < 20; i++) {
        
        // Set random colours.
        clrMask = qran_range(0, 0x1000);
        for (int j = 0; j < 6; j++) {
          clrs[j] = 3 & clrMask;
          clrMask >>= 2;
        }
        
        // Build pillar and add to pilArray.
        pil = pilConstr(clrs, titlePilCoords[i][0], titlePilCoords[i][1]);
        pilID = addPilToPilArray(&pil);
        if (pilID == -1)
          break;
        
        // Hide pillar and add to hiddenPillars array.
        pilSetAnim(&pilArray[pilID], PIL_ANIM_HIDDEN);
        hiddenPillars[i] = pilID;
        
        // FIXME, randomize height whilst animating raising & lowering pillars.
        // We favour height = 0 by mapping values > 3 to 0.
        // This way, nine out of twelve results will be zero,
        // assuming each value has the same chance of being generated.
        //height = qran_range(0, 16);
        //height >>= (height & 0xC);
        height = 0;
        
        pilArray[pilID].height = height;
        //pilArray[pilID].spriteData = pilSpriteData[pilArray[pilID].animID][pilArray[pilID].height];
        //pilArray[pilID].updateTiles = true;
      }
      
      //pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
      
      setGameState(gGameState, TITLE_LOAD);
      
      break;
      
    case TITLE_LOAD: ;
      
      // Unhide one of the pillars every few frames.
      int count, id;
      if ((gStateClock <= 95) && (gStateClock % 5 == 0)) {
        count = gStateClock / 5;
        
        id = qran_range(0, 20 - count);
        pilID = hiddenPillars[id];
        
        pilSetAnim(&pilArray[pilID], PIL_ANIM_IDLE);
        
        // Remove unhidden pillar, and shift rest of hiddenPillars array.
        for (int i = id; i < 19 - count; i++)
          hiddenPillars[i] = hiddenPillars[i+1];
      }
      
      // Sinusoid title. TODO, pretify.
      int freq = 40000 - gStateClock * 40;
      if (freq > 0) {
        generateSinusoid(freq, 10);
      }
      else
        irq_delete(II_HBLANK);
      
      /*
      switch (gStateClock % 360) {
        case 0:
          pilArray[13].height = 1;
          pilSetAnim(&pilArray[13], PIL_ANIM_LOWER);
          break;
        case 60:
          pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
          break;
        case 120:
          pilSetAnim(&pilArray[13], PIL_ANIM_RAISE);
          break;
        case 180:
          pilSetAnim(&pilArray[13], PIL_ANIM_RAISE);
          break;
        case 240:
          pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
          break;
        case 300:
          pilSetAnim(&pilArray[13], PIL_ANIM_LOWER);
          break;
      }
      */
      
      pilRunAnims();
      pilDrawAll();
      
      gStateClock++;
      break;
  }
}