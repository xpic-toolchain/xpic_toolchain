#! /usr/bin/env python
# encoding: utf-8
import shutil

# Hilscher Waf version
HILSCHER_VERSION = '1.8.0.1'

from waflib import Configure, Options, Utils, Task, Node, Logs, Scripting, Context, ConfigSet
from waflib.Configure import conf, ConfigurationContext
from waflib.TaskGen import feature, after_method, before_method, taskgen_method, task_gen
from waflib.Context import WSCRIPT_FILE
from waflib.Tools import c, cxx, asm
import hilscher_toolchains
import os
import re

# We dont want to write "configure" when building. The following makes that
# waf will automatically run "configure" if required
# from waflib.Scripting import autoconfigure
# Configure.autoconfig = True

# do not automatically install anything
c.cprogram.inst_to = None

''' ------------------------------------------
    BUILD  CONDITIONS (release, debug, debugrel)
    ------------------------------------------ '''
from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext

# The output directory has to be derived from the conditions conditions only
def variant_dir(bld):
    return os.path.join(bld.out_dir, Options.options.conditions)

def get_doc(cls):
    try:
        cond = Options.options.conditions
    except:
        cond = 'release'

        for x in sys.argv:
            if x.startswith('--conditions'):
                cond = x.split('=')[1]

    return ({
       BuildContext    : 'Perform a %s build',
       CleanContext    : 'Clean a %s build',
       InstallContext  : 'Generate a %s distribution',
       UninstallContext: 'Clean a %s distribution',
    } [cls]) % cond

def setenv(self, name, env=None):
    if name not in self.all_envs or env:
            if not env:
                    env = ConfigSet.ConfigSet()
            else:
                    env = env.derive()
            self.all_envs[name] = env
    self.variant = name

def force_env(self, env):
    self.all_envs["force"] = env
    self.variant = "force"

class cproperty(property):
    def __get__(self, cls, owner):
        return self.fget(owner)

BuildContext.variant_dir = property(variant_dir, None)
BuildContext.setenv    = setenv
BuildContext.force_env = force_env
BuildContext.env = property(BuildContext.get_env, BuildContext.force_env)

BuildContext.__doc__ = cproperty(get_doc, None)
CleanContext.__doc__ = cproperty(get_doc, None)
InstallContext.__doc__ = cproperty(get_doc, None)
UninstallContext.__doc__ = cproperty(get_doc, None)


def opt_commasep(option, opt, value, parser):
    setattr(parser.values, option.dest, value.split(','))
''' ------------------------------------------
    CONFIGURATION
    ------------------------------------------ '''
def options(conf):
    conf.add_option('--custom-toolchain', action='store_true', dest='use_custom_toolchain', default=False, help='Use a custom toolchain for all builds')
    conf.add_option('--compiler-prefix', action='store', dest='custom_toolchain_prefix', default='arm-none-eabi', help='Select a custom compiler prefix (e.g. "arm-none-eabi" [default])')
    conf.add_option('--compiler-path', action='store', dest='custom_toolchain_path', default=None, help=r'Select a custom compiler path (e.g. "C:\Programme\Codesourcery\Sourcery G++ Lite\bin")')
    conf.add_option('--compiler-driver', action='store', dest='custom_toolchain_driver', default='gcc', help=r'Select a custom compiler driver (e.g. "gcc" [default])')
    conf.add_option('--cxxcompiler-driver', action='store', dest='custom_toolchain_cxxdriver', default='g++', help=r'Select a custom C++ compiler driver (e.g. "g++" [default])')
    conf.add_option('--conditions', type="choice", choices="debug debugrel release".split(), default='release',
                    help=r'Select a build condition (debug, debugrel or release[default] )')

    # Overwrite default installation root
    default_destdir = os.environ.get('DESTDIR', 'dist')
    conf.add_option('--destdir', help='installation root [default: %r]' % default_destdir, default=default_destdir, dest='destdir')

    conf.add_option('--dump-environment', action='callback', type='string', default=[], callback=opt_commasep,
                    help='Log selected environment variables to file (e.g. "DEFINES,CFLAGS')

''' configuration stuff    '''
def configure(conf):
    opt = Options.options

    conf.msg('Hilscher Waf version', HILSCHER_VERSION)
    conf.check_waf_version(mini="1.6.11")

    conf.setenv('')
    conf.cc_common_flags()

    conf.env['use_custom_toolchain'] = opt.use_custom_toolchain

    for x in ['patch', 'svn']:
      conf.find_program(x, var=x.upper(), mandatory=False)

    from waflib import Context
    requested_toolchains = getattr(Context.g_module, 'REQUIRED_TOOLCHAINS', ['hitex', 'codesourcery'])

    for toolchain in requested_toolchains:
        try:
            find_cross_gcc(conf, toolchain)
        except conf.errors.ConfigurationError:
            conf.start_msg("Toolchain '" + toolchain + "'")
            conf.end_msg("not found (Some projects may not be available for building)", 'YELLOW')

    if opt.use_custom_toolchain or opt.custom_toolchain_path:
      find_cross_gcc(conf, 'custom')

@conf
def get_conditions(self):
    return Options.options.conditions

import shlex
from waflib import Utils, Logs
from waflib.Build import BuildContext

@conf
def setup_build(ctx, id, device=None, toolchain=None):
    '''Function either prepares an environment for building (configure command)
       or loads a preconfigured environment (all other commands)'''
    envname = 'id_%s' % id

    if ctx.cmd == 'configure':
        ctx.configure_device(device, toolchain)
        ctx.setenv(envname, ctx.env)
        ctx.env.append_value('DEFINES', '__%s' % device.upper())
    elif isinstance(ctx, BuildContext):
        if envname not in ctx.all_envs:
            ctx.fatal('Environment for building %s not setup' % id)
        else:
            ctx.setenv(envname)

@conf
def find_cross_gcc(conf, name):
    '''This function is only left for backwards compat'''
    conf.configure_toolchain(name)

    conf.start_msg("Toolchain '" + name + "'")
    conf.end_msg("%s %s" %(conf.env.CC_NAME, ".".join(conf.env.CC_VERSION)))

    conf.setenv(name, conf.env)
    conf.setenv('')


def autorecurse(ctx, mandatory=True, excl=None):
  """
  Function for automatic search of targets (wscript)

  @param excl <b> list of directories: </b> directories to exclude from search
  @param mandatory <b> bool: </b> whether sub wscript files are required to exist
  """
  if getattr(Options.options, 'destdir', None):
      # Destdir has already been set, so use it
      exclude_dirs = [Options.options.destdir]
  else:
      # Destdir not set yet, so exclude default dest dir per default
      default_dest = ctx.path.find_node(os.environ.get('DESTDIR', 'dist'))

      if default_dest:
          exclude_dirs = [default_dest.abspath()]
      else:
          exclude_dirs = []

  # convert excl to list
  if excl is not None:
      for x in Utils.to_list(excl):
          node = ctx.path.find_node(x)

          if node:
              exclude_dirs.append(node.abspath())

  # do not search in output directory
  if None is not getattr(ctx, 'out_dir', None):
      exclude_dirs.append(ctx.out_dir)

  # Don't search in variant directory (also used during waf configure as output dir
  if None is not getattr(ctx, 'bldnode', None):
      exclude_dirs.append(ctx.bldnode)

  # now look to the file system
  # do not use the waf internal search functions as they will
  # create a complete node tree representation of
  # the project directory what we dont want

  directories = []
  for root, dirs, files in os.walk(ctx.path.abspath(), topdown=True):
      # if os.walk is used with topdown=True we can modify
      # dirs in place to avoid searching directories unnecessarily
      for x in dirs[:]:
          if (root + os.path.sep + x) in exclude_dirs:
              del dirs[dirs.index(x)]

      if WSCRIPT_FILE in files and root != ctx.path.abspath():
          node = ctx.path.find_dir(root[len(ctx.path.abspath()):])
          directories.append(node.path_from(ctx.path))

  directories.sort()

  # finally recurse into all directories where a wscript has been found
  ctx.recurse(directories, mandatory=mandatory)

# mantis21130: Allow autorecursing in configure/options stage
Context.Context.autorecurse = autorecurse

''' ------------------------------------------
    SDK support (header / defines only components
    ------------------------------------------ '''
@conf
def sdkcomponent(bld, name, **kw):
    features = Utils.to_list(kw.pop('features', []))

    bld(name=name,
        SDK=True,
        hidden_from_list=True,
        features=features + ['use', 'includes'],
        **kw)

from waflib.Tools import ccroot
''' ------------------------------------------
    EXTERNAL COMPONENT support
    ------------------------------------------ '''
