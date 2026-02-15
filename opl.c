/* 
 * MDFourier for DOS - OPL2/OPL3 Driver Implementation
 * Copyright (C)2024 - Based on X68000 YM2151 driver by Artemio Urbina
 *
 * This file is part of MDFourier
 *
 * Replaces YM2151 (X68000) with OPL2/OPL3 (Adlib/SoundBlaster)
 */

#include <dos.h>
#include <conio.h>
#include "opl.h"

static int opl_available = 0;

/* Delay for OPL register writes (3.3 microseconds minimum) */
static void opl_delay(void) {
    int i;
    for (i = 0; i < 6; i++) {
        inp(OPL2_ADDRESS);  /* Read port to waste time */
    }
}

/* Detect if Adlib/SoundBlaster OPL is present */
int OPL_detect(void) {
    u8 status1, status2;
    int i;
    
    /* Reset both timers */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    opl_delay();
    outp(OPL2_DATA, 0x60);
    opl_delay();
    
    /* Reset IRQ */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    opl_delay();
    outp(OPL2_DATA, 0x80);
    opl_delay();
    
    /* Read status */
    status1 = inp(OPL2_ADDRESS);
    
    /* Set timer 1 */
    outp(OPL2_ADDRESS, OPL_TIMER1);
    opl_delay();
    outp(OPL2_DATA, 0xFF);
    opl_delay();
    
    /* Start timer 1 */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    opl_delay();
    outp(OPL2_DATA, 0x21);
    opl_delay();
    
    /* Wait at least 80 microseconds */
    for (i = 0; i < 100; i++) {
        opl_delay();
    }
    
    /* Read status */
    status2 = inp(OPL2_ADDRESS);
    
    /* Reset timers and IRQ again */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    opl_delay();
    outp(OPL2_DATA, 0x60);
    opl_delay();
    
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    opl_delay();
    outp(OPL2_DATA, 0x80);
    opl_delay();
    
    /* Check if detection worked */
    /* status1 should have bits 7,6,5 = 0 */
    /* status2 should have bits 7,6 = 1, bit 5 = 0 */
    if ((status1 & 0xE0) == 0x00 && (status2 & 0xE0) == 0xC0) {
        opl_available = 1;
        return 1;
    }
    
    opl_available = 0;
    return 0;
}

/* Write to OPL register */
void OPL_writeReg(const u8 reg, const u8 data) {
    if (!opl_available)
        return;
        
    outp(OPL2_ADDRESS, reg);
    opl_delay();
    outp(OPL2_DATA, data);
    opl_delay();
    opl_delay();  /* Extra delay for safety */
}

/* Reset OPL chip */
void OPL_reset(void) {
    int i;
    
    if (!opl_available)
        return;
    
    /* Reset all registers */
    for (i = 0; i < 0xF5; i++) {
        OPL_writeReg(i, 0);
    }
    
    /* Enable waveform select */
    OPL_writeReg(0x01, 0x20);
}

/* Set volume for a channel/slot */
void OPL_setvolume(u8 channel, u8 slot, u8 volume) {
    u8 reg;
    
    if (!opl_available || channel >= 9)
        return;
    
    /* Calculate register offset for volume (0x40 + slot offset + channel) */
    /* OPL2 has 2 operators per channel */
    /* Operator offsets: modulator=0x00, carrier=0x03 */
    if (slot == 0)
        reg = 0x40 + channel;  /* Modulator */
    else
        reg = 0x43 + channel;  /* Carrier */
    
    /* Volume is 0-63, where 0 is loudest, 63 is quietest */
    /* Mask to 6 bits and set */
    OPL_writeReg(reg, volume & 0x3F);
}
