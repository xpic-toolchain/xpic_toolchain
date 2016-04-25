@echo off
set i=0
cd test_sources
%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-2 --target=xpic -c -save-temps b.c
%PATH_GNU_XPIC%\bin\xpic-ar cr libb.a b.o 
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d libb.a > objdump_libb.txt

%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-2 --target=xpic -c -save-temps d.c
%PATH_GNU_XPIC%\bin\xpic-ar cr libd.a d.o 
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d libd.a > objdump_libd.txt

%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-2 --target=xpic -c -save-temps a.c

REM enter manually wrong order inside command line
%PATH_GNU_XPIC%\bin\xpic-ld -L. a.o -ld -lb -o a_manually_wrong.out -Map a_manually_wrong.map 1> error.log 2>&1
fc error.log error.log_ref > null
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with wrong lib order failed!
)


REM enter manually correct order inside command line
%PATH_GNU_XPIC%\bin\xpic-ld -L. a.o -lb -ld -o a_manually.out -Map a_manually.map
fc a_manually.map a_manually.map_ref > null
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with correct lib order failed!
)
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d a_manually.out > objdump_a_manually.txt
fc objdump_a_manually.txt objdump_a_manually.txt_ref > null
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with correct lib order failed!
)


REM let linker search for correct order
%PATH_GNU_XPIC%\bin\xpic-ld -L. a.o -( -ld -lb -) -o a_linker_automatic.out -Map a_linker_automatic.map
fc a_linker_automatic.map a_linker_automatic.map_ref > null
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with let linker search for correct lib order failed!
)
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d a_linker_automatic.out > objdump_a_linker_automatic.txt
fc objdump_a_linker_automatic.txt objdump_a_linker_automatic.txt_ref > null
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with let linker search for correct lib order failed!
)

if %i%==5 (
  echo test succeeded.
)

cd ..


REM C:\Daten\Projekte\XPIC_compiler_source\xpic_toolchain\testsuite\latest\offline_tests\ld_order>%PATH_GNU_XPIC%\bin\xpic-ld -o offline_nested_packed_structures_te
REM st1.elf -T linker.ld offline_nested_packed_structures_test1.o
REM C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201604200908\xpic-llvm\bin\xpic-ld: cannot open linker script file linker.ld: No such file or directory
REM 
REM C:\Daten\Projekte\XPIC_compiler_source\xpic_toolchain\testsuite\latest\offline_tests\ld_order>%PATH_GNU_XPIC%\bin\xpic-ld --help
REM Usage: C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201604200908\xpic-llvm\bin\xpic-ld [options] file...
REM Options:
REM   -a KEYWORD                  Shared library control for HP/UX compatibility
REM   -A ARCH, --architecture ARCH
REM                               Set architecture
REM   -b TARGET, --format TARGET  Specify target for following input files
REM   -c FILE, --mri-script FILE  Read MRI format linker script
REM   -d, -dc, -dp                Force common symbols to be defined
REM   -e ADDRESS, --entry ADDRESS Set start address
REM   -E, --export-dynamic        Export all dynamic symbols
REM   --no-export-dynamic         Undo the effect of --export-dynamic
REM   -EB                         Link big-endian objects
REM   -EL                         Link little-endian objects
REM   -f SHLIB, --auxiliary SHLIB Auxiliary filter for shared object symbol table
REM   -F SHLIB, --filter SHLIB    Filter for shared object symbol table
REM   -g                          Ignored
REM   -G SIZE, --gpsize SIZE      Small data size (if no size, same as --shared)
REM   -h FILENAME, -soname FILENAME
REM                               Set internal name of shared library
REM   -I PROGRAM, --dynamic-linker PROGRAM
REM                               Set PROGRAM as the dynamic linker to use
REM   -l LIBNAME, --library LIBNAME
REM                               Search for library LIBNAME
REM   -L DIRECTORY, --library-path DIRECTORY
REM                               Add DIRECTORY to library search path
REM   --sysroot=<DIRECTORY>       Override the default sysroot location
REM   -m EMULATION                Set emulation
REM   -M, --print-map             Print map file on standard output
REM   -n, --nmagic                Do not page align data
REM   -N, --omagic                Do not page align data, do not make text readonly
REM   --no-omagic                 Page align data, make text readonly
REM   -o FILE, --output FILE      Set output file name
REM   -O                          Optimize output file
REM   -Qy                         Ignored for SVR4 compatibility
REM   -q, --emit-relocs           Generate relocations in final output
REM   -r, -i, --relocatable       Generate relocatable output
REM   -R FILE, --just-symbols FILE
REM                               Just link symbols (if directory, same as --rpath)
REM   -s, --strip-all             Strip all symbols
REM   -S, --strip-debug           Strip debugging symbols
REM   --strip-discarded           Strip symbols in discarded sections
REM   --no-strip-discarded        Do not strip symbols in discarded sections
REM   -t, --trace                 Trace file opens
REM   -T FILE, --script FILE      Read linker script
REM   --default-script FILE, -dT  Read default linker script
REM   -u SYMBOL, --undefined SYMBOL
REM                               Start with undefined reference to SYMBOL
REM   --unique [=SECTION]         Don't merge input [SECTION | orphan] sections
REM   -Ur                         Build global constructor/destructor tables
REM   -v, --version               Print version information
REM   -V                          Print version and emulation information
REM   -x, --discard-all           Discard all local symbols
REM   -X, --discard-locals        Discard temporary local symbols (default)
REM   --discard-none              Don't discard any local symbols
REM   -y SYMBOL, --trace-symbol SYMBOL
REM                               Trace mentions of SYMBOL
REM   -Y PATH                     Default search path for Solaris compatibility
REM   -(, --start-group           Start a group
REM   -), --end-group             End a group
REM   --accept-unknown-input-arch Accept input files whose architecture cannot be determined
REM   --no-accept-unknown-input-arch
REM                               Reject input files whose architecture is unknown
REM   --add-needed                Set DT_NEEDED tags for DT_NEEDED entries in
REM                                 following dynamic libs
REM   --no-add-needed             Do not set DT_NEEDED tags for DT_NEEDED entries
REM                                 in following dynamic libs
REM   --as-needed                 Only set DT_NEEDED for following dynamic libs if used
REM   --no-as-needed              Always set DT_NEEDED for following dynamic libs
REM   -assert KEYWORD             Ignored for SunOS compatibility
REM   -Bdynamic, -dy, -call_shared
REM                               Link against shared libraries
REM   -Bstatic, -dn, -non_shared, -static
REM                               Do not link against shared libraries
REM   -Bsymbolic                  Bind global references locally
REM   -Bsymbolic-functions        Bind global function references locally
REM   --check-sections            Check section addresses for overlaps (default)
REM   --no-check-sections         Do not check section addresses for overlaps
REM   --cref                      Output cross reference table
REM   --defsym SYMBOL=EXPRESSION  Define a symbol
REM   --demangle [=STYLE]         Demangle symbol names [using STYLE]
REM   --embedded-relocs           Generate embedded relocs
REM   --fatal-warnings            Treat warnings as errors
REM   --no-fatal-warnings         Do not treat warnings as errors (default)
REM   -fini SYMBOL                Call SYMBOL at unload-time
REM   --force-exe-suffix          Force generation of file with .exe suffix
REM   --gc-sections               Remove unused sections (on some targets)
REM   --no-gc-sections            Don't remove unused sections (default)
REM   --print-gc-sections         List removed unused sections on stderr
REM   --no-print-gc-sections      Do not list removed unused sections
REM   --hash-size=<NUMBER>        Set default hash table size close to <NUMBER>
REM   --help                      Print option help
REM   -init SYMBOL                Call SYMBOL at load-time
REM   -Map FILE                   Write a map file
REM   --no-define-common          Do not define Common storage
REM   --no-demangle               Do not demangle symbol names
REM   --no-keep-memory            Use less memory and more disk I/O
REM   --no-undefined              Do not allow unresolved references in object files
REM   --allow-shlib-undefined     Allow unresolved references in shared libaries
REM   --no-allow-shlib-undefined  Do not allow unresolved references in shared libs
REM   --allow-multiple-definition Allow multiple definitions
REM   --no-undefined-version      Disallow undefined version
REM   --default-symver            Create default symbol version
REM   --default-imported-symver   Create default symbol version for imported symbols
REM   --no-warn-mismatch          Don't warn about mismatched input files
REM   --no-warn-search-mismatch   Don't warn on finding an incompatible library
REM   --no-whole-archive          Turn off --whole-archive
REM   --noinhibit-exec            Create an output file even if errors occur
REM   -nostdlib                   Only use library directories specified on
REM                                 the command line
REM   --oformat TARGET            Specify target of output file
REM   -qmagic                     Ignored for Linux compatibility
REM   --reduce-memory-overheads   Reduce memory overheads, possibly taking much longer
REM   --relax                     Relax branches on certain targets
REM   --retain-symbols-file FILE  Keep only symbols listed in FILE
REM   -rpath PATH                 Set runtime shared library search path
REM   -rpath-link PATH            Set link time shared library search path
REM   -shared, -Bshareable        Create a shared library
REM   -pie, --pic-executable      Create a position independent executable
REM   --sort-common [=ascending|descending]
REM                               Sort common symbols by alignment [in specified order]
REM   --sort-section name|alignment
REM                               Sort sections by name or maximum alignment
REM   --spare-dynamic-tags COUNT  How many tags to reserve in .dynamic section
REM   --split-by-file [=SIZE]     Split output sections every SIZE octets
REM   --split-by-reloc [=COUNT]   Split output sections every COUNT relocs
REM   --stats                     Print memory usage statistics
REM   --target-help               Display target specific options
REM   --task-link SYMBOL          Do task level linking
REM   --traditional-format        Use same format as native linker
REM   --section-start SECTION=ADDRESS
REM                               Set address of named section
REM   -Tbss ADDRESS               Set address of .bss section
REM   -Tdata ADDRESS              Set address of .data section
REM   -Ttext ADDRESS              Set address of .text section
REM   -Ttext-segment ADDRESS      Set address of text segment
REM   --unresolved-symbols=<method>
REM                               How to handle unresolved symbols.  <method> is:
REM                                 ignore-all, report-all, ignore-in-object-files,
REM                                 ignore-in-shared-libs
REM   --verbose                   Output lots of information during link
REM   --version-script FILE       Read version information script
REM   --version-exports-section SYMBOL
REM                               Take export symbols list from .exports, using
REM                                 SYMBOL as the version.
REM   --dynamic-list-data         Add data symbols to dynamic list
REM   --dynamic-list-cpp-new      Use C++ operator new/delete dynamic list
REM   --dynamic-list-cpp-typeinfo Use C++ typeinfo dynamic list
REM   --dynamic-list FILE         Read dynamic list
REM   --warn-common               Warn about duplicate common symbols
REM   --warn-constructors         Warn if global constructors/destructors are seen
REM   --warn-multiple-gp          Warn if the multiple GP values are used
REM   --warn-once                 Warn only once per undefined symbol
REM   --warn-section-align        Warn if start of section changes due to alignment
REM   --warn-shared-textrel       Warn if shared object has DT_TEXTREL
REM   --warn-alternate-em         Warn if an object has alternate ELF machine code
REM   --warn-unresolved-symbols   Report unresolved symbols as warnings
REM   --error-unresolved-symbols  Report unresolved symbols as errors
REM   --whole-archive             Include all objects from following archives
REM   --wrap SYMBOL               Use wrapper functions for SYMBOL
REM   @FILE                       Read options from FILE
REM C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201604200908\xpic-llvm\bin\xpic-ld: supported targets: elf32-xpic elf32-little elf32-big srec symbolsrec verilo
REM g tekhex binary ihex ldrxpic
REM C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201604200908\xpic-llvm\bin\xpic-ld: supported emulations: xpic
REM C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201604200908\xpic-llvm\bin\xpic-ld: emulation specific options:
REM xpic:
REM   --build-id[=STYLE]          Generate build ID note
REM   --pmem-wrap-around=<val>    Make the linker relaxation machine assume that a
REM                                 program counter wrap-around occures at address
REM                                 <val>.  Supported values: 8k, 16k, 32k and 64k.
REM   --no-call-ret-replacement   The relaxation machine normally will
REM                                 substitute two immediately following call/ret
REM                                 instructions by a single jump instruction.
REM                                 This option disables this optimization.
REM   --no-stubs                  If the linker detects to attempt to access
REM                                 an instruction beyond 128k by a reloc that
REM                                 is limited to 128k max, it inserts a jump
REM                                 stub. You can de-active this with this switch.
REM   --debug-stubs               Used for debugging xpic-ld.
REM   --debug-relax               Used for debugging xpic-ld.
REM 
REM Report bugs to <http://www.sourceware.org/bugzilla/>

