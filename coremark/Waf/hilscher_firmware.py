#! /usr/bin/env python
# encoding: utf-8

from waflib import Task, Utils
from waflib.Configure import conf
from waflib.TaskGen import feature, after_method, before_method
from waflib.Context import STDOUT, BOTH
from waflib import Logs
from hilscher_netx import aboot_header_elements, aboot_magiccookie, aboot_signature
import hilscher_toolchains
import re

''' ------------------------------------------
   Generate information about used libraries
   ------------------------------------------ '''
@feature('libsused')
@after_method('apply_link')
def build_libsused(self):
  # create task and adapt its env

  inputs = []

  use = Utils.to_list(getattr(self, 'use', []))
  used_components = []

  def libsused_add_component(component, toolchain):
    # Check if component is already in list
    if not any(_comp['name'] == component for _comp in used_components):
      used_components.append( { 'name'      : component,
                                'toolchain' : toolchain,
                              })

  def libsused_add_recursive_use(use_list, toolchain):
    for _component in use_list:
      tg = self.bld.get_tgen_by_name(_component)

      if not getattr(tg,'posted',None):
        tg.post()

      if getattr(tg, 'link_task', None):
        inputs.append(tg.link_task.outputs[0])     
        toolchain = getattr(tg, 'toolchain', toolchain)
        libsused_add_component(_component, toolchain)

      if(getattr(tg, 'use', None)):
        libsused_add_recursive_use(tg.use, toolchain)

  processed_target = self.name

  if len(use) == 0:
    # Generate global libs_used file
    output = self.path.find_or_declare(self.target)

    for g in self.bld.groups:
      for tg in g:
        if getattr(tg, 'features', None):
          # Only add version information of libraries referenced in programs, or other libraries
          if 'cprogram' in tg.features:
            # We need to parse all used components 
            libsused_add_recursive_use(getattr(tg, 'use', []), tg.toolchain)
          elif 'cstlib' in tg.features:
            libsused_add_component(tg.name, tg.toolchain)
            libsused_add_recursive_use(getattr(tg, 'use', []), tg.toolchain)
  else:
    # This function will be called for thumb_objs / arm_objs if given as target feature
    # or from a libsused build having a use component. 
    if not 'cprogram' in self.features:
        if 'nxo' in self.features:
            # Generate application specific libs used file
            libsused_add_recursive_use(use, self.toolchain)
            tg = self.bld.get_tgen_by_name(self.moduleelf)
            output = self.path.find_or_declare(self.target + '_usedlibs.txt')
            processed_target = tg.name
        elif any(x in self.features for x in ['c', 'cxx', 'asm']):
            # This is a thumb_obj / arm_obj target
            return
        else:
            # The component being investigated is passed in use component
            # This is triggered by a call to bld.generate_libsused()
            tg = self.bld.get_tgen_by_name(use[0])
            libsused_add_recursive_use(Utils.to_list(getattr(tg,'use', [])), tg.toolchain)

            if any(x in self.features for x in ['nxo', 'nxf']):
                # NXO / NXF may provide features = 'libsused' and we need to 
                # use the tg's stuff of first use component, instead of self
                output = self.path.find_or_declare(self.target + '_usedlibs.txt')
                processed_target = tg.name
            else:
                output           = self.path.find_or_declare(self.target)
                processed_target = tg.name
    else:
        # Generate application specific libs used file
        libsused_add_recursive_use(use, self.toolchain)
        output = self.path.find_or_declare(self.target + '_usedlibs.txt')

  self.libs_used_task                   = self.create_task('LibsusedTask', inputs, output)
  self.libs_used_task.env               = self.env.derive()
  self.libs_used_task.used_components   = used_components
  self.libs_used_task.processed_target  = processed_target
  
  if getattr(self, 'install_path', None):
    self.bld.install_files(self.install_path, self.libs_used_task.outputs[0])

@conf
def generate_libsused(bld, name, target, use = [], **kw):
  use = Utils.to_list(use)

  if use:
      #user passed use parameter, so we need to check it
      use = Utils.to_list(use)
      if len(use) > 1:
          # error, we only provide libsused generation for a single target
          bld.fatal("Libused generation only works for a single target, and multiple targets (%u)were given via use" % len(use))

      tg = bld.get_tgen_by_name(use[0])
      if not 'cprogram' in getattr(tg, 'features', []):
          # This is not a cprogram. So we try to check the used components
          # e.g. a firmware target will have the cprogram target in use
          for x in Utils.to_list(getattr(tg, 'use', [])):
              tg = bld.get_tgen_by_name(x)
              if 'cprogram' in getattr(tg, 'features', []):
                  # We've found the possible target, so override use parameter from function call
                  use = [tg.name]
                  break
          else:
              # No cprogram found in given use component
              bld.fatal("Libsused generation did not find any useable program for generation in use parameters (target=%r, use=%r)" % (name, use))

  bld(name             = name,
      target           = target,
      features         = 'libsused',
      use              = use,
      hidden_from_list = True,
      **kw)

class LibsusedTask(Task.Task):
    color   = 'PINK'

    def __str__(self):
        tgt_str = ' '.join([a.nice_path(self.env) for a in self.outputs])
        return '[LIBSUSED] %s -> %s\n' % (self.processed_target, tgt_str)

    def run(self):
        env = self.env
        bld = self.generator.bld
        cwd = bld.variant_dir

        tg_target = bld.get_tgen_by_name(self.processed_target)
        output = ""

        if getattr(tg_target, 'link_task', None):
          output += "Target:  %r\n" % tg_target.link_task.outputs[0].name

          if getattr(tg_target, 'version', None):
              output += "Version: %r\n" % tg_target.version

        output += "\nUsed Libraries\n"
        output += "============================================================================\n"

        sorted_list = sorted(self.used_components, key=lambda k: k['name'])

        prev_prefix = None
        
        for _compdict in sorted_list:
          comp      = _compdict['name']
          toolchain = _compdict['toolchain']
          tg        = bld.get_tgen_by_name(comp)
          
          comp_prefix= comp[ : comp.rfind('/')]
          if prev_prefix != comp_prefix:
            output += '\n'
            output += comp_prefix + '\n'
            output += '-'*len(comp_prefix) + '\n'
            prev_prefix = comp_prefix

          if not getattr(tg, 'link_task', None):
            Logs.debug('libsused: Skipping used task %r, as it has no link_task' % (comp))
            continue
          else:
            out_str = ("%s" % tg.link_task.outputs[0].name).ljust(40)

            if getattr(tg, 'version', None) and getattr(tg, 'description', None):
              # We got a version information from the library itself
              Logs.debug('libsused: Got version information from component %r (V%r, %r)' % (comp, tg.version, tg.description))
              out_str += tg.description + ' V' + tg.version
            else:
              if toolchain is None:
                Logs.debug('libsused: Unable to extract version from component %r, as no toolchain is specified' % comp)
                continue

              # We need to extract it via objdump
              Logs.debug('libsused: Extracting version information from component %r' % (comp))

              try:
                bld.setenv(toolchain)
                (_objdump, _err) = bld.cmd_and_log([bld.env.OBJDUMP, '-sj' '.hilscher_version', tg.link_task.outputs[0].abspath()], cwd=cwd, output=BOTH, quiet=BOTH)

                if len(_err) > 0:
                  # arm-hitex-elf-objdump does not return an error code, but supplies an error on stderr
                  Logs.debug("libsused: Error extracting version information from %s (error=%s)" % (tg.link_task.outputs[0].abspath() ,_err))
                  out_str += "Unknown version"
                  continue
                else:
                  _SECTION_HEADER = "Contents of section .hilscher_version:"
                  _idx = _objdump.index(_SECTION_HEADER)

                  _objdump = _objdump[_idx + len(_SECTION_HEADER) : ].lstrip('\r\n')

                  for line in _objdump.splitlines():
                    if len(line) == 0:
                      break

                    # Extract text string from objdump output
                    # Output of objdump is expected in the following format:
                    # ' 0000 20726358 2056322e 302e382e 3138206e   rcX V2.0.8.18 n'
                    # ' 0010 65745820 3130302f 35303020 44726976  etX 100/500 Driv'
                    # ' 0020 65727300                             ers.            '
                    #
                    # So we can use a double space to seperate text from hexdump
                    _result = line[ line.index('  ') + 2 : ].lstrip(' ')
                    out_str += _result

                  out_str = out_str.rstrip('. ')

              except Exception as e:
                if getattr(e, 'stderr', None):
                  Logs.debug("libsused: Error extracting version information from %s (error=%s)" % (tg.link_task.outputs[0].abspath(), e.stderr))
                else:
                  Logs.debug("libsused: Error extracting version information from %s (error=%s)" % (tg.link_task.outputs[0].abspath(), e))
                out_str += "Unknown version"

            output += out_str + '\n'

        f = open(self.outputs[0].abspath(),'wt')
        f.write(output)
        f.close()

