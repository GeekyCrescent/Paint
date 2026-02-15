/* 
 * MDFourier for DOS - OPL2/OPL3 Driver
 * Copyright (C)2024 - Based on X68000 YM2151 driver by Artemio Urbina
 *
 * This file is part of MDFourier
 *
 * Replaces YM2151 (X68000) with OPL2/OPL3 (Adlib/SoundBlaster)
 */
 
#ifndef _OPL_H_
#define _OPL_H_

#include "types.h"

/* OPL2 base I/O ports (Adlib/SoundBlaster) */
#define OPL2_ADDRESS 0x388
#define OPL2_DATA    0x389

/* OPL3 extended ports (SoundBlaster Pro/16) */
#define OPL3_ADDRESS_L 0x388
#define OPL3_DATA_L    0x389
#define OPL3_ADDRESS_R 0x38A
#define OPL3_DATA_R    0x38B

/* OPL Register definitions */
#define OPL_TEST_REG       0x01
#define OPL_TIMER1         0x02
#define OPL_TIMER2         0x03
#define OPL_TIMER_CTRL     0x04
#define OPL_KBD_SPLIT      0x08
#define OPL_AMP_VIB        0x20
#define OPL_LEVEL          0x40
#define OPL_ATTACK_DECAY   0x60
#define OPL_SUSTAIN_RELEASE 0x80
#define OPL_FREQ_LOW       0xA0
#define OPL_FREQ_HIGH      0xB0
#define OPL_FEEDBACK       0xC0
#define OPL_WAVE_SELECT    0xE0

/* OPL3 specific */
#define OPL3_NEW_MODE      0x105

/* Stereo panning (OPL3 only, for OPL2 both channels always used) */
#define STEREO_RIGHT   0x20
#define STEREO_LEFT    0x10
#define STEREO_BOTH    0x30

/* Functions */
int OPL_detect(void);
void OPL_reset(void);
void OPL_writeReg(const u8 reg, const u8 data);
void OPL_setvolume(u8 channel, u8 slot, u8 volume);

#endif // _OPL_H_
