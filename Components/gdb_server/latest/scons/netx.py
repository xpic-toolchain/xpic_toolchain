import os
import sys
import os.path
import re
import copy
import shutil
import tempfile 

from SCons             import *
from SCons.Script      import *
from SCons.Environment import *

def get_scons_version():
  """Get Scons version.
  
  Stolen from Scons/Script/SConscript.py
  """
  version_string = SCons.__version__
  
  version = string.split(string.split(version_string, ' ')[0], '.')
  v_major = int(version[0])
  v_minor = int(re.match('\d+', version[1]).group())
  if len(version) >= 3:
      v_revision = int(re.match('\d+', version[2]).group())
  else:
      v_revision = 0
  return v_major, v_minor, v_revision
      
  
def getstatusoutput(cmd):
    """Return (status, output) of executing cmd in a shell."""

    import sys
    mswindows = (sys.platform == "win32")

    import os
    if not mswindows:
                cmd = '{ ' + cmd + '; }'

    pipe = os.popen(cmd + ' 2>&1', 'r')
    text = pipe.read()
    sts = pipe.close()
    if sts is None: sts = 0
    if text[-1:] == '\n': text = text[:-1]
    return sts, text

class MyTempFileMunge:
    def __init__(self, cmd):
        self.cmd = cmd

    def __call__(self, target, source, env, for_signature):
        if for_signature:
            # If we're being called for signature calculation, it's
            # because we're being called by the string expansion in
            # Subst.py, which has the logic to strip any $( $) that
            # may be in the command line we squirreled away.  So we
            # just return the raw command line and let the upper
            # string substitution layers do their thing.
            return self.cmd

        # Now we're actually being called because someone is actually
        # going to try to execute the command, so we have to do our
        # own expansion.
        cmd = env.subst_list(self.cmd, SCons.Subst.SUBST_CMD, target, source)[0]
        try:
            maxline = int(env.subst('$MAXLINELENGTH'))
        except ValueError:
            maxline = 2048

        if (reduce(lambda x, y: x + len(y), cmd, 0) + len(cmd)) <= maxline:
            return self.cmd

        # We do a normpath because mktemp() has what appears to be
        # a bug in Windows that will use a forward slash as a path
        # delimiter.  Windows's link mistakes that for a command line
        # switch and barfs.
        #
        # We use the .lnk suffix for the benefit of the Phar Lap
        # linkloc linker, which likes to append an .lnk suffix if
        # none is given.
        (fd, tmp) = tempfile.mkstemp('.lnk', text=True)
        native_tmp = SCons.Util.get_native_path(os.path.normpath(tmp))

        if env['SHELL'] and env['SHELL'] == 'sh':
            # The sh shell will try to escape the backslashes in the
            # path, so unescape them.
            native_tmp = string.replace(native_tmp, '\\', r'\\\\')
            # In Cygwin, we want to use rm to delete the temporary
            # file, because del does not exist in the sh shell.
            rm = env.Detect('rm') or 'del'
        else:
            # Don't use 'rm' if the shell is not sh, because rm won't
            # work with the Windows shells (cmd.exe or command.com) or
            # Windows path names.
            rm = 'del'

        prefix = env.subst('$TEMPFILEPREFIX')
        if not prefix:
            prefix = '@'

        args = map(SCons.Subst.quote_spaces, cmd[1:])
        temp_str = string.join(args, " ")
        temp_str = string.replace(temp_str, '\\', r'/')
        
        os.write(fd, temp_str + "\n")
        os.close(fd)
        # XXX Using the SCons.Action.print_actions value directly
        # like this is bogus, but expedient.  This class should
        # really be rewritten as an Action that defines the
        # __call__() and strfunction() methods and lets the
        # normal action-execution logic handle whether or not to
        # print/execute the action.  The problem, though, is all
        # of that is decided before we execute this method as
        # part of expanding the $TEMPFILE construction variable.
        # Consequently, refactoring this will have to wait until
        # we get more flexible with allowing Actions to exist
        # independently and get strung together arbitrarily like
        # Ant tasks.  In the meantime, it's going to be more
        # user-friendly to not let obsession with architectural
        # purity get in the way of just being helpful, so we'll
        # reach into SCons.Action directly.
        if SCons.Action.print_actions:
            print("Using tempfile "+native_tmp+" for command line:\n"+
                  str(cmd[0]) + " " + temp_str)
        return [ cmd[0], prefix + native_tmp + '\n' + rm, native_tmp ] 

