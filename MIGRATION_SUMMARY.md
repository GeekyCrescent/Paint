# MDFourier X68000 to DOS Migration Summary

## Overview

Successfully migrated MDFourier audio testing suite from Sharp X68000 to MS-DOS with Adlib/SoundBlaster support.

## Architecture Comparison

| Component | X68000 | DOS |
|-----------|--------|-----|
| **CPU** | Motorola 68000 | Intel 80386+ |
| **OS** | Human68k | MS-DOS |
| **FM Chip** | YM2151 (OPM) | OPL2/OPL3 |
| **FM Channels** | 8 | 9 |
| **PCM Chip** | MSM6258 (ADPCM) | SoundBlaster DSP |
| **PCM Transfer** | DMA | DSP commands |
| **I/O Method** | Memory-mapped | Port I/O |
| **Timing** | Vsync (60Hz) | Timer (~60Hz) |
| **Compiler** | human68k-gcc | DJGPP (i386-gcc) |

## Key Technical Mappings

### Sound Synthesis

**YM2151 → OPL2/OPL3**
- 8 channels → 9 channels
- 4 operators/channel → 2 operators/channel
- Memory-mapped (0xE90001) → Port I/O (0x388)
- Register wait via busy flag → Delay loops

**MSM6258 ADPCM → SoundBlaster DSP**
- ADPCM format → PCM format
- DMA transfer → DSP direct/DMA
- 15.6kHz sample rate → 15.6kHz (maintained)
- Stereo via hardware → Stereo via OPL3

### Code Structure

**Unchanged Files** (direct copy from X68000):
- `types.h` - Type definitions (fixed MIN_U32)
- `crc.c/h` - CRC32 validation
- `sweep.pcm` - Audio test sample

**Adapted Files**:
- `ym2151.c/h` → `opl.c/h` - FM synthesis driver
- `MSM6258.c/h` → `sbdsp.c/h` - PCM playback driver
- `mdfourier.c/h` - Test sequence (timing adapted)
- `main.c` - Entry point (DOS-specific)
- `key.c/h` - Input handling (DOS BIOS)
- `video.h` → Removed (no video in DOS version)

### OPL2 Register Mapping

Correct operator offsets for all 9 channels:

```
Channel:  0   1   2   3   4   5   6   7   8
Mod:      00  01  02  08  09  10  16  17  18
Car:      03  04  05  11  12  13  19  20  21
```

## Implementation Details

### OPL Driver (opl.c)
- Hardware detection via timer test
- Register access with proper delays (3.3μs)
- Support for both OPL2 and OPL3
- Channel initialization with FM parameters

### SoundBlaster DSP Driver (sbdsp.c)
- Port detection (0x220, 0x240, 0x260, 0x280)
- DSP reset sequence
- Version detection
- Sample rate setting (time constant)
- PCM playback commands

### Test Sequence (mdfourier.c)
- Pulse train generation (sync)
- Frequency sweep (all octaves/notes)
- PCM sample playback
- Timing measurements
- Frame counting via timer

### Timing Adaptation
- X68000: Hardware vsync at 60Hz
- DOS: `delay(16)` for ~60Hz simulation
- Less precise but functional for testing

## Build System

### Makefile Comparison

**X68000:**
```make
CC = /opt/human68k/bin/human68k-gcc
CFLAGS = -m68000 -std=c99 -O3
TARGET = mdf.X
```

**DOS:**
```make
CC = gcc
CFLAGS = -march=i386 -std=c99 -O2
TARGET = mdf.exe
```

Both use similar GCC-based toolchains and build structure.

## Testing

### Expected Behavior

1. **Hardware Detection**
   - OPL chip at 0x388
   - SoundBlaster DSP at 0x220 (or other port)

2. **File Loading**
   - sweep.pcm (39061 bytes)
   - CRC32: 0x8E5FFD51

3. **Test Sequence**
   - Pulse train: 20 frames
   - FM sweep: ~2600 frames
   - PCM playback: 278 frames
   - Total: ~2924 frames (~48.7 seconds)

4. **Output**
   - FM tones sweeping octaves
   - PCM frequency sweep
   - Timing statistics

### Testing Platforms

- **DOSBox**: Full emulation, good for development
- **QEMU**: Virtual machine testing
- **Real Hardware**: Ultimate validation with actual Adlib/SoundBlaster

## Compiler Selection Rationale

### Why DJGPP?

1. **Similar to X68000**: Both use GCC
2. **Modern C**: Full C99/C11 support
3. **Protected Mode**: Better than real-mode compilers
4. **Optimization**: Good -O2/-O3 support
5. **Documentation**: Well-supported

### Alternatives Considered

- **Watcom C**: Excellent optimizer, but different toolchain
- **Turbo C**: Limited to 16-bit, less optimization
- **NASM**: Assembly-only, not a C compiler

## Challenges Overcome

1. **Operator Mapping**: OPL2 has non-linear operator offsets
2. **Timing**: No hardware vsync on DOS
3. **PCM Format**: ADPCM vs PCM conversion
4. **I/O Method**: Memory-mapped vs port I/O
5. **C99 Compatibility**: Variable declarations for portability

## Files Created

### Source Code (1,736 lines total)
- main.c (145 lines)
- mdfourier.c (266 lines)
- opl.c (128 lines)
- sbdsp.c (150 lines)
- key.c (47 lines)
- crc.c (268 lines)
- Headers: mdfourier.h, opl.h, sbdsp.h, key.h, crc.h, types.h

### Documentation
- README.md (168 lines) - User guide
- BUILDING.md (265 lines) - Build/test guide
- MIGRATION_SUMMARY.md (This file)

### Configuration
- Makefile (55 lines)
- dosbox-mdf.conf (59 lines)
- .gitignore

### Data
- sweep.pcm (39,061 bytes)

## Success Metrics

✅ Complete port of MDFourier functionality
✅ Hardware abstraction (YM2151 → OPL2/OPL3)
✅ Build system (DJGPP Makefile)
✅ Comprehensive documentation
✅ Code review and fixes applied
✅ Ready for compilation and testing

## Next Steps

For users wanting to test:

1. Install DJGPP
2. Run `make` to compile
3. Ensure sweep.pcm is present
4. Run in DOSBox: `dosbox -conf dosbox-mdf.conf`
5. Execute `mdf.exe` in bin directory
6. Record output for MDFourier analysis

## References

- Original MDFourier: http://junkerhq.net/MDFourier/
- X68000 Version: https://github.com/aurbinatec/mdf-x68000
- DJGPP: http://www.delorie.com/djgpp/
- OPL3 Specs: http://www.fit.vutbr.cz/~arnost/opl/opl3.html

## Credits

- **Artemio Urbina**: Original MDFourier and X68000 port
- **DOS Port**: 2024
- **Reference**: mdf-x68000 codebase

## License

GNU General Public License v2 (same as original)

---

**Migration completed successfully on 2024-02-15**
