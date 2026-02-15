# Building and Testing MDFourier for DOS

This document provides detailed instructions for building and testing the MDFourier DOS port.

## Compiler Selection

### DJGPP (Recommended)

**Why DJGPP?**
- Same GCC family as X68000's human68k-gcc
- Modern C99 standard support
- 32-bit protected mode (DPMI)
- Excellent optimization
- Well-documented

**Installation:**

1. Download DJGPP from http://www.delorie.com/djgpp/
2. Extract to `C:\DJGPP`
3. Set environment variables:
   ```
   SET DJGPP=C:\DJGPP\DJGPP.ENV
   SET PATH=%PATH%;C:\DJGPP\BIN
   ```

**Build:**
```bash
make
```

Output: `bin/mdf.exe`

### Alternative: Open Watcom

Good alternative with excellent optimization.

**Makefile.wat:**
```make
CC = wcl386
CFLAGS = -zq -ox -bt=dos
SOURCES = main.c mdfourier.c opl.c sbdsp.c key.c crc.c

mdf.exe: $(SOURCES)
    $(CC) $(CFLAGS) $(SOURCES) -fe=bin\mdf.exe
```

**Build:**
```
wmake -f Makefile.wat
```

### Alternative: Turbo C

For maximum compatibility with older 286/386 systems.

**Note:** May require adjustments for 16-bit compilation.

## Testing

### DOSBox Configuration

Create `dosbox.conf`:

```ini
[cpu]
core=auto
cputype=auto
cycles=max

[mixer]
nosound=false
rate=44100
blocksize=1024
prebuffer=25

[sblaster]
sbtype=sb16
sbbase=220
irq=7
dma=1
hdma=5
sbmixer=true
oplmode=auto
oplemu=default
oplrate=44100

[autoexec]
mount c .
c:
cd bin
```

**Run:**
```
dosbox -conf dosbox.conf
```

### Testing in DOSBox

1. Start DOSBox
2. Navigate to bin directory
3. Run `mdf.exe`
4. Expected output:
   ```
   MDFourier for DOS v0.1 -- http://junkerhq.net/MDFourier
     Based on X68000 version by Artemio Urbina 2021-2022
     DOS port 2024

   Detecting Adlib/SoundBlaster...
     OPL chip detected at port 0x388
   Detecting SoundBlaster DSP...
     SoundBlaster DSP detected at port 0x220
   
   Loading PCM samples...
     Loaded 39061 bytes
   Verifying PCM samples...
     CRC32 verified OK

   Press SPACE when ready to record MDFourier
   Press ESC to measure PCM playback duration only
   ```

5. Press SPACE to run full test
6. Listen for:
   - FM tone sweeps (all octaves)
   - PCM sweep playback
   - Pulse trains

### Testing on Real Hardware

**Requirements:**
- DOS-compatible PC with 386+ CPU
- Adlib or SoundBlaster card
- `sweep.pcm` file in same directory

**Steps:**
1. Copy `bin/mdf.exe` and `sweep.pcm` to DOS disk/partition
2. Boot to DOS
3. Run `mdf.exe`
4. Follow on-screen instructions

**Common Issues:**

**"No Adlib/SoundBlaster detected"**
- Check sound card installation
- Verify I/O port (usually 0x388 for Adlib)
- Try running with different BASE= in CONFIG.SYS

**"Could not load sweep.pcm"**
- Ensure sweep.pcm is in current directory
- Check file size (should be 39061 bytes)
- Verify filename is correct (case-sensitive on some DOS versions)

**"CRC32 mismatch"**
- File may be corrupted
- Re-download sweep.pcm from source
- Continue anyway to test hardware

## Validation

### Expected Results

**FM Sweep Test:**
- Duration: ~40 seconds
- 8 octaves × 16 notes = 128 tones
- Each note plays on 2 channels (left/right simulation)
- Smooth frequency progression

**PCM Playback:**
- Duration: ~4.6 seconds (278 frames)
- Sample rate: 15.6 kHz
- 8-bit unsigned PCM
- Frequency sweep from low to high

