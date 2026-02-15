/* 
 * MDFourier for DOS - SoundBlaster DSP Driver Implementation
 * Copyright (C)2024 - Based on X68000 MSM6258 driver by Artemio Urbina
 *
 * This file is part of MDFourier
 *
 * Replaces MSM6258 ADPCM (X68000) with SoundBlaster DSP
 */

#include <dos.h>
#include <conio.h>
#include "sbdsp.h"

u16 sb_base_port = 0x220;  /* Default SoundBlaster base port */
static int dsp_available = 0;

/* Detect SoundBlaster at given base port */
int DSP_detect(u16 base_port) {
    u8 major, minor;
    
    sb_base_port = base_port;
    
    if (!DSP_reset()) {
        return 0;
    }
    
    if (!DSP_get_version(&major, &minor)) {
        return 0;
    }
    
    dsp_available = 1;
    return 1;
}

/* Reset DSP */
int DSP_reset(void) {
    int i;
    u8 data;
    
    /* Write 1 to reset port */
    outp(sb_base_port + DSP_RESET, 1);
    
    /* Wait 3 microseconds */
    for (i = 0; i < 10; i++) {
        inp(sb_base_port);
    }
    
    /* Write 0 to reset port */
    outp(sb_base_port + DSP_RESET, 0);
    
    /* Wait for 0xAA from DSP (ready signal) */
    for (i = 0; i < 1000; i++) {
        if (inp(sb_base_port + DSP_READ_STATUS) & 0x80) {
            data = inp(sb_base_port + DSP_READ_DATA);
            if (data == 0xAA) {
                return 1;
            }
        }
        /* Small delay */
        inp(sb_base_port);
    }
    
    return 0;
}

/* Write byte to DSP */
void DSP_write(u8 value) {
    int timeout = 10000;
    
    if (!dsp_available)
        return;
    
    /* Wait until DSP is ready for write */
    while ((inp(sb_base_port + DSP_WRITE_STATUS) & 0x80) && timeout > 0) {
        timeout--;
    }
    
    /* Write the value */
    outp(sb_base_port + DSP_WRITE_DATA, value);
}

/* Read byte from DSP */
u8 DSP_read(void) {
    int timeout = 10000;
    
    if (!dsp_available)
        return 0;
    
    /* Wait until data is available */
    while (!(inp(sb_base_port + DSP_READ_STATUS) & 0x80) && timeout > 0) {
        timeout--;
    }
    
    /* Read the value */
    return inp(sb_base_port + DSP_READ_DATA);
}

/* Get DSP version */
int DSP_get_version(u8 *major, u8 *minor) {
    if (!dsp_available)
        return 0;
    
    DSP_write(DSP_CMD_GET_VERSION);
    *major = DSP_read();
    *minor = DSP_read();
    
    return (*major > 0);
}

/* Set sample rate (for SB16+ or use time constant for older) */
void DSP_set_sample_rate(u16 rate) {
    u8 time_constant;
    
    if (!dsp_available)
        return;
    
    /* For older SoundBlasters, use time constant */
    /* Time constant = 256 - (1000000 / sample_rate) */
    time_constant = 256 - (1000000 / rate);
    
    DSP_write(DSP_CMD_SET_TIME_CONSTANT);
    DSP_write(time_constant);
}

/* Play a buffer of PCM data */
void DSP_play_buffer(u8 *buffer, u16 length) {
    if (!dsp_available || !buffer || length == 0)
        return;
    
    /* Set up DMA transfer would go here in a real implementation */
    /* For this port, we're simplifying and doing direct output */
    
    /* Send play command */
    DSP_write(DSP_CMD_PLAY_8BIT);
    
    /* Send length (low byte, high byte) */
    DSP_write((length - 1) & 0xFF);
    DSP_write(((length - 1) >> 8) & 0xFF);
    
    /* Note: In real implementation, DMA would transfer the buffer */
    /* This is a simplified version for the port */
}

/* Stop DSP playback */
void DSP_stop(void) {
    if (!dsp_available)
        return;
    
    DSP_write(DSP_CMD_STOP_8BIT);
    DSP_write(DSP_CMD_PAUSE_8BIT);
}