''' ------------------------------------------
   NXO/Loadable Module
   ------------------------------------------ '''
@feature('remove_librcx')
@after_method('apply_link', 'process_use')
def process_removelibrcx(self):
  # features are also applied to Objects, so we will receive
  # a call for thumb_objs / arm_objs which don't have link task
  if not getattr(self, 'link_task', None):
      return

  tmp_list = list(self.link_task.env['STLIB'])
  for lib_to_remove in tmp_list:
      # find archives the name starts with 'rcx'
      # do not remove 'rcx_module*' and 'rcx_netxNNN_2portswitch'
      m = re.match('rcx(_(vol|bas|mid|midshm|hif_cifx|netx\d+(_physhif|_hif)?))?$', lib_to_remove)
      if m:
          # remove and warn, because there is a wrong use-component in wscript for this target
          Logs.pprint('YELLOW', "Warning: use-component '{0}' removed from build of target '{1}'".format(lib_to_remove, self.name))
          self.link_task.env['STLIB'].remove(lib_to_remove)

@conf
def module(bld, target, fileheader_source, platform = None, toolchain = None,**kw):
  kw['platform']  = platform
  kw['toolchain'] = toolchain

  name             = kw.pop('name', target)
  description      = kw.pop('description', '')
  use              = kw.pop('use',        [])
  source           = kw.pop('source',     [])
  source_arm       = kw.pop('source_arm', [])

  if not handle_toolchain(bld,kw):
    return

  tempelf_target = bld.path.find_or_declare(target)
  tempelf_target = str(tempelf_target.change_ext(''))
  tempelf_name   = target + '.nxo'

  install_path = kw.pop('install_path', None)

  features_program = Utils.to_list(kw.pop('features', [])) + ['remove_libc', 'remove_librcx', 'buildstamp']
  features_nxo     = []

  if 'libsused' in features_program:
      features_program.remove('libsused')
      features_nxo = ['libsused']

  # programm
  bld.program(
          target      = tempelf_target,
          name        = tempelf_name,
          features    = features_program,
          source      = source,
          source_arm  = source_arm,
          use         = use,
          linkflags   = Utils.to_list(kw.pop('linkflags', [])) + ['-Wl,-r'],
          hidden_from_list = True,
          **kw)

  nxo_sources = Utils.to_list(fileheader_source) + Utils.to_list(kw.get('taglist_source', []))

  bld(features      = features_nxo + ['c', 'nxo'],
      source        = nxo_sources,
      target        = target,
      name          = name,
      moduleelf     = tempelf_name,
      fileheader_source = fileheader_source,
      use           = Utils.to_list(use),
      description   = description,
      install_path  = install_path,
      **kw)

import io
from struct import unpack_from, pack

