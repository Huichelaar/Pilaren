#ifndef VRAMBUFFER
#define VRAMBUFFER

extern SCR_ENTRY bgmap[3][0x400];
extern COLOR palBuffer[32][16];
extern u32 syncPalFlags;
extern OBJ_ATTR oamBuffer[128];
extern u8 syncBGMapFlags;
extern u8 oamBufferConsumed;
extern u16 pad;

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
//const void testOAMBufferSort();

#endif // VRAMBUFFER