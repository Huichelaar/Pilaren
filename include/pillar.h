#ifndef PILLAR
#define PILLAR

#define PIL_ARRAY_SIZE 20
#define PIL_TILE_WIDTH 17
#define PIL_TILE_HEIGHT 17
#define PIL_TILE(x, y) {x*PIL_TILE_WIDTH, y*PIL_TILE_HEIGHT}

extern u32 pilCounter;
extern s16 pilCamX;
extern s16 pilCamY;

struct Pillar {
  int pilID;
  u8 colour[6];                               // colour of each pane of pillar.
  u8 turned;                                  // Pillars can be in one of two positions.
  u8 animID;                                  // What animation pillar is performing.
  u16 animTimer;                              // How far along pillar is in animation.
  u8 height;                                  // Height of pillar. Size x 2 == #panes visible.
  u8 updateTiles;                             // If set, pillar tiles will be loaded by pilRunAnims.
  s16 x;                                      // X-pos of pillar on the grid.
  s16 y;                                      // Y-pos of pillar on the grid.
  const struct PillarSpriteData* spriteData;  // SpriteData currently used by pillar.
  OBJ_ATTR objMask;                           // masked with object before loading into OAMBuffer.
  struct Pillar* left;                        // These make it
  struct Pillar* right;                       // easier to find
  struct Pillar* up;                          // colour-matching
  struct Pillar* down;                        // rows & columns.
};
extern struct Pillar pilArray[PIL_ARRAY_SIZE];

enum {
  // Pillar animation states.
  PIL_ANIM_HIDDEN = 0,
  PIL_ANIM_IDLE = 1,
  PIL_ANIM_RAISE = 2,
  PIL_ANIM_TURN = 3,
  
  // Pillar pane colours.
  PIL_CLR_WHITE = 0,
  PIL_CLR_BLUE = 1,
  PIL_CLR_RED = 2,
  PIL_CLR_YELLOW = 3
};

struct PillarSpriteData {
  OBJ_ATTR obj;                                 // Filler used for timer.
  const struct PillarTileData* tileData;
  const struct PillarSpriteData* nextFrame;
};
extern const struct PillarSpriteData* pilSpriteData[4][4];

struct PillarTileData {
  const void* tileOffs;                         // Offset added to base tiles ptr.
  const void* VRAMOffs;                         // Offset added to OBJ VRAM address of pillar.
  const u16 size;                               // Number of tiles to copy.
  const u8 paneInfluence[6];                    // Used to find colour-correct variant of tile.
  const struct PillarTileData* nextTiles;
};

const void initPilArray();
struct Pillar pilConstr(u8* colours, s16 x, s16 y);
int addPilToPilArray(struct Pillar* pil);
const void removePil(int pilID);
struct Pillar* genRandPil();
const void pilLoadTiles(struct Pillar* pil, int offsVRAM);
const void pilSetAnim(struct Pillar* pil, u8 animID);
const void pilRunAnims();
const void pilDrawAll();

#endif // PILLAR