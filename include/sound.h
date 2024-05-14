#ifndef SOUND
#define SOUND

const void playNote(int note, int octave);
const void startSoundMixer();
IWRAM_CODE const void stopSample();

extern const s8 YODADEATH2[];

#endif // SOUND