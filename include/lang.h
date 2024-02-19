#ifndef LANG
#define LANG

#define LANG_COUNT 3
#define LANG_LIMIT 10

enum {
  // language select states.
  LANG_START = 0,
  LANG_LOAD = 1,
  LANG_WAITFORINPUT = 2,
};

enum {
  // languages.
  // Increment LANG_COUNT when adding languages.
  // Make sure not to go > LANG_LIMIT.
  LANG_NL = 0,
  LANG_EN = 1,
  LANG_NULL = 2,
};
extern u8 gLang;
extern const struct LanguageWidget* langCursor;

struct LanguageWidget {
  const int id;
  const TILE4* tiles;
  const COLOR* pal;
  const s16 x;
  const s16 y;
  const struct LanguageWidget* left;
  const struct LanguageWidget* right;
  const struct LanguageWidget* up;
  const struct LanguageWidget* down;
  const struct LanguageWidget* leftUp;
  const struct LanguageWidget* leftDown;
  const struct LanguageWidget* rightUp;
  const struct LanguageWidget* rightDown;
  
};
extern const struct LanguageWidget langWidget[LANG_LIMIT];

const void langStart();
const void langLoad();
const void langInput();
const void langUpdate();

#endif // LANG