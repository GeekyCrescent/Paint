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

#include "types.h"
#include "opl.h"

/* Stereo definitions in opl.h */

extern int frame_count;

void opl_keyoff(u8 channel);
void opl_keyoffAll();
void opl_init();
void opl_loadchannel(u8 channel);
void opl_play(u8 channel, u8 note, u8 octave, u8 pan);
void ExecutePulseTrain(u8 channel);
void ExecuteSilence();
int ExecuteFM(u16 framelen);
void ExecuteMDF(u16 framelen, u8 *pcm, u16 pcm_size);
void ExecutePCMOnly(u8 *pcm, u16 pcm_size);