arm_env = Environment()
env_netx500 = Environment()
env_netx500_interwork = Environment()
env_netx500_interwork16 = Environment()

_buildroot = ''
_build_options_c   = ''
_build_options_asm = ''

if ARGUMENTS.get('target', '0').lower() == 'debug':
  print "**********************"
  print "*** Debug build... ***"
  print "**********************"
  _buildroot = 'debug'

  _build_options_c   = '-gdwarf-2 -O0 '
  _build_options_asm = '-gdwarf2 '

elif ARGUMENTS.get('target', '0').lower() == 'debugrel':
  print "***********************************************"
  print "*** Release with debugging symbols build... ***"
  print "***********************************************"
  _buildroot = 'debugrel'

  _build_options_c   = '-gdwarf-2 -Os '
  _build_options_asm = '-gdwarf2 '
else:
  print "************************"
  print "*** Release build... ***"
  print "************************"
  _buildroot = 'release'

  _build_options_c = '-Os '

#_build_options_c += '-Wcast-align -Wcast-qual -Wconversion -Wsign-compare -Wstrict-aliasing -Wshadow -Wswitch-default -Wswitch -Wswitch-enum '
#_build_options_c += '-Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable '

_library_includes_netx500 = '';
_library_includes_netx50  = '';

# Check O/S
if sys.platform.startswith("win"):

# Windows detection of tools, is wrong, so we need to override the environment tools

  arm_env = Environment(
    ENV     = os.environ,
    tools   = ['gcc','gas','g++','gnulink','ar','Subversion','m4']
  )

  arm_env["TEMPFILE"]     = MyTempFileMunge

# Check if user provided a compiler prefix
  if ARGUMENTS.get('prefix'):
    prefix = ARGUMENTS.get('prefix')

    if(ARGUMENTS.get('libdir_netx500')):
      _library_includes_netx500 = ARGUMENTS.get('libdir_netx500')
      _library_includes_netx500 = _library_includes_netx500.split(',')

    if(ARGUMENTS.get('libdir_netx50')):
      _library_includes_netx50 = ARGUMENTS.get('libdir_netx50')
      _library_includes_netx50 = _library_includes_netx50.split(',')

  else:
    prefix = 'arm-hitex-elf-'

    if arm_env['ENV']['PATH_GNU_ARM'] == None:
      raise EnvironmentError, 'PATH_GNU_ARM is not defined in your environment. Make sure the Hitex Toolchain is correctly installed!'
    else:
      _toolpath = arm_env['ENV']['PATH_GNU_ARM']
      # append backspace for new HiTOP installations 
      if _toolpath[-1] != "\\":
        _toolpath += "\\"
      print "Found Hitex Arm Compiler in \"" +  _toolpath + "\""
      arm_env['ENV']['PATH'] = _toolpath  + "bin" + ";" + arm_env['ENV']['PATH_GNU_ARM']

      _library_includes_netx500 = [_toolpath + 'arm-hitex-elf/lib/interwork/arm926ej-s',
                                   _toolpath + 'lib/gcc/arm-hitex-elf/%CCVERSION%/interwork/arm926ej-s']
      _library_includes_netx50  = [_toolpath + 'arm-hitex-elf/lib/interwork/arm926ej-s',
                                   _toolpath + 'lib/gcc/arm-hitex-elf/%CCVERSION%/interwork/arm926ej-s']
elif sys.platform.startswith("linux"):
  arm_env = Environment(
    ENV     = os.environ
  )
  if ARGUMENTS.get('prefix'):
    prefix = ARGUMENTS.get('prefix')
  else:
    prefix = '/home/christoph/arm-gcc-4.0.3/bin/arm-elf-'

  if(ARGUMENTS.get('libdir_netx500')):
    _library_includes_netx500 = ARGUMENTS.get('libdir_netx500')
    _library_includes_netx500 = _library_includes_netx500.split(',')
  else:
    _library_includes_netx500 = ['/home/christoph/arm-gcc-4.0.3/arm-elf/lib/interwork/arm926ej-s',
                               '/home/christoph/arm-gcc-4.0.3/lib/gcc/arm-elf/4.0.3/interwork/arm926ej-s']

  if(ARGUMENTS.get('libdir_netx50')):
    _library_includes_netx50 = ARGUMENTS.get('libdir_netx50')
    _library_includes_netx50 = _library_includes_netx50.split(',')
  else:
    _library_includes_netx50  = ['/home/christoph/arm-gcc-4.0.3/arm-elf/lib/interwork/arm926ej-s',
                               '/home/christoph/arm-gcc-4.0.3/lib/gcc/arm-elf/4.0.3/interwork/arm926ej-s']


  
