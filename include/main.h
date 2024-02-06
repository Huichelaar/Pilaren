#ifndef MAIN
#define MAIN

#define STATE_DATA 0x02030000  // Used differently depending on what state game is in.

extern u32 gClock;
extern u32 gStateClock;
enum {
  // game states.
  GAME_START = 0,
	GAME_RESET = 1,
  GAME_TITLE = 2,
};
extern u32 gGameState;
enum {
  // bootup states.
  BOOTUP_START = 0,
  BOOTUP_FADE2BLACK = 1,
  BOOTUP_WAITFORINPUT = 2,
  
  // title states.
  TITLE_START = 0,
  TITLE_FADE2GREEN = 1,
  TITLE_LOAD = 2,
};
extern u32 gGenericState;     

const void VBlankHandler();
const int setGameState(u32 gameState, u32 genericState);
const void updateRoutine();
const void bootupUpdate();

#endif // MAIN