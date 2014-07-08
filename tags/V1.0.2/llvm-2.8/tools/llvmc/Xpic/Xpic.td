//===- Xpic.td - XPIC-LLVMC toolchain descriptions ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains compilation graph description used by llvmc.
//
//===----------------------------------------------------------------------===//

include "llvm/CompilerDriver/Common.td"

// Options

def OptList : OptionList<[
 (switch_option "emit-llvm",
    (help "Emit LLVM .ll files instead of native object files")),
 (switch_option "E",
    (help "Stop after the preprocessing stage, do not run the compiler")),
 (switch_option "fsyntax-only",
    (help "Stop after checking the input for syntax errors")),
 (switch_option "opt",
    (help "Enable opt")),
 (switch_option "O0",
    (help "Turn off optimization"), (zero_or_more)),
 (switch_option "O1",
    (help "Optimization level 1"), (zero_or_more)),
 (switch_option "O2",
    (help "Optimization level 2"), (zero_or_more)),
 (switch_option "O3",
    (help "Optimization level 3"), (zero_or_more)),
 (switch_option "Os",
    (help "Code size optimization"), (zero_or_more)),
 (switch_option "S",
    (help "Stop after compilation, do not assemble")),
 (switch_option "c",
    (help "Compile and assemble, but do not link")),
 (switch_option "pthread",
    (help "Enable threads")),
 (switch_option "m32",
    (help "Generate code for a 32-bit environment"), (hidden)),
 (switch_option "m64",
    (help "Generate code for a 64-bit environment"), (hidden)),
 (switch_option "fPIC",
    (help "Relocation model: PIC"), (hidden)),
 (switch_option "mdynamic-no-pic",
    (help "Relocation model: dynamic-no-pic"), (hidden)),
 (parameter_option "linker",
    (help "Choose linker (possible values: gcc, g++)")),
 (parameter_option "mtune",
    (help "Target a specific CPU type"), (hidden), (forward_not_split)),

 // TODO: Add a conditional compilation mechanism to make Darwin-only options
 // like '-arch' really Darwin-only.

 (parameter_option "arch",
    (help "Compile for the specified target architecture"), (hidden)),
// (parameter_option "march",
//    (help "A synonym for -mtune"), (hidden), (forward_not_split)),
 (parameter_option "mcpu",
    (help "A deprecated synonym for -mtune"), (hidden), (forward_not_split)),
 (parameter_list_option "include",
    (help "Include the named file prior to preprocessing")),
 (parameter_list_option "framework",
    (help "Specifies a framework to link against")),
 (parameter_list_option "weak_framework",
    (help "Specifies a framework to weakly link against"), (hidden)),
 (parameter_option "filelist", (hidden),
    (help "Link the files listed in file")),
 (parameter_option "Map",
    (help "Output a map file")),
 (prefix_list_option "F",
    (help "Add a directory to framework search path")),
 (prefix_list_option "I",
    (help "Add a directory to include path")),
 (prefix_list_option "D",
    (help "Define a macro")),
 (prefix_list_option "Wa,", (comma_separated),
    (help "Pass options to assembler")),
 (parameter_list_option "Xassembler", (hidden),
    (help "Pass options to assembler")),
 (prefix_list_option "Wllc,", (comma_separated),
    (help "Pass options to llc")),
 (prefix_list_option "L",
    (help "Add a directory to link path")),
 (prefix_option "T",
    (help "Define a linker script")),
 (prefix_list_option "l",
    (help "Search a library when linking")),
 (prefix_list_option "Wl,",
    (help "Pass options to linker")),
 (parameter_list_option "Xlinker", (hidden),
    (help "Pass options to linker")),
 (prefix_list_option "Wo,", (comma_separated),
    (help "Pass options to opt")),
 (prefix_list_option "m",
     (help "Enable or disable various extensions (-mmmx, -msse, etc.)"),
     (hidden)),
 (switch_option "dynamiclib", (hidden),
     (help "Produce a dynamic library")),
 (switch_option "prebind", (hidden),
     (help "Prebind all undefined symbols")),
 (switch_option "dead_strip", (hidden),
     (help "Remove unreachable blocks of code")),
 (switch_option "single_module", (hidden),
     (help "Build the library so it contains only one module")),
 (parameter_option "install_name", (hidden),
     (help "File name the library will be installed in")),
 (parameter_option "compatibility_version", (hidden),
     (help "Compatibility version number")),
 (parameter_option "current_version", (hidden),
     (help "Current version number")),
 (switch_option "nodefaultlibs", (hidden),
     (help "Do not use the standard system libraries when linking.  Only the libraries you specify will be passed to the linker."))
]>;

