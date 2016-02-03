Starten im Verzeichnis .../coremark/

..\xpic.bat
..\waf_1.6.11\waf.bat distclean
..\waf_1.6.11\waf.bat configure
..\waf_1.6.11\waf.bat build --conditions=release
..\waf_1.6.11\waf.bat build --conditions=debugrel
..\waf_1.6.11\waf.bat build --conditions=debug


Die Parameter für die 3 unterschiedlichen Bedingungen sind in der folgenden Datei enthalten
D:\projekte\XPIC_Projekt\coremark\Waf\hilscher_toolchains.py
Siehe Bereich "configure_toolchain_xpic"

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g', '-gdwarf-4'])
        f(x + '_compile_debugrel', ['-Os', '-g', '-gdwarf-4'])
        f(x + '_compile_release',  ['-Os'])

