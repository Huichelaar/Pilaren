#include <stdio.h>
#include <tonc.h>
#include "vrambuffer.h"
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
      
      loadPalette((COLOR*)pilPal, 16, 16);
      setSyncPalFlagsByID(16);
      
      initPilArray();
      pilCamX = 0;
      pilCamY = 0;
      
      u8 clrs[6] = {PIL_CLR_BLUE, PIL_CLR_WHITE, PIL_CLR_RED, PIL_CLR_YELLOW, PIL_CLR_RED, PIL_CLR_RED};
      struct Pillar pil = pilConstr(clrs, 0, 1);
      int pilID = addPilToPilArray(&pil);
      
      if (pilID != -1)
        pilUnhide(&pilArray[pilID]);
      
      pilArray[pilID].height = 3;
      pilArray[pilID].spriteData = pilSpriteData[pilArray[pilID].animID][pilArray[pilID].height];
      pilArray[pilID].updateTiles = true;
      
      setGameState(gGameState, TITLE_FADE2GREEN);
      
      break;
      
    case TITLE_FADE2GREEN:
      
      pilRunAnims();
      
      
      break;
  }
}