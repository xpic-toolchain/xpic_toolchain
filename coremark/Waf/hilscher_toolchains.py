#! /usr/bin/env python
# encoding: utf-8

import os
import sys

import waflib.Tools.gcc
import waflib.Tools.gxx

from waflib import Options, Utils
from waflib.Configure import conf
from waflib.TaskGen import feature
###############################################################################
###############################################################################
# Support functions to define toolchains and devices
###############################################################################
###############################################################################

class toolchain(object):
    ''' a special decorator class used to wrap common actions around
        a toolchain configure func and toregister all defined toolchains'''
    toolchains = {}

    def __init__(self, name, *aliases):
        super(toolchain,self).__init__()
        self.name = name

        self.toolchains[self.name] = self

        for x in aliases:
            self.toolchains[x] = self

    def __call__(self,*args,**kwargs):
        configure_func =  getattr(self,'configure_function',None)

        if configure_func:
            return self.configure_toolchain(*args,**kwargs)
        else:
            # this will be invoked on the first call
            return self.setup_configure_func(*args,**kwargs)

    def setup_configure_func(self,func):
        self.configure_function = func
        return self

    def configure_toolchain(self,ctx):
        envname = 'toolchain_%s' % self.name

        if envname in ctx.all_envs:
            ctx.setenv(envname)
        else:
            ctx.setenv('')
            ctx.setenv(envname,ctx.env)
            ctx.env['TOOLCHAIN'] = self.name

            try:
                self.configure_function(ctx)
            except:
                ctx.setenv('')
                del ctx.all_envs[envname]
                raise

    def select_toolchain(self,ctx):
        envname = 'toolchain_%s' % self.name

        if envname in ctx.all_envs:
            ctx.setenv(envname)
        else:
            ctx.fatal('Toolchain %s not configured' % self.name)

    @classmethod
    def alias(cls, toolchain, aliases):
        toolchains = cls.toolchains

        orig = toolchains[toolchain]

        aliases = Utils.to_list(aliases)
        for x in Utils.to_list(aliases):
            if x not in toolchains:
                toolchains[x] = orig

@conf
def configure_toolchain(conf,name):
    global toolchain
    try:
        func = toolchain.toolchains[name]
    except KeyError:
        raise conf.errors.ConfigurationError('Unkown toolchain %s' % name)

    func(conf)

@conf
def select_toolchain(ctx,name):
    global toolchain
    try:
        func = toolchain.toolchains[name].select_toolchain
    except KeyError:
        raise ctx.errors.ConfigurationError('Unkown toolchain %s' % name)
    func(ctx)

class device(object):
    ''' a special decorator class used to wrap common actions around
        a device configure func and to register all know device setups'''
    devices = {}

    def __init__(self,devices,toolchains):
        super(device,self).__init__()

        devices = Utils.to_list(devices)
        self.name       = '_'.join(devices)
        self.toolchains = Utils.to_list(toolchains)

        global toolchain

        for x in self.toolchains:
            if x not in toolchain.toolchains:
                raise Exception('Unknown Toolchain %s' % x)

        for x in devices:
            self.devices[x] = self

    def __call__(self,*args,**kwargs):
        configure_func =  getattr(self,'configure_function',None)

        if configure_func:
            return self.configure_device(*args,**kwargs)
        else:
            # this will be invoked on the first call
            return self.setup_configure_func(*args,**kwargs)

    def setup_configure_func(self,func):
        self.configure_function = func
        return self

    def configure_device(self,ctx,device, toolchain = None):
        toolchain = Utils.to_list(toolchain)

        for x in (toolchain or self.toolchains):
            try:
                ctx.configure_toolchain(x)
                break
            except ctx.errors.ConfigurationError:
                pass
        else:
            raise ctx.errors.ConfigurationError('Unable to configure a suitable toolchain for %s' % device)

        envname = 'device_%s_%s' % (self.name,ctx.env['TOOLCHAIN'])

        if envname in ctx.all_envs:
            ctx.setenv(envname)
        else:
            ctx.setenv(envname,ctx.env)
            ctx.env['DEVICE'] = self.name

            try:
                self.configure_function(ctx)
            except:
                ctx.setenv('')
                del ctx.all_envs[envname]
                raise

