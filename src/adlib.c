/* Paint for DOS - Adlib/SoundBlaster Sound Driver Implementation
 * Copyright (C) 2024
 * 
 * Based on OPL2/OPL3 FM synthesis chip programming
 */

#include <dos.h>
#include <conio.h>
#include "adlib.h"

static int adlib_available = 0;

/* Delay for Adlib register writes (3.3 microseconds) */
static void adlib_delay(void) {
    int i;
    for (i = 0; i < 6; i++) {
        inp(OPL2_ADDRESS);  /* Read port to waste time */
    }
}

/* Detect if Adlib/SoundBlaster is present */
int adlib_detect(void) {
    u8 status1, status2;
    int i;
    
    /* Reset both timers */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    adlib_delay();
    outp(OPL2_DATA, 0x60);
    adlib_delay();
    
    /* Reset IRQ */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    adlib_delay();
    outp(OPL2_DATA, 0x80);
    adlib_delay();
    
    /* Read status */
    status1 = inp(OPL2_ADDRESS);
    
    /* Set timer 1 */
    outp(OPL2_ADDRESS, OPL_TIMER1);
    adlib_delay();
    outp(OPL2_DATA, 0xFF);
    adlib_delay();
    
    /* Start timer 1 */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    adlib_delay();
    outp(OPL2_DATA, 0x21);
    adlib_delay();
    
    /* Wait at least 80 microseconds */
    for (i = 0; i < 100; i++) {
        adlib_delay();
    }
    
    /* Read status */
    status2 = inp(OPL2_ADDRESS);
    
    /* Reset timers and IRQ again */
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    adlib_delay();
    outp(OPL2_DATA, 0x60);
    adlib_delay();
    
    outp(OPL2_ADDRESS, OPL_TIMER_CTRL);
    adlib_delay();
    outp(OPL2_DATA, 0x80);
    adlib_delay();
    
    /* Check if detection worked */
    /* status1 should have bits 7,6,5 = 0 */
    /* status2 should have bits 7,6 = 1, bit 5 = 0 */
    if ((status1 & 0xE0) == 0x00 && (status2 & 0xE0) == 0xC0) {
        return 1;
    }
    
    return 0;
}

/* Write to Adlib register */
void adlib_write(u8 reg, u8 data) {
    outp(OPL2_ADDRESS, reg);
    adlib_delay();
    outp(OPL2_DATA, data);
    adlib_delay();
}

/* Initialize Adlib */
void adlib_init(void) {
    int i;
    
    adlib_available = adlib_detect();
    
    if (!adlib_available) {
        return;
    }
    
    /* Reset all registers */
    for (i = 0; i < 0xF5; i++) {
        adlib_write(i, 0);
    }
    
    /* Enable waveform select */
    adlib_write(0x01, 0x20);
    
    /* Set up basic instrument on channel 0 */
    /* Modulator settings (operator 0) */
    adlib_write(0x20, 0x01);  /* AM/VIB/EG/KSR/Multiple */
    adlib_write(0x40, 0x10);  /* Key scale level / Output level */
    adlib_write(0x60, 0xF0);  /* Attack rate / Decay rate */
    adlib_write(0x80, 0x77);  /* Sustain level / Release rate */
    adlib_write(0xE0, 0x00);  /* Waveform select */
    
    /* Carrier settings (operator 3) */
    adlib_write(0x23, 0x01);  /* AM/VIB/EG/KSR/Multiple */
    adlib_write(0x43, 0x00);  /* Key scale level / Output level */
    adlib_write(0x63, 0xF0);  /* Attack rate / Decay rate */
    adlib_write(0x83, 0x77);  /* Sustain level / Release rate */
    adlib_write(0xE3, 0x00);  /* Waveform select */
    
    /* Feedback / Connection */
    adlib_write(0xC0, 0x01);
}

/* Cleanup Adlib */
void adlib_cleanup(void) {
    int i;
    
    if (!adlib_available) {
        return;
    }
    
    /* Turn off all channels */
    for (i = 0; i < 9; i++) {
        adlib_write(0xB0 + i, 0);
    }
    
    /* Reset all registers */
    for (i = 0; i < 0xF5; i++) {
        adlib_write(i, 0);
    }
}

/* Play a note on specified channel */
void adlib_play_note(u8 channel, u8 note, u8 octave) {
    u16 freq;
    
    if (!adlib_available || channel >= 9) {
        return;
    }
    
    /* Frequency values for notes (F-number for octave 4) */
    static const u16 note_freq[] = {
        343, 363, 385, 408, 432, 458, 485, 514, 544, 577, 611, 647  /* C to B */
    };
    
    if (note >= 12) note = 0;
    if (octave > 7) octave = 4;
    
    freq = note_freq[note];
    
    /* Set frequency (low 8 bits) */
    adlib_write(0xA0 + channel, freq & 0xFF);
    
    /* Set octave and key on (high 2 bits of freq + octave + key on bit) */
    adlib_write(0xB0 + channel, ((freq >> 8) & 0x03) | (octave << 2) | 0x20);
}

/* Stop note on specified channel */
void adlib_stop_note(u8 channel) {
    if (!adlib_available || channel >= 9) {
        return;
    }
    
    /* Turn off key-on bit */
    adlib_write(0xB0 + channel, 0);
}

/* Play predefined sound effect */
void adlib_play_sound(u8 sound_id) {
    if (!adlib_available) {
        return;
    }
    
    switch (sound_id) {
        case SFX_DRAW:
            /* Quick beep for drawing */
            adlib_play_note(0, 7, 5);  /* G5 */
            delay(50);
            adlib_stop_note(0);
            break;
            
        case SFX_SELECT:
            /* Selection sound */
            adlib_play_note(1, 4, 5);  /* E5 */
            delay(100);
            adlib_stop_note(1);
            break;
            
        case SFX_CLEAR:
            /* Clear sound */
            adlib_play_note(2, 0, 4);  /* C4 */
            delay(150);
            adlib_stop_note(2);
            break;
    }
}
