#include <stdio.h>
#include <tonc.h>
#include "videobuffer.h"
#include "lcdiobuffer.h"
#include "gfx/pil.h"
#include "pillar.h"
#include "main.h"
#include "title.h"

//#define ADR1 STATE_DATA
//#define ADR2 ADR1+sizeof(struct TitleABCD)
//#define ADR3 ADR2+sizeof(struct TitleABCD2)

//struct TitleABCD* title = (struct TitleABCD*)(ADR1);
//struct TitleABCD2* title2 = (struct TitleABCD2*)(ADR2);
//struct TitleABCD* title3 = (struct TitleABCD*)(ADR3);

const void titleUpdate() {
  
  switch(gGenericState) {
    case TITLE_START:
      //title->param1 = ADR1;
      //title2->param1 = ADR2;
      //title3->param1 = ADR3;
      
      // TODO
      
      // Enable sprites.
      lcdioBuffer.dispcnt |= DCNT_OBJ;
      
      loadPalette((COLOR*)pilPal, 16, 16);
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
      u8 clrs[6] = {PIL_CLR_BLUE, PIL_CLR_WHITE, PIL_CLR_RED, PIL_CLR_YELLOW, PIL_CLR_RED, PIL_CLR_RED};
      int pilID, clrMask;
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
        
        pilSetAnim(&pilArray[pilID], PIL_ANIM_IDLE);
        
        // FIXME, randomize height whilst animating raising & lowering pillars.
        // We favour height = 0 by mapping values > 3 to 0.
        // This way, nine out of twelve results will be zero,
        // assuming each value has the same chance of being generated.
        //height = qran_range(0, 16);
        //height >>= (height & 0xC);
        height = 1;
        
        pilArray[pilID].height = height;
        pilArray[pilID].spriteData = pilSpriteData[pilArray[pilID].animID][pilArray[pilID].height];
        pilArray[pilID].updateTiles = true;
      }
      
      //pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
      
      setGameState(gGameState, TITLE_FADE2GREEN);
      
      break;
      
    case TITLE_FADE2GREEN:
      
      if (!(gStateClock % 78))
        pilSetAnim(&pilArray[13], PIL_ANIM_TURN);
      
      pilRunAnims();
      pilDrawAll();
      
      gStateClock++;
      break;
  }
}