@conf
def configure_device(conf,name,toolchain = None):
    global device
    try:
        func = device.devices[name]
    except KeyError:
        raise conf.errors.ConfigurationError('Unkown device %s' % name)
    func(conf,name, toolchain)

@conf
def apply_device_flags(ctx,name):
    '''Function to update the current environment with device
       specific flags. This function is meant for backwards compat'''
    global device

    try:
        func = device.devices[name]
    except KeyError:
        raise ctx.errors.ConfigurationError('Unkown device %s' % name)

    func.configure_function(ctx)
    ctx.env.append_value('DEFINES', '__' + name.upper())


###############################################################################
###############################################################################
# Definition of setup functions for common compiler types
###############################################################################
###############################################################################
@conf
def setup_gnu_binutils(conf, path_list = None):
    u'''Setup GNU binutils variables for current environment'''
    env = conf.env

    prefix = env['GNU_BINUTILS_PREFIX'] or ''

    for tool in 'ar readelf objdump objcopy strip nm'.split():
        executable_name = prefix + tool
        conf.find_program(executable_name, var=tool.upper(), path_list=path_list)

    env.ARFLAGS = 'rcs'

@conf
def setup_gnu_gcc_toolchain(conf, prefix, compiler = None, cxxcompiler = None, path_list=None):
    env = conf.env

    env.stash()
    try:
        env['GNU_GCC_PREFIX']      = prefix
        env['GNU_BINUTILS_PREFIX'] = prefix

        conf.setup_gnu_binutils(path_list)

        compiler    = compiler or (env['GNU_GCC_PREFIX'] + 'gcc')
        cxxcompiler = cxxcompiler or (env['GNU_GCC_PREFIX'] + 'g++')

        cc = conf.find_program(compiler, var='CC', path_list=path_list)
        cc = conf.cmd_to_list(cc)

        cxx = conf.find_program(cxxcompiler, var='CXX', mandatory=False, path_list=path_list)
        cxx = conf.cmd_to_list(cxx)

        conf.env.CC  = cc
        conf.env.AS  = cc

        env['CC_NAME']    = 'gcc'
        env['CC_VERSION'] = conf.cmd_and_log(env.CC + ["-dumpversion"]).strip().split('.')
        env['MACHINE']    = env['MACHINE'] or conf.cmd_and_log(env.CC +["-dumpmachine"]).strip()
        env['DEST_OS']    = 'Unkown'

        # backwards compat. Since we support clang now as well,
        # there is no common CC_PREFIX anymore. (CLANG is not prefixed)
        # please use 'MACHINE' for future developments
        env['CC_PREFIX']  = prefix.strip('-')

        conf.cc_load_tools()
        conf.gcc_common_flags()
        conf.cc_add_flags()

        conf.env.AS_TGT_F = '-o'

        if(cxx):
            env['CXX_NAME'] = 'gcc'
            env['CXX'] = cxx

            conf.cxx_load_tools()
            conf.gxx_common_flags()
            conf.cxx_add_flags()

        conf.link_add_flags()
    except conf.errors.ConfigurationError:
        env.revert()
        raise