class Elffile:
  ELF_MAGIC           = "\x7FELF"

  FILE_OFFS_IDENT      = 0

  EI_CLASS             = 4
  ELFCLASSNONE         = 0
  ELFCLASS32           = 1
  ELFCLASS64           = 2

  EI_DATA              = 5
  ELFDATANONE          = 0
  ELFDATA2LSB          = 1  # Little-Endian
  ELFDATA2MSB          = 2  # Big-Endian

  EI_VERSION           = 6
  EV_NONE              = 0
  EV_CURRENT           = 1

  FILE_OFFS_TYPE      = 16
  ET_NONE             = 0
  ET_REL              = 1
  ET_EXEC             = 2
  ET_DYN              = 3

  FILE_OFFS_MACHINE   = 18
  EM_ARM              = 40

  FILE_OFFS_VERSION   = 20
  FILE_OFFS_ENTRY     = 24
  FILE_OFFS_PHOFF     = 28
  FILE_OFFS_SHOFF     = 32
  FILE_OFFS_FLAGS     = 36
  FILE_OFFS_EHSIZE    = 40
  FILE_OFFS_PHENTSIZE = 42
  FILE_OFFS_PHNUM     = 44
  FILE_OFFS_SHENTSIZE = 46
  FILE_OFFS_SHNUM     = 48
  FILE_OFFS_SHSTRNDX  = 50

  class Elf32Rel:
    def __init__(self):
      pass

  class Elf32Section:
    OFFS_SH_NAME      = 0
    OFFS_SH_TYPE      = 4
    OFFS_SH_FLAGS     = 8
    OFFS_SH_ADDR      = 12
    OFFS_SH_OFFSET    = 16
    OFFS_SH_SIZE      = 20
    OFFS_SH_LINK      = 24
    OFFS_SH_INFO      = 28
    OFFS_SH_ADDRALIGN = 32
    OFFS_SH_ENTSIZE   = 36

    SHT_NULL          = 0
    SHT_PROGBITS      = 1
    SHT_SYMTAB        = 2
    SHT_STRTAB        = 3
    SHT_RELA          = 4
    SHT_HASH          = 5
    SHT_DYNAMIC       = 6
    SHT_NOTE          = 7
    SHT_NOBITS        = 8
    SHT_REL           = 9
    SHT_SHLIB         = 10
    SHT_DYNSYM        = 11

    SHF_WRITE         = 1
    SHF_ALLOC         = 2
    SHF_EXECINSTR     = 4

    SHN_UNDEF         = 0
    SHN_LORESERVE     = 0xff00
    SHN_LOPROC        = 0xff00
    SHN_HIPROC        = 0xff1f
    SHN_ABS           = 0xfff1
    SHN_COMMON        = 0xfff2
    SHN_HIRESERVE     = 0xffff

    def __init__(self):
      pass

    def parse_rel_section(self):
      ret = []

      if self.sh_type == self.SHT_REL:
        for _relidx in range(0, self.sh_size, self.sh_entsize):
          rel = Elffile.Elf32Rel()
          rel.r_offset = unpack_from("<L", self.data, _relidx)[0]
          rel.r_info   = unpack_from("<L", self.data, _relidx + 4)[0]
          rel.r_sym    = rel.r_info >> 8
          rel.r_type   = rel.r_info & 0xFF
          ret.append(rel)

      return ret


  class Elf32Sym:
    OFFS_ST_NAME    = 0
    OFFS_ST_VALUE   = 4
    OFFS_ST_SIZE    = 8
    OFFS_ST_INFO    = 12
    OFFS_ST_OTHER   = 13
    OFFS_ST_SHNDX   = 14

    ST_SIZE         = 16
    
    STT_NOTYPE      = 0
    STT_OBJECT      = 1
    STT_FUNC        = 2
    STT_SECTION     = 3
    STT_FILE        = 4
    STT_LOPROC      = 13
    STT_HIPROC      = 15

    STB_LOCAL       = 0
    STB_GLOBAL      = 2
    STB_WEAK        = 3
    STB_LOPROC      = 13
    STB_HIPROC      = 15

    def __init__(self):
      pass
      
    def get_binding(self):
      return self.st_info & 0xF0 >> 4

    def get_type(self):
      return self.st_info & 0xF

  class ARM32Relocations:
    R_ARM_NONE          =   0
    R_ARM_PC24          =   1
    R_ARM_ABS32         =   2
    R_ARM_REL32         =   3
    R_ARM_PC13          =   4
    R_ARM_ABS16         =   5
    R_ARM_ABS12         =   6
    R_ARM_THM_ABS5      =   7
    R_ARM_ABS8          =   8
    R_ARM_SBREL32       =   9
    R_ARM_THM_CALL      =  10
    R_ARM_THM_PC8       =  11
    R_ARM_AMP_VCALL9    =  12
    R_ARM_SWI24         =  13
    R_ARM_THM_SWI8      =  14
    R_ARM_XPC25         =  15
    R_ARM_THM_XPC22     =  16
    R_ARM_COPY          =  20
    R_ARM_GLOB_DAT      =  21
    R_ARM_JUMP_SLOT     =  22
    R_ARM_RELATIVE      =  23
    R_ARM_GOTOFF        =  24
    R_ARM_GOTPC         =  25
    R_ARM_GOT32         =  26
    R_ARM_PLT32         =  27
    R_ARM_CALL          =  28
    R_ARM_JUMP24        =  29
    R_ARM_THM_JUMP24    =  30
    R_ARM_V4BX          =  40
    R_ARM_PREL31        =  42
    R_ARM_GNU_VTENTRY   = 100
    R_ARM_GNU_VTINHERIT = 101
    R_ARM_THM_PC11      = 102
    R_ARM_THM_PC9       = 103
    R_ARM_RXPC25        = 249
    R_ARM_RSBREL32      = 250
    R_ARM_THM_RPC22     = 251
    R_ARM_RREL32        = 252
    R_ARM_RABS22        = 253
    R_ARM_RPC24         = 254
    R_ARM_RBASE         = 255

    @staticmethod
    def get_name(r_type):
      _reloc_names = { Elffile.ARM32Relocations.R_ARM_NONE          :  "R_ARM_NONE",
                       Elffile.ARM32Relocations.R_ARM_PC24          :  "R_ARM_PC24",
                       Elffile.ARM32Relocations.R_ARM_ABS32         :  "R_ARM_ABS32",
                       Elffile.ARM32Relocations.R_ARM_REL32         :  "R_ARM_REL32",
                       Elffile.ARM32Relocations.R_ARM_PC13          :  "R_ARM_PC13",
                       Elffile.ARM32Relocations.R_ARM_ABS16         :  "R_ARM_ABS16",
                       Elffile.ARM32Relocations.R_ARM_ABS12         :  "R_ARM_ABS12",
                       Elffile.ARM32Relocations.R_ARM_THM_ABS5      :  "R_ARM_THM_ABS5",
                       Elffile.ARM32Relocations.R_ARM_ABS8          :  "R_ARM_ABS8",
                       Elffile.ARM32Relocations.R_ARM_SBREL32       :  "R_ARM_SBREL32",
                       Elffile.ARM32Relocations.R_ARM_THM_CALL      :  "R_ARM_THM_CALL",
                       Elffile.ARM32Relocations.R_ARM_THM_PC8       :  "R_ARM_THM_PC8",
                       Elffile.ARM32Relocations.R_ARM_AMP_VCALL9    :  "R_ARM_AMP_VCALL9",
                       Elffile.ARM32Relocations.R_ARM_SWI24         :  "R_ARM_SWI24",
                       Elffile.ARM32Relocations.R_ARM_THM_SWI8      :  "R_ARM_THM_SWI8",
                       Elffile.ARM32Relocations.R_ARM_XPC25         :  "R_ARM_XPC25",
                       Elffile.ARM32Relocations.R_ARM_THM_XPC22     :  "R_ARM_THM_XPC22",
                       Elffile.ARM32Relocations.R_ARM_COPY          :  "R_ARM_COPY",
                       Elffile.ARM32Relocations.R_ARM_GLOB_DAT      :  "R_ARM_GLOB_DAT",
                       Elffile.ARM32Relocations.R_ARM_JUMP_SLOT     :  "R_ARM_JUMP_SLOT",
                       Elffile.ARM32Relocations.R_ARM_RELATIVE      :  "R_ARM_RELATIVE",
                       Elffile.ARM32Relocations.R_ARM_GOTOFF        :  "R_ARM_GOTOFF",
                       Elffile.ARM32Relocations.R_ARM_GOTPC         :  "R_ARM_GOTPC",
                       Elffile.ARM32Relocations.R_ARM_GOT32         :  "R_ARM_GOT32",
                       Elffile.ARM32Relocations.R_ARM_PLT32         :  "R_ARM_PLT32",
                       Elffile.ARM32Relocations.R_ARM_CALL          :  "R_ARM_CALL",
                       Elffile.ARM32Relocations.R_ARM_JUMP24        :  "R_ARM_JUMP24",
                       Elffile.ARM32Relocations.R_ARM_V4BX          :  "R_ARM_V4BX",
                       Elffile.ARM32Relocations.R_ARM_THM_JUMP24    :  "R_ARM_THM_JUMP24",
                       Elffile.ARM32Relocations.R_ARM_PREL31        :  "R_ARM_PREL31",
                       Elffile.ARM32Relocations.R_ARM_GNU_VTENTRY   :  "R_ARM_GNU_VTENTRY",
                       Elffile.ARM32Relocations.R_ARM_GNU_VTINHERIT :  "R_ARM_GNU_VTINHERIT",
                       Elffile.ARM32Relocations.R_ARM_THM_PC11      :  "R_ARM_THM_PC11",
                       Elffile.ARM32Relocations.R_ARM_THM_PC9       :  "R_ARM_THM_PC9",
                       Elffile.ARM32Relocations.R_ARM_RXPC25        :  "R_ARM_RXPC25",
                       Elffile.ARM32Relocations.R_ARM_RSBREL32      :  "R_ARM_RSBREL32",
                       Elffile.ARM32Relocations.R_ARM_THM_RPC22     :  "R_ARM_THM_RPC22",
                       Elffile.ARM32Relocations.R_ARM_RREL32        :  "R_ARM_RREL32",
                       Elffile.ARM32Relocations.R_ARM_RABS22        :  "R_ARM_RABS22",
                       Elffile.ARM32Relocations.R_ARM_RPC24         :  "R_ARM_RPC24",
                       Elffile.ARM32Relocations.R_ARM_RBASE         :  "R_ARM_RBASE",
                      }

      if r_type in _reloc_names:
        ret = _reloc_names[r_type]
      else:
        ret = "(%d)" % r_type

      return ret


  def __init__(self, elffilename=None):
    self._elffiledata = None

    if(elffilename):
      self.load(elffilename)

  def load(self, file):
    elffile = io.open(file, "rb")
    _elffiledata = elffile.read()
    elffile.close()

    # Check ELF file for being a ELF program for ARM Little Endian
    if _elffiledata[0:len(self.ELF_MAGIC)] != self.ELF_MAGIC:
        raise Exception("Input file '%s' is not an ELF file" % (file))

    if _elffiledata[self.EI_VERSION] != chr(self.EV_CURRENT):
        raise Exception("Input file '%s' has unknown ELF file version" % (file))

    if _elffiledata[self.EI_CLASS] != chr(self.ELFCLASS32):
        raise Exception("Input file '%s' is not an ELF32 file" % (file))

    if _elffiledata[self.EI_DATA] != chr(self.ELFDATA2LSB):
        raise Exception("Input file '%s' is not an ELF32 LSB file" % (file))

    e_type = unpack_from("<H", _elffiledata, self.FILE_OFFS_TYPE)[0]
    if e_type != self.ET_REL:
        raise Exception("Input file '%s' is not a relocatable (e_type=%r)" % (file, e_type))

    e_machine = unpack_from("<H", _elffiledata, self.FILE_OFFS_MACHINE)[0]

    if e_machine != self.EM_ARM:
        raise Exception("Input file '%s' is not an ARM target ELF (e_machine=%r)" % (file, e_machine))

    e_version = unpack_from("<H", _elffiledata, self.FILE_OFFS_VERSION)[0]

    if e_version != self.EV_CURRENT:
        raise Exception("File '%s' has unknown ELF file version (e_version=%r)" % (file, e_version))

    self._elffiledata = _elffiledata
    self.e_shoff     = unpack_from("<L", _elffiledata, self.FILE_OFFS_SHOFF)[0]
    self.e_shnum     = unpack_from("<H", _elffiledata, self.FILE_OFFS_SHNUM)[0]
    self.e_shentsize = unpack_from("<H", _elffiledata, self.FILE_OFFS_SHENTSIZE)[0]
    # Get index of section name table
    self.e_shstrnidx = unpack_from("<H", _elffiledata, self.FILE_OFFS_SHSTRNDX)[0]

    self.stringtable_section = None

    if self.e_shstrnidx != 0:
      self.stringtable_section = self.parse_single_section(self.e_shstrnidx)

  def _get_symbol_name(self, section, name_offset):
    ret = ""

    if section:
      # Parse name of section
      for ch in section.data[name_offset:]:
        if(ch == chr(0)):
          break
        ret += ch

    return ret

  def parse_symbol_table(self, symtab_section, strtab_section=None):
    symbol_count = len(symtab_section.data) / self.Elf32Sym.ST_SIZE

    ret = []

    for sym in range(0, symbol_count):
      symbol = self.Elf32Sym()
      offset = sym * self.Elf32Sym.ST_SIZE

      symbol.st_name  = unpack_from("<L", symtab_section.data, offset + symbol.OFFS_ST_NAME)[0]
      symbol.st_value = unpack_from("<L", symtab_section.data, offset + symbol.OFFS_ST_VALUE)[0]
      symbol.st_size  = unpack_from("<L", symtab_section.data, offset + symbol.OFFS_ST_SIZE)[0]
      symbol.st_info  = unpack_from("<B", symtab_section.data, offset + symbol.OFFS_ST_INFO)[0]
      symbol.st_other = unpack_from("<B", symtab_section.data, offset + symbol.OFFS_ST_OTHER)[0]
      symbol.st_shndx = unpack_from("<H", symtab_section.data, offset + symbol.OFFS_ST_SHNDX)[0]

      symbol.name = self._get_symbol_name(strtab_section, symbol.st_name)

      ret.append(symbol)

    return ret

  def parse_single_section(self, section_number, get_data=True):
    if section_number > self.e_shnum:
      raise Exception("Invalid section (%r) requested. shnum=%r" % (section_number, self.e_shnum))

    ret = self.Elf32Section()
    section_header_start = self.e_shoff + section_number * self.e_shentsize

    ret.sh_name = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_NAME)[0]
    ret.name = self._get_symbol_name(self.stringtable_section, ret.sh_name)

    ret.idx           = section_number
    ret.sh_type       = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_TYPE)[0]
    ret.sh_flags      = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_FLAGS)[0]
    ret.sh_addr       = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_ADDR)[0]
    ret.sh_offset     = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_OFFSET)[0]
    ret.sh_size       = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_SIZE)[0]
    ret.sh_link       = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_LINK)[0]
    ret.sh_info       = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_INFO)[0]
    ret.sh_addralign  = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_ADDRALIGN)[0]
    ret.sh_entsize    = unpack_from("<L", self._elffiledata, section_header_start + self.Elf32Section.OFFS_SH_ENTSIZE)[0]

    if get_data:
      ret.data = self._elffiledata[ret.sh_offset : ret.sh_offset + ret.sh_size]

    return ret


  def parse_sections(self, include_sections = None):
    ret = {}

    for shidx in range(0, self.e_shnum):
      section = self.parse_single_section(shidx)
      ret[section.name] = section

    return ret

