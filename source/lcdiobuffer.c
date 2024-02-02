#include <stdio.h>
#include <tonc.h>
#include "lcdiobuffer.h"

IWRAM_DATA struct LCDIOBuffer lcdioBuffer;

const void flushLCDIOBuffer() {
  // 0x400 0000
  REG_DISPCNT = lcdioBuffer.dispcnt;
  REG_DISPSTAT = lcdioBuffer.dispstat;
  
  // 0x400 0008
  REG_BG0CNT = lcdioBuffer.bg0cnt;
  REG_BG1CNT = lcdioBuffer.bg1cnt;
  REG_BG2CNT = lcdioBuffer.bg2cnt;
  REG_BG3CNT = lcdioBuffer.bg3cnt;
  
  // 0x400 0010
  REG_BG0HOFS = lcdioBuffer.bg0hofs;
  REG_BG0VOFS = lcdioBuffer.bg0vofs;
  REG_BG1HOFS = lcdioBuffer.bg1hofs;
  REG_BG1VOFS = lcdioBuffer.bg1vofs;
  REG_BG2HOFS = lcdioBuffer.bg2hofs;
  REG_BG2VOFS = lcdioBuffer.bg2vofs;
  REG_BG3HOFS = lcdioBuffer.bg3hofs;
  REG_BG3VOFS = lcdioBuffer.bg3vofs;
  
  // 0x400 0020
  REG_BG2PA = lcdioBuffer.bg2pa;
  REG_BG2PB = lcdioBuffer.bg2pb;
  REG_BG2PC = lcdioBuffer.bg2pc;
  REG_BG2PD = lcdioBuffer.bg2pd;
  REG_BG2X = lcdioBuffer.bg2x;
  REG_BG2Y = lcdioBuffer.bg2y;
  
  // 0x400 0030
  REG_BG3PA = lcdioBuffer.bg3pa;
  REG_BG3PB = lcdioBuffer.bg3pb;
  REG_BG3PC = lcdioBuffer.bg3pc;
  REG_BG3PD = lcdioBuffer.bg3pd;
  REG_BG3X = lcdioBuffer.bg3x;
  REG_BG3Y = lcdioBuffer.bg3y;
  
  // 0x400 0040
  REG_WIN0H = lcdioBuffer.win0h;
  REG_WIN1H = lcdioBuffer.win1h;
  REG_WIN0V = lcdioBuffer.win0v;
  REG_WIN1V = lcdioBuffer.win1v;
  REG_WININ = lcdioBuffer.winin;
  REG_WINOUT = lcdioBuffer.winout;
  
  // 0x400 004C
  REG_MOSAIC = lcdioBuffer.mosaic;
  REG_BLDCNT = lcdioBuffer.bldcnt;
  REG_BLDALPHA = lcdioBuffer.bldalpha;
  REG_BLDY = lcdioBuffer.bldy;
};