import os
import sys
import os.path
import re
import copy
import shutil
import tempfile 
import string

from SCons             import *
from SCons.Script      import *
from SCons.Environment import *


# ----------------------------------------------------------
# UTILITIES
# ----------------------------------------------------------

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


xpic_env = None


# ----------------------------------------------------------
# BUILD TYPE
# ----------------------------------------------------------

_buildroot = ''
_build_options_c   = ''
_build_options_asm = '-mmcu=xpic '

buildarg = ARGUMENTS.get('target', 'release').lower()
if buildarg == 'debug':
  print "**********************"
  print "*** Debug build... ***"
  print "**********************"
  _buildroot = 'debug'

  _build_options_c = '-gdwarf-2 -O0'
  _build_options_asm += '-gdwarf2 '

elif buildarg == 'debugrel':
  print "***********************************************"
  print "*** Release with debugging symbols build... ***"
  print "***********************************************"
  _buildroot = 'debugrel'

  _build_options_c = '-gdwarf-2 -O1'
  _build_options_asm += '-gdwarf2 '
else:
  print "************************"
  print "*** Release build... ***"
  print "************************"
  _buildroot = 'release'

  _build_options_c = '-Os'


# ----------------------------------------------------------
# TOOLCHAIN DIRECTORIES
# ----------------------------------------------------------

# Get the directory of the tool chain with the following precedence:
# 1. scons command line variable "xpic-tools"
# 2. environment variable "XPIC_TOOLS"
# 3. environment variable "PATH_GNU_XPIC"
# 4. hope that PATH variable contains binaries and lib directories

_xpicpath = ARGUMENTS.get('xpic-tools', '')

# Check O/S
if sys.platform.startswith("win"):
  # Windows detection of tools, is wrong, so we need to override the environment tools
  xpic_env = Environment(
    ENV     = os.environ,
    TOOLS   = ['gcc','gas','g++','gnulink','ar','Subversion','m4']
  )
  xpic_env["TEMPFILE"]     = MyTempFileMunge

elif sys.platform.startswith("linux"):
  xpic_env = Environment(
    ENV     = os.environ
  )

if _xpicpath == '':
  if 'XPIC_TOOLS' in xpic_env['ENV'].keys():
    _xpicpath = xpic_env['ENV']['XPIC_TOOLS']
  else:
    if 'PATH_GNU_XPIC' in xpic_env['ENV'].keys():
      _xpicpath = xpic_env['ENV']['PATH_GNU_XPIC']
      
if _xpicpath == '':
  prefix = '' # tools may be in PATH
else:
  prefix = _xpicpath + "/bin/" 

_library_includes_netx10 = ARGUMENTS.get('libdir-netx10', _xpicpath + '/lib,' + _xpicpath + '/lib/gcc/xpic-hilscher-elf/0.1.1')
library_includes_netx10 = _library_includes_netx10.split(',')


# ----------------------------------------------------------
# CUSTOM BUILDERS
# ----------------------------------------------------------

def generate_image(source, target, env, for_signature):
  return '%s %s %s %s' % (env['IMAGE'], env['IMAGELAGS'], source[0], target[0])

image = Builder(generator = generate_image, suffix='$IMAGESUFFIX')
xpic_env['BUILDERS']['Image'] = image


# ----------------------------------------------------------
# BUILD VARIABLES
# ----------------------------------------------------------

# Override compiler/linker/assembler flags and prefixes
xpic_env['IMAGE']      = '"' + prefix + 'xpic-objcopy' + '"'

xpic_env['AR']         = '"' + prefix + 'xpic-ar' + '"'
xpic_env['ARFLAGS']    = 'ru'
xpic_env['AS']         = '"' + prefix + 'xpic-as' + '"'
xpic_env['ASFLAGS']    = str(_build_options_asm)
xpic_env['CC']         = '"' + prefix + 'clang' + '"'
xpic_env['CXX']        = 'g++'
xpic_env['CCFLAGS']    = '-target xpic -Wimplicit -I"'+_xpicpath+'/include" -I"'+_xpicpath+'/lib/gcc/xpic-hilscher-elf/0.1.1/include"'
xpic_env.Append(CCFLAGS = ' ' + _build_options_c)
xpic_env['IMAGELAGS']  = "-O srec"

  
xpic_env['LD']         = '"' + prefix + 'xpic-ld' + '"'
xpic_env['LINK']       = '"' + prefix + 'xpic-ld' + '"'
xpic_env['RANLIB']     = '"' + prefix + 'xpic-ranlib' + '"'
xpic_env['LIBS']       = ['m', 'c', 'gcc']
xpic_env['LIBPATH']    = library_includes_netx10
xpic_env['LINKFLAGS']  = '--fatal-warnings '
xpic_env['LINKCOM']    = str(xpic_env['LINKCOM']) + ' -T$LDFILE'