def NxoBuilder(self, outputfile, firmware_elfname, header_elfname, taglist_elfname):

    cmd = [self.env['STRIP'], firmware_elfname, '-g', '--strip-unneeded', '-o', firmware_elfname + '.stripped']
    result = self.generator.bld.cmd_and_log(cmd, output=STDOUT, quiet=STDOUT)

    fw_elf       = Elffile(firmware_elfname + '.stripped')
    fw_sections  = fw_elf.parse_sections()
    symbol_table = fw_sections['.symtab']
    success      = True

    # Check symbol table for undefined / common symbols which are not allowed in NXO's
    fw_symbols = fw_elf.parse_symbol_table(symbol_table, fw_sections['.strtab'] or None)

    # Sort sections by idx, so we can index them via st_shndx
    sections_byidx = [fw_sections[x] for x in sorted(fw_sections, key=lambda x : fw_sections[x].idx)]

    Logs.debug('nxo: Checking %u symbols for undefined references' % (len(fw_symbols) - 1))

    # First symbol is always undefined and can safely be ignored
    for symbol in fw_symbols[1:]:
      if symbol.st_shndx == Elffile.Elf32Section.SHN_UNDEF:
        # Found undefined symbol which is not allowed in NXO
        Logs.pprint('RED', 'ERROR: Found undefined symbol %r' % symbol.name)
        success = False
      elif symbol.st_shndx == Elffile.Elf32Section.SHN_COMMON:
        # Found COMMON symbol which is not allowed in NXO
        Logs.pprint('RED', 'ERROR: Found COMMON symbol %r' % symbol.name)
        success = False

    if success:
      # Check for unsupported relocation types
      Logs.debug('nxo: Checking for unsupported relocation types')
      log_relocationtypes = {}

      supported_relocations = [ Elffile.ARM32Relocations.R_ARM_NONE,
                                Elffile.ARM32Relocations.R_ARM_ABS32,
                                Elffile.ARM32Relocations.R_ARM_REL32,
                                Elffile.ARM32Relocations.R_ARM_CALL,
                                Elffile.ARM32Relocations.R_ARM_JUMP24,
                                Elffile.ARM32Relocations.R_ARM_V4BX,
                                Elffile.ARM32Relocations.R_ARM_PLT32,
                                Elffile.ARM32Relocations.R_ARM_PC24,
                                Elffile.ARM32Relocations.R_ARM_XPC25,
                                Elffile.ARM32Relocations.R_ARM_PREL31,
                                Elffile.ARM32Relocations.R_ARM_THM_CALL,
                                Elffile.ARM32Relocations.R_ARM_THM_JUMP24,
                                Elffile.ARM32Relocations.R_ARM_THM_XPC22,
                                Elffile.ARM32Relocations.R_ARM_THM_PC11,
                              ]

      for section_name in fw_sections:
        section = fw_sections[section_name]
        if section.sh_type == section.SHT_RELA:
          # Found COMMON symbol which is not allowed in NXO
          Logs.pprint('RED', 'ERROR: Found unsupported RELA relocation section %r' % section_name)
          success = False
        elif section.sh_type == section.SHT_REL:
          # Check REL section for supported relocation types
          relocs = section.parse_rel_section()
          for _reloc in relocs:
            ref_symbol = fw_symbols[_reloc.r_sym]

            _log_reloc_name = _reloc_name = Elffile.ARM32Relocations.get_name(_reloc.r_type)

            if (ref_symbol.st_info & 0xf) == ref_symbol.STT_SECTION:
              ref_symbol_name = sections_byidx[ref_symbol.st_shndx].name
            else:
              ref_symbol_name = ref_symbol.name

            if _reloc.r_sym > len(fw_symbols):
              # Invalid symbol reference in relocation
              Logs.pprint('RED', 'ERROR: Found relocation with invalid symbol reference %d in section %s at r_offset %#010x' % (_reloc.r_sym, section_name, _reloc.r_offset ))
              success = False
            elif _reloc.r_type not in supported_relocations:
              # Unsupported relocation
              Logs.pprint('RED', 'ERROR: Found unsupported relocation type %s in section %s for symbol %s' %
                          (_reloc_name,
                          section_name,
                          ref_symbol_name) )
              success = False
            else:
              # Check relocations for other unsupported stuff (e.g. unallowed ARM<-->Thumb transitions, which would need veneers)
              if _reloc.r_type in [ Elffile.ARM32Relocations.R_ARM_CALL,
                                    Elffile.ARM32Relocations.R_ARM_JUMP24,
                                    Elffile.ARM32Relocations.R_ARM_PLT32,
                                    Elffile.ARM32Relocations.R_ARM_PC24,
                                    Elffile.ARM32Relocations.R_ARM_XPC25,
                                  ]:
                # Check for unallowed transitions from ARM-->Thumb
                if ref_symbol.st_value & 1:
                  # Mark this as a ARM-->Thumb transition for the log
                  _log_reloc_name = _log_reloc_name + " (ARM->Thumb)"
              elif _reloc.r_type in [ Elffile.ARM32Relocations.R_ARM_THM_CALL,
                                      Elffile.ARM32Relocations.R_ARM_THM_JUMP24,
                                      Elffile.ARM32Relocations.R_ARM_THM_XPC22,
                                    ]:
                if (ref_symbol.st_value & 1) == 0:
                  # Mark this as a Thumb-->ARM transition for the log
                  _log_reloc_name = _log_reloc_name + " (Thumb-->ARM)"

            if not _log_reloc_name in log_relocationtypes:
              log_relocationtypes[_log_reloc_name] = 1
            else:
              log_relocationtypes[_log_reloc_name] = log_relocationtypes[_log_reloc_name] + 1

    Logs.debug('nxo: Following relocations have been checked:')
    for _tmp in log_relocationtypes:
        Logs.debug('nxo:   * %s (Count=%u)' % (_tmp, log_relocationtypes[_tmp]))

    if success:
      fileheader_data = ".NXO" + ('\0' * 60)

      # Create NXO file
      header_binname = header_elfname.change_ext('.bin')

      # Use Objcopy to generate binary file
      cmd = [self.env['OBJCOPY'], '-O', 'binary', header_elfname.abspath(), header_binname.abspath()]
      result = self.generator.bld.cmd_and_log(cmd, output=STDOUT, quiet=STDOUT)

      tmp = io.open(header_binname.abspath(), "rb")
      fileheader_data += tmp.read()
      tmp.close()

      tmp = io.open(firmware_elfname + '.stripped', "rb")
      elf_data = tmp.read()
      tmp.close()

      taglist_data = None
      if taglist_elfname:
        # Check Taglist object file
        # It must not contain multiple data sections starting all at offset 0, which results in an unusable file produced by OBJCOPY
        taglist_elf       = Elffile(taglist_elfname.abspath())
        taglist_sections  = taglist_elf.parse_sections()

        # Dict offset -> section name, with sections being included in bin built by OBJCOPY
        _tagsections_in_image = {}

        for _section_name, _tag_section in taglist_sections.items():
          if (_tag_section.sh_size != 0) and (_tag_section.sh_flags & _tag_section.SHF_ALLOC):
            Logs.debug('nxo: Checking section %r at memory offset %#010x' % (_section_name, _tag_section.sh_addr))
            if _tag_section.sh_addr in _tagsections_in_image:
              Logs.pprint('RED', 'ERROR: Taglist contains overlapping sections %r and %r at offset %#010x. Make sure to only provide a single instance for the taglist structure!' % (_section_name, _tagsections_in_image[_tag_section.sh_addr],  _tag_section.sh_addr))
              return False
            else:
              _tagsections_in_image[_tag_section.sh_addr] = _section_name

        taglist_binname = taglist_elfname.change_ext('.bin')
        cmd = [self.env['OBJCOPY'], '-O', 'binary', taglist_elfname.abspath(), taglist_binname.abspath()]
        result = self.generator.bld.cmd_and_log(cmd, output=STDOUT, quiet=STDOUT)

        tmp = io.open(taglist_binname.abspath(), "rb")
        taglist_data = tmp.read()
        tmp.close()

        # Append EndOfList Marker, if it does not exist yet. Some firmwares rely on the linker (taglist.ld)
        # to add the missing 64 Bit zeros, but as we extract the taglist from an object the EOL is missing 
        # in this case
        _offset   = 0
        _eolfound = False

        while _offset < len(taglist_data):
          _tagtype   = unpack_from("<L", taglist_data, _offset)[0]
          _taglength = unpack_from("<L", taglist_data, _offset + 4)[0]

          _eolfound = False
          if (_tagtype is 0) and (_taglength is 0):
            _eolfound = True

          _offset += 8 + _taglength

          # new tags always start at DWORD aligned offsets
          if (_taglength % 4) != 0:
            _offset += 4 - _taglength % 4

        if not _eolfound:
          Logs.pprint('YELLOW', "[NXO] '%s': Adding missing END_OF_LIST marker to taglist" % outputfile)
          taglist_data += pack("<Q", 0)

      offset_common_header_data_size    = 72
      offset_common_header_data_offs    = 76
      offset_common_header_taglist_size = 100
      offset_common_header_taglist_offs = 104

      # Modify Common Header to include correct data and tag list size / offset
      fileheader_data = fileheader_data[:offset_common_header_data_size] + pack("<L", len(elf_data)) + fileheader_data[offset_common_header_data_size+4:]
      fileheader_data = fileheader_data[:offset_common_header_data_offs] + pack("<L", len(fileheader_data)) + fileheader_data[offset_common_header_data_offs+4:]
      if taglist_data:
        fileheader_data = fileheader_data[:offset_common_header_taglist_size] + pack("<L", len(taglist_data)) + fileheader_data[offset_common_header_taglist_size+4:]
        fileheader_data = fileheader_data[:offset_common_header_taglist_offs] + pack("<L", len(fileheader_data) + len(elf_data)) + fileheader_data[offset_common_header_taglist_offs+4:]

      nxo_file = io.open(outputfile, "wb")
      nxo_file.write(fileheader_data + elf_data)
      if taglist_data:
        nxo_file.write(taglist_data)
      size = nxo_file.tell()

      if size % 4:
        success = False
        Logs.pprint('RED', "ERROR: [NXO] '%s': Resulting filesize is not a multiple of UINT32 (size=%r)" % (outputfile, size))

      nxo_file.close()

      if success:
        # Update NXO checksums
        nxupdate_fn(outputfile, outputfile)

    return success