@conf
def setup_llvm_toolchain(conf, target, path_list=None):
    env = conf.env

    env.stash()
    try:
        env['MACHINE'] = target

        if target:
            env['GNU_BINUTILS_PREFIX'] = "%s-" % target
        else:
            env['GNU_BINUTILS_PREFIX'] = ""

        conf.setup_gnu_binutils(path_list)

        config = conf.find_program('llvm-config', var='LLVM_CONFIG', path_list=path_list)
        env['LLVM_CONFIG'] = conf.cmd_to_list(config)

        cc = conf.find_program('clang', var='CC', path_list=path_list)
        cc = conf.cmd_to_list(cc) + ['-target', target]

        env['CC_NAME'] = 'clang'
        env['CXX_NAME'] = 'clang'
        env['CC_VERSION'] = conf.cmd_and_log(env.LLVM_CONFIG + ["--version"]).strip().split('.')

        env.CC     = cc
        env['CXX'] = cc
        env.AS     = cc
        env.ARFLAGS = 'rcs'

        env['LINK_CC']  = env['CC']
        env['LINK_CXX'] = env['CC']

        conf.clang_common_flags()

        f = env.append_value
        f('INCLUDES', Utils.to_list(conf.cmd_and_log(env.LLVM_CONFIG + ["--includedir"])))
        f('LIBPATH',  Utils.to_list(conf.cmd_and_log(env.LLVM_CONFIG + ["--libdir"])))

        conf.env.AS_TGT_F = ['-c', '-o']
        conf.cc_add_flags()
        conf.link_add_flags()

        #env.stash()
        #try:
        #    conf.env.prepend_value('LINKFLAGS', ['-v'])
        #    conf.env.prepend_value('CFLAGS', ['-v'])
        #    conf.check_cc(lib='c', msg = 'Checking if clang supports target %s' % target, okmsg='Yes', errmsg='No')
        #finally:
        #    env.revert()

    except conf.errors.ConfigurationError:
        env.revert()
        raise

@conf
def cc_common_flags(conf):
    u'''Common flags when compiling c code '''
    f = conf.env.append_value

    # disable assert macros in release mode
    f('DEFINES_compile_release',  ['NDEBUG'])
    f('DEFINES_compile_debugrel', ['NDEBUG'])


@conf
def clang_common_flags(conf):
    """
    Common flags for g++ on nearly all platforms
    """
    v = conf.env

    v['CC_SRC_F']           = []
    v['CC_TGT_F']           = ['-c', '-o']

    v['CXX_SRC_F']           = []
    v['CXX_TGT_F']           = ['-c', '-o']


    v['CCLNK_SRC_F']         = []
    v['CCLNK_TGT_F']         = ['-o']
    v['CXXLNK_SRC_F']        = []
    v['CXXLNK_TGT_F']        = ['-o']
    v['CPPPATH_ST']          = '-I%s'
    v['DEFINES_ST']          = '-D%s'

    v['LIB_ST']              = '-l%s' # template for adding libs
    v['LIBPATH_ST']          = '-L%s' # template for adding libpaths
    v['STLIB_ST']            = '-l%s'
    v['STLIBPATH_ST']        = '-L%s'
    v['RPATH_ST']            = '-Wl,-rpath,%s'

    v['SONAME_ST']           = '-Wl,-h,%s'
    v['SHLIB_MARKER']        = '-Wl,-Bdynamic'
    v['STLIB_MARKER']        = '-Wl,-Bstatic'

    # program
    v['cprogram_PATTERN']    = '%s'
    v['cxxprogram_PATTERN']  = '%s'

    # shared library
    v['CXXFLAGS_cshlib']   = ['-fPIC']
    v['LINKFLAGS_cshlib']  = ['-shared']
    v['cshlib_PATTERN']    = 'lib%s.so'

    v['CXXFLAGS_cxxshlib']   = ['-fPIC']
    v['LINKFLAGS_cxxshlib']  = ['-shared']
    v['cxxshlib_PATTERN']    = 'lib%s.so'

    # static lib
    v['LINKFLAGS_cstlib']  = ['-Wl,-Bstatic']
    v['cstlib_PATTERN']    = 'lib%s.a'

    v['LINKFLAGS_cxxstlib']  = ['-Wl,-Bstatic']
    v['cxxstlib_PATTERN']    = 'lib%s.a'