xpic_env['_LIBFLAGS']  = '--start-group ${_stripixes(LIBLINKPREFIX, LIBS, LIBLINKSUFFIX, LIBPREFIX, LIBSUFFIX, __env__)} --end-group'

xpic_env['OBJSUFFIX'] = '.o'
xpic_env['PROGSUFFIX'] = '.elf'
xpic_env['IMAGESUFFIX'] = '.srec'

# --- Override Build outputs if not verbose ---
show_commands = ARGUMENTS.get('show-commands', 'short')
if show_commands not in ("short","full"):
  print "warning: invalid value '" + show_commands + "' for varible 'show-commands', default to 'short'"
  show_commands = 'short'
if show_commands == 'short':
  xpic_env['CCCOMSTR']     = "[CC]  $SOURCE"
  xpic_env['CXXCOMSTR']    = "[C++] $SOURCE"
  xpic_env['ASCOMSTR']     = "[AS]  $SOURCE"
  xpic_env['LINKCOMSTR']   = "[ELF] $SOURCE"
  xpic_env['ARCOMSTR']     = "[LIB] $TARGET"
  xpic_env['INSTALLSTR']   = "[INSTALL] $SOURCE --> $TARGET"

# --- Override build command to be able to use long command lines on windows
xpic_env['CCCOM']        = "${TEMPFILE('%s')}" % xpic_env['CCCOM']
xpic_env['CXXCOM']       = "${TEMPFILE('%s')}" % xpic_env['CXXCOM']
xpic_env['ASCOM']        = "${TEMPFILE('%s')}" % xpic_env['ASCOM']
xpic_env['SHCCCOM']      = "${TEMPFILE('%s')}" % xpic_env['SHCCCOM']
xpic_env['SHCXXCOM']     = "${TEMPFILE('%s')}" % xpic_env['SHCXXCOM']
xpic_env['LINKCOM']      = "${TEMPFILE('%s')}" % xpic_env['LINKCOM']
xpic_env['ARCOM']        = "${TEMPFILE('%s')}" % xpic_env['ARCOM']


#for key,value in xpic_env.Dictionary().items():
#  print "%s = %s" % (key,value)

print "Using Prefix     : " + prefix


# ----------------------------------------------------------
# TEST CONFIGURATION
# ----------------------------------------------------------

# Try to execute compiler and check the version
_ccstatus, _ccoutput = getstatusoutput(xpic_env['CC'] + ' --version')
_regccversion   = re.compile( r"[.]* (\(GCC\)|llvm version) (\d\.\d\.?\d?)", re.I )
_gccversion     = re.findall(_regccversion, _ccoutput)
if( (_gccversion == None) or (len(_gccversion) == 0) ):
  print "Compiler Version : unable to detect (Parsing error)"
  # We could quit() here but we give this unknown compiler a chance and continue
else:
  print "Compiler Version : "  + _gccversion[0][1]

print "Library Includes"
print " * netX10   : ", _library_includes_netx10

if get_scons_version() >= (1,2,0):
  xpic_env.Decider('MD5-timestamp')
else:
  xpic_env.SourceSignatures('timestamp')
  

# ----------------------------------------------------------
# CHIP ENVIRONMENTS
# ----------------------------------------------------------
if get_scons_version() >= (1,2,0):
  env_netx10 = xpic_env.Clone()
else:
  env_netx10 = xpic_env.Copy()
  
env_netx10['CCFLAGS'] = str(env_netx10['CCFLAGS']) + ' '
env_netx10['ASFLAGS'] = str(env_netx10['ASFLAGS']) + ' --defsym __NETX10=1 '


# ----------------------------------------------------------
# COMMON FUNCTIONS
# ----------------------------------------------------------

def _generate_build_dir(build_dir_prefix, src_list, env):
  """Generate build directory prefixes for each file. This functions searches for the first invalid character (/\:..)
     and uses the path info as builddir prefix """

