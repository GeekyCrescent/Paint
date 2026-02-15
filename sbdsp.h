/* 
 * MDFourier for DOS - SoundBlaster DSP Driver
 * Copyright (C)2024 - Based on X68000 MSM6258 driver by Artemio Urbina
 *
 * This file is part of MDFourier
 *
 * Replaces MSM6258 ADPCM (X68000) with SoundBlaster DSP
 */

#ifndef _SBDSP_H_
#define _SBDSP_H_

#include "types.h"

/* SoundBlaster DSP I/O ports (base + offset) */
#define DSP_RESET        0x06
#define DSP_READ_DATA    0x0A
#define DSP_WRITE_DATA   0x0C
#define DSP_WRITE_STATUS 0x0C
#define DSP_READ_STATUS  0x0E

/* DSP Commands */
#define DSP_CMD_SET_TIME_CONSTANT  0x40
#define DSP_CMD_SET_SAMPLE_RATE    0x41  /* SB16 only */
#define DSP_CMD_PLAY_8BIT          0x14
#define DSP_CMD_PLAY_8BIT_AUTO     0x1C
#define DSP_CMD_PLAY_16BIT         0xB6  /* SB16 only */
#define DSP_CMD_PAUSE_8BIT         0xD0
#define DSP_CMD_RESUME_8BIT        0xD4
#define DSP_CMD_STOP_8BIT          0xDA
#define DSP_CMD_GET_VERSION        0xE1

/* Sample rates */
#define SAMPLE_RATE_7KHZ   7000
#define SAMPLE_RATE_11KHZ  11025
#define SAMPLE_RATE_15KHZ  15600
#define SAMPLE_RATE_22KHZ  22050

/* Functions */
int DSP_detect(u16 base_port);
int DSP_reset(void);
void DSP_write(u8 value);
u8 DSP_read(void);
int DSP_get_version(u8 *major, u8 *minor);
void DSP_set_sample_rate(u16 rate);
void DSP_play_buffer(u8 *buffer, u16 length);
void DSP_stop(void);

/* Global SoundBlaster base port */
extern u16 sb_base_port;

#endif // _SBDSP_H_