// Option preprocessor.

def Preprocess : OptionPreprocessor<
(case (not (any_switch_on ["O0", "O1", "O2", "O3", "Os"])),
           (set_option "Os"),
      (and (switch_on "Os"), (any_switch_on ["O0", "O1", "O2", "O3"])),
           (unset_option ["O0", "O1", "O2", "O3"]),
      (and (switch_on "O3"), (any_switch_on ["O0", "O1", "O2"])),
           (unset_option ["O0", "O1", "O2"]),
      (and (switch_on "O2"), (any_switch_on ["O0", "O1"])),
           (unset_option ["O0", "O1"]),
      (switch_on ["O1", "O0"]),
           (unset_option "O0"))
>;

// Tools


class clang_based<string language, string cmd, string ext_E> : Tool<
[(in_language language),
 (out_language "llvm-bitcode"),
 (output_suffix "bc"),
 (command cmd),
 (actions (case (switch_on "E"),
                    [(forward "E"), (stop_compilation), (output_suffix ext_E)],
                (and (switch_on "E"), (empty "o")), (no_out_file),
                (switch_on "fsyntax-only"), (stop_compilation),
                (switch_on "emit-llvm"), (stop_compilation),
                (switch_on "O0"), (forward "O0"),
                (switch_on "O1"), (forward "O1"),
                (switch_on "O2"), (forward "O2"),
                (switch_on "O3"), (forward "O3"),
                (switch_on "Os"), (forward "Os"),
                (not_empty "D"), (forward "D"),
                (not_empty "include"), (forward "include"),
                (not_empty "I"), (forward "I"))),
 (sink)
]>;

def clang_c : clang_based<"c", "clang -ccc-host-triple xpic -x c -c -emit-llvm", "i">;
//def clang_c : clang_based<"c", "clang -x c -c -emit-llvm", "i">;
def clang_cpp : clang_based<"c++", "clang -x c++ -c -emit-llvm", "i">;
def clang_objective_c : clang_based<"objective-c",
    "clang -x objective-c -c -emit-llvm", "mi">;
def clang_objective_cpp : clang_based<"objective-c++",
    "clang -x objective-c++ -c -emit-llvm", "mi">;

def opt : Tool<
[(in_language "llvm-bitcode"),
 (out_language "llvm-bitcode"),
 (output_suffix "bc"),
 (actions (case (not_empty "Wo,"), (forward_value "Wo,"),
                (switch_on "O1"), (forward "O1"),
                (switch_on "O2"), (forward "O2"),
                (switch_on "O3"), (forward "O3"))),
 (command "opt -f")
]>;

def llvm_as : Tool<
[(in_language "llvm-assembler"),
 (out_language "llvm-bitcode"),
 (output_suffix "bc"),
 (command "llvm-as"),
 (actions (case (switch_on "emit-llvm"), (stop_compilation)))
]>;

def llvm_gcc_assembler : Tool<
[(in_language "assembler"),
 (out_language "object-code"),
 (output_suffix "o"),
 (command "xpic-as"),
 (actions (case
          (switch_on "c"), (stop_compilation),
          (not_empty "arch"), (forward "arch"),
          (not_empty "Xassembler"), (forward "Xassembler"),
          (not_empty "Wa,"), (forward_value "Wa,")))
]>;

def llc : Tool<
[(in_language ["llvm-bitcode", "llvm-assembler"]),
 (out_language "assembler"),
 (output_suffix "s"),
 (command "llc -march=xpic"),
 (actions (case
          (switch_on "S"), (stop_compilation),
          (switch_on "O0"), [(forward "O0"), (append_cmd "-regalloc=linearscan")],          
          (switch_on "O1"), (forward "O1"),
          (switch_on "O2"), (forward "O2"),
          (switch_on "O3"), (forward "O3"),
          (switch_on "Os"), (append_cmd "-O1"),
          (switch_on "fPIC"), (append_cmd "-relocation-model=pic"),
          (switch_on "mdynamic-no-pic"),
                     (append_cmd "-relocation-model=dynamic-no-pic"),
          /*(not_empty "march"), (forward "march"),*/
          (not_empty "mtune"), (forward_as "mtune", "-mcpu"),
          (not_empty "mcpu"), (forward "mcpu"),
          (not_empty "m"), (forward_transformed_value "m", "ConvertToMAttr"),
          (not_empty "Wllc,"), (forward_value "Wllc,")))
]>;

