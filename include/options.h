#ifndef OPTIONS
#define OPTIONS

struct MenuItem;    // menu.h

enum {
  // options states.
  OPTIONS_START = 0,
  OPTIONS_IDLE = 1,
  OPTIONS_RETURN = 2,
};

const void optionsStart();
const void optionsIdle();
const void optionsReturn();
const void exitOp();
const void hoverOpItems(const struct MenuItem* mi);
const void optionsUpdate();

#endif // OPTIONS