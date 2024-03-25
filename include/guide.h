#ifndef GUIDE
#define GUIDE
#include "lang.h"

#define GUIDE_PAGECOUNT 2

enum {
  // guide states.
  GUIDE_START = 0,
  GUIDE_IDLE = 1,
};

const void guideStart();
const void guidePageInit();
const void guidePageIdle();
const void guidePageFlip(int id);
const void guideIdle();
const void guideUpdate();

// Data
extern const char* guidePageText[GUIDE_PAGECOUNT][LANG_COUNT];

#endif // GUIDE