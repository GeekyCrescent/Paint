/* 
 * MDFourier for DOS
 * Copyright (C)2024 - Based on X68000 version by Artemio Urbina
 *
 * This file is part of MDFourier
 *
 * MDFourier is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
  
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include "key.h"
#include "crc.h"
#include "mdfourier.h"
#include "opl.h"
#include "sbdsp.h"

#define PCM_SIZE   39061
#define PCM_CRC32  0x8E5FFD51

u8 *loadPCM(char *filename, u16 *size) {
    FILE *file = NULL;
    u8 *file_buffer = NULL;
    u16 file_size = 0;
    
    if (!size)
        return NULL;
    *size = 0;
    
    file = fopen(filename, "rb");
    if (!file)
        return NULL;
    
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);    
    rewind(file);
    
    if (!file_size) {        
        fclose(file);
        return NULL;
    }
    
    /* Truncate if larger than expected */
    if (file_size > PCM_SIZE)
        file_size = PCM_SIZE;

    file_buffer = (u8*)malloc(sizeof(u8) * file_size);
    if (!file_buffer) {        
        fclose(file);
        return NULL;
    }
    
    if (fread(file_buffer, sizeof(u8), file_size, file) != file_size) {        
        fclose(file);
        free(file_buffer);
        return NULL;
    }
    
    fclose(file);
    
    *size = file_size;
    return file_buffer;
}

int main(void) {
    int input = 0;
    unsigned char *pcm_sweep = NULL;
    unsigned short pcm_size = 0;
    unsigned long crc32;
    u16 sb_ports[] = {0x220, 0x240, 0x260, 0x280, 0};
    int i, sb_found = 0;
    
    printf("MDFourier for DOS v0.1 -- http://junkerhq.net/MDFourier\n");
    printf("  Based on X68000 version by Artemio Urbina 2021-2022\n");
    printf("  DOS port 2024\n\n");
    
    /* Detect Adlib/SoundBlaster */
    printf("Detecting Adlib/SoundBlaster...\n");
    if (!OPL_detect()) {
        printf("ERROR: No Adlib/SoundBlaster OPL2/OPL3 detected!\n");
        printf("  Make sure your sound card is installed and configured.\n");
        return 1;
    }
    printf("  OPL chip detected at port 0x388\n");
    
    /* Try to detect SoundBlaster DSP */
    printf("Detecting SoundBlaster DSP...\n");
    for (i = 0; sb_ports[i] != 0; i++) {
        if (DSP_detect(sb_ports[i])) {
            printf("  SoundBlaster DSP detected at port 0x%X\n", sb_ports[i]);
            sb_found = 1;
            break;
        }
    }
    
    if (!sb_found) {
        printf("  Warning: SoundBlaster DSP not detected.\n");
        printf("  FM synthesis will work, but PCM playback may not.\n");
    }
    
    printf("\nLoading PCM samples...\n");
    pcm_sweep = loadPCM("sweep.pcm", &pcm_size);
    if (!pcm_sweep) {
        printf("Could not load PCM samples from 'sweep.pcm' file\n");
        printf("  Make sure sweep.pcm is in the current directory.\n");
        return 1;
    }
    printf("  Loaded %u bytes\n", pcm_size);
    
    printf("Verifying PCM samples...\n");
    crc32 = crc32buf((char*)pcm_sweep, pcm_size);
    if (crc32 != PCM_CRC32) {
        printf("WARNING: PCM samples CRC32 mismatch: 0x%lX != 0x%lX\n", 
               crc32, (unsigned long)PCM_CRC32);
        printf("  Continuing anyway, but results may be incorrect.\n");
    } else {
        printf("  CRC32 verified OK\n");
    }

    printf("\nPress SPACE when ready to record MDFourier\n");
    printf("Press ESC to measure PCM playback duration only\n");
    
    do {
        input = read_input();
        delay(50);
    } while (!input);
    
    if (input == 1) {
        printf("\nStarting MDFourier test tones (ESC aborts)\n");
        printf("-----------------------------------------------\n");
        ExecuteMDF(20, pcm_sweep, pcm_size);
        printf("-----------------------------------------------\n");
        printf("MDFourier test complete!\n");
    }
    
    if (input == -1) {
        printf("\nStarting MDFourier PCM Playback test\n");
        printf("-----------------------------------------------\n");
        ExecutePCMOnly(pcm_sweep, pcm_size);
        printf("-----------------------------------------------\n");
        printf("PCM test complete!\n");
    }
    
    if (pcm_sweep) {
        free(pcm_sweep);
        pcm_sweep = NULL;
        pcm_size = 0;
    }

    printf("\nThank you for using MDFourier for DOS!\n");
    return 0;
}