###############################################################################
###############################################################################
# Definition of standard toolchains and devices
###############################################################################
###############################################################################
@toolchain('hitex')
def configure_toolchain_hitex(conf):
    path = None
    if 'PATH_GNU_ARM' in os.environ:
        path = [os.path.join(os.environ['PATH_GNU_ARM'], 'bin')]

    conf.setup_gnu_gcc_toolchain(prefix = 'arm-hitex-elf-', path_list = path)
    conf.gcc_arm_flags()

@toolchain('codesourcery')
def configure_toolchain_codesourcery(conf):
    # get path to toolchain
    path = None

    if 'CS_PATH' in os.environ:
        path = [os.path.join(os.environ['CS_PATH'],'bin')]
    else:
       if sys.platform.startswith("win"):
         try:
           import _winreg as winreg
         except:
           import winreg

         aReg = winreg.ConnectRegistry(None, winreg.HKEY_LOCAL_MACHINE)
         aKey = None


         winreg.CloseKey(aReg)

    conf.setup_gnu_gcc_toolchain(prefix = 'arm-none-eabi-', path_list=path)
    conf.gcc_arm_flags()

    f = conf.env.append_value

    f('CFLAGS',    ['-ffunction-sections', '-fdata-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('CXXFLAGS',  ['-ffunction-sections', '-fdata-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('ASFLAGS',   ['-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('LINKFLAGS', ['-Wl,-gc-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])


@toolchain('custom')
def configure_toolchain_custom(conf):
    opt = Options.options

    conf.setup_gnu_gcc_toolchain(
        prefix = opt.custom_toolchain_prefix + '-',
        compiler=opt.custom_toolchain_driver,
        cxxcompiler=opt.custom_toolchain_cxxdriver,
        path_list=[opt.custom_toolchain_path]
    )

    conf.gcc_arm_flags()

    f = conf.env.append_value

    f('CFLAGS',    ['-ffunction-sections', '-fdata-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('CXXFLAGS',  ['-ffunction-sections', '-fdata-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('ASFLAGS',   ['-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('LINKFLAGS', ['-Wl,-gc-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])

@toolchain('arm-eabi')
def configure_toolchain_arm_eabi(conf):
    exc = None
    for x in 'arm-none-eabi- arm-eabi-'.split():
        try:
            conf.setup_gnu_gcc_toolchain(prefix = x)
            break
        except conf.errors.ConfigurationError as e:
            exc = exc or e
    else:
        raise exc

    conf.gcc_arm_flags()

    f = conf.env.append_value

    f('CFLAGS',    ['-ffunction-sections', '-fdata-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('CXXFLAGS',  ['-ffunction-sections', '-fdata-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('ASFLAGS',   ['-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])
    f('LINKFLAGS', ['-Wl,-gc-sections', '-msoft-float', '-mfpu=vfp', '-mfloat-abi=soft',])


@toolchain('llvm-xpic', 'llvm_xpic')
def configure_toolchain_xpic(conf):
    path = None

    if 'PATH_LLVM_XPIC' in os.environ:
        # new version llvm
        path = [os.path.join(os.environ['PATH_LLVM_XPIC'], 'bin')]
        os_env = os.environ['PATH_LLVM_XPIC']
    elif 'PATH_GNU_XPIC' in os.environ:
        # old version llvm
        path = [os.path.join(os.environ['PATH_GNU_XPIC'], 'bin')]
        os_env = os.environ['PATH_GNU_XPIC']

    conf.setup_llvm_toolchain(target='xpic', path_list=path)

    f = conf.env.append_value

    f('DEFINES', ['__XPIC__'])

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g', '-gdwarf-2'])     # change debug option to OLD format gdwarf-2; just for this test!
        f(x + '_compile_debugrel', ['-Os', '-g', '-gdwarf-2'])     # change debug option to OLD format gdwarf-2; just for this test!
        f(x + '_compile_release',  ['-Os'])

    # fix wrong directories in xpic compiler
    libdir_list = Utils.to_list(conf.cmd_and_log(conf.env['LLVM_CONFIG'] + ["--libdir"]))
    f('LIBPATH',  [x+"/gcc/xpic-hilscher-elf/0.1.1" for x in libdir_list])

    f('STLIB_default_standardlibs',   ['m', 'c', 'gcc'])   # order not correct
    #f('STLIB_default_standardlibs',   ['c', 'gcc', 'm'])    # order correct
		
	# used inside old environment
    f('ASFLAGS', ['-Wa,-mmcu=xpic2','-c','--target=xpic', '-P'])
    f('LINKFLAGS', ['--target=xpic','-L' + os_env + '/lib' , '-L' + os_env + '/lib/gcc/xpic-hilscher-elf/0.1.1'])


@toolchain('llvm-arm')
def configure_toolchain_llvm_arm(conf):
    '''this doesnt work yet'''
    path = None
    conf.setup_llvm_toolchain(target='arm-none-eabi', path_list=path)

    conf.gcc_arm_flags()

    f = conf.env.append_value

@toolchain('gcc-linux')
def configure_toolchain_gcc_linux(conf):
    if sys.platform in ("linux","linux2"):
        conf.setup_gnu_gcc_toolchain(prefix='')
        conf.env.append_value('LINKFLAGS', ['-Wl,-gc-sections'])
    else:
        conf.fatal('GCC Linux toolchain not available none linux os')

@toolchain('mingw32', 'mingw')
def configure_toolchain_mingw32(conf):
    if sys.platform in ("win32","win64"):
        conf.setup_gnu_gcc_toolchain(prefix = 'mingw32-')

        conf.env['DEST_OS'] = 'win32'
        conf.gcc_modifier_platform()
        conf.env.append_value('LINKFLAGS', ['-Wl,-gc-sections'])
    else:
        conf.fatal('MinGW toolchain not available none windows os')

@toolchain('mingw64')
def configure_toolchain_mingw64(conf):
    if sys.platform in ("win64"):
        conf.setup_gnu_gcc_toolchain(prefix = 'mingw64')

        conf.env['DEST_OS'] = 'win32'
        conf.gcc_modifier_platform()
        conf.env.append_value('LINKFLAGS', ['-Wl,-gc-sections'])
    else:
        conf.fatal('MinGW 64 toolchain not available none 64 bit windows os')

# Lets define some common aliases
if sys.platform in ("linux","linux2"):
    toolchain.alias('gcc-linux', ['linux', 'posix', 'native'])
elif sys.platform in ("win32"):
    toolchain.alias('mingw32', ['win', 'native'])

    @toolchain('posix')
    def configure_toolchain_unavail(conf):
        conf.fatal('Posix toolchain not available none posix os')
elif sys.platform in ('win64'):
    @toolchain('win', 'native')
    def configure_toolchain_native(conf):
        try:
            configure_toolchain_mingw64.configure_toolchain(conf)
        except conf.errors.ConfigurationError:
            configure_toolchain_mingw32.configure_toolchain(conf)

    @toolchain('posix')
    def configure_toolchain_unavail(conf):
        conf.fatal('Posix toolchain not available none posix os')

@conf
def cc_version(conf):
    return tuple(int(x) for x in conf.env['CC_VERSION'])

@conf
def gcc_flags(conf):
    f = conf.env.append_value

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x,[ '-Wall',
              '-Wredundant-decls',
              '-Wno-inline',
              '-Winit-self'])

    f('ASFLAGS',[ '-Wall',
                  '-Wredundant-decls',
                  '-Wno-inline',
                  '-c'])

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_check_ansi', ['-ansi',
                              '-pedantic'])

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_check_extra', ['-Wconversion',
                               '-Wmissing-field-initializers',
                               '-Wsign-compare',
                               '-Wpointer-arith',
                               '-Wcast-qual'])

    if conf.cc_version() >= (4,1,2):
        f('CFLAGS_check_extra', ['-Wc++-compat'])

    # define CFLAGS_warninglevel1 and CXXFLAGS_warninglevel2
    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_warninglevel1', [ "-Wsystem-headers",
                                  "-Wbad-function-cast",
                                  "-Wsign-compare",
                                  "-Waggregate-return",
                                  "-Wswitch-default",
                                  "-Wstrict-prototypes",
                                  '-Wpointer-arith',
                                  ])


@feature("check_extra", "check_ansi", "warninglevel1")
def compile_check_dummy(self):
    # make waf happy
    pass

@conf
def gcc_arm_flags(conf):
    f = conf.env.append_value

    f('CFLAGS_compile_arm',   ['-marm'])
    f('CXXFLAGS_compile_arm',   ['-marm'])
    f('LINKFLAGS_compile_arm',   ['-marm'])
    f('CFLAGS_compile_thumb', ['-mthumb'])
    f('CXXFLAGS_compile_thumb', ['-mthumb'])
    f('LINKFLAGS_compile_thumb', ['-mthumb'])

    conf.env['cprogram_PATTERN']   = '%s.elf'
    conf.env['cxxprogram_PATTERN'] = '%s.elf'


@conf
def gcc_netx_flags(conf):
    conf.gcc_flags()

    f = conf.env.append_value

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g', '-gdwarf-2'])
        f(x + '_compile_debugrel', ['-Os', '-g', '-gdwarf-2'])
        f(x + '_compile_release',  ['-Os'])

    f('ASFLAGS_compile_debug',    ['-Wa,-gdwarf2'])
    f('ASFLAGS_compile_debugrel', ['-Wa,-gdwarf2'])

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x,[ '-mlong-calls',
              '-mapcs',
              '-mthumb-interwork',
              '-fshort-enums',
              '-fno-common'])

    f('ASFLAGS',[ '-mapcs',
                  '-mthumb-interwork',
                  '-fshort-enums',
                  '-c'])

    f('LINKFLAGS', [ '-mthumb-interwork', '-nostdlib'])

    f('DEFINES', ['_NETX_'])
    f('STLIB_nxo_standardlibs', ['m', 'gcc'])
    f('STLIB_default_standardlibs',   ['m', 'c', 'gcc'])


