#ifndef GUIDE
#define GUIDE
#include "lang.h"

#define GUIDE_PAGECOUNT 4
#define GUIDE_PRERENDERTEXT false//true      // Change to false to draw guidetext at runtime.

extern u8 guidePageID;

enum {
  // guide states.
  GUIDE_START = 0,
  GUIDE_IDLE = 1,
  GUIDE_RETURN = 2,
};

const void guideStart();
const void guidePageInit();
const void guidePageIdle();
const void guidePageFlip(int id);
const void guideIdle();
const void guideReturn();
const void guideUpdate();

// Data
extern const char* guidePageText[GUIDE_PAGECOUNT][LANG_COUNT];
extern const void* guidePageTextTiles[GUIDE_PAGECOUNT][LANG_COUNT];
extern const u16 guidePageTextMap[640];

#endif // GUIDE