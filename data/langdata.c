#include <stdio.h>
#include <tonc.h>
#include "gfx/lang/lang.h"
#include "lang.h"

const struct LanguageWidget langWidget[LANG_LIMIT] = {
  {LANG_NL, (const TILE4*)&flag_nlTiles, (const COLOR*)&flag_nlPal, 26, 3,
  &langWidget[9], &langWidget[1], &langWidget[7], &langWidget[5], &langWidget[7], &langWidget[7], &langWidget[5], &langWidget[3]},
  {LANG_EN, (const TILE4*)&flag_enTiles, (const COLOR*)&flag_enPal, 94, 3,
  &langWidget[0], &langWidget[2], &langWidget[8], &langWidget[6], &langWidget[2], &langWidget[3], &langWidget[8], &langWidget[4]},
  {2, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 162, 3,
  &langWidget[1], &langWidget[3], &langWidget[9], &langWidget[7], &langWidget[8], &langWidget[4], &langWidget[9], &langWidget[1]},
  {3, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 60, 37,
  &langWidget[2], &langWidget[4], &langWidget[5], &langWidget[8], &langWidget[0], &langWidget[5], &langWidget[1], &langWidget[6]},
  {4, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 128, 37,
  &langWidget[3], &langWidget[5], &langWidget[6], &langWidget[9], &langWidget[1], &langWidget[6], &langWidget[2], &langWidget[7]},
  {5, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 26, 71,
  &langWidget[4], &langWidget[6], &langWidget[0], &langWidget[3], &langWidget[9], &langWidget[0], &langWidget[3], &langWidget[8]},
  {6, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 94, 71,
  &langWidget[5], &langWidget[7], &langWidget[1], &langWidget[4], &langWidget[3], &langWidget[8], &langWidget[4], &langWidget[9]},
  {7, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 162, 71,
  &langWidget[6], &langWidget[8], &langWidget[2], &langWidget[0], &langWidget[4], &langWidget[9], &langWidget[0], &langWidget[0]},
  {8, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 60, 105,
  &langWidget[7], &langWidget[9], &langWidget[3], &langWidget[1], &langWidget[5], &langWidget[1], &langWidget[6], &langWidget[2]},
  {9, (const TILE4*)&flag_nullTiles, (const COLOR*)&flag_nullPal, 128, 105,
  &langWidget[8], &langWidget[0], &langWidget[4], &langWidget[2], &langWidget[6], &langWidget[2], &langWidget[7], &langWidget[5]}
};
