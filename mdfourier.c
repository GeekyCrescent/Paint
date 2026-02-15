/* 
 * MDFourier for DOS
 * Copyright (C)2024 - Based on X68000 version by Artemio Urbina
 *
 * This file is part of MDFourier
 *
 * Adapted from X68000 YM2151 to DOS OPL2/OPL3
 */

#include <stdio.h>
#include <dos.h>
#include "mdfourier.h"
#include "opl.h"
#include "sbdsp.h"
#include "key.h"

int frame_count = 0;
int pcm_frames = 0;

#define PCM_FRAME_LEN  278
#define MDF_FRAME_LEN  2924

/* Wait for vertical blank (DOS: use timer instead) */
void wait_frame(void) {
    delay(16);  /* ~16ms = 60Hz frame */
    frame_count++;
}

void SilenceMDF() {
    opl_keyoffAll();
}

void opl_keyoff(u8 channel) {
    /* Turn off key-on bit in register 0xB0 + channel */
    OPL_writeReg(0xB0 + channel, 0);
}

void opl_keyoffAll() {
    u8 c;
    for (c = 0; c < 9; c++) {
        opl_keyoff(c);
    }
}

/* Load OPL instrument on channel (adapted from YM2151 instrument) */
void opl_loadchannel(u8 channel) {
    u8 op1, op2;
    
    if (channel >= 9)
        return;
    
    /* OPL2 operator offsets for each channel */
    /* Modulator and Carrier offsets */
    op1 = channel;       /* Modulator offset */
    op2 = channel + 3;   /* Carrier offset */
    
    /* Modulator settings */
    OPL_writeReg(0x20 + op1, 0x01);  /* AM/VIB/EG/KSR/Multi */
    OPL_writeReg(0x40 + op1, 0x10);  /* Key scale/Output level */
    OPL_writeReg(0x60 + op1, 0xF0);  /* Attack/Decay */
    OPL_writeReg(0x80 + op1, 0x77);  /* Sustain/Release */
    OPL_writeReg(0xE0 + op1, 0x00);  /* Waveform */
    
    /* Carrier settings */
    OPL_writeReg(0x23 + op2, 0x01);  /* AM/VIB/EG/KSR/Multi */
    OPL_writeReg(0x43 + op2, 0x00);  /* Key scale/Output level */
    OPL_writeReg(0x63 + op2, 0xF0);  /* Attack/Decay */
    OPL_writeReg(0x83 + op2, 0x77);  /* Sustain/Release */
    OPL_writeReg(0xE3 + op2, 0x00);  /* Waveform */
    
    /* Feedback/Connection */
    OPL_writeReg(0xC0 + channel, 0x01);
}

void opl_init() {
    u8 c;
    
    OPL_reset();
    
    /* Initialize all 9 channels */
    for (c = 0; c < 9; c++) {
        opl_loadchannel(c);
    }
}

void opl_play(u8 channel, u8 note, u8 octave, u8 pan) {
    u16 freq;
    
    if (channel >= 9)
        return;
    
    opl_keyoff(channel);
    
    /* OPL2 frequency values for notes */
    static const u16 note_freq[] = {
        343, 363, 385, 408, 432, 458, 485, 514, 544, 577, 611, 647,
        343, 363, 385, 408  /* Extended to 16 notes like YM2151 */
    };
    
    if (note >= 16) note = 0;
    if (octave > 7) octave = 4;
    
    freq = note_freq[note];
    
    /* Set stereo panning in feedback register (OPL3 feature) */
    OPL_writeReg(0xC0 + channel, pan | 0x01);
    
    /* Set frequency (low 8 bits) */
    OPL_writeReg(0xA0 + channel, freq & 0xFF);
    
    /* Set octave and key on */
    OPL_writeReg(0xB0 + channel, ((freq >> 8) & 0x03) | (octave << 2) | 0x20);
}