@feature('nxo')
@after_method('apply_link', 'process_source')
def build_nxo(self):
    '''
    Function to build a netX loadable module.
    '''
    if not getattr(self, 'use', None):
      self.bld.fatal("Building a NXO requires 'use' parameter")

    inputs =  [None,None,None]
    outputs = [self.path.find_or_declare(self.target)]
    
    # Search our elf file to convert
    try:
        tg = self.bld.get_tgen_by_name(self.moduleelf)
        # is already posted
        if not getattr(tg,'posted',None):
          tg.post()
    except self.bld.errors.WafError:
        Logs.pprint('YELLOW',"Skipping nxo-file \'%s\' because no matching tgen found. (Probably the source elf was not build because of missing toolchain)" % (target))
        return
        
    if getattr(tg, 'link_task', None):
      # Only use .elf files as inputs
      for x in tg.link_task.outputs:
          if x.suffix().lower() == '.elf':
              inputs[0] = x
    else:
        self.bld.fatal('Input tgen has no link task')
      
    fileheader_node = self.path.find_node(self.fileheader_source)
    
    if getattr(self, 'taglist_source', None):
        taglist_node = self.path.find_node(self.taglist_source)
    else:
        taglist_node = none

    for x in self.compiled_tasks:
        if (x.inputs[0] == fileheader_node):
            inputs[1] = x.outputs[0]
        elif (x.inputs[0] == taglist_node):
            inputs[2] = x.outputs[0]

    # create task and adapt its env
    self.nxobuilder_task = self.create_task('NxoBuilderTask', inputs,outputs)
    self.nxobuilder_task.env  = tg.env.derive()

    if getattr(self, 'install_path', None):
        self.bld.install_files(getattr(self, 'install_path', None), self.nxobuilder_task.outputs[0])