@conf
def externalcomponent(bld, target, path, name, **kw):
    """
    Function to define external component (library).
    <b> Mandatory parameter </b>
    @param target \b string: file name of the library without prefix "lib' and extention '.a' (librcx.a -> rcx). It can be also a path to the library (relative to the current wscript), but also without prefix and extension (Lib/librcx.a -> Lib/rcx).
    @param name   \b string: name of the task (referenced by "use")
    @param path   \b string: path to this library
    \n\n  Optional parameters: \n\n
    @param export_includes <b> list of strings </b>: list of include directories, to include in build of sources, if this component will used (listed in: use =[])
    @param export_defines <b> list of strings </b>: list of definitions, to include in build of sources, if this component will used (listed in: use =[])
    """
    # mandatory parameters
    kw['target'] = target
    kw['name'] = name

    search_paths = []

    if path:
        node = bld.path.find_node(path)
        if node:
            search_paths.append(node)
    else:
        search_paths.append(bld.path)

    lib_type = 'stlib'

    names = [x % target for x in ccroot.lib_patterns[lib_type]]

    found = None

    for path in search_paths:
        for name in names:
            found = found or path.find_node(name)

    if found:
        bld(features='fake_lib',
            lib_paths=search_paths,
            lib_type=lib_type,
            hidden_from_list=True,
            **kw
        )
    else:
      Logs.pprint('YELLOW', 'Skipping external component "%s" as file "%s" was not found.' % (name, names))

''' ------------------------------------------
    BUILDING Rules (stlib)
    ------------------------------------------ '''
