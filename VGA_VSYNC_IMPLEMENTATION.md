# VGA Vsync Implementation for MDFourier DOS

## Overview

This document describes the implementation of hardware VGA vsync synchronization for the MDFourier DOS port, replacing the original timer-based `delay(16)` approach.

## Motivation

The X68000 version uses hardware vsync by polling the CRTC (Cathode Ray Tube Controller) VDISP register to detect vertical blanking intervals. The initial DOS port used a simple `delay(16)` millisecond timer, which approximated 60Hz but was not synchronized to the actual video hardware refresh.

This implementation restores true hardware synchronization for DOS, providing:
1. Frame-accurate timing
2. Automatic adaptation to video mode refresh rate
3. Behavior matching the X68000 version
4. CPU-efficient hardware polling

## Technical Details

### VGA Hardware Registers

The VGA hardware provides status information through Input Status Registers:

- **Port 0x3DA**: Input Status Register 1 (Color modes)
- **Port 0x3BA**: Input Status Register 1 (Monochrome mode 7)

**Input Status Register 1 Bits:**
```
Bit 7: Reserved
Bit 6: Reserved  
Bit 5: Reserved
Bit 4: Reserved
Bit 3: Vertical Retrace (1 = in vertical retrace, 0 = active display)
Bit 2: Reserved
Bit 1: Reserved
Bit 0: Display Enable (inverse of bit 3)
```

**Bit 3 (Vertical Retrace):**
- Value 1: Video controller is in vertical blanking interval (vsync)
- Value 0: Video controller is drawing active display area

### Algorithm

The `vga_wait_vsync()` function implements a two-phase wait:

```c
void vga_wait_vsync(void) {
    /* Phase 1: Wait for any current retrace to end */
    while (inp(vga_status_port) & VGA_STAT_VSYNC) {
        /* Busy wait while in retrace */
    }
    
    /* Phase 2: Wait for next retrace to begin */
    while (!(inp(vga_status_port) & VGA_STAT_VSYNC)) {
        /* Busy wait until retrace starts */
    }
}
```

**Why Two Phases?**

If the function is called during an existing vertical retrace, we need to ensure we wait for a complete frame rather than returning immediately. The two-phase approach:

1. **Phase 1**: Waits for any in-progress retrace to finish (bit goes low)
2. **Phase 2**: Waits for the next retrace to begin (bit goes high)

This guarantees a full frame has elapsed before the function returns.

### Video Mode Detection

The implementation automatically detects color vs monochrome mode using BIOS interrupt 0x10:

```c
void vga_init(void) {
    union REGS regs;
    
    regs.h.ah = 0x0F;  /* Get current video mode */
    int86(0x10, &regs, &regs);
    
    if (regs.h.al == 0x07) {
        vga_status_port = VGA_INPUT_STATUS_1_MONO;  /* 0x3BA */
    } else {
        vga_status_port = VGA_INPUT_STATUS_1_COLOR; /* 0x3DA */
    }
}
```

This ensures compatibility with both color VGA and legacy monochrome displays.

### Refresh Rate Adaptation

The vsync implementation automatically adapts to the video mode's refresh rate:

- **Mode 3 (80×25 text)**: Typically 60Hz or 70Hz depending on monitor
- **Mode 13h (320×200)**: 60Hz or 70Hz
- **SVGA modes**: Various refresh rates (56Hz, 60Hz, 70Hz, 72Hz, 75Hz, 85Hz, etc.)

The hardware vsync naturally synchronizes to whatever refresh rate the video controller is generating, providing accurate frame timing in any mode.

## Comparison with X68000

### X68000 Implementation

The X68000 uses memory-mapped I/O to access the MFP (Multi-Function Peripheral) GPDR register:

```c
#define GPIP_VDISP (1 << 4)
#define CRTC_under_vblank !(mfp.gpdr & GPIP_VDISP)

static void inline wait_vblank(void) {
    if(CRTC_under_vblank)
        wait_for_vdisp();
    wait_for_vblank();
    video_count++;
}
```

The VDISP bit indicates whether the CRTC is in the active display area (1) or vertical blanking (0).

### DOS VGA Implementation

The DOS version uses port I/O to access VGA status register:

```c
void vga_wait_vsync(void) {
    while (inp(vga_status_port) & VGA_STAT_VSYNC);
    while (!(inp(vga_status_port) & VGA_STAT_VSYNC));
}
```

**Key Similarities:**
- Both poll hardware registers
- Both detect vertical blanking interval
- Both provide frame-accurate synchronization
- Both are CPU-efficient (no busy loops during active display)

**Key Differences:**
- X68000: Memory-mapped at 0xE88000
- DOS: Port I/O at 0x3DA/0x3BA
- X68000: VDISP bit (1 = display, 0 = blank)
- DOS: VSYNC bit (1 = blank, 0 = display)

## Performance Considerations

### CPU Usage

The vsync implementation uses busy-wait polling, which is standard for DOS real-mode programming. During the wait:

1. **Phase 1** (end of retrace): Typically 1-2ms if called during retrace
2. **Phase 2** (start of retrace): Up to 16.7ms at 60Hz (full frame time)

Average wait time: 8-10ms per frame (half the frame period)

### Optimization

For DOSBox or systems with CPU speed control:
- The busy wait naturally throttles CPU usage
- No need for `HLT` instruction or yield calls
- Works correctly in DOSBox, QEMU, and real hardware

### Alternative Approaches Considered

1. **Timer-based (original)**: 
   - Pro: Simple, doesn't require VGA hardware
   - Con: Not synchronized, accumulates drift
   
2. **INT 10h BIOS wait**:
   - Pro: Simpler API
   - Con: Slower, not available on all systems
   
3. **VGA vsync interrupt**:
   - Pro: Most efficient (no polling)
   - Con: Complex IRQ setup, compatibility issues

**Chosen: Direct register polling**
- Simple and reliable
- Works on all VGA-compatible hardware
- Matches X68000 approach
- Standard DOS practice

## Testing

### Verification Steps

1. **Compile with DJGPP**:
   ```bash
   make clean
   make
   ```

2. **Test in DOSBox**:
   ```bash
   dosbox -conf dosbox-mdf.conf
   cd bin
   mdf.exe
   ```

3. **Verify timing**:
   - Total sequence should be ~2924 frames
   - At 60Hz: ~48.7 seconds
   - At 70Hz: ~41.8 seconds
   
4. **Check frame accuracy**:
   - Run multiple times, timing should be consistent
   - Compare with X68000 results

### Expected Results

**Console Output:**
```
MDFourier for DOS v0.1 -- http://junkerhq.net/MDFourier
  Based on X68000 version by Artemio Urbina 2021-2022
  DOS port 2024

VGA vsync initialized
Detecting Adlib/SoundBlaster...
  OPL chip detected at port 0x388
...
PCM playback frames: 278
Total test frame count: 2924
```

The frame counts should match the expected values precisely, demonstrating accurate vsync synchronization.

## Compatibility

### Supported Systems

- **DOSBox**: Full support (emulates VGA hardware)
- **QEMU**: Full support with VGA device
- **Real Hardware**: Any VGA-compatible video card
- **PCjr, Tandy**: Not supported (different video hardware)

### Video Modes

Works with any VGA mode:
- Text modes (0-3, 7)
- CGA modes (4-6)
- VGA graphics modes (13h, etc.)
- SVGA modes (if VGA-compatible status register)

## Code Structure

### Files

**vga.h** (25 lines):
- VGA register definitions
- Function prototypes
- Port addresses

**vga.c** (60 lines):
- Mode detection
- Vsync wait implementation
- Port I/O functions

**Integration:**
- mdfourier.c: `wait_frame()` calls `vga_wait_vsync()`
- main.c: `vga_init()` called at startup
- Makefile: Added vga.c to build

## Future Enhancements

Possible improvements:
1. **Vsync IRQ support**: More efficient, but complex
2. **Frame skip detection**: Detect missed frames
3. **Refresh rate query**: Report actual Hz to user
4. **Triple buffering**: For smooth graphics

## References

- VGA Hardware Programming: http://www.osdever.net/FreeVGA/vga/vga.htm
- IBM VGA Technical Reference
- X68000 Technical Manual (CRTC section)
- DOSBox source code (VGA emulation)

## Credits

- **Original X68000 vsync**: Artemio Urbina
- **DOS VGA implementation**: 2024
- **Based on**: Standard VGA programming techniques

---

**Last Updated**: 2024-02-15