class NxoBuilderTask(Task.Task):
    color   = 'PINK'

    def __init__(self, *args, **kwargs):
        Task.Task.__init__(self, *args, **kwargs)
        cmdline = None
        inst_to = None

    def run(self):
        inputfile  = self.inputs[0].abspath()
        outputfile = self.outputs[0].get_bld().abspath()
        fileheader = self.inputs[1]
        taglist    = self.inputs[2]
        
        if not NxoBuilder(self, outputfile, inputfile, fileheader, taglist):
          self.generator.bld.fatal("NXO building failed")

''' ------------------------------------------
   Firmware
   ------------------------------------------ '''
from hilscher_netx import  program,handle_toolchain

@conf
def firmware(bld, target, linkerscript,BOOTBLOCKERFLAGS, platform=None, toolchain=None,**kw):
  """
  Function to build a firmware (nxf/nxo[todo]) from sources and libraries.
  <b>  Mandatory parameters  </b> 
  @param target    \b string: file name of the target (e.g. netX.nxf)
  @param platform  \b string: type of platform, that specified compiler parameter set. Values: \n\e netx - common parameters for all netx platforms \n\e netx500 - parameters specific for netX500 \n\e netx100 - parameters specific for netX100 \n\e netx50  - parameters specific for netX50 \n\e netx51  - parameters specific for netX51 \n\e netx10  - parameters specific for netX10
  @param toolchain \b string: identification of toolchain to use in build. \n For example: \e 'hitex' or \e 'codesourcery'. See 'hilscher_toolchains.py' for more toolchains or define therein a new one. \n
  @param linkerscript \b string: linker script
  @param BOOTBLOCKERFLAGS <b> list of strings: </b>parameters for bootblocker
  \n\n\n
  <b> Optional parameters: </b>\n\n
  @param description \b string: description of this target, will be listed in "waf list"  
  @param sources       <b> list of strings: </b> list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'thumb' mode
  @param sources_thumb <b> list of strings: </b> (equivalent to \e sources) list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'thumb' mode
  @param sources_arm   <b> list of strings: </b> list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'arm' mode
  @param includes <b> list of strings: </b> list of include directories to include in build of source files
  @param use <b> list of strings: </b> list of libraries to use in compile stage (their 'export_defines' and 'export_includes' are used for build too)
  @param defines <b> list of strings: </b> list of define directives to use in build of source files
  \n\n\n
  <b> Additional parameters for compiler, assembler and linker: </b>\n\n
  @param stlib <b> list of strings: </b> used static libraries
  @param stlibpath <b> list of strings: </b> path to static libraries
  @param linkflags <b> list of strings: </b> options, forwarded to the linker
  @param cflags    <b> list of strings: </b> options, forwarded to the C-Compiler
  @param cxxflags  <b> list of strings: </b> options, forwarded to the C++-Compiler
  \n\n\n
  <b> Installation: </b>\n\n
  @param install_path \b string:  (relative) path to install current component while „waf install“
  \n\n\n
  """
  # mandatory parameters
  kw['platform']         = platform
  kw['toolchain']        = toolchain
  kw['linkerscript']     = linkerscript

  if not handle_toolchain(bld,kw):
    return
  
  tgt = bld.path.find_or_declare(target)
  
  nxf_target = target
  nxf_name   = target
    
  prog_target = str(tgt.change_ext(''))
  prog_name   = prog_target
  
  name = kw.pop('name', [])
  if name:
    nxf_name  = name
    prog_name = name + '.elf'
  
  install_path = kw.pop('install_path', [])
  #description  = kw.pop('description',  [])
  #if getattr(kw, 'install_path', None):
  #  install_path = kw.pop('install_path')
  
  features_program = Utils.to_list(kw.pop('features', [])) + ['buildstamp']
  features_nxf     = []

  if 'libsused' in features_program:
      features_program.remove('libsused')
      features_nxf = ['libsused']

  # programm
  bld.program(target           = prog_target, 
              name             = prog_name, 
              install_path     = [], 
              hidden_from_list = True,
              features         = features_program,
              **kw)

  # nxf
  buildnxf(bld,
           target           = target,
           name             = nxf_name,
           use              = prog_name,
           features         = features_nxf,
           BOOTBLOCKERFLAGS = BOOTBLOCKERFLAGS,
           description      = kw.get('description',  " "),
           displaygroup     = kw.pop('displaygroup', 'default'),
           version          = kw.pop('version', None),
           install_path     = install_path)

