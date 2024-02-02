#include <stdio.h>
#include <tonc.h>
#include "vrambuffer.h"
#include "main.h"
#include "title.h"

const void titleUpdate() {
  switch(gGenericState) {
    case TITLE_START:
    
      // TODO
      
      setGameState(gGameState, TITLE_FADE2GREEN);
      
      break;
      
    case TITLE_FADE2GREEN:
      
      break;
  }
}