=== LLVM Compiler Runtime Library for xPIC ===

libgcc, compiled for xPIC

This library implements the low level functions that are not supported directly by the llvm backend (llc).
It is a mix of gcc and llvm sources ported for the xPIC.

The llvm-gcc distribution from the llvm homepage includes the i386-... port for PC of the runtime library, so we have to replace it with a library compiled for the xPIC.

The build script "SConstruct" is based on SCons, available at http://www.scons.org

- Build with scons:
scons
- Build clean (remove objects)
scons -c
- Install library to ${PATH_GNU_XPIC}/lib/gcc/xpic-hilscher-elf/0.1.1
scons install