''' ------------------------------------------
   NXF Update support
   ------------------------------------------ '''
@conf
def buildnxf(bld, target, *k, **kw):
    """
    Function to build a firmware (nxf) from executable (elf).
    <b>  Mandatory parameters  </b> 
    @param target    \b string: file name of firmware
    @param use <b> list of strings: </b> target, from binary will be constructed
    @param BOOTBLOCKERFLAGS <b> list of strings: </b>parameters for bootblocker
    \n\n\n
    <b> Optional parameters: </b>\n\n
    @param description \b string: description of this target, will be listed in "waf list" 
    \n\n\n
    <b> Installation: </b>\n\n
    @param install_path \b string:  (relative) path to install current component while „waf install“
    \n\n\n
    """
    if (not 'BOOTBLOCKERFLAGS' in kw) and (not 'BOOTBLOCKERDATA' in kw):
        bld.fatal("Bootblocker flags are missing")
        
    kw['target']    = target
    kw['name_patch']= '~'+target+'.bin'
    if not 'name' in kw:
      kw['name']    = target

    features = Utils.to_list(kw.pop('features', [])) + ['bootimage', 'nxf']

    bld(features = features, **kw)


def nxupdate_fn(inputfilename, outputfilename):
    import hashlib, array, zlib

    # Get the application data.
    inputfile = open(inputfilename, 'rb')
    filedata = array.array('I')
    filedata.fromstring(inputfile.read())
    inputfile.close()

    offset_aboot_header_checksum = aboot_header_elements['BootChksm']
    offset_aboot_appl_checksum   = aboot_header_elements['AppChksm']
    offset_common_header_md5     = 21
    offset_common_header_crc32   = 31

    # Update NXF checksums  
    filedata[offset_aboot_header_checksum] = 0
    filedata[offset_aboot_appl_checksum]   = 0

    filedata[offset_common_header_md5]     = 0
    filedata[offset_common_header_md5 + 1] = 0
    filedata[offset_common_header_md5 + 2] = 0
    filedata[offset_common_header_md5 +3 ] = 0
    
    filedata[offset_common_header_crc32]   = 0

    # Calculate file MD5
    m = hashlib.md5()
    m.update(filedata)
    file_md5_string = m.digest()

    file_md5 = array.array('I')
    file_md5.fromstring(file_md5_string)
    
    filedata[offset_common_header_md5]     = file_md5[0]
    filedata[offset_common_header_md5 + 1] = file_md5[1]
    filedata[offset_common_header_md5 + 2] = file_md5[2]
    filedata[offset_common_header_md5 +3 ] = file_md5[3]
    
    # Calculate common header CRC32B
    header_crc32 = zlib.crc32(filedata[:32]) & 0xffffffff
    filedata[offset_common_header_crc32]   = header_crc32

    # Only update header checksum on .NXF files, which can be detected via 
    # cookie and signature
    offset_cookie    = aboot_header_elements['MagCok']
    offset_signature = aboot_header_elements['Signt']

    if (filedata[offset_cookie] & 0xFFFFFF00) == aboot_magiccookie and filedata[offset_signature] == aboot_signature:
      Logs.debug('Skipping generation of bootheader checksums for %r' % outputfilename)

      filedata[offset_aboot_header_checksum] = 0
      filedata[offset_aboot_appl_checksum]   = 0

      # Update application and bootheader checksum
      appl_chksum = 0
      for value in filedata[16:]:
        appl_chksum += value
        appl_chksum &= 0xffffffff

      filedata[offset_aboot_appl_checksum] = appl_chksum

      boot_chksum = 0
      for value in filedata[:16]:
        boot_chksum += value
        boot_chksum &= 0xffffffff
        
      filedata[offset_aboot_header_checksum] = (boot_chksum - 1) ^ 0xFFFFFFFF

    outputfile = open(outputfilename, 'wb')
    filedata.tofile(outputfile)
    outputfile.close()


class nxupdate(Task.Task):
    ''' Run objcopy on the target'''
    color     = 'PINK'
    inst_to   = None
    cmdline   = None

    def run(self):
      inputfile   = self.inputs[0].get_bld().abspath()
      outputfile  = self.outputs[0].get_bld().abspath()
      nxupdate_fn(inputfile, outputfile)

@feature('nxf')
@after_method('build_bootimage')
def build_nxf(self):
    '''
    Function to build a netX bootable image by performing an OBJCOPY and
    prepending a netX bootheader (64 byte).
    '''    
    target = self.path.find_or_declare(self.target)
    #target = self.path.make_node(self.name)

    input = self.to_list(getattr(self, 'use', None))
    
    self.nxf_task = self.create_task('nxupdate')
    
    #target = self.path.find_or_declare(self.name_real)
    
    self.nxf_task.outputs     = [target]
    
    self.nxf_task.inputs.extend(self.bootblocker_task.outputs)
    self.nxf_task.set_run_after(self.bootblocker_task)
      
    if getattr(self, 'install_path', None):
      self.bld.install_files(getattr(self, 'install_path', None), self.nxf_task.outputs)


""" HELPER functions """