# Override compiler/linker/assembler flags and prefixes
arm_env['AR']         = prefix + 'ar'
arm_env['ARFLAGS']    = 'ru'
arm_env['AS']         = prefix + 'as'
arm_env['ASFLAGS']    = str(_build_options_asm)
arm_env['CC']         = prefix + 'gcc'
arm_env['CXX']        = prefix + 'g++'
arm_env['CCFLAGS']    = str(arm_env['CCFLAGS']) + " " + str(_build_options_c) + ' -fno-builtin -fno-common -mapcs -Wall '

arm_env['RANLIB']     = prefix + 'ranlib'  
arm_env['LD']         = prefix + 'ld'
arm_env['LINK']       = prefix + 'ld'
arm_env['LIBS']       = ['c', 'm', 'gcc']
arm_env['LINKFLAGS']  = '-nostdlib -static '
arm_env['LINKCOM']    = str(arm_env['LINKCOM']) + ' -T$LDFILE -Map $MAPFILE'

arm_env['_LIBFLAGS']  = '--start-group ${_stripixes(LIBLINKPREFIX, LIBS, LIBLINKSUFFIX, LIBPREFIX, LIBSUFFIX, __env__)} --end-group'

arm_env['PROGSUFFIX'] = '.elf'

# --- Override Build outputs ---
arm_env['CCCOMSTR']     = "[CC]  $SOURCE"
arm_env['CXXCOMSTR']    = "[C++] $SOURCE"
arm_env['ASCOMSTR']     = "[AS]  $SOURCE"
arm_env['LINKCOMSTR']   = "[ELF] $SOURCE"
arm_env['ARCOMSTR']     = "[LIB] $TARGET"
arm_env['INSTALLSTR']   = "[INSTALL] $SOURCE --> $TARGET"

# --- Override build command to be able to use long command lines on windows
arm_env['CCCOM']        = "${TEMPFILE('%s')}" % arm_env['CCCOM']
arm_env['CXXCOM']       = "${TEMPFILE('%s')}" % arm_env['CXXCOM']
arm_env['ASCOM']        = "${TEMPFILE('%s')}" % arm_env['ASCOM']
arm_env['SHCCCOM']      = "${TEMPFILE('%s')}" % arm_env['SHCCCOM']
arm_env['SHCXXCOM']     = "${TEMPFILE('%s')}" % arm_env['SHCXXCOM']
arm_env['LINKCOM']      = "${TEMPFILE('%s')}" % arm_env['LINKCOM']
arm_env['ARCOM']        = "${TEMPFILE('%s')}" % arm_env['ARCOM']

#for key,value in arm_env.Dictionary().items():
#  print "%s = %s" % (key,value)

print "Using Prefix     : " + prefix

# Try to execute compiler and check the version
_ccstatus, _ccoutput = getstatusoutput(arm_env['CC'] + ' --version')
_ccversion = "4.0.3"
if(0 != _ccstatus):
    print "Compiler Version : unable to detect (Execution error, Probably compiler is not in your PATH)"
    print "                   Error = " + _ccoutput
else:
  _regccversion   = re.compile( r"[.]* \(GCC\) (\d.\d.\d)", re.I )
  _gccversion     = re.findall(_regccversion, _ccoutput)
  if( (_gccversion == None) or (len(_gccversion) == 0) ):
    print "Compiler Version : unable to detect (Parsing error)"
  else:
    _ccversion = _gccversion[0]
    print "Compiler Version : "  + _ccversion

library_includes_netx500 = []
for path in _library_includes_netx500:
  library_includes_netx500.append(path.replace("%CCVERSION%", _ccversion))
library_includes_netx50 = []
for path in _library_includes_netx50:
  library_includes_netx50.append(path.replace("%CCVERSION%", _ccversion))

print "Library Includes"
print " * netX100/500   : ", library_includes_netx500
print " * netX10/50     : ", library_includes_netx50

if get_scons_version() >= (1,2,0):
  arm_env.Decider('MD5-timestamp')