**Total Sequence:**
- Expected: 2924 frames (~48.7 seconds at 60Hz)
- Actual timing may vary due to DOS timer vs X68000 vsync

### Comparison with X68000

| Metric | X68000 | DOS | Notes |
|--------|--------|-----|-------|
| FM Chip | YM2151 | OPL2/3 | Different operators |
| Channels | 8 | 9 | OPL has one more |
| PCM Method | DMA | DSP | Different implementation |
| Timing | Vsync | Timer | Less precise on DOS |
| Port I/O | Memory-mapped | Port I/O | Architecture difference |

## Troubleshooting

### Build Issues

**"gcc not found"**
```
SET PATH=%PATH%;C:\DJGPP\BIN
```

**"Out of environment space"**
- Increase environment space in CONFIG.SYS:
  ```
  SHELL=C:\DOS\COMMAND.COM /E:4096 /P
  ```

**Link errors**
- Ensure all .c files are present
- Check Makefile syntax
- Try `make clean` then `make`

### Runtime Issues

**"General Protection Fault"**
- Check DPMI memory settings
- In DOSBox, increase memory:
  ```ini
  [cpu]
  memsize=64
  ```

**No sound output**
- Verify OPL detection message
- Check DOSBox sblaster settings
- Try different `oplmode` (auto/opl2/opl3)

**Timing issues**
- Expected on DOS vs X68000
- DOS lacks true vsync
- Timer-based simulation approximate

## Performance Optimization

### Compiler Flags

**DJGPP:**
```make
CFLAGS = -O3 -march=i386 -mtune=i386 -ffast-math -funroll-loops
```

**Watcom:**
```make
CFLAGS = -ox -ot -ol+ -oi+ -ei -6r
```

### Assembly Optimization

For critical sections, consider NASM:

**opl_fast.asm:**
```nasm
; Fast OPL register write
global _OPL_writeReg_fast
_OPL_writeReg_fast:
    push ebp
    mov ebp, esp
    mov al, [ebp+8]   ; reg
    mov dx, 0x388
    out dx, al
    ; delay
    in al, dx
    in al, dx
    in al, dx
    mov al, [ebp+12]  ; data
    inc dx
    out dx, al
    pop ebp
    ret
```

Build:
```
nasm -f coff opl_fast.asm
gcc -o mdf.exe *.o opl_fast.o
```

## Advanced Testing

### Recording Output

Use DOSBox's wave recording:
1. Start DOSBox
2. Press Ctrl+F6 to start recording
3. Run MDFourier test
4. Press Ctrl+F6 to stop
5. Find .wav file in DOSBox capture directory
6. Analyze with MDFourier analysis tools

### Real Hardware Recording

1. Connect line-out to recording device
2. Run `mdf.exe` and press SPACE
3. Record output to WAV file
4. Use MDFourier analysis software:
   - https://junkerhq.net/MDFourier/

## Cross-Compilation

### Linux to DOS (DJGPP)

Install cross-compiler:
```bash
sudo apt-get install gcc-djgpp
```

Modify Makefile:
```make
CC = i586-pc-msdosdjgpp-gcc
CFLAGS = -Wall -O2 -std=c99 -march=i386
```

Build:
```bash
make
```

Result: DOS executable built on Linux!

## Success Criteria

✅ **Successful Build:**
- No compilation errors
- `bin/mdf.exe` created
- ~50-100 KB file size

✅ **Successful Detection:**
- OPL chip detected at 0x388
- DSP detected (optional)
- sweep.pcm loaded and verified

✅ **Successful Test:**
- FM tones play smoothly
- PCM sweep audible
- No crashes or hangs
- Timing statistics displayed

## Next Steps

Once testing is successful:
1. Record output audio
2. Compare with X68000 version
3. Analyze frequency response
4. Document any differences
5. Consider optimizations

## Reference

- MDFourier: http://junkerhq.net/MDFourier/
- DJGPP: http://www.delorie.com/djgpp/
- OPL3: http://www.fit.vutbr.cz/~arnost/opl/opl3.html
- DOSBox: https://www.dosbox.com/