@conf
def SpecFWFileName(self, DeviceFamily, DeviceForm, netXType,
                   PrimaryProtocol, SecondaryProtocol, HWCompInd):
    """
    Function constructs the name of firmware according to document "Specification Firmware File Names" (revision 3, 2013-04-03) [H:\Manual netX Architecture\System\Firmware Names]
    
    @param DeviceFamily \b string: device family
    @param DeviceForm \b string: device form
    @param netXType \b string: netX type
    @param PrimaryProtocol \b string: primary protocol on stack
    @param SecondaryProtocol \b string: secondary protocol on stack
    @param HWCompInd \b string: hardware compatibility index  
    @return  \b string: Firmware file name
    """        
    #Table 3:
    device_family = {'netbrick'          :{None :'B0', '' :'B0',},
                     'cifx'              :{None :'C0', '' :'C0',},
                     'netdimm'           :{None :'D0', '' :'D0',},
                     'eu5c'              :{None :'E0', '' :'E0',},
                     'nethost'           :{'t100' :'FT'},
                     'netsmart'          :{None :'G0', '' :'G0',},
                     'nethmi'            :{'j500' :'HJ', 'b500' :'HB',},
                     'netic'             :{None :'I0', '' :'I0',},
                     'netjack'           :{None :'J0', '' :'J0',},
                     'netlink'           :{'usb':'LU', 'ethernet':'LN'},
                     'comx'              :{None :'M0', '' :'M0',},
                     'netpac'            :{None :'P0', '' :'P0',},
                     'netrapid'          :{None :'R0', '' :'R0',},
                     'netplc'            :{'c100':'SC', 'd100':'SD', 'j100':'SJ', 'j500':'SJ', 'm100':'SM', 't100':'ST'},
                     'nettap'            :{None :'T0', '' :'T0',},
                     'netx'              :{None :'X0', '' :'X0',},
                     'rcx'               :{None :'X0', '' :'X0',}, 
                     'rcx base firmware' :{None :'X0', '' :'X0',},
                    }
    #Table 3, remark:
    file_extention = ".nxf"
    if DeviceFamily.lower() in ['rcx', 'rcx base firmware']:
        file_extention = ".nxo"    
    # Table 4:
    netx_type    =  {'netx5'   : '4',
                     'netx6'   : '8',
                     'netx10'  : '5',
                     'netx50'  : '3',
                     'netx51'  : '6',
                     'netx52'  : '7',
                     'netx100' : '2',
                     'netx500' : '1',
                    }
    #Table 5:
    protocol_table = { None                                   : '00',
                ''                                      : '00',
                'rcx'                                   : '00',
                'profibus-dp master'                    : '01',
                'profibus dp master'                    : '01',  
                'profibus-dp slave'                     : '02',
                'profibus dp slave'                     : '02',  
                'profibus-mpi'                          : '03',
                'profibus mpi'                          : '03',
                'canopen master'                        : '04',
                'canopen slave'                         : '05',
                'devicenet master'                      : '06',
                'devicenet slave'                       : '07',
                'as-interface master'                   : '08',
                'cc-link slave'                         : '09',
                'componet slave'                        : '0A',
                'io-link master'                        : '0B',
                'profinet io controller'                : '0C',
                'pnm'                                   : '0C',
                'profinet io device'                    : '0D',
                'pns'                                   : '0D',
                'ethercat master'                       : '0E',
                'ecm'                                   : '0E',
                'ethercat slave'                        : '0F',
                'ecs'                                   : '0F',
                'ethernet/ip scanner/master'            : '0G', 
                'ethernet/ip adapter/slave'             : '0H', 
                'sercos iii master'                     : '0I',
                's3m'                                   : '0I', 
                'sercos iii slave'                      : '0J',
                's3s'                                   : '0J', 
                'powerlink controlled node'             : '0K', 
                'open modbus/tcp'                       : '0L', 
                'rfc 1006'                              : '0M', 
                'df1'                                   : '0N', 
                '3964r'                                 : '0P', 
                'ascii'                                 : '0Q',
                'modbus rtu (master/slave)'             : '0R',
                'modbus rtu'                            : '0R',
                'netscript (programmable serial)'       : '0S',
                'netscript'                             : '0S',
                'varan client (server)'                 : '0T',
                'varan client'                          : '0T',
                'varan server'                          : '0T',
                'varan'                                 : '0T',
                'smartwire-dt master'                   : '0U',
                'marshaller'                            : '0V',
                'atvise (ethernet)'                     : '10',
                'atvise'                                : '10',
                'profibus-dp master and codesys'        : '11',
                'profibus dp master and codesys'        : '11',
                'devicenet master and codesys'          : '16',
                'profinet io controller and codesys'    : '1C',
                'pnm and codesys'                       : '1C',
                'ethernet/ip scanner/master and codesys': '1G',
                'eip and codesys'                       : '1G',
                'codesys and local i/o'                 : '1Z',
                'profibus-dp master and proconos eclr'  : '21',
                'profibus dp master and proconos eclr'  : '21',
                'canopen master and proconos eclr'      : '24',
                'devicenet master and proconos eclr'    : '26',
                'proconos eclr and local i/o'           : '2Z',
                'profibus-dp master and ibh s7'         : '31',
                'profibus dp master and ibh s7'         : '31',
                'canopen master and ibh s7'             : '34',
                'devicenet master and ibh s7'           : '36',
                'ibh s7 and local i/o'                  : '3Z',
                }

    def get_protocol_id(protocol):
        ltd = False

        if isinstance(protocol, str):
            protocol = protocol.lower()

            if protocol.endswith(" limited"):
                protocol = protocol[:-8]
                ltd = True

            id = protocol_table[protocol]
        else:
            id = protocol_table[protocol]

        if ltd:
            if id[0] == '0':
                id = 'Z' + id[1:]
            else:
                self.fatal("Can not generate limited firmware name for protocol {protocol}".format(**locals()))

        return id

    error_msg = "error: wrong parameter specified in \"SpecFWFileName\" on argument "
    out = ''
    # 1st letter
    try:
        loc_device_form = device_family[DeviceFamily.lower()]
    except KeyError:
        self.fatal(error_msg + "\"DeviceFamily = %s\"" % DeviceFamily)
    # 2nd letter
    try:
        if DeviceForm:
            DeviceForm = DeviceForm.lower()
        out = loc_device_form[ DeviceForm ]
    except KeyError:
        self.fatal(error_msg + "\"DeviceForm = %s\". Available \"DeviceForm\": %s" % (DeviceForm, ', '.join([str(var) for var,value in loc_device_form.iteritems()]) ) )
    #3rd letter
    try:
        out += netx_type[ netXType.replace(' ','').lower() ]
    except KeyError:
        self.fatal(error_msg + "\"netXType = %s\"" % netXType)
    #4th and 5th letters    
    try:
        out += get_protocol_id (PrimaryProtocol)
    except KeyError:
        self.fatal(error_msg + "\"PrimaryProtocol = %s\"" % PrimaryProtocol)
    #6th and 7th letters
    try:
        out += get_protocol_id (SecondaryProtocol)
    except KeyError:
        self.fatal(error_msg + "\"SecondaryProtocol = %s\"" % SecondaryProtocol)
    #8th letter and firmware extention
    out += str(HWCompInd) + file_extention

    return out