@device('netx', 'hitex codesourcery arm-eabi')
def configure_device_netx(conf):
    conf.gcc_netx_flags()

    for x in 'CFLAGS CXXFLAGS ASFLAGS LINKFLAGS'.split():
        conf.env.append_value(x, '-march=armv5te')


@device('netx50','hitex codesourcery arm-eabi')
def configure_device_netx50(conf):
    conf.gcc_netx_flags()

    if conf.cc_version()[0:2] < (4,1):
        mcpu = '-mcpu=arm9e'
    else:
        mcpu = '-mcpu=arm966e-s'

    for x in 'CFLAGS CXXFLAGS ASFLAGS'.split():
        conf.env.append_value(x, mcpu)

    if conf.env.TOOLCHAIN == 'hitex':
        # Workaround for hitex linking
        # hitex compiler chooses hardware fp library whenever something
        # else than arm926ej-s is specified
        conf.env.append_value('LINKFLAGS', '-mcpu=arm926ej-s')
    else:
        conf.env.append_value('LINKFLAGS', mcpu)

@device('netx10 netx51 netx52','codesourcery arm-eabi')
def configure_device_netx51(conf):
    conf.gcc_netx_flags()

    if conf.cc_version()[0:2] < (4,1):
        mcpu = '-mcpu=arm9e'
    else:
        mcpu = '-mcpu=arm966e-s'

    for x in 'CFLAGS CXXFLAGS ASFLAGS'.split():
        conf.env.append_value(x, mcpu)

    if conf.env.TOOLCHAIN == 'hitex':
        # Workaround for hitex linking
        # hitex compiler chooses hardware fp library whenever something
        # else than arm926ej-s is specified
        conf.env.append_value('LINKFLAGS', '-mcpu=arm926ej-s')
    else:
        conf.env.append_value('LINKFLAGS', mcpu)