# Iterate over source list
  for i in range(len(src_list)):
    src_dir, tmp_srcfile = os.path.split(src_list[i])

    tmp_dir          = src_dir
    src_file         = tmp_srcfile
    src_dir_stripped = ""

# Split path until we read an invalid char

    while tmp_srcfile:
      tmp_dir, tmp_srcfile = os.path.split(tmp_dir)
      if not re.match(r"\.\.|\\\\|//|:", tmp_srcfile):
        if src_dir_stripped!="":
          src_dir_stripped = tmp_srcfile + "/" + src_dir_stripped
        else:
          src_dir_stripped = tmp_srcfile
      else:
    # Illegal entry found
        break

# Source dir is is now transformed from e.g. "./RCX/Sources/oldSyslock.s" --> "build/release/RCX/Sources/oldSyslock.s"
    _build_dir = build_dir_prefix + src_dir_stripped

    src_list[i] = _build_dir + "/" + src_file

#Add build directory to environment
    VariantDir(_build_dir, src_dir ,duplicate = 0)


def _netx_xpic_compile(MyEnv, src_list, inc_list, cdefine_list, build_dir_prefix):
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

def _netx_xpic_compile_variants(MyEnv, src, outvar_list, inc_list, cdefine_list, build_dir_prefix):
  """Compile a netX object list using the given environment"""
  obj_list = []
    
  for outvar in outvar_list:
    if get_scons_version() >= (1,2,0):
      tempenv = MyEnv.Clone()
    else:
      tempenv = MyEnv.Copy()
    
    tempenv.Append(CPPPATH      = inc_list)
    tempenv.Append(CPPDEFINES   = cdefine_list + outvar[1])
    tempenv.Append(ASCOM        = tempenv['_CPPINCFLAGS'])
    
    trg = re.sub("[^/]*$", outvar[0], src)
    obj_list.append(tempenv.Object(build_dir_prefix+"/"+trg, source=src))

  return obj_list


def _netx_xpic_makeelf(MyEnv, src_list, build_dir_prefix, elfname, libpath, libs, ld_file):
  if get_scons_version() >= (1,2,0):
    tempenv = MyEnv.Clone()
  else:
    tempenv = MyEnv.Copy()

  build_src_list = copy.copy(src_list)

  tempenv.Append(LIBPATH      = libpath)
  tempenv.Prepend(LIBS        = libs)

  #Add Mapfile command to linker. TODO: How to delete the map file on a clean build
  tempenv['LINKCOM']    += ' -Map \"'+ build_dir_prefix + "/" + _buildroot + "/" + elfname + '.map' + "\""

  return tempenv.Program(target=build_dir_prefix + "/" + _buildroot + "/" + elfname, source=build_src_list, LDFILE=ld_file)

def _netx_xpic_armimage(MyEnv, elf):
  srec = MyEnv.Image(elf[0])
  return MyEnv.Command(re.sub("srec","c",str(srec[0])),srec[0],"srec2c.pl -s $SOURCE -t $TARGET")


# ----------------------------------------------------------
# API FUNCTIONS
# ----------------------------------------------------------
def netx10_xpic_compile(src_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_xpic_compile(env_netx10, src_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)

def netx10_xpic_compile_variants(src, outvar_list, inc_list, cdefine_list, build_dir_prefix):
  return _netx_xpic_compile_variants(env_netx10, src, outvar_list, inc_list, cdefine_list, build_dir_prefix + "/" + _buildroot)

def netx10_xpic_makelib(obj_list, build_dir_prefix, libname, installdir):
  _install = env_netx10.StaticLibrary(build_dir_prefix + "/" + _buildroot + "/" + libname, obj_list)

  env_netx10.Install(installdir, _install)
  env_netx10.Alias('install', installdir)

  return _install

def netx10_xpic_install(obj_list, installdir):
  env_netx10.Install(installdir, obj_list)
  env_netx10.Alias('install', installdir)

def netx10_xpic_makeelf(obj_list, build_dir_prefix, elfname, libpath, libs, ld_file):
  return _netx_xpic_makeelf(env_netx10, obj_list, build_dir_prefix, elfname, libpath, libs, ld_file)

def netx10_xpic_armimage(elf):
  return _netx_xpic_armimage(env_netx10, elf)
# ----------------------------------------------------------
