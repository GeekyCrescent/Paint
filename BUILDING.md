# Building Paint for DOS

This document provides detailed instructions for building Paint for DOS using various compilers.

## Compiler Options

### DJGPP (Recommended)

**DJGPP** is a complete port of GCC to DOS, providing a modern C99/C11 compiler.

#### Advantages
- Modern C standard support (C99, C11)
- Good optimization
- Large memory model (32-bit protected mode)
- Active community
- Similar to the human68k-gcc used in mdf-x68000 reference

#### Installation
1. Download DJGPP from http://www.delorie.com/djgpp/
2. Extract to `C:\DJGPP` (or any directory)
3. Set environment variables:
   ```
   SET DJGPP=C:\DJGPP\DJGPP.ENV
   SET PATH=%PATH%;C:\DJGPP\BIN
   ```

#### Building
```bash
cd Paint
make
```

The executable will be at `bin/paint.exe`

---

### Open Watcom C/C++

**Watcom** is another excellent DOS compiler with great optimization.

#### Advantages
- Excellent code optimization
- DOS4GW DOS extender included
- Integrated debugger
- Good documentation

#### Installation
1. Download from http://www.openwatcom.org/
2. Install to default location
3. Add to PATH

#### Building with Watcom
Create `makefile.wat`:
```make
CC = wcl386
CFLAGS = -zq -ox -bt=dos -I=include
LDFLAGS = 

SOURCES = src\main.c src\vga.c src\input.c src\adlib.c

paint.exe: $(SOURCES)
    $(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -fe=bin\paint.exe
```

Build:
```
wmake -f makefile.wat
```

---

### Turbo C / Borland C

**Turbo C** is a classic 16-bit DOS compiler.

#### Notes
- 16-bit real mode (limited memory)
- May require code modifications for large memory model
- Good for minimal systems

#### Building with Turbo C
```
tcc -I include -ebin\paint.exe src\*.c
```

---

### NASM (Assembly Optimization)

For critical performance sections, NASM can be used for assembly optimization.

#### Use Cases
- VGA memory operations
- Adlib port I/O timing
- Fast graphics primitives

#### Example
```nasm
; vga_asm.asm
section .text
global _vga_putpixel_fast

_vga_putpixel_fast:
    ; Fast pixel plotting in assembly
    push ebp
    mov ebp, esp
    ; ... implementation
    pop ebp
    ret
```

Build:
```
nasm -f obj vga_asm.asm
gcc -o paint.exe main.o vga.o vga_asm.o
```

---

## Recommended Configuration

### For Modern DOS Development (DOSBox, FreeDOS)
**Use DJGPP**
- Best compatibility
- Modern C features
- Easy to set up

### For Maximum Compatibility (Old Hardware)
**Use Watcom or Turbo C**
- Smaller executables
- Better for 286/386 systems
- No DOS extender overhead

### For Maximum Performance
**DJGPP + NASM**
- C for logic
- Assembly for graphics/sound hotspots

---

## Build Troubleshooting

### Issue: "gcc not found"
**Solution**: Ensure DJGPP is in PATH
```
SET PATH=%PATH%;C:\DJGPP\BIN
```

### Issue: "Out of memory" during compilation
**Solution**: Increase DPMI memory in DOSBox config:
```
[cpu]
cycles=max
memsize=64
```

### Issue: Mouse not working
**Solution**: Ensure mouse driver is loaded:
```
CTMOUSE.EXE
```
Or in DOSBox, set `autolock=true`

### Issue: No sound
**Solution**: 
1. Check Adlib/SoundBlaster emulation in DOSBox:
   ```
   [sblaster]
   sbtype=sb16
   oplmode=auto
   ```
2. Verify port 0x388 is available

---

## Cross-Compilation

### Linux to DOS (DJGPP Cross-Compiler)

1. Install DJGPP cross-compiler:
   ```bash
   sudo apt-get install gcc-djgpp
   ```

2. Modify Makefile:
   ```make
   CC = i586-pc-msdosdjgpp-gcc
   ```

3. Build:
   ```bash
   make
   ```

---

## Testing Builds

### In DOSBox
```
dosbox -conf dosbox.conf
```

dosbox.conf:
```ini
[autoexec]
mount c ~/Paint
c:
cd bin
paint.exe
```

### In QEMU
```bash
qemu-system-i386 -hda freedos.img -cdrom paint.iso -boot d
```

### On Real Hardware
1. Copy `bin/paint.exe` to DOS boot disk
2. Boot DOS
3. Run `paint.exe`

---

## Performance Optimization

### Compiler Flags

DJGPP:
```
CFLAGS = -O3 -march=i386 -ffast-math -funroll-loops
```

Watcom:
```
CFLAGS = -ox -ot -ol+ -ei
```

### Code Optimization Tips
1. Use inline functions for frequently called small functions
2. Minimize memory allocations
3. Use fixed-point math instead of floating-point
4. Cache VGA memory pointer
5. Batch Adlib register writes

---

## Comparison with mdf-x68000 Build System

The mdf-x68000 project uses:
```make
CC = /opt/human68k/bin/human68k-gcc
CFLAGS = -m68000 -mtune=68000 -std=c99 -O3
```

Our DOS equivalent:
```make
CC = gcc  # DJGPP
CFLAGS = -march=i386 -mtune=i386 -std=c99 -O3
```

Both use:
- GCC-based toolchain
- C99 standard
- Similar optimization levels
- Cross-compilation setup
- Static linking

---

## Recommended Setup Summary

| Target Platform | Compiler | Pros |
|----------------|----------|------|
| DOSBox/FreeDOS | DJGPP | Modern, easy to use |
| 386+ Real Hardware | Watcom | Optimized, small binary |
| 286 Systems | Turbo C | Maximum compatibility |
| Performance Critical | DJGPP+NASM | Best performance |

## Conclusion

**For this project, DJGPP is the best choice** because:
1. Similar to mdf-x68000's human68k-gcc
2. Modern C standard support
3. Good optimization
4. Easy to set up and use
5. Works well in DOSBox and on real hardware
