#ifndef TITLE
#define TITLE

struct MenuItem;    // menu.h

extern u8* hiddenPillars;
extern u8* titleLoadProgress;

enum {
  // title states.
  TITLE_START = 0,
  TITLE_LOAD = 1,
  TITLE_INPUT = 2
};

// titleLoadProgress bits.
enum {
  TITLELOAD_TITLE = 0x1,
  TITLELOAD_PILLARS = 0x2,
  TITLELOAD_MENU = 0x4,
  TITLELOAD_ALL = 0x7
};

const void titleStart();
const void titleLoad();
const void titleInput();
const void titleUpdate();
const void selectCuGm(const struct MenuItem* mi);
const void hoverCuGmItems(const struct MenuItem* mi);
const void selectGu(const struct MenuItem* mi);
const void selectOp(const struct MenuItem* mi);

#endif // TITLE