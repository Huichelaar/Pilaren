#ifndef LCDIOBUFFER
#define LCDIOBUFFER

struct LCDIOBuffer {
  // Interrupts & status.
  u32 dispcnt;
  u16 dispstat;
  u16 pad6;         // REG_VCOUNT is read-only.
  
  // BG control.
  u16 bg0cnt;
  u16 bg1cnt;
  u16 bg2cnt;
  u16 bg3cnt;
  
  // BG Offset.
  u16 bg0hofs;
  u16 bg0vofs;
  u16 bg1hofs;
  u16 bg1vofs;
  u16 bg2hofs;
  u16 bg2vofs;
  u16 bg3hofs;
  u16 bg3vofs;
  
  // Affine BGs.
  u16 bg2pa;
  u16 bg2pb;
  u16 bg2pc;
  u16 bg2pd;
  u32 bg2x;
  u32 bg2y;
  
  u16 bg3pa;
  u16 bg3pb;
  u16 bg3pc;
  u16 bg3pd;
  u32 bg3x;
  u32 bg3y;
  
  // Window.
  u16 win0h;
  u16 win1h;
  u16 win0v;
  u16 win1v;
  u16 winin;
  u16 winout;
  
  // Mosaic + special colour effects.
  u32 mosaic;
  u16 bldcnt;
  u16 bldalpha;
  u16 bldy;
};
extern struct LCDIOBuffer lcdioBuffer;
const void flushLCDIOBuffer();

#endif // LCDIOBUFFER