@device('netx100 netx500','hitex codesourcery arm-eabi')
def configure_device_netx100(conf):
    conf.gcc_netx_flags()

    for x in 'CFLAGS CXXFLAGS ASFLAGS LINKFLAGS'.split():
        conf.env.append_value(x, '-mcpu=arm926ej-s')

@device('xpic','llvm-xpic')
def configure_device_xpic(conf):
    conf.env.prepend_value('CFLAGS',  ['-Wa,-mmcu=xpic'])
    conf.env.prepend_value('CXXFLAGS',['-Wa,-mmcu=xpic'])
    conf.env.prepend_value('ASFLAGS', ['-Wa,-mmcu=xpic'])
    conf.env.prepend_value('LINKFLAGS', '-mcpu=xpic')

    conf.env['cprogram_PATTERN']   = '%s.elf'
    conf.env['cxxprogram_PATTERN'] = '%s.elf'


@device('xpic2','llvm-xpic')
def configure_device_xpic2(conf):
    conf.env.prepend_value('CFLAGS',  ['-Wa,-mmcu=xpic2'])
    conf.env.prepend_value('CXXFLAGS',['-Wa,-mmcu=xpic2'])
    conf.env.prepend_value('ASFLAGS', ['-Wa,-mmcu=xpic2'])
    conf.env.prepend_value('LINKFLAGS', '-mcpu=xpic2')


    conf.env['cprogram_PATTERN']   = '%s.elf'
    conf.env['cxxprogram_PATTERN'] = '%s.elf'

