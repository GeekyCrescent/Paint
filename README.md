# MDFourier for DOS

MDFourier audio analysis tool ported to DOS with Adlib/SoundBlaster support.

## Overview

This is a DOS port of the MDFourier audio testing suite from the Sharp X68000. MDFourier generates test tones for audio analysis and verification.

Features:
- OPL2/OPL3 FM synthesis (Adlib/SoundBlaster)
- PCM audio playback via SoundBlaster DSP
- Frequency sweep test tones
- Pulse train generation for sync
- Frame-accurate timing measurements

Read more about MDFourier at: https://junkerhq.net/MDFourier/

## Requirements

### Hardware
- IBM PC compatible with 386 or higher processor
- Adlib or SoundBlaster compatible sound card (required)
  - OPL2 (Adlib, SoundBlaster 1.0/2.0)
  - OPL3 (SoundBlaster Pro, SoundBlaster 16)

### Software
- MS-DOS 5.0 or higher, or compatible (FreeDOS, DOSBox)
- DJGPP compiler for building from source

## Building from Source

### Using DJGPP (Recommended)

DJGPP is a complete GCC port for DOS, similar to the human68k-gcc used in the X68000 version.

1. Install DJGPP:
   - Download from: http://www.delorie.com/djgpp/
   - Set up environment variables

2. Compile:
   ```
   make
   ```

3. Executable will be at `bin/mdf.exe`

### Alternative Compilers

The code is compatible with:
- **Watcom C/C++**: Excellent optimizer for DOS
- **Turbo C**: For maximum compatibility with older systems

## Running MDFourier

Ensure `sweep.pcm` is in the same directory as the executable.

### On Real DOS Hardware
```
cd bin
mdf.exe
```

### Using DOSBox
```
dosbox
mount c /path/to/mdfourier
c:
cd bin
mdf.exe
```

DOSBox configuration for best results:
```ini
[sblaster]
sbtype=sb16
oplmode=auto
oplrate=44100
```

## Usage

1. Run the program
2. Press **SPACE** to start full MDFourier test
3. Press **ESC** to run PCM playback test only

The program will:
- Detect your Adlib/SoundBlaster hardware
- Load and verify the sweep.pcm audio file
- Generate test tones across all octaves and notes
- Play the PCM sweep audio
- Report timing statistics

## Architecture

### Files
```
MDFourier-DOS/
├── main.c       - Main entry point
├── mdfourier.c  - Test sequence logic
├── mdfourier.h  - Test interface
├── opl.c        - OPL2/OPL3 driver (replaces YM2151)
├── opl.h        - OPL interface
├── sbdsp.c      - SoundBlaster DSP driver (replaces MSM6258)
├── sbdsp.h      - DSP interface
├── key.c        - Keyboard input
├── key.h        - Input interface
├── crc.c        - CRC32 verification
├── crc.h        - CRC interface
├── types.h      - Type definitions
├── sweep.pcm    - Test audio sample
├── Makefile     - Build configuration
└── build/       - Object files
```

## Technical Details

### Sound Hardware

#### OPL2/OPL3 FM Synthesis
- **Chip**: Yamaha YM3812 (OPL2) or YMF262 (OPL3)
- **Port**: 0x388-0x389 (standard Adlib)
- **Channels**: 9 FM channels (OPL2) / 18 channels (OPL3)
- **Synthesis**: 2-operator FM per channel
- **Detection**: Hardware probe via timer registers

#### SoundBlaster DSP
- **Ports**: 0x220-0x22F (typical, also checks 0x240, 0x260, 0x280)
- **Sample Rate**: 15.6 kHz (matching X68000 ADPCM)
- **Format**: 8-bit unsigned PCM
- **Detection**: DSP reset and version query

### Comparison with X68000 Version

This port maintains the same test structure as the original:

| Component | X68000 | DOS |
|-----------|--------|-----|
| **FM Chip** | YM2151 (8 channels) | OPL2/OPL3 (9 channels) |
| **PCM** | MSM6258 ADPCM + DMA | SoundBlaster DSP |
| **Timing** | Vsync (60Hz) | Timer-based simulation |
| **Compiler** | human68k-gcc | DJGPP (i386-gcc) |
| **I/O** | Memory-mapped (0xE90001) | Port I/O (0x388) |

### Test Sequence

1. **Pulse Train** - 10 sync pulses for alignment
2. **Silence** - 20 frames of silence
3. **FM Sweep** - All 8 octaves × 16 notes on 2 channels
4. **PCM Playback** - 39061 byte sweep audio
5. **Silence** - 20 frames
6. **Pulse Train** - 10 sync pulses

Expected timing:
- PCM playback: 278 frames (~4.6 seconds)
- Total sequence: 2924 frames (~48.7 seconds)

## Compiler Selection

### Why DJGPP?

DJGPP was chosen as the primary compiler because:

1. **Similar to Reference**: Uses GCC like human68k-gcc
2. **Modern C Support**: Full C99/C11 standard
3. **32-bit Protected Mode**: Better memory handling
4. **Good Optimization**: -O2/-O3 with -march=i386
5. **Active Community**: Well-documented and maintained

### Alternative: NASM

For performance-critical sections, NASM assembly can be used:
- OPL register I/O timing
- PCM buffer management
- Frame timing loops

## Credits

- **Original MDFourier**: Artemio Urbina (http://junkerhq.net/MDFourier/)
- **X68000 Version**: Artemio Urbina (https://github.com/aurbinatec/mdf-x68000)
- **DOS Port**: 2024
- **OPL Programming**: Based on Yamaha YM3812/YMF262 specifications
- **SoundBlaster**: Based on Creative Labs DSP documentation

## License

MDFourier is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

## Links

- MDFourier Project: https://junkerhq.net/MDFourier/
- X68000 Version: https://github.com/aurbinatec/mdf-x68000
- DJGPP: http://www.delorie.com/djgpp/
- OPL3 Documentation: http://www.fit.vutbr.cz/~arnost/opl/opl3.html
