#ifndef VIDEO
#define VIDEO

#define VRAMBUFFER 0x02020000   // Used to buffer tiles to be sent to VRAM on next VBlank. size: 0x18000 bytes.
#define LAYER_COUNT 20

extern SCR_ENTRY bgmap[3][0x400];
extern COLOR palBuffer[32*16];
extern u32 syncPalFlags;
extern OBJ_ATTR oamBuffer[128];
extern s16 oamAffBuffer[32][4];
extern u8 syncBGMapFlags;
extern u8 oamIndex;
extern u8 oamAffIndex;

struct oamLayer {         // Layers are used as
  s8 oamID;               // an additional means to
  s8 prev;                // control which sprites are 
};                        // drawn over which other sprites.
extern struct oamLayer oamLayerBuffer[128];
extern s8 oamLayerIndex;
extern s8 oamLayerEnd[LAYER_COUNT];
enum {
  BUFFER_FILL = 0,
  BUFFER_COPY = 1
};
struct copyOnVBlankEntry {      // Used to delay copying data until after next VDraw.
  int mode;                     // mode is BUFFER_FILL or BUFFER_COPY.
  void* src;
  void* dest;
  int size;
};
extern struct copyOnVBlankEntry copyOnVBlankQueue[1000];

INLINE s8 VERT_OBJ_LAYER(int y) {
  return min(max(y, 0), SCREEN_HEIGHT + 63) / ((SCREEN_HEIGHT + 64) / LAYER_COUNT);
}

const void setSyncBGMapFlagsByID (int bg);
const void setSyncBGMapFlagsByMask (int bg);
const void setSyncPalFlagsByID(int pal);
const void setSyncPalFlagsByMask(int pal);
const void loadColours(COLOR* clrs, int colSlot, int size);
const void setColour(COLOR col, int colSlot);
const void flushBGBuffer();
const void flushPalBuffer();
const void clearBGMapBuffer();
const void mergeOAMBuffer(OBJ_ATTR* oamArr, OBJ_ATTR* arrL, OBJ_ATTR* arrR, int lSize, int rSize);
OBJ_ATTR* sortOAMBuffer(OBJ_ATTR* oamArr, int arrSize);
const void flushOAMBuffer();
const void flushAffOAMBuffer();
const void clearOAMBuffers();
int addToOAMBuffer(OBJ_ATTR* object, u32 layer);
int addAffToOAMBuffer(OBJ_AFFINE* affMatr, OBJ_ATTR* object, u32 layer);
const void flushCopyOnVBlankQueue();
int addToCopyOnVBlankQueue(void* src, void* dest, int size, int mode);
const void mapTilemap(SCR_ENTRY* src, SCR_ENTRY* dest, int xOffs, int yOffs, int width, int height, SCR_ENTRY mask);

#endif // VIDEO