else:
  arm_env.SourceSignatures('timestamp')
  
arm_env.BuildDir('build', 'src', duplicate=0)

# ----------------------------------------------------------
# -------------------- NETX 500 ----------------------------
# ----------------------------------------------------------
if get_scons_version() >= (1,2,0):
  env_netx500 = arm_env.Clone()
else:
  env_netx500 = arm_env.Copy()
  
env_netx500['CCFLAGS'] = str(env_netx500['CCFLAGS']) + ' -mcpu=arm926ej-s -mlong-calls -fshort-enums '
env_netx500['ASFLAGS'] = str(env_netx500['ASFLAGS']) + ' -mcpu=arm926ej-s  --defsym __NETX500=1 '
  
if get_scons_version() >= (1,2,0):
  env_netx500_interwork = env_netx500.Clone(
    LIBPATH = library_includes_netx500
  )
else:
  env_netx500_interwork = env_netx500.Copy(
    LIBPATH = library_includes_netx500
  )
  
env_netx500_interwork['CCFLAGS'] = str(env_netx500_interwork['CCFLAGS']) + ' -mthumb-interwork'
env_netx500_interwork['ASFLAGS'] = str(env_netx500_interwork['ASFLAGS']) + ' -mthumb-interwork --defsym __NETX500=1 '

if get_scons_version() >= (1,2,0):
  env_netx500_interwork16 = env_netx500_interwork.Clone()
else:
  env_netx500_interwork16 = env_netx500_interwork.Copy()
  
env_netx500_interwork16['CCFLAGS'] = str(env_netx500_interwork16['CCFLAGS']) + ' -mthumb'
env_netx500_interwork16['ASFLAGS'] = str(env_netx500_interwork16['ASFLAGS']) + ' -mthumb --defsym __NETX500=1 '

# ----------------------------------------------------------


# ----------------------------------------------------------
# -------------------- NETX 50 -----------------------------
# ----------------------------------------------------------
if get_scons_version() >= (1,2,0):
  env_netx50 = arm_env.Clone()
else:
  env_netx50 = arm_env.Copy() 
env_netx50['CCFLAGS'] = str(env_netx50['CCFLAGS']) + ' -mcpu=arm9e -fshort-enums -mlong-calls '
env_netx50['ASFLAGS'] = str(env_netx50['ASFLAGS']) + ' -mcpu=arm9e --defsym __NETX50=1 '

if get_scons_version() >= (1,2,0):
  env_netx50_interwork = env_netx50.Clone(
    LIBPATH = library_includes_netx50
  )
else:
  env_netx50_interwork = env_netx50.Copy(
    LIBPATH = library_includes_netx50
  )  

env_netx50_interwork['CCFLAGS'] = str(env_netx50_interwork['CCFLAGS']) + ' -mthumb-interwork'
env_netx50_interwork['ASFLAGS'] = str(env_netx50_interwork['ASFLAGS']) + ' -mthumb-interwork  --defsym __NETX50=1 '

if get_scons_version() >= (1,2,0):
  env_netx50_interwork16 = env_netx50_interwork.Clone()
else:
  env_netx50_interwork16 = env_netx50_interwork.Copy()
  
env_netx50_interwork16['CCFLAGS'] = str(env_netx50_interwork16['CCFLAGS']) + ' -mthumb'
env_netx50_interwork16['ASFLAGS'] = str(env_netx50_interwork16['ASFLAGS']) + ' -mthumb --defsym __NETX50=1 '


def _generate_build_dir(build_dir_prefix, src_list, env):
  """Generate build directory prefixes for each file. This functions searches for the first invalid character (/\:..)
     and uses the path info das builddir prefix """

# Iterate over source list
  for i in range(len(src_list)):
    src_dir, tmp_srcfile = os.path.split(str(src_list[i]))

    tmp_dir          = src_dir
    src_file         = tmp_srcfile
    src_dir_stripped = ""

# Split path until we read an invalid char
    while tmp_srcfile:
      tmp_dir, tmp_srcfile = os.path.split(tmp_dir)
      if not re.match(r"\.\.|\\\\|//|:", tmp_srcfile):
        if src_dir_stripped!="":
          src_dir_stripped = "/" + tmp_srcfile + src_dir_stripped
        else:
          src_dir_stripped = "/" + tmp_srcfile
      else:
# Illegal entry found
        break

