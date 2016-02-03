a)
Die Parameter für die 3 unterschiedlichen Übersetzungsbedingungen (release, debugrel, debug) sind in der folgenden Datei enthalten
D:\projekte\XPIC_Projekt\coremark\Waf\hilscher_toolchains.py                                                     
                                                                                                                 
Es müssen alle 3 Optionen fehlerfrei kompiliert werden können.                                                   
# dies ist mit der aktuellen Version nicht möglich

b)
Die Kompilation muss auch unabhängig von der Reihenfolge der Übergabe der Bibliotheken sein.
D:\projekte\XPIC_Projekt\coremark\Waf\hilscher_toolchains.py
Siehe Bereich "configure_toolchain_xpic"
    #f('STLIB_default_standardlibs',   ['m', 'c', 'gcc'])  # funktioniert nicht
    f('STLIB_default_standardlibs',   ['c', 'gcc', 'm'])   # funktioniert

c)
DWARF4 und DWARF2 muss unterstützt werden

D:\projekte\XPIC_Projekt\coremark\Waf\hilscher_toolchains.py
Siehe Bereich "configure_toolchain_xpic"

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g', '-gdwarf-4'])
        f(x + '_compile_debugrel', ['-Os', '-g', '-gdwarf-4'])
        f(x + '_compile_release',  ['-Os'])

d)
Es darf kein Fehler gezeigt werden, wenn die Code- und Datenbereiche weit auseinander gelegt werden.
Hierzu sind Anpassungen im Linkerscript erforderlich.
Beispielhaft siehe die Linkerscripte im Verzeichnis ...\xpic_toolchain\coremark\Targets\netX51_xpic\LDScripts