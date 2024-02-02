#ifndef MAIN
#define MAIN


extern u32 gClock;
extern u32 gStateClock;
extern u32 gGameState;
enum {
  // game states.
  GAME_START = 0,
	GAME_RESET = 1,
  GAME_TITLE = 2,
};
extern u32 gGenericState;
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

const void VBlankHandler();
const int setGameState(u32 gameState, u32 genericState);
const void updateRoutine();
const void bootupUpdate();

#endif // MAIN