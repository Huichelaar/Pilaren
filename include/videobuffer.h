#ifndef VIDEOBUFFER
#define VIDEOBUFFER

#define VRAMBUFFER 0x02020000   // Used to buffer tiles to be sent to VRAM on next VBlank. size: 0x18000 bytes.

extern SCR_ENTRY bgmap[3][0x400];
extern COLOR palBuffer[32][16];
extern u32 syncPalFlags;
extern OBJ_ATTR oamBuffer[128];
extern u8 syncBGMapFlags;
extern u8 oamBufferConsumed;

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

const void setSyncBGMapFlagsByID (int bg);
const void setSyncBGMapFlagsByMask (int bg);
const void setSyncPalFlagsByID(int pal);
const void setSyncPalFlagsByMask(int pal);
const void loadPalette(COLOR* pal, int palSlot, int size);
const void setColour(COLOR col, int palSlot, int colSlot);
const void flushBGBuffer();
const void flushPalBuffer();
const void clearBGMapBuffer();
const void mergeOAMBuffer(OBJ_ATTR* oamArr, OBJ_ATTR* arrL, OBJ_ATTR* arrR, int lSize, int rSize);
OBJ_ATTR* sortOAMBuffer(OBJ_ATTR* oamArr, int arrSize);
const void flushOAMBuffer();
const void clearOAMBuffer();
int addToOAMBuffer(OBJ_ATTR* object, int priority);
const void flushCopyOnVBlankQueue();
int addToCopyOnVBlankQueue(void* src, void* dest, int size, int mode);

#endif // VIDEOBUFFER