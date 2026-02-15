/* Paint for DOS - Adlib/SoundBlaster Sound Driver
 * Copyright (C) 2024
 * 
 * Supports OPL2 (Adlib/SoundBlaster) and OPL3 (SoundBlaster Pro/16)
 */

#ifndef _ADLIB_H_
#define _ADLIB_H_

#include "types.h"

/* OPL2 Register addresses (Adlib/SoundBlaster base) */
#define OPL2_ADDRESS 0x388
#define OPL2_DATA    0x389

/* OPL3 Extended addresses (SoundBlaster Pro/16) */
#define OPL3_ADDRESS_L 0x388
#define OPL3_DATA_L    0x389
#define OPL3_ADDRESS_R 0x38A
#define OPL3_DATA_R    0x38B

/* OPL Register offsets */
#define OPL_TEST_REG       0x01
#define OPL_TIMER1         0x02
#define OPL_TIMER2         0x03
#define OPL_TIMER_CTRL     0x04
#define OPL_KBD_SPLIT      0x08
#define OPL_AMP_VIB        0x20
#define OPL_ATTACK_DECAY   0x60
#define OPL_SUSTAIN_RELEASE 0x80
#define OPL_FREQ_LOW       0xA0
#define OPL_FREQ_HIGH      0xB0
#define OPL_FEEDBACK       0xC0

/* Sound functions */
int adlib_detect(void);
void adlib_init(void);
void adlib_cleanup(void);
void adlib_write(u8 reg, u8 data);
void adlib_play_sound(u8 sound_id);
void adlib_play_note(u8 channel, u8 note, u8 octave);
void adlib_stop_note(u8 channel);

/* Sound effect IDs */
#define SFX_DRAW     0
#define SFX_SELECT   1
#define SFX_CLEAR    2

#endif /* _ADLIB_H_ */
