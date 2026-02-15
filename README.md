# Paint for DOS

A simple paint program for DOS with Adlib/SoundBlaster audio support.

## Overview

This is a DOS port of a paint application featuring:
- VGA Mode 13h graphics (320x200, 256 colors)
- Mouse and keyboard input
- Multiple drawing tools (line, square, rectangle, circle, triangle)
- Color selection
- Adlib/SoundBlaster FM synthesis sound effects

## Requirements

### Hardware
- IBM PC compatible with 386 or higher processor
- VGA graphics card
- Mouse (recommended but not required)
- Adlib or SoundBlaster compatible sound card (optional)

### Software
- MS-DOS 5.0 or higher, or compatible (FreeDOS, DOSBox)
- DJGPP compiler for building from source

## Building from Source

### Using DJGPP

1. Install DJGPP (GCC port for DOS):
   - Download from: http://www.delorie.com/djgpp/
   - Set up DJGPP environment variables

2. Compile the program:
   ```
   make
   ```

3. The executable will be created at `bin/paint.exe`

### Alternative Compilers

The code is compatible with:
- **Watcom C/C++**: Add appropriate flags in Makefile
- **Turbo C**: May require minor modifications for inline assembly

## Running Paint

### On Real DOS Hardware
```
cd bin
paint.exe
```

### Using DOSBox
```
dosbox
mount c /path/to/paint
c:
cd bin
paint.exe
```

## Controls

### Drawing Tools
- **L** - Line tool
- **S** - Square tool
- **R** - Rectangle tool
- **C** - Circle tool
- **T** - Triangle tool

### Colors
- **K** - Black
- **W** - White
- **G** - Green
- **B** - Blue
- **R** - Red
- **Y** - Yellow

### Other
- **U** - Undo/Clear screen
- **ESC** - Exit program
- **Mouse Left Click** - Set start point / Draw shape

## Sound Features

The program includes sound effects using Adlib/SoundBlaster OPL2 FM synthesis:
- Drawing sound effect
- Tool selection beep
- Clear screen sound

If no Adlib card is detected, the program will run silently.

## Architecture

### Files Structure
```
Paint/
├── include/
│   ├── types.h      - Type definitions
│   ├── vga.h        - VGA graphics interface
│   ├── input.h      - Keyboard/mouse input
│   └── adlib.h      - Adlib sound driver
├── src/
│   ├── main.c       - Main application
│   ├── vga.c        - VGA implementation
│   ├── input.c      - Input handler
│   └── adlib.c      - Adlib driver
├── build/           - Object files
├── bin/             - Compiled executable
└── Makefile         - Build configuration
```

## Technical Details

### Graphics
- **Mode**: VGA Mode 13h (320x200, 256 colors)
- **Algorithms**: Bresenham's line and circle algorithms
- **Drawing**: Direct linear framebuffer access at 0xA0000

### Sound
- **Chip**: OPL2 (Yamaha YM3812) or OPL3 (YMF262)
- **Port**: 0x388-0x389 (standard Adlib)
- **Synthesis**: FM synthesis with 9 channels
- **Detection**: Automatic hardware detection

### Input
- **Mouse**: INT 33h (Microsoft Mouse Driver)
- **Keyboard**: BIOS INT 16h for scan codes

## Comparison with Reference (mdf-x68000)

This DOS port is inspired by the [mdf-x68000](https://github.com/aurbinatec/mdf-x68000) project which targets the Sharp X68000 computer. Key similarities:

1. **Sound Architecture**: Both use FM synthesis
   - X68000: YM2151 (8 channels)
   - DOS: OPL2/OPL3 (9 channels)

2. **Build System**: Similar Makefile structure with cross-compiler
   - X68000: human68k-gcc
   - DOS: DJGPP (i386-gcc)

3. **Direct Hardware Access**: Both use memory-mapped I/O and port I/O
   - X68000: YM2151 at 0xE90001
   - DOS: OPL2 at 0x388

## License

This software is provided as-is for educational purposes.

## Credits

- Inspired by the Python turtle graphics paint program
- Based on mdf-x68000 architecture by Artemio Urbina
- VGA graphics using standard BIOS interrupts
- Adlib programming based on OPL2/OPL3 specifications

## Future Enhancements

- [ ] SVGA high resolution modes (640x480, 800x600)
- [ ] PCX/BMP file save/load
- [ ] More drawing tools (ellipse, polygon, fill)
- [ ] Adjustable brush width
- [ ] Pattern fills
- [ ] SoundBlaster digital audio (PCM samples)
- [ ] Gravis UltraSound support