// Base class for linkers
class llvm_gcc_based_linker <string cmd_prefix> : Tool<
[(in_language "object-code"),
 (out_language "executable"),
 (output_suffix "out"),
 (command cmd_prefix),
 (works_on_empty (case (not_empty "filelist"), true,
                       (default), false)),
 (join),
 (actions (case
          (switch_on "pthread"), (append_cmd "-lpthread"),
          (not_empty "L"), (forward "L"),
          (not_empty "F"), (forward "F"),
          (not_empty "arch"), (forward "arch"),
          (not_empty "Map"), (forward "Map"),
          (not_empty "T"), (forward "T"),
          (not_empty "framework"), (forward "framework"),
          (not_empty "weak_framework"), (forward "weak_framework"),
          (not_empty "filelist"), (forward "filelist"),
          (switch_on "m32"), (forward "m32"),
          (switch_on "m64"), (forward "m64"),
          (not_empty "l"), (forward "l"),
          (not_empty "Xlinker"), (forward "Xlinker"),
          (not_empty "Wl,"), (forward_value "Wl,"),
          (switch_on "dynamiclib"), (forward "dynamiclib"),
          (switch_on "prebind"), (forward "prebind"),
          (switch_on "dead_strip"), (forward "dead_strip"),
          (switch_on "single_module"), (forward "single_module"),
          (not_empty "compatibility_version"),
                     (forward "compatibility_version"),
          (not_empty "current_version"), (forward "current_version"),
          (not_empty "install_name"), (forward "install_name"),
          (not (switch_on "nodefaultlibs")), [(append_cmd "$CALL(MakeSysLibPath, '/lib')"),(append_cmd "$CALL(MakeSysLibPath, '/lib/gcc/xpic-hilscher-elf/0.1.1')"),(append_cmd "-lm"),(append_cmd "-lc"),(append_cmd "-lgcc")] ))
]>;

// Default linker
def llvm_gcc_linker : llvm_gcc_based_linker<"xpic-ld">;
// Alternative linker for C++
def llvm_gcc_cpp_linker : llvm_gcc_based_linker<"xpic-ld">;

// Language map

def LanguageMap : LanguageMap<[
     (lang_to_suffixes "c++", ["cc", "cp", "cxx", "cpp", "CPP", "c++", "C"]),
     (lang_to_suffixes "c++-header", ["hpp"]),
     (lang_to_suffixes "c", ["c"]),
     (lang_to_suffixes "c-header", ["h"]),
     (lang_to_suffixes "objective-c++", ["mm"]),
     (lang_to_suffixes "objective-c++-header", ["hmm"]),
     (lang_to_suffixes "objective-c", ["m"]),
     (lang_to_suffixes "objective-c-header", ["hm"]),
     (lang_to_suffixes "assembler", ["s", "S", "asm", "ASM"]),
     (lang_to_suffixes "llvm-assembler", ["ll"]),
     (lang_to_suffixes "llvm-bitcode", ["bc"]),
     (lang_to_suffixes "object-code", ["o", "*empty*"]),
     (lang_to_suffixes "executable", ["elf"])
     ]>;

// Compilation graph

def CompilationGraph : CompilationGraph<[                             
    (edge "root", "llc"),
    
    (edge "root", "clang_c"),
    (edge "root", "clang_cpp"),
    (edge "root", "clang_objective_c"),
    (edge "root", "clang_objective_cpp"),

    (edge "clang_c", "llc"),
    (edge "clang_cpp", "llc"),
    (edge "clang_objective_c", "llc"),
    (edge "clang_objective_cpp", "llc"),

    (edge "llvm_as", "llc"),

    (optional_edge "root", "llvm_as",
                         (case (switch_on "emit-llvm"), (inc_weight))),
    (optional_edge "llvm_as", "opt", (case (switch_on "opt"), (inc_weight))),
    (edge "opt", "llc"),

    (edge "llc", "llvm_gcc_assembler"),
    (edge "root", "llvm_gcc_assembler"),
    (edge "llvm_gcc_assembler", "llvm_gcc_linker"),
    (optional_edge "llvm_gcc_assembler", "llvm_gcc_cpp_linker",
                 (case
                     (or (input_languages_contain "c++"),
                         (input_languages_contain "objective-c++")),
                     (inc_weight),
                     (or (parameter_equals "linker", "g++"),
                         (parameter_equals "linker", "c++")), (inc_weight))),


    (edge "root", "llvm_gcc_linker"),
    (optional_edge "root", "llvm_gcc_cpp_linker",
                 (case
                     (or (input_languages_contain "c++"),
                         (input_languages_contain "objective-c++")),
                     (inc_weight),
                     (or (parameter_equals "linker", "g++"),
                         (parameter_equals "linker", "c++")), (inc_weight)))
                         
    ]>;