@conf
def stlib(bld, target, toolchain=None, platform=None, **kw):
    """
    Function to build a static library.
    <b>  Mandatory parameters  </b>
    @param target    \b string: file name of the target library without prefix 'lib' and extention '.a' (librcx.a -> rcx). It can be also a path to the library (relative to the current wscript), but also without prefix and extension (Lib/librcx.a -> Lib/rcx).
    @param toolchain \b string: identification of toolchain to use in build. \n For example: \e 'hitex' or \e 'codesourcery'. See 'hilscher_toolchains.py' for more toolchains or define therein a new one. \n
    @param platform  \b string: type of platform, that specified compiler parameter set. Values: \n\e netx - common parameters for all netx platforms \n\e netx500 - parameters specific for netX500 \n\e netx100 - parameters specific for netX100 \n\e netx50  - parameters specific for netX50 \n\e netx51  - parameters specific for netX51 \n\e netx10  - parameters specific for netX10
    \n\n\n
    <b> Optional parameters: </b>\n\n
    @param name      \b string: name of the task (referenced by "use")
    @param description \b string: description of this target, will be listed in "waf list"
    @param sources       <b> list of strings: </b> list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'thumb' mode
    @param sources_thumb <b> list of strings: </b> (equivalent to \e sources) list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'thumb' mode
    @param sources_arm   <b> list of strings: </b> list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'arm' mode
    @param includes <b> list of strings: </b> list of include directories to include in build of source files
    @param use <b> list of strings: </b> list of libraries to use in compile stage (exactly their 'export_defines' and 'export_includes' are useful for build of a library)
    @param defines <b> list of strings: </b> list of define directives to use in build of source files
    \n\n\n
    <b> Parameters for inheritance. You can include this target library to another target, in the \e use list. Automatically another target inherits \e public_includes and \e public_defines of this target </b> \n\n
    @param export_includes <b> list of strings: </b> list of include directories, to inherit in build of sources, if this component used (listed in: use =[])
    @param export_defines <b> list of strings: </b> list of definitions, to inherit in build of sources, if this component used (listed in: use =[])
    \n\n\n
    <b> Additional parameters for compiler and linker: </b>\n\n
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
    kw['target'] = target
    kw['platform'] = platform
    kw['toolchain'] = toolchain

    if not handle_toolchain(bld, kw):
        return

    if not 'name' in kw:
       kw['name'] = target
    else:
       kw['target'] = kw['name'] + '/' + target  # place targets with equal names in to different folders, mantis19622


    # separate arm and thumb sources:
    process_source(bld, kw)

    # get extra features
    features = Utils.to_list(kw.pop('features', []))

    # create build task
    bld(features=["c", "cstlib"] + features, **kw)


@conf
def program(bld, *k, **kw):
    """
    Function to build an executable (elf).
    <b>  Mandatory parameters  </b>
    @param target    \b string: file name of the target (excluding ".elf" extension: netX) and a reference for "use"
    @param toolchain \b string: identification of toolchain to use in build. \n For example: \e 'hitex' or \e 'codesourcery'. See 'hilscher_toolchains.py' for more toolchains or define therein a new one. \n
    @param platform  \b string: type of platform, that specified compiler parameter set. Values: \n\e common - don't set any additional parameter  \n\e netx - common parameters for all netx platforms \n\e netx500 - parameters specific for netX500 \n\e netx100 - parameters specific for netX100 \n\e netx50  - parameters specific for netX50 \n\e netx51  - parameters specific for netX51 \n\e netx10  - parameters specific for netX10
    @param linkerscript \b string: linker script
    \n\n\n
    <b> Optional parameters: </b>\n\n
    @param description \b string: description of this target, will be listed in "waf list"
    @param sources       <b> list of strings: </b> list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'thumb' mode
    @param sources_thumb <b> list of strings: </b> (equivalent to \e sources) list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'thumb' mode
    @param sources_arm   <b> list of strings: </b> list of source files (*.c/*.s) in absolute or relative path, that compiler translates in 'arm' mode
    @param includes <b> list of strings: </b> list of include directories to include in build of source files
    @param use <b> list of strings: </b> list of libraries to use in compile stage (exactly their 'export_defines' and 'export_includes' are useful for build of a library)
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

    if not handle_toolchain(bld, kw):
        return

    if not 'name' in kw:
        kw['name'] = kw['target']

    ext_pattern = os.path.splitext(bld.env.cprogram_PATTERN)[1]
    root_target, ext_target  = os.path.splitext(kw['target'])

    if ext_target and (ext_pattern == ext_target):
        # deprecated target syntax
        Logs.pprint('YELLOW', u'Deprecated target syntax for "%s". (Use "%s" instead)' %(kw['target'], root_target))
        target = bld.path.find_or_declare(kw['target'])
    else:
        target = bld.path.find_or_declare(bld.env.cprogram_PATTERN % kw['target'])

    if "linkerscript" in kw:
        ldfile = bld.path.find_node(kw['linkerscript'])

        if not ldfile:
            bld.fatal('Error: "%s", target "%s": Linkerscript "%s" not found!' % (bld.path.find_node('wscript').nice_path(bld.env), kw['target'], kw['linkerscript']))

        bld.add_manual_dependency(target, ldfile)
    else:
        if not "linkerscript_optional" in bld.env.TOOL_OPTIONS:
            bld.fatal("No linkerscript for target %s" % (kw['target']))

    process_source(bld, kw)

    # get extra features
    features = Utils.to_list(kw.pop('features', []))

    bld(features=["c", "cprogram", 'compile_thumb'] + features, **kw)


@conf
def bootimage(bld, *k, **kw):
    """
    Function to build a binary image from executable (elf).
    <b>  Mandatory parameters  </b>
    @param target    \b string: file name of binary
    @param use <b> list of strings: </b> target, from binary will be extracted
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

    bld(features="bootimage", **kw)


""" Function to add --start-group, --end-group linker flags to group all libraries """
@feature('group_lib')
@after_method('propagate_uselib_vars')
def process_grouplib(self):
    link_task = getattr(self, 'link_task', None)

    if link_task:
        link_task.env.append_value('STLIB_MARKER', '-Wl,--start-group')
        link_task.env.prepend_value('SHLIB_MARKER', '-Wl,--end-group')

""" Function to add cpecified linkerscript """
@feature('cprogram', 'cxxprogram')
@after_method('propagate_uselib_vars')
def add_linkerscript(self):
    if hasattr(self, "linkerscript"):
        ldfile = self.path.find_node(self.linkerscript)
        self.link_task.env.append_value('LINKFLAGS', ['-T', ldfile.bldpath()])

""" Function to add generation of map file """
@feature('cprogram', 'cxxprogram')
@after_method('apply_link', 'propagate_uselib_vars')
def generate_mapfile(self):
    # Name may contain compiler prefixes (e.g. /arm-hitex-elf/....) and
    # this would also be used as path for the map file. We must only use
    # the basefilename and replace the extension with .map
    _path, _filename = os.path.split(self.name)
    _basename, _ext = os.path.splitext(_filename)

    mapfile = _basename + '.map'
    mapfile_node = self.link_task.outputs[0].parent.find_or_declare(mapfile)
    self.link_task.outputs.append(mapfile_node)
    self.link_task.env.append_value('LINKFLAGS', '-Wl,-Map=%s' % mapfile_node.abspath())
    self.link_task.env.append_value('LINKFLAGS', '-Wl,-cref')

'''
   Buildstamp
'''
@feature('buildstamp')
@before_method('process_source')
def set_buildstamp(self):
    '''
    Sets defines BUILDSTAMP_BUILD_DATE_YEAR, BUILDSTAMP_BUILD_DATE_MONTH and BUILDSTAMP_BUILD_DATE_DAY according to the given date
    <b> Optional parameter </b>
    @param svn_buildstamp \b boolean: has to be set on 'True' to use svn-buildstamp [default: False]
    """
    '''
    conf = self.bld

    cache = getattr(conf, 'buildstamp_cache', {})
    conf.buildstamp_cache = cache

    # default to current date / time
    import datetime
    today = datetime.date.today()
    year, month, day = today.year, today.month, today.day

    # Use svn tag date from actual path
    if conf.root.find_node('.svn'):
        # read svn info from root of the project
        Logs.debug('buildstamp: We are on a SVN working copy (%r)' % self.name)

        for x in [self.path, conf.srcnode]:
          try:
            year, month, day = cache[x.abspath()]
            Logs.debug("buildstamp: cache hit (%r) for (%r)" % (x.abspath(), self.name))
            break
          except KeyError:
            if not x.find_node(".svn"):
              Logs.debug("buildstamp: no .svn found (%r)" % self.name)
              continue

            svn_info = svn_get_info(conf, x)

            if not svn_info:
              continue

            if any(y in svn_info['URL'] for y in ['/tag/', '/tags/']):
                # we are on a tag, so use tag date
                Logs.debug('buildstamp: SVN Tag detected (%r), trying to extract tag date (%r)' % (svn_info['URL'], self.name))
                svn_date = re.match('(\d+)-(\d+)-(\d+)\s+', svn_info['Last Changed Date'])

                year, month, day = tuple(map(int, svn_date.groups()))
                Logs.debug('buildstamp: Build stamp extracted from SVN tag date (%r)' % self.name)

            cache[x.abspath()] = (year, month, day)
            break
        else:
            Logs.debug('buildstamp: Error getting SVN information (%r)' % self.name)

    # set environments
    Logs.debug('buildstamp: Timestamp set to %u-%u-%u (%r)' % (year, month, day, self.name))
    self.env.append_value('DEFINES', 'BUILDSTAMP_BUILD_DATE_YEAR=(%u)' % year)
    self.env.append_value('DEFINES', 'BUILDSTAMP_BUILD_DATE_MONTH=(%u)' % month)
    self.env.append_value('DEFINES', 'BUILDSTAMP_BUILD_DATE_DAY=(%u)' % day)

'''
 svn stuff
'''
@conf
def svn_get_info(bld, path_or_url):
    """
    Function to get a svn information from specified directory.
    <b> Mandatory parameter </b>
    @param source_dir \b string: directory to get svn information about
    @return  \b dict or \b None: svn information or None on fail
    """
    if not bld.env.SVN:
      return None

    e = os.environ.copy()
    e['LANG'] = 'C'

    if isinstance(path_or_url, bld.node_class):
        path_or_url = path_or_url.abspath()

    if not path_or_url:
        Logs.pprint('YELLOW', "warning: can't find directory \'%s\', that contains svn information " % (path_or_url))
        return None

    try:
        out = bld.cmd_and_log(
                  [ bld.env.SVN, 'info' , path_or_url],
                  output=Context.STDOUT,
                  quiet=Context.BOTH,
                  env=e,
        )
    except:
        Logs.pprint('YELLOW', "warning: can't find svn information in directory \'%s\'.)." % (path_or_url))
        return None

    info = dict(map(str.strip, line.split(':', 1)) for line in out.splitlines() if line)
    return info

@feature('*')
@before_method('propagate_uselib_vars')
def apply_conditions(self):
    self.features = ['compile_' + Options.options.conditions] + Utils.to_list(getattr(self, 'features', []))

@feature('compile_thumb', 'compile_arm')
def dummy_func(self):
    '''Dummy function to avoid warning messages. The features
       'compile_xxx' are used to switch between arm/thumb compile
       just by adding 'compile_xxx' to the feature list, the environment
       variabeles called YYYY are extend by the values defined in
       environment variables 'YYYY_compile_xxx' '''
    pass

class patch(Task.Task):
    '''Patch a file'''
    color = 'CYAN'
    def run(self):
        inputs = self.inputs[:len(self.outputs)]
        patches = self.inputs[len(self.outputs):]
        outputs = self.outputs

        # copy all files from input to output
        for i, o in zip(inputs, outputs):
            o.parent.mkdir()
            shutil.copy(i.abspath(), o.abspath())

        cwd = self.cwd

        for i, x in enumerate(patches):
            cmd = '%s %s' % (self.env.PATCH, self.env.PATCHFLAGS)

            if self.patchargs[i]:
              cmd = cmd + ' ' + ' '.join(self.patchargs[i])

            try:
                self.generator.bld.cmd_and_log(cmd, cwd=cwd,
                                               quiet=True,
                                               stdin=open(x.abspath(), 'rt'))
            except self.generator.bld.errors.WafError:
                raise self.generator.bld.errors.WafError('Applying patch %s failed' % x.nice_path(self.env))

""" Separates sources between 'thumb' and 'arm' to compile """
def process_source(bld, kw):
    source_thumb = Utils.to_list(kw.pop('source', [])) + Utils.to_list(kw.pop('source_thumb', []))
    source_arm = Utils.to_list(kw.pop('source_arm', []))

    if not source_arm + source_thumb:
        bld.fatal("No source files have been specified")

    name = kw.pop('name')
    target = kw.pop('target')
    features = Utils.to_list(kw.pop('features', []))
    use = Utils.to_list(kw.pop('use', []))[:]
    hide_from_list = kw.pop('hidden_from_list', [])

    # mantis 0020827
    for x in kw:
        if isinstance(kw[x], list):
            kw[x] = kw[x][:]

    from waflib.Tools.c_aliases import get_extensions

    if 'patches' in kw:
        patches_to_apply = []
        files_to_patch = []
        patchargs = []

        for x in Utils.to_list(kw['patches']):
            patch_node = bld.path.find_node(x)

            if not patch_node:
                raise bld.fatal('Patch file %s not found' % x)

            try:
                out = bld.cmd_and_log([bld.env['PATCH'], '--dry-run', '-p0'],
                                      cwd=bld.path.abspath(),
                                      stdin=open(patch_node.abspath(), 'rt'),
                                      quiet=Context.BOTH
                                      )
                patchargs.append([])
            except (bld.errors.WafError) as e:
                results = [(e.stdout, e.stderr)]

                try:
                    out = bld.cmd_and_log([bld.env['PATCH'], '--dry-run', '-p0', '--binary'],
                                          cwd=bld.path.abspath(),
                                          stdin=open(patch_node.abspath(), 'rt'),
                                          quiet=Context.BOTH,
                                          )
                    patchargs.append(['--binary'])
                except (bld.errors.WafError) as e:
                    results.append((e.stdout, e.stderr))
                    msg = "Failed to invoke patch on %s: Non-binary failed with: %s; %s\nBinary failed with: %s; %s" % ((x,) + results[0] + results[1])
                    raise bld.fatal(msg)

            for y in out.splitlines():
                if y.startswith('checking file '):
                    node = bld.path.find_node(y[14:])
                elif y.startswith('patching file '):
                    node = bld.path.find_node(y[14:])
                elif y.startswith('(Stripping trailing CRs from patch'):
                    continue
                else:
                    bld.fatal('Parsing patch %s output failed: %s' % (x, y))

                if not node:
                    bld.fatal('File %s addressed by patch %s not found' % (y[14:], x))
                files_to_patch.append(node)

            patches_to_apply.append(patch_node)

        # Create patched files within a subdirectory named according to target
        n = bld.path.get_bld().make_node('patched_' + target)
        files_patched = [ n.make_node(x.path_from(bld.path)) for x in files_to_patch]

        # adapt includes
        includes = Utils.to_list(kw.get("includes", []))
        kw["includes"] = list(n.find_or_declare(x).path_from(bld.path) for x in includes) + (includes)

        tsk = patch(env=bld.env.derive(),)
        tsk.env.PATCHFLAGS = '-p0'
        tsk.cwd = n.abspath()
        tsk.set_inputs(files_to_patch + patches_to_apply)
        tsk.set_outputs(files_patched)
        tsk.patchargs = patchargs
        bld.add_to_group(tsk)

        for i, x in enumerate(source_thumb[:]):
            if not isinstance(x, bld.node_class):
              x = bld.path.get_src().find_node(x)

            try:
                y = files_to_patch.index(x)
                source_thumb[i] = files_patched[y]
            except ValueError:
                pass

        for i, x in enumerate(source_arm[:]):
            if not isinstance(x, bld.node_class):
              x = bld.path.get_src().find_node(x)

            try:
                y = files_to_patch.index(x)
                source_arm[i] = files_patched[y]
            except ValueError:
                pass

    for x in source_thumb + source_arm:
        if isinstance(x, str):
            nod = bld.path.get_src().find_node(x)
        else:  # must be a nod already
            nod = x
            x = nod.nice_path(bld.env)

        if nod is None:
            raise bld.errors.WafError("Source file %s not found" % x)

    # arm
    if source_thumb:
        feature_assembly = []
        exts = get_extensions(source_thumb)
        if's'in exts or'S'in exts or'asm'in exts or'ASM'in exts:
            feature_assembly = ['asm']

        bld.objects(
            source=source_thumb,
            features=['compile_thumb', 'c', 'cxx', 'asm'] + features,
            use=use[:],
            target=name + "_thumb_objs",
            hidden_from_list=True,
            **kw)

    if source_arm:
        feature_assembly = ''
        exts = get_extensions(source_arm)
        if's'in exts or'S'in exts or'asm'in exts or'ASM'in exts:
            feature_assembly = ' asm'

        bld.objects(
            source=source_arm,
            features=['compile_arm', 'c', 'cxx', 'asm'] + features,
            use=use[:],
            hidden_from_list=True,
            target=name + "_arm_objs",
             **kw)

    kw['use'] = use
    kw['features'] = features

    if source_arm:
        kw['use'].append(name + "_arm_objs")

    if source_thumb:
        kw['use'].append(name + "_thumb_objs")

    if hide_from_list:
      kw['hidden_from_list'] = hide_from_list,

    kw['dist_sources'] = source_thumb + source_arm
    kw['name'] = name
    kw['target'] = target

# we need this to create unique environment ids
index = 0

@conf
def handle_toolchain(bld, kw):
    target = kw.get('target', None)
    build_id = kw.get('build_id', None)
    toolchain = kw.get('toolchain', None)
    platform = kw.get('platform', None)

    if bld.env.use_custom_toolchain:
      toolchain = 'custom'
      Logs.pprint('YELLOW', "Forcing custom toolchain for %s" % target)

    if build_id is not None:
        # just select the specified environment
        # this is the new method
        try:
            bld.setup_build(id=build_id)
        except bld.errors.WafError:
            Logs.pprint('YELLOW', "Skipping target \'%s\' because toolchain \'%s\' not found." % (target, toolchain))
            return False
    elif (toolchain is not None) and (platform is not None):
        # setup the build contexts environment to the needs of our next build
        # this is the old method
        global index
        index = index + 1

        try:
            bld.select_toolchain(toolchain)
        except bld.errors.ConfigurationError:
            Logs.pprint('YELLOW', "Skipping target \'%s\' because toolchain \'%s\' not found." % (target, toolchain))
            return False

        bld.setenv('target_%s_%u' % (target, index), bld.env)

        # finally apply the platform (device) specific flags
        bld.apply_device_flags(platform)
    else:
        bld.fatal('Target %s does not specify toolchain and platform or specify the build id!' % target)

    return True

""" Find the location of a foreign library. Used by :py:class:`waflib.Tools.ccroot.read_shlib` and :py:class:`waflib.Tools.ccroot.read_stlib`. Modified function, original in ccroot.py """
@feature('fake_lib')
def process_lib(self):
    from waflib.Tools.ccroot import lib_patterns
    from waflib              import Errors
    node = None

    paths = self.to_list(self.lib_paths)

    if not paths:
        self.bld.fatal("Can't find component \'%s\' defined in \'%s/%s\'" % (self.name, self.path.abspath(), WSCRIPT_FILE))

    name = self.target or self.name

    names = [x % name for x in lib_patterns[self.lib_type]]
    for x in paths:
        if not isinstance(x, Node.Node):
            x = self.bld.root.find_node(x) or self.path.find_node(x)
            if not x:
                continue

        for y in names:
            node = x.find_node(y)
            if node:
                node.sig = Utils.h_file(node.abspath())
                break
        else:
            continue
        break
    else:
        raise Errors.WafError('could not find library %r' % self.name)

    self.link_task = self.create_task('fake_%s' % self.lib_type, [], [node])

class dumpenv(Task.Task):
    def run(self):
        tgen = self.generator
        bld = tgen.bld

        compiled_files = [ (o.name, tsk) for tsk in self.dump_tasks for o in tsk.outputs ]
        compiled_files = sorted(compiled_files, key = lambda x : x[0])

        len_field_name = max(len(n) for n,e in compiled_files)
        fmt_str = '%%-%ds: %%s' % (len_field_name) + os.linesep

        outputs = self.outputs

        out = open(outputs[0].abspath(),'wb')

        try:
            header = ''
            for var in self.vars:
                if header:
                    out.write(os.linesep * 2)

                header = 'Values used for environment variable %s' % var
                out.write(header + os.linesep)
                out.write('=' * len(header) + os.linesep * 2)

                for name, tsk in compiled_files:
                    tsk_vars = getattr(tsk,'vars',[])

                    if (not tsk_vars) or var in tsk_vars:
                        out.write(fmt_str % (name, tsk.env.get_flat(var)))
        finally:
            out.close()

@taskgen_method
def add_tsk_to_dumpenv_task(self, tsk):
    opt = Options.options

    # if this option is set, we shall create a list of environment settings
    if ((not getattr(self,"hidden_from_list", False)) or 'cprogram' in self.features) and opt.dump_environment:
        bldpath = self.path.get_bld()

        if not hasattr(self,"dumpenv_task"):
            link_task = getattr(self,"link_task", None)

            dumpenv_task = self.dumpenv_task = self.create_task("dumpenv", [], [])

            dumpenv_task.vars = list(opt.dump_environment)

            if link_task:
                outputs = [link_task.outputs[0].change_ext('_environment.txt')]
                dumpenv_task.dump_tasks = [link_task]
            else:
                outputs = [bldpath.find_or_declare('_' + self.target + os.path.sep + '_environment.txt')]
                dumpenv_task.dump_tasks = []

            dumpenv_task.set_outputs(outputs)

            if getattr(self, 'install_path', None):
                self.bld.install_files(self.install_path, dumpenv_task.outputs)

        else:
            dumpenv_task = self.dumpenv_task

        dumpenv_task.set_inputs(tsk.outputs)
        dumpenv_task.dump_tasks.append(tsk)

"""
  Create the compilation task: c, cxx, asm, etc. The output node is created automatically (object file with a typical **.o** extension).
  The task is appended to the list *compiled_tasks* which is then used by :py:func:`waflib.Tools.ccroot.apply_link`
  Extention: object-files (out) were placed in target specific directory (netx51/config.o  netx500/config.o, else it will be the same config.o file!)
  @param name \b string: name of the task class
  @param node <b> waflib.Node.Node: </b> the target file to compile
  @return <b> waflib.Task.Task: </b> the task created
  """
@taskgen_method
def create_compiled_task(self, name, node):
    bldpath = self.path.get_bld()
    out = '_' + self.target + os.path.sep + (os.path.splitext(node.name)[0] + '.o')

    compiled_node = bldpath.find_or_declare(out)
    task = self.create_task(name, node, compiled_node)

    try:
        self.compiled_tasks.append(task)
    except AttributeError:
        self.compiled_tasks = [task]

    self.add_tsk_to_dumpenv_task(task)

    return task

@taskgen_method
def accept_node_to_link(self, node):
    """
    Backport from recent waf
    """
    return not node.name.endswith('.pdb')

@taskgen_method
def add_objects_from_tgen(self, tg):
  """
  Add the objects from the depending compiled tasks as link task inputs. Additionaly
  dump build environment variables for the compiled tasks

  Some objects are filtered: for instance, .pdb files are added
  to the compiled tasks but not to the link tasks (to avoid errors)
  PRIVATE INTERNAL USE ONLY
  """
  try:
    link_task = self.link_task
  except AttributeError:
    pass
  else:
    for tsk in getattr(tg, 'compiled_tasks', []):
      for x in tsk.outputs:
        if self.accept_node_to_link(x):
          link_task.inputs.append(x)

      self.add_tsk_to_dumpenv_task(tsk)

@feature('c', 'cxx', 'asm', 'includes')
@after_method('apply_incpaths', 'process_use')
def process_includes(self):
    ''' Function removes all empty folders from includes
        To disable this functionality add to your target parameter: features = 'no_filter_includes'
    '''
    # if feature 'no_filter_includes' is set, skip filtering of empty "includes"
    if 'no_filter_includes' in self.features:
        return

    l = self.to_list(getattr(self,'includes_nodes',[]))

    def filter_dir(x):
       try:
            if x.is_bld():
                return True
            else:
                return len(x.listdir()) > 0
       except OSError:
           return False

    self.include_nodes = list(x for x in l if filter_dir(x))

    # update 'INCPATHS'
    self.env['INCPATHS'] = [x.abspath() for x in self.includes_nodes]

''' ------------------------------------------
    Adjusted 'LIST' command
    ------------------------------------------ '''
'''lists the targets to execute'''
class ListContext(BuildContext):
  cmd = 'list'
  def execute(self):
    """
    See :py:func:`waflib.Context.Context.execute`.
    """
    self.restore()
    if not self.all_envs:
      self.load_envs()

    self.recurse([self.run_dir])
    self.pre_build()

    # display the time elapsed in the progress bar
    self.timer = Utils.Timer()

    for g in self.groups:
      for tg in g:
        try:
          f = tg.post
        except AttributeError:
          pass
        else:
          f()

    try:
      # force the cache initialization
      self.get_tgen_by_name('')
    except:
      pass
    lst = list(self.task_gen_cache_names.keys())
    lst.sort()

    SIZE_TARGET = len('Target')
    SIZE_TOOLCHAIN = len('Toolchain')
    SIZE_DESCRIPTION = len('Description')

    target_list = {}

    for k in lst:
      # Strip out external components which cannot be build anyway
      tg = self.get_tgen_by_name(k)
      toolchain = None
      if getattr(tg, 'hidden_from_list', False):
        Logs.debug('list: Supressing hidden component %s' % k)
      else:
        if getattr(tg, 'toolchain', None):
          toolchain = tg.toolchain
        elif getattr(tg, 'build_id', None):
          toolchain = tg.env.TOOLCHAIN
        elif getattr(tg, 'use', None):
          # Search use components to valid toolchain
          for _use in Utils.to_list(tg.use):
            _usetg = self.get_tgen_by_name(_use)
            toolchain = getattr(_usetg, 'toolchain', None)
        else:
            toolchain = None

        if not toolchain:
          Logs.debug("list: Supressing component without 'toolchain' attribute %s" % k)
          continue

        toolchain_env = self.all_envs[toolchain].derive()

        if 'CC' in toolchain_env:
          avail = '*'.center(3)
        else:
          avail = ' '.center(3)

        target = k
        description = getattr(tg, 'description', '')
        if getattr(tg, 'version', ''):
          description += ' - ' + tg.version

        if len(target) > SIZE_TARGET:
          SIZE_TARGET = len(target)

        if len(toolchain) > SIZE_TOOLCHAIN:
          SIZE_TOOLCHAIN = len(toolchain)

        if len(description) > SIZE_DESCRIPTION:
          SIZE_DESCRIPTION = len(description)

        displaygroup = getattr(tg, 'displaygroup', 'default')

        if displaygroup in target_list:
          target_list[displaygroup].append({ 'avail'       : avail,
                                             'target'      : target,
                                             'toolchain'   : toolchain,
                                             'description' : description, })
        else:
          target_list[displaygroup] = [ { 'avail'       : avail,
                                        'target'      : target,
                                        'toolchain'   : toolchain,
                                        'description' : description, } ]

    TOTAL_SIZE = 3 + (SIZE_TARGET + 2) + (SIZE_TOOLCHAIN + 2) + SIZE_DESCRIPTION
    Logs.pprint('GREEN', 'Available Targets:')
    Logs.pprint('GREEN', '')
    Logs.pprint('GREEN', 'A'.center(3) + 'Target'.ljust(SIZE_TARGET + 2) + 'Toolchain'.ljust(SIZE_TOOLCHAIN + 2) + 'Description')
    Logs.pprint('GREEN', '-' * TOTAL_SIZE)

    for group in target_list:
      Logs.pprint('RED', '-' * len(group))
      Logs.pprint('RED', group)
      Logs.pprint('RED', '-' * len(group))

      for x in target_list[group]:
        output = x['avail'].center(3) + x['target'].ljust(SIZE_TARGET + 2)
        output += x['toolchain'].ljust(SIZE_TOOLCHAIN + 2) + x['description']
        Logs.pprint('GREEN', output)

    Logs.pprint('GREEN', '-' * TOTAL_SIZE)

''' ------------------------------------------
    Windows XP command line length workaround
    ------------------------------------------ '''
import sys
import string

if sys.platform.startswith("win"):
    import waflib.Context
    import tempfile

    exec_cmd_orig = waflib.Context.Context.exec_command

  # Windows has a limitation in command line length and we
  # may need to put the command line in a file and call the
  # executable with @file to pass the command line parameters
    winver = tuple(sys.getwindowsversion()[0:2])

  # This is Windows XP or Server 2003 (5.1, 5.2) or
  # Vista / 7, which can handle command lines of 8191 chars
    if winver > (5,0):
        max_commandline_len = 8191
  # NT4, 200 can handle a max command line length of 2047
    else:
        max_commandline_len = 2047


    """Overwrites waf.Context.Context.exec_command to support long command lines"""
    def exec_command(self, cmd, **kw):
        global max_commandline_len, exec_cmd_orig

        native_path = None

        if isinstance(cmd,str):
            cmd = cmd.strip()
            cmd_length = len(cmd)
        else:
            cmd_length = len(" ".join(('"' + x + '"') for x in cmd))

        if cmd_length > max_commandline_len:
            Logs.debug('runner: longcmd_line: %r' % cmd)

            (fd, path) = tempfile.mkstemp('.lnk', dir=os.path.abspath(kw.get('cwd','.')), text=True)

            if isinstance(cmd,str):
                if cmd[0] == '"':
                    cmd, dummy, params = cmd[1:].partition('"')
                    cmd    = '"' + cmd
                else:
                    cmd, dummy, params = cmd.partition(' ')

                params = params.strip()
            else:
                # process all parameters in command string:
                args = (x.decode() for x in cmd[1:])
                # escape backslashes
                args = (x.replace('\\','\\\\') for x in args)
                # escape quotation marks
                args = (x.replace('"','\\"') for x in args)

                args = tuple(args)

                params = ('"' + '" "'.join(args) + '"')
                cmd = '"' + cmd[0] + '"'

            Logs.debug('runner: longcmd_cmd: %r' % cmd)
            Logs.debug('runner: longcmd_params: %r' % params)

            os.write(fd, params + "\n")
            os.close(fd)

            native_path = os.path.basename(path)
            cmd = cmd + " @" + native_path

        try:
            retval = exec_cmd_orig.__get__(self, self.__class__)(cmd, **kw)
        finally:
            if native_path:
              os.unlink(os.path.abspath(path))

        return retval

    waflib.Context.Context.exec_command = exec_command
''' ------------------------------------------
   BOOTIMAGE support
   ------------------------------------------ '''
import array
import re
import shlex

from waflib.Context import STDOUT
from waflib.Logs import error
from waflib import Errors

aboot_header_elements = dict(
  {  'MagCok':             0,

    'MemCtrl':            1,
    'Speed':              1,

    'AppEnt':             2,
    'AppChksm':           3,
    'AppFilSiz':          4,
    'AppSrtAdd':          5,
    'Signt':              6,

    'SdramGeneralCtrl':   7,
    'ExpBusReg':          7,
    'SramCtrl':           7,

    'SdramTimingCtrl':    8,
    'IoRegMode0':         8,

    'IoRegMode1':         9,
    'SdramMR':            9,

    'IfConf1':            10,

    'IfConf2':            11,

    'MiscAsicCtrl':       12,
    'Serial':             13,
    'SrcType':            14,
    'BootChksm':          15}
)

aboot_src_device_types = {
  'srb' : 1,
  'spi' : 2,
  'i2c' : 3,
  'mmc' : 4,
  'dpm' : 5,
  'edpm': 6,
  'ext' : 7,
}

aboot_signature = 0x5854454e
aboot_magiccookie = 0xf8beaf00

class netx_bootimage(Task.Task):
    ''' Run objcopy on the target'''
    color = 'PINK'
    inst_to = None
    cmdline = None

    def get_segment_table(self, filename):
      atSegments = []

      cmd = [self.env['OBJDUMP'], '-h', '-w', filename]
      result = self.generator.bld.cmd_and_log(cmd, output=STDOUT, quiet=STDOUT)

      for match_obj in re.finditer('[ \t]*([0-9]+)[ \t]+([^ \t]+)[ \t]+([0-9a-fA-F]+)[ \t]+([0-9a-fA-F]+)[ \t]+([0-9a-fA-F]+)[ \t]+([0-9a-fA-F]+)[ \t]+([0-9*]+)[ \t]+([a-zA-Z ,]+)', result):
        uiAlign = eval(match_obj.group(7))
        astrFlags = match_obj.group(8).split(', ')
        atSegments.append(dict({
          'idx':    int(match_obj.group(1)),
          'name' :  match_obj.group(2),
          'size' :  int(match_obj.group(3), 16),
          'vma' :   int(match_obj.group(4), 16),
          'lma' :   int(match_obj.group(5), 16),
          'file_off' :  int(match_obj.group(6), 16),
          'align' :  uiAlign,
          'flags' :  astrFlags
        }))

      return atSegments

    def get_load_address(self, segments):
      # Set an invalid lma
      ulLowestLma = 0x100000000

      # Loop over all segments.
      for tSegment in segments:
        # Get the segment with the lowest 'lma' entry which has also the flags 'CONTENTS', 'ALLOC' and 'LOAD'.
        if (tSegment['lma'] < ulLowestLma) and ('CONTENTS' in tSegment['flags']) and ('ALLOC' in tSegment['flags']) and ('LOAD' in tSegment['flags']):
          ulLowestLma = tSegment['lma']

      if ulLowestLma == 0x100000000:
        raise Exception("failed to extract load address!")

      return ulLowestLma

    def get_exec_address(self, filename):
      # Get the start address.

      cmd = [self.env['OBJDUMP'], '-f', filename]
      result = self.generator.bld.cmd_and_log(cmd, output=STDOUT, quiet=STDOUT)

      match_obj = re.search('start address 0x([0-9a-fA-F]+)', result)
      if not match_obj:
        raise Exception('Failed to extract start address. (Command %s, Output %s)' % (cmd.join(' '), result))

      return int(match_obj.group(1), 16)


    def get_estimated_bin_size(self, segments):
      ulLoadAddress = self.get_load_address(segments)
      ulBiggestOffset = 0

      # Loop over all segments.
      for tSegment in segments:
        # Get the segment with the biggest offset to ulLoadAddress which has also the flags 'CONTENTS', 'ALLOC' and 'LOAD'.
        if ('CONTENTS' in tSegment['flags']) and ('ALLOC' in tSegment['flags']) and ('LOAD' in tSegment['flags']):
          ulOffset = tSegment['lma'] + tSegment['size'] - ulLoadAddress
          if ulOffset > ulBiggestOffset:
            ulBiggestOffset = ulOffset

      return ulBiggestOffset

    def find_device_node(self, xmlfile, strDeviceTyp, strChipTyp, strDevice):

      # Find all nodes of the requested type.
      tPlatformElement = None
      for tElement in xmlfile.findall(strDeviceTyp):
        if tElement.get('netx_version') == strChipTyp:
          tPlatformElement = tElement
          break

      if tPlatformElement == None:
        raise Exception("Chiptyp %s not found in %s list." % strChipTyp, strDeviceTyp)

      tDeviceElement = None

      for tElement in tPlatformElement.findall('Device'):
        if ('id' in tElement.attrib) and (tElement.attrib['id'] == strDevice):
          tDeviceElement = tElement
          break

      if tDeviceElement == None:
        raise Exception("Device %s not found in %s %s list." % (strDevice , strChipTyp, strDeviceTyp))

      return tDeviceElement

    def get_parameters_from_node(self, tDeviceElement, kind):
      global aboot_src_device_types
      ret = {}

      for tElement in tDeviceElement.findall('Param'):
        if not 'name' in tElement.attrib:
          raise Exception("Error in netx xml: the Param node has no name attribute!")

        strFieldName = tElement.attrib['name']
        ulValue = int(tElement.text.strip(), 0)

        ret[strFieldName] = ulValue;

      if 'type' not in tDeviceElement.attrib:
          raise Exception('Error in netx.xml: Device node %s misses "type" attribute' % tDeviceElement.attrib['id'])
      x = tDeviceElement.attrib['type']

      if 'src' == kind:
          # try to set 'SrcType' from 'type' attribute
          if x.lower() not in aboot_src_device_types:
              raise Exception('Error in netx.xml: Device node %s specifies unknown "type" value %s' % (tDeviceElement.attrib['id'], x))

          # This will only set SrcType if not already defined
          ret.setdefault('SrcType', aboot_src_device_types[x.lower()])

      return ret

    def bootblocker(self):
      patch_data = dict()

      # Parse command line
      # -s  --source                source device (typically a flash device)
      # -d  --destination           destination device (typically a RAM device)
      # -t  --toolset               toolset e.g. HITEX_ARM_WIN_4_00
      # -ct --chiptype              chip type (netx500|netx100|netx50|netx10|netx51_52)
      # -ud --userdata              userdata value (32 bit dec or hex)
      # -o  --output                output file
      # -x  --xmlfile               xml file
      # -tl --taglist               append taglist from binary file <st

      cmdline = " ".join(self.cmdline)
      cmdline_split = shlex.split(cmdline)

      if len(cmdline_split) % 2 != 0:
        Logs.error("Invalid command line passed %s" % cmdline)
        raise Exception("Invalid command line")

      itemiter = iter(cmdline_split)

      options = {'-x' : os.path.dirname(__file__).replace('\\','/')+'/netx.xml'}

      while itemiter:
        try:
          option = next(itemiter)
          arg = next(itemiter)
        except StopIteration:
          break

        options[option] = arg

      if not '-s' in options:
        Logs.error("Option: -s (Source Device) missing")
        raise Exception("Option: -s (Source Device) missing")
      elif not '-d' in options:
        Logs.error("Option: -d (Destination Device) missing")
        raise Exception("Option: -d (Destination Device) missing")
      elif not '-ct' in options:
        Logs.error("Option: -ct (Chip Type) missing")
        raise Exception("Option: -ct (Chip Type) missing")

      # Read the xml file.
      import xml.etree.ElementTree

      netx_xml = xml.etree.ElementTree.ElementTree()
      xmlfile = options['-x']
      netx_xml.parse(xmlfile)

      # Find source and destination device settings
      src_device = options['-s']
      dst_device = options['-d']
      chiptype = options['-ct']

      # Detect chip type node
      if chiptype.lower() == "netx100" or chiptype.lower() == "netx500":
        chip_node = "netx 500"
      elif chiptype.lower() == "netx50":
        chip_node = "netx 50"
      elif chiptype.lower() == "netx51_52":
        chip_node = "netx 51_52"
      elif chiptype.lower() == "netx10":
        chip_node = "netx 10"
      else:
        Logs.error("Unknown Chiptype %s" % chiptype)
        raise Exception("Unknown Chiptype")

      # Search XML file for data
      src_node = self.find_device_node(netx_xml, 'src', chip_node, src_device)
      src_patches = self.get_parameters_from_node(src_node, kind='src')

      dst_node = self.find_device_node(netx_xml, 'dst', chip_node, dst_device)
      dst_patches = self.get_parameters_from_node(dst_node, kind='dst')

      x = src_patches.copy()
      x.update(dst_patches)

      return x

    def run(self):
      inputfile = self.inputs[0].get_bld().abspath()
      outputfile = self.outputs[0].get_bld().abspath()

      segments = self.get_segment_table(inputfile)
      Logs.debug("Segments : " + str(segments))
      ulEstimatedBinSize = self.get_estimated_bin_size(segments)

      # Do not create files larger than 512MB.
      if ulEstimatedBinSize >= 0x20000000:
        Logs.error('The resulting file \'%s\' seems to extend 512MBytes. Too scared to continue!' % self.outputs[0])
        return False

      ulExecAddress = self.get_exec_address(inputfile)
      ulLoadAddress = self.get_load_address(segments)

      Logs.debug("ExecAddr : " + hex(ulExecAddress))
      Logs.debug("LoadAddr : " + hex(ulLoadAddress))

      if isinstance(self.cmdline, dict):
        # use BOOTBLOCKERDATA
        patch_data = self.cmdline
      else:
        # use netx.xml and parse bootblocker command line
        patch_data = self.bootblocker()

      # Build bootheader
      aBootBlock = array.array('I', [0] * 16)
      aBootBlock[0x00] = aboot_magiccookie  # Magic cookie.
      aBootBlock[0x01] = 0x00000000  # unCtrl
      aBootBlock[0x02] = ulExecAddress  # execution address
      aBootBlock[0x05] = ulLoadAddress  # load address
      aBootBlock[0x06] = aboot_signature  # 'NETX' signature
      aBootBlock[0x07] = 0x00000000  # krams
      aBootBlock[0x08] = 0x00000000  # krams
      aBootBlock[0x09] = 0x00000000  # krams
      aBootBlock[0x0a] = 0x00000000  # krams
      aBootBlock[0x0b] = 0x00000000  # krams
      aBootBlock[0x0c] = 0x00000001  # misc_asic_ctrl dummy
      aBootBlock[0x0d] = 0x00000000  # user data
      aBootBlock[0x0e] = 0x00000000  # src type

      for key, value in patch_data.items():
        if not key in aboot_header_elements:
          Logs.error('Invalid option %s' % key)
          raise Exception('Invalid option %s' % key)

        offset = aboot_header_elements[key]
        aBootBlock[offset] = value

      # Use Objcopy to generate binary file
      cmd = [self.env['OBJCOPY'], '-O', 'binary', inputfile, outputfile]
      result = self.generator.bld.cmd_and_log(cmd, output=STDOUT, quiet=STDOUT)

      # Get the application data.
      tBinFile = open(outputfile, 'rb')
      strBinFile = tBinFile.read()
      tBinFile.close()

      # Pad the application size to a multiple of dwords.
      uiPadBytes = len(strBinFile) & 3;
      if uiPadBytes != 0:
        uiPadBytes = 4 - uiPadBytes
        strBinFile += '\0' * uiPadBytes

      # Get the size of the evetually padded data.
      ulApplicationDwordSize = int(len(strBinFile) / 4);

      aulApplicationData = array.array('I')
      aulApplicationData.fromstring(strBinFile)

      # Build the application checksum.
      ulApplicationChecksum = 0
      for ulData in aulApplicationData:
        ulApplicationChecksum += ulData
        ulApplicationChecksum &= 0xffffffff

      aBootBlock[0x03] = ulApplicationChecksum  # application checksum
      aBootBlock[0x04] = ulApplicationDwordSize  # application dword size

      # Build the bootblock checksum.
      ulBootblockChecksum = 0
      for ulData in aBootBlock:
        ulBootblockChecksum += ulData
        ulBootblockChecksum &= 0xffffffff
      ulBootblockChecksum = (ulBootblockChecksum - 1) ^ 0xffffffff
      aBootBlock[0x0f] = ulBootblockChecksum

      # Write the bootimage.
      fOutput = open(outputfile, 'wb')
      aBootBlock.tofile(fOutput)
      aulApplicationData.tofile(fOutput)
      fOutput.close()

@feature('bootimage')
@after_method('check_tgen_availability')
def build_bootimage(self):
    '''
    Function to build a netX bootable image by performing an OBJCOPY and
    prepending a netX bootheader (64 byte).
    '''

    if not getattr(self, 'BOOTBLOCKERFLAGS', None):
        # BOOTBLOCKERFLAGS missing, check if someone has given
        # absolute data for elements
        if not getattr(self, 'BOOTBLOCKERDATA', None):
          self.bld.fatal("BOOTBLOCKERFLAGS or BOOTBLOCKERDATA is missing")
        else:
          cmdline = getattr(self, 'BOOTBLOCKERDATA', None)
    else:
        cmdline = getattr(self, 'BOOTBLOCKERFLAGS', None)

    if getattr(self, 'name_patch', None):  # firmware build, so "build_bootimage" produces temporary binary file
      target = self.path.find_or_declare(self.name_patch)
    else:
      target = self.path.find_or_declare(self.target)

    if not getattr(self, 'use', None):
        self.bld.fatal("Building a bootimage requires 'use' parameter")

    for x in self.to_list(self.use):
        try:
            tg = self.bld.get_tgen_by_name(x)
            # is already posted?
            if not getattr(tg, 'posted', None):
                tg.post()
            break
        except self.bld.errors.WafError:
            continue
    else:
        Logs.pprint('YELLOW', "Skipping nxf-file \'%s\' because no matching tgen found. (Probably the source elf was not build because of missing toolchain)" % (target))
        return

    input = tg.link_task.outputs[0]

    self.bootblocker_task = self.create_task('netx_bootimage', [input], [target])
    self.bootblocker_task.cmdline = cmdline
    self.bootblocker_task.env = tg.env.derive()

    if not getattr(self, 'name_patch', None):  # do not install temporary files
      if getattr(self, 'install_path', None):
        self.bld.install_files(getattr(self, 'install_path', None), self.bootblocker_task.outputs)

@feature('*')
@before_method('process_rule')
def check_tgen_availability(self):
    """ Disable all task generators which refer to not existing tgens (use) """
    tgen_disabled = False
    msg = None

    # check if some use component is either missing
    # or has been disabled by itself
    for x in self.to_list(getattr(self, "use", [])):
        try:
            y = self.bld.get_tgen_by_name(x)
        except self.bld.errors.WafError:
            tgen_disabled = "use-component \'%s\' for target \'%s\' not found." % (x, self.name)
        else:
            if not getattr(y, "posted", None):
                y.post()

            try:
                tgen_disabled = tgen_disabled or y.tgen_disabled
            except AttributeError:
                msg = "task_gen {y.name} is missing tgen_disabled attribute when processing tgen {self.name}".format(**locals())

    # we always want to cleanup!
    if self.bld.cmd in ["clean", "list"]:
        tgen_disabled = False

    self.tgen_disabled = tgen_disabled

    if tgen_disabled:
        self.use = []


    if (not getattr(self, 'hidden_from_list', False)) or ('cprogram' in self.features):
        if msg:
            Logs.pprint('YELLOW', "Warning: " + msg)

        if tgen_disabled:
            Logs.pprint('YELLOW', 'Warning: target \'{self.name}\' disabled: {tgen_disabled}'.format(**locals()))


    # finally check if we should disable this tgen
    if tgen_disabled:
        # droping all tgen methods == generating no tasks and thus no output
        # we must do it like this because of the python reference stuff
        # self.meths = [] will not work!!
        while len(self.meths):
            del self.meths[0]


''' ------------------------------------------
    Workaround for 'use' with target names containing
    virtual path (e.g. /arm-none-eabi/4.5.2/pnsap)
    ------------------------------------------ '''
"""
  Process the ``use`` attribute which contains a list of task generator names::
    def build(bld):
      bld.shlib(source='a.c', target='lib1')
      bld.program(source='main.c', target='app', use='lib1')
  See :py:func:`waflib.Tools.ccroot.use_rec`.
  """
@feature('c', 'cxx', 'd', 'use', 'fc')
@before_method('apply_incpaths', 'propagate_uselib_vars')
@after_method('apply_link', 'process_source')
def process_use(self):
    use_not = self.tmp_use_not = set([])
    use_seen = self.tmp_use_seen = []  # we would like an ordered set
    use_prec = self.tmp_use_prec = {}
    self.uselib = self.to_list(getattr(self, 'uselib', []))
    names = self.to_list(getattr(self, 'use', []))

    for x in names:
      self.use_rec(x)

    # Remove all sdk components from use dependency ordering
    for x in use_prec.keys()[:]:
        try:
            y = self.bld.get_tgen_by_name(x)
        except Errors.WafError:
            pass
        else:
            if hasattr(y,"SDK"):
                del use_prec[x]


    for x in use_not:
      if x in use_prec:
        del use_prec[x]

    # topological sort
    out = []
    tmp = []
    for x in self.tmp_use_seen:
      for k in use_prec.values():
        if x in k:
          break
      else:
        tmp.append(x)

    while tmp:
      e = tmp.pop()
      out.append(e)
      try:
        nlst = use_prec[e]
      except KeyError:
        pass
      else:
        del use_prec[e]
        for x in nlst:
          for y in use_prec:
            if x in use_prec[y]:
              break
          else:
            tmp.append(x)
    if use_prec:
      raise Errors.WafError('Cycle detected in the use processing %r' % use_prec)
    out.reverse()

    link_task = getattr(self, 'link_task', None)
    for x in out:
      y = self.bld.get_tgen_by_name(x)

      var = y.tmp_use_var
      if var and link_task:
        if var == 'LIB' or y.tmp_use_stlib:
          ''' WAF per default uses the last item from target path, but we are using
              always a slash '''
          self.env.append_value(var, [y.target[y.target.rfind('/') + 1:]])
          self.link_task.dep_nodes.extend(y.link_task.outputs)
          tmp_path = y.link_task.outputs[0].parent.path_from(self.bld.bldnode)
          self.env.append_value(var + 'PATH', [tmp_path])
      else:
        if y.tmp_use_objects:
          self.add_objects_from_tgen(y)

      if getattr(y, 'export_includes', None):
          lst = y.to_incnodes(y.export_includes)
          self.env.append_value('INCLUDES', [x.abspath() for x in lst])

      if getattr(y, 'export_defines', None):
        self.env.append_value('DEFINES', self.to_list(y.export_defines))

      if link_task and getattr(y, 'lib', None):
        self.env.append_value('LIB', self.to_list(y.lib))

    # and finally, add the uselib variables (no recursion needed)
    for x in names:
      try:
        y = self.bld.get_tgen_by_name(x)
      except:
        if not self.env['STLIB_' + x] and not x in self.uselib:
          self.uselib.append(x)
      else:
        for k in self.to_list(getattr(y, 'uselib', [])):
          if not self.env['STLIB_' + k] and not k in self.uselib:
            self.uselib.append(k)

    # sort libs accordind their appearence in 'use'
    if getattr(self, 'link_task', None):
      seen_in_stlib = []
      for u in getattr(self,"use", []):
        if u in self.tmp_use_not: continue

        y = self.bld.get_tgen_by_name(u)
        if y.tmp_use_var != 'STLIB': continue

        lib_name = y.target[y.target.rfind('/') + 1:]
        if lib_name in self.env['STLIB']:
          seen_in_stlib.append(lib_name)
          self.env['STLIB'].remove(lib_name)


      self.env['STLIB'] = seen_in_stlib + self.env['STLIB']

@feature('cprogram', 'remove_libc')
@after_method('apply_link')
@before_method('process_use', 'propagate_uselib_vars')
def add_standardlibs(self):
    uselib = Utils.to_list(getattr(self, "uselib", []))
    if 'remove_libc' in self.features:
        uselib.append("nxo_standardlibs")
    else:
        uselib.append("default_standardlibs")

    self.uselib = uselib

# Redefine asm class:
# make sure that the assembler also passed the C-Defines, as we are using gcc as assembler
# which may include .h files with #ifdef
class asm(Task.Task):
    color = 'NORMAL'
    run_str = '${AS} ${ASFLAGS} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${AS_SRC_F}${SRC} ${AS_TGT_F}${TGT}'


@feature('*')
@after_method('apply_incpaths', 'process_includes')
def parse_version(self):
    bld = self.bld
    version_include = getattr(self,"version_include", None)

    if 'nxf' in self.features:
        # case of usual firmware
        usetg = bld.get_tgen_by_name( Utils.to_list(self.use)[0] )
        if usetg and getattr(usetg,"version", None):
          self.version = usetg.version
    elif version_include is not None:
        if 'bootimage' in self.features:
            # case of rom-image
            self_tg = bld.get_tgen_by_name( Utils.to_list(self.use)[0] )
        else:
            # other targets
            self_tg = self
        include_nodes = getattr(self_tg, "include_nodes", [])[:]

        for x in getattr(self_tg, "export_includes", []):
            include_nodes.append(self_tg.path.find_node(x))

        for x in include_nodes:
            node = x.find_node(version_include)

            if node:
                break
        else:
            bld.fatal("Version Header %r not found for %r " % (version_include, self.name))

        content = node.read();

        r = re.compile(r"^#define\s+\w+_VERSION_(MAJOR|MINOR|BUILD|REVISION)\s+(\S.*)$", re.M)
        vars = dict((name, int(eval(definition))) for name, definition in r.findall(content))

        self.version = "%d.%d.%d.%d" % (vars['MAJOR'], vars['MINOR'], vars['BUILD'], vars['REVISION'])

from string            import Template

task_log_defaults = {
    'c'               : '[CC]        $SOURCES',
    'cxx'             : '[C++]       $SOURCES',
    'asm'             : '[ASM]       $SOURCES',
    'cstlib'          : '[LIB]       $TARGETS',
    'cprogram'        : '[ELF]       $TARGETS',
    'cxxprogram'      : '[ELF]       $TARGETS',
    'patch'           : '[PATCH]     $CWD',
    'insight_autogen' : '[AUTOG]     $SOURCES',
    'NxoBuilderTask'  : '[NXO]       $TARGETS',
    'nxupdate'        : '[NXF]       $TARGETS',
    'rst2html'        : '[RST2HTML]  $TARGETS',
    'svnexport'       : '[SVNEXPORT] $TARGETS',
    'netx_bootimage'  : '[BOOTIMAGE] $TARGETS',
    'dumpenv'         : '[DUMPENV]   $TARGET',
}

def task_str(self):
    global task_log_defaults

    try:
        self.log_tmpl
    except AttributeError:
        tasknamefield_len = 0

        for x in Task.classes.values():
            name = x.__name__.replace('_task', "")
            logstring = task_log_defaults.get(name, None)
            logstring = getattr(x, "log_str", logstring)

            if logstring:
                logname = logstring.split(None, 1)[0]
                tasknamefield_len = max(tasknamefield_len, len(logname))

        for x in Task.classes.values():
            name = x.__name__.replace('_task', '')
            logstring = "[{0}] $SOURCES $TARGETS".format(name.upper())
            logstring = task_log_defaults.get(name, logstring)
            logstring = getattr(x, "log_str", logstring)

            fields = logstring.split(None, 1)
            fields[0] = fields[0] + " " * (tasknamefield_len - len(fields[0]))
            x.log_tmpl = Template(" ".join(fields))

    env = self.env

    class Mapping(dict):
        def __getitem__(self, key):
            try:
                return super(Mapping, self).__getitem__(key)
            except KeyError:
                task = super(Mapping, self).__getitem__("task")

                try:
                    return getattr(task, key)
                except AttributeError:
                    return super(Mapping, self).__getitem__(key)

    kw = Mapping((("task", self),))
    kw['SOURCES'] = ' '.join([x.nice_path(env) for x in self.inputs])
    kw['TARGETS'] = ' '.join([x.nice_path(env) for x in self.outputs])
    kw['TARGET']  = self.generator.target

    bld = self.generator.bld
    cwdnode = bld.root.find_node(getattr(self, 'cwd', getattr(bld, "cwd", bld.variant_dir)))

    kw['CWD'] = cwdnode.nice_path(env)
    kw['CLASS'] = self.__class__.__name__.replace('_task', '')


    log = self.log_tmpl.substitute(kw)

    if len(log) > 80:
        log = log[:77] + "..."

    return log + "\n"



Task.Task.__str__ = task_str

from waflib.Build import inst

def install_files_fixed(self, dest, files, env=None, chmod=Utils.O644, relative_trick=False, cwd=None, add=True, postpone=True):
    """
    Create a task to install files on the system::

        def build(bld):
            bld.install_files('${DATADIR}', self.path.find_resource('wscript'))

    :param dest: absolute path of the destination directory
    :type dest: string
    :param files: input files
    :type files: list of strings or list of nodes
    :param env: configuration set for performing substitutions in dest
    :type env: Configuration set
    :param relative_trick: preserve the folder hierarchy when installing whole folders
    :type relative_trick: bool
    :param cwd: parent node for searching srcfile, when srcfile is not a :py:class:`waflib.Node.Node`
    :type cwd: :py:class:`waflib.Node.Node`
    :param add: add the task created to a build group - set ``False`` only if the installation task is created after the build has started
    :type add: bool
    :param postpone: execute the task immediately to perform the installation
    :type postpone: bool
    """
    tsk = inst(env=env or self.env)
    tsk.bld = self
    tsk.path = cwd or self.path
    tsk.chmod = chmod

    if isinstance(files, Node.Node):
        files =  [files]
    else:
        files = Utils.to_list(files)

        for f in files:
            if not isinstance(f, Node.Node):
                node = self.path.find_node(f)

                if not node:
                    self.fatal("File %s does not exist (%s/wscript)" % (f,self.path.relpath()));

        files = list((f if isinstance(f, Node.Node) else self.path.find_node(f)) for f in files)

    tsk.source = files
    tsk.dest = dest
    tsk.exec_task = tsk.exec_install_files
    tsk.relative_trick = relative_trick
    if add: self.add_to_group(tsk)
    self.run_task_now(tsk, postpone)
    return tsk


InstallContext.install_files = install_files_fixed