@device('linux', 'gcc-linux')
def configure_device_linux(conf):
    conf.gcc_flags()

    f = conf.env.append_value

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g'])
        f(x + '_compile_debugrel', ['-O3', '-g'])
        f(x + '_compile_release', ['-O3'])

    f('STLIB',   ['m', 'c', 'gcc'])
    f('TOOL_OPTIONS', ["linkerscript_optional"])

@device('win32', 'mingw32')
def configure_device_win32(conf):
    conf.gcc_flags()

    f = conf.env.append_value

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g',])
        f(x + '_compile_debugrel', ['-O3', '-g',])
        f(x + '_compile_release',  ['-O3'])

    f('ASFLAGS_compile_debug',    ['-Wa,-g'])
    f('ASFLAGS_compile_debugrel', ['-Wa,-g'])
    f('TOOL_OPTIONS', ["linkerscript_optional"])

@device('win64', 'mingw64')
def configure_device_win64(conf):
    conf.gcc_flags()

    f = conf.env.append_value

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g',])
        f(x + '_compile_debugrel', ['-O3', '-g',])
        f(x + '_compile_release',  ['-O3'])

    f('ASFLAGS_compile_debug',    ['-Wa,-g'])
    f('ASFLAGS_compile_debugrel', ['-Wa,-g'])
    f('TOOL_OPTIONS', ["linkerscript_optional"])

@device('native', 'native')
def configure_device_native(conf):
    conf.gcc_flags()

    f = conf.env.append_value

    for x in 'CFLAGS CXXFLAGS'.split():
        f(x + '_compile_debug',    ['-O0', '-g',])
        f(x + '_compile_debugrel', ['-O3', '-g',])
        f(x + '_compile_release',  ['-O3'])

    f('ASFLAGS_compile_debug',    ['-Wa,-g'])
    f('ASFLAGS_compile_debugrel', ['-Wa,-g'])
    f('TOOL_OPTIONS', ["linkerscript_optional"])