# Source dir is is now transformed from e.g. "./RCX/Sources/oldSyslock.s" --> "build/release/RCX/Sources/oldSyslock.s"
    _build_dir = build_dir_prefix + src_dir_stripped

    src_list[i] = _build_dir + "/" + src_file

#Add build directory to environment
    env.BuildDir(_build_dir, src_dir ,duplicate = 0)

def _netx_compile(MyEnv, src_list, inc_list, cdefine_list, build_dir_prefix):
  """Compile a netX object list using the given environment"""

  if get_scons_version() >= (1,2,0):
    tempenv = MyEnv.Clone()
  else:
    tempenv = MyEnv.Copy()

  build_src_list = copy.copy(src_list)

  _generate_build_dir(build_dir_prefix, build_src_list, tempenv)

  tempenv.Append(CPPPATH      = inc_list)
  tempenv.Append(CPPDEFINES   = cdefine_list)
  tempenv.Append(ASCOM        = tempenv['_CPPINCFLAGS'])

  return tempenv.Object(build_src_list)

def _netx_makeelf(MyEnv, src_list, build_dir_prefix, elfname, libpath, libs, ld_file):
  if get_scons_version() >= (1,2,0):
    tempenv = MyEnv.Clone()
  else:
    tempenv = MyEnv.Copy()

  build_src_list = copy.copy(src_list)
#  _generate_build_dir(build_dir_prefix + "/" + _buildroot, build_src_list, tempenv)

  tempenv.Append(LIBPATH      = libpath)
  tempenv.Prepend(LIBS         = libs)

#Add Mapfile command to linker. TODO: How to delete the map file on a clean build
  tempenv['MAPFILE'] = '\"'+ build_dir_prefix + "/" + _buildroot + "/" + elfname + '.map' + "\""
  
  return tempenv.Program(build_dir_prefix + "/" + _buildroot + "/" + elfname, build_src_list, LDFILE=ld_file)

# ----------------------------------------------------------
# -------------------- NETX 500 ----------------------------
# ----------------------------------------------------------
def netx500_compile_thumb(src_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_compile(env_netx500_interwork16, src_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)

def netx500_compile_arm(src_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_compile(env_netx500_interwork, src_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)

def netx500_makelib(obj_list, build_dir_prefix, libname, installdir):
  _install = env_netx500_interwork.StaticLibrary(build_dir_prefix + "/" + _buildroot + "/" + libname, obj_list)

  env_netx500_interwork.Install(installdir, _install)

  return _install

def netx500_makeelf(obj_list, build_dir_prefix, elfname, libpath, libs, ld_file):
  return _netx_makeelf(env_netx500_interwork, obj_list, build_dir_prefix, elfname, libpath, libs, ld_file)
# ----------------------------------------------------------

# ----------------------------------------------------------
# -------------------- NETX 50 -----------------------------
# ----------------------------------------------------------
def netx50_compile_thumb(src_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_compile(env_netx50_interwork16, src_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)

def netx50_compile_arm(src_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_compile(env_netx50_interwork, src_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)


def netx50_makelib(obj_list, build_dir_prefix, libname, installdir):
  _install = env_netx50_interwork.StaticLibrary(build_dir_prefix + "/" + _buildroot + "/" + libname, obj_list)

  env_netx50_interwork.Install(installdir, _install)

  return _install

def netx50_makeelf(obj_list, build_dir_prefix, elfname, libpath, libs, ld_file):
  return _netx_makeelf(env_netx50_interwork, obj_list, build_dir_prefix, elfname, libpath, libs, ld_file)

# ----------------------------------------------------------
# -------------------- NETX 10 -----------------------------
# ----------------------------------------------------------
def netx10_compile_thumb(src_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_compile(env_netx50_interwork16, src_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)

def netx10_compile_arm(src_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_compile(env_netx50_interwork, src_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)


def netx10_makelib(obj_list, build_dir_prefix, libname, installdir):
  _install = env_netx50_interwork.StaticLibrary(build_dir_prefix + "/" + _buildroot + "/" + libname, obj_list)

  env_netx50_interwork.Install(installdir, _install)

  return _install

def netx10_makeelf(obj_list, build_dir_prefix, elfname, libpath, libs, ld_file):
  return _netx_makeelf(env_netx50_interwork, obj_list, build_dir_prefix, elfname, libpath, libs, ld_file)

# ----------------------------------------------------------