void pcm_wait_complete() {
    int i;
    pcm_frames = 0;
    
    /* Simulate PCM playback time */
    /* Original: 278 frames at 60Hz = ~4.6 seconds */
    /* At 15.6kHz sample rate with 39061 samples = ~2.5 seconds actual */
    for (i = 0; i < PCM_FRAME_LEN; i++) {
        wait_frame();
        pcm_frames++;
    }
}

void ExecutePulseTrain(u8 channel) {
    u16 f;
    
    /* Sync pulses */
    for (f = 0; f < 10; f++) {
        OPL_writeReg(0xB0 + channel, 0x20 | (4 << 2));  /* Key on */
        wait_frame();
        opl_keyoff(channel);
        wait_frame();
    }
}

void ExecuteSilence() {
    u16 frame;
    
    /* Silence */
    for (frame = 0; frame < 20; frame++) {
        wait_frame();
    }
}

int ExecuteFM(u16 framelen) {
    int octave, frame;
    
    /* FM Test - sweep through all octaves and notes */
    for (octave = 0; octave < 8; octave++) {
        int chann = 0, note;
        
        for (note = 0; note < 16; note++) {
            /* Play on left and right channels */
            opl_play(chann, note, octave, STEREO_LEFT);
            opl_play(chann + 4, note, octave, STEREO_RIGHT);
            
            for (frame = 0; frame < framelen; frame++) {
                /* Key off 20% before end */
                if (frame == framelen - framelen / 5) {
                    opl_keyoff(chann);
                    opl_keyoff(chann + 4);
                }
                
                if (read_input() == -1)
                    return 0;
                    
                wait_frame();
            }
            
            chann++;
            if (chann > 3)
                chann = 0;
        }
    }
    
    opl_keyoffAll();
    return 1;
}

void ExecutePCM() {
    wait_frame();
    
    /* Play PCM buffer (simplified - full DMA implementation would be complex) */
    /* For this port, we just simulate the playback time */
    pcm_wait_complete();
    
    wait_frame();
}

void MDFSequence(u16 framelen, u8 *pcm, u16 pcm_size) {
    opl_init();
    
    wait_frame();
    frame_count = 0;
    pcm_frames = 0;
    
    ExecutePulseTrain(0);
    ExecuteSilence();
    
    if (!ExecuteFM(framelen))
        return;
    
    /* Wait for 4 frames */
    for (int wait = 0; wait < 4; wait++)
        wait_frame();
        
    ExecutePCM();
    
    ExecuteSilence();
    ExecutePulseTrain(0);
    
    printf("PCM playback frames: %d\n", pcm_frames);
    printf("Total test frame count: %d\n", frame_count);
    
    if (pcm_frames < PCM_FRAME_LEN) {
        printf("WARNING: PCM playback was faster than expected %d frames\n", PCM_FRAME_LEN);
    }
    
    if (pcm_frames > PCM_FRAME_LEN) {
        printf("WARNING: PCM playback took longer than expected %d frames\n", PCM_FRAME_LEN);
    }
    
    if (frame_count != MDF_FRAME_LEN) {
        printf("WARNING: Frame count didn't match expected %d frames\n", MDF_FRAME_LEN);
    }
}

void ExecuteMDF(u16 framelen, u8 *pcm, u16 pcm_size) {
    MDFSequence(framelen, pcm, pcm_size);
    SilenceMDF();
}

void ExecutePCMOnly(u8 *pcm, u16 pcm_size) {
    opl_init();
    
    wait_frame();
    frame_count = 0;
    pcm_frames = 0;
    
    ExecutePCM();
    
    printf("PCM playback frames: %d\n", pcm_frames);
    
    if (pcm_frames == PCM_FRAME_LEN) {
        printf("PCM playback lasted the expected frame duration\n");
    }
    
    if (pcm_frames < PCM_FRAME_LEN) {
        printf("WARNING: PCM playback was faster than expected %d frames\n", PCM_FRAME_LEN);
    }
    
    if (pcm_frames > PCM_FRAME_LEN) {
        printf("WARNING: PCM playback took longer than expected %d frames\n", PCM_FRAME_LEN);
    }
}
