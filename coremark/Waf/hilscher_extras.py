#! /usr/bin/env python
# encoding: utf-8

from waflib import Task, Logs, Context, Options, Node, Errors, Utils
from waflib.Configure import conf
from waflib.Build import BuildContext
from waflib.TaskGen import feature, after_method, before_method
from waflib.Context import WSCRIPT_FILE
import shutil
import time, os, re, sys, string

from waflib import TaskGen, Configure

def options(conf):
    conf.add_option('--generate-report', action='append', dest='generate_report', type='choice',
                    choices='all size'.split(), default = [], metavar='REPORTS',
                    help='Generate a report about each created firmware. REPORTS is a comma separated list of report types to include: size, <tbd>, all. (Defaults to \'all\')')


@feature('cprogram', 'cxxprogram', 'cxxshlib')
@after_method('apply_link', 'generate_mapfile')
def apply_sizereport(self):
    '''Create a size report'''

    if not isinstance(self.bld, BuildContext):
        return

    if ( 'all' in Options.options.generate_report) or ('size' in Options.options.generate_report):
        elffile    = self.link_task.outputs[0]
        mapfile    = self.link_task.outputs[1]

        reportfile = elffile.change_ext('.html')

        self.create_task('sizereport', [elffile, mapfile], reportfile)


TEMPLATE_SIZEREPORT_HEAD='''
<html>
<head>
<title>%(title)s</title>
<style>
table {
    border : 2px solid black;
    border-collapse: collapse;
    min-width: 40em;
}

td, th {
    border : 1px solid black;
    border-collapse: collapse;
    padding: 2px 2px;
}

th {
    background: #CCCCCC;
    text-align: left;
}

.rightalign {
    text-align: right;
}

</style>
</head>
<body>
<h1>%(title)s</h1>
'''

TEMPLATE_SIZEREPORT_FOOT='''
</body>
</html>
'''


class Container(dict):
    def __init__(self,*args,**kwargs):
        dict.__init__(self,*args,**kwargs)
        self.__dict__ = self


class sizereport(Task.Task):
    ''' Generate a sizereport out of elf file '''
    color = 'CYAN'

    _symbols_cache_by_section = None
    _symbols_cache_by_module  = None

    @property
    def symbols_by_name(self):
        return self.symbols

    @property
    def symbols_by_section(self):
        if self._symbols_cache_by_section is None:
            _symbols_cache_by_section = {}

            for x in self.symbols_by_name.values():
                try:
                    _symbols_cache_by_section.setdefault(x.section, []).append(x)
                except AttributeError:
                    pass

            self._symbols_cache_by_section = _symbols_cache_by_section

        return self._symbols_cache_by_section

    @property
    def symbols_by_module(self):
        if self._symbols_cache_by_module is None:
            _symbols_cache_by_module = {}

            for x in self.symbols_by_name.values():
                try:
                    _symbols_cache_by_module.setdefault(x.module, []).append(x)
                except AttributeError:
                    pass

            self._symbols_cache_by_module = _symbols_cache_by_module

        return self._symbols_cache_by_module

    @property
    def sections_by_name(self):
        return self.sections

    def addSymbol(self, name):
        self._symbols_cache_by_section = None
        self._symbols_cache_by_module  = None
        return self.symbols.setdefault(name, Container(name=name))

    def addSection(self, name):
        return self.sections.setdefault(name, Container(name=name))

    def run(self):
        self.symbols  = {}
        self.sections = {}

        env = self.env
        bld = self.generator.bld
        cwd = bld.variant_dir


        sections = {}
        # run & parse the objdump output
        self.parseObjDumpOutput(bld.cmd_and_log([env.OBJDUMP, '-h', '-w' , self.inputs[0].abspath()], cwd=cwd, quiet=True))

        # run & parse the nm output
        self.parseNmOutput(bld.cmd_and_log([env.NM, '-f', 'sysv' , self.inputs[0].abspath()], cwd=cwd, quiet=True))

        #parse the map file
        self.parseMapFile(self.inputs[1].abspath())

        # Now generate all the content. we use separate functions
        # for each kind of output. this is slower, but easier to extend
        # currently only one kind of content is available
        items = list(x() for x in (self.generateHTMLIntro,
                                   self.generateHTMLSecByName,
                                   self.generateHTMLModulesByName))

        # now write html file output
        global TEMPLATE_SIZEREPORT_HEAD, TEMPLATE_SIZEREPORT_FOOT

        out = open(self.outputs[0].abspath(),'wt')

        try:
            params = Container()
            params.title ='Sizereport for %s (%s)' % (self.inputs[0].name, bld.variant)

            out.write(TEMPLATE_SIZEREPORT_HEAD % params)

            # output TOC
            out.write('<h2>Table of Contents</h2>\n')
            out.write('<ol>\n')
            for toc,content in items:
                out.write('<li>%s</li>\n' % ('\n'.join(toc)))
            out.write('</ol>\n')

            # output content
            for toc,content in items:
                out.write(('\n'.join(content) + '\n'))

            out.write(TEMPLATE_SIZEREPORT_FOOT % params)
        finally:
            out.close()

    def parseObjDumpOutput(self,output):
        def null_handler(*args):
            pass

        def section_handler(self, line):
            # skipt first line after section header
            if line.startswith('Idx'):
                return

            idx, name, size, vma, lma, offset, align, flags = line.split(None,7)

            # parse the objdump output into container
            x = self.addSection(name)

            x.idx    = int(idx,10)
            x.size   = int(size,16)
            x.vma    = int(vma,16)
            x.lma    = int(lma,16)
            x.offset = int(offset,16)
            x.align  = eval(align)
            x.flags  = flags.split(', ')

        def symbol_handler(self,line,sections):
            m = re.match('([0-9a-fA-F]{8})\s+(.{7})\s(\S+)\s([0-9a-fA-F]{8})\s+(.+)', line)

            if m:
                value,flags,section,sizealign,name = m.groups()

                # skip the section named symbol
                if name == section:
                    return

                x = self.addSymbol(name)

                x.value   = int(value,16)
                x.flags   = filter(lambda x : x.strip(),list(flags))
                x.section = section
                x.size    = int(sizealign,16)

        handler = null_handler

        # Add special sections
        for x in '*ABS* *UND*'.split():
            self.addSection(x)

        # Parse the objdump output
        for x in output.splitlines():
            if x == 'Sections:':
                handler = section_handler
            elif x == 'SYMBOL TABLE:':
                handler = symbol_handler
            elif x.strip() != '':
                handler(self,x)

    def parseNmOutput(self,output):
        def null_handler(self,line):
            if line.startswith('Name'):
                return symbol_handler
            else:
                return null_handler

        def symbol_handler(self,line):
            try:
                name, value, clas, type, size, line,section = tuple(x.strip() for x in line.split('|'))
            except ValueError:
                raise Exception(line)

            # parse the nm output into container

            x = self.addSymbol(name)

            if size:
                x.size    = int(size,16)
            if value:
                x.value   = int(value,16)

            x.section = section

            return symbol_handler

        handler = null_handler

        for x in output.splitlines():
            if not x.strip():
                continue
            else:
                handler = handler(self, x)



    def parseMapFile(self, path):


        def dummy_handler(self, line):
            return dummy_handler


        re_reference_archive = re.compile("^(\S.+)[(](.+)[)]$").match
        re_reference_why     = re.compile("^\s+((\S.+)[(](.+)[)]\s+)?[(](\w+)[)]$").match

        def reference_handler(self, line):
            if re_reference_archive(line):
                pass
            elif re_reference_why(line):
                pass
            else:
                return dummy_handler

            return reference_handler

        re_mapping_section        = re.compile("^([.]\S+)(\s+(0x[0-9a-f]+)(\s+(0x[0-9a-f]+))?)?(\s+load address\s+(0x[0-9a-f]+))?$").match
        re_mapping_section_module = re.compile("^\s+([.]\S+)$").match
        re_mapping_len_module     = re.compile("^\s+([.]\S+)?\s*(0x[0-9a-f]+)\s+(0x[0-9a-f]+)(\s+(\S[^()]+)([(](\S+)[)])?)?$").match
        re_mapping_fill           = re.compile("^\s+([*]fill[*])\s+(0x[0-9a-f]+)\s+(0x[0-9a-f]+)\s+(\w+)$").match
        re_mapping_symbol         = re.compile("^\s+\[?(0x[0-9a-f]+)\]?\s+(\w+)$").match
        re_mapping_issymbol       = re.compile("^\w+$").match
        re_mapping_linkerscript   = re.compile("^\s+(\S.+)$").match

        state = Container()
        state.section        = None
        state.module_section = None
        state.address        = None
        state.size           = None
        state.archive        = None
        state.archive_member = None

        def mapping_handler(self, line):

            if line.lower().startswith("load"):
                pass
            elif re_mapping_len_module(line):
                module_section, state.address, state.size, dummy_a, state.archive, dummy_b, state.archive_member = re_mapping_len_module(line).groups()

                if module_section is None:
                    module_section = state.module_section
                else:
                    state.module_section = module_section

                if module_section is not None:
                    l = module_section[1:].split(".",1)

                    if len(l) >= 2:
                        section, name = l

                        try:
                            symbol = self.symbols_by_name[name]
                        except KeyError:
                            pass
                        else:
                            symbol.module = os.path.basename(state.archive)
            elif re_mapping_section_module(line):
                state.module_section, = re_mapping_section_module(line).groups()
            elif re_mapping_symbol(line):
                value, symbol = re_mapping_symbol(line).groups()

                if re_mapping_issymbol(symbol):
                    x = self.addSymbol(symbol)
                    x.module = os.path.basename(state.archive)
            elif re_mapping_section(line):
                state.section, dummy_a, state.section_addr, dummy_b, state.section_size, \
                  dummy_c, state.section_addr_load = re_mapping_section(line).groups()
                state.address     = None
                state.size        = None
                state.path_module = None
                state.name_object = None
            elif re_mapping_fill(line):
                pass
            elif re_mapping_linkerscript(line):
                pass
            else:
                return dummy_handler

            return mapping_handler


        handler = dummy_handler
        with open(path,"rt") as infile:
            for line in infile:
                line = line.rstrip()

                if not line:
                    pass
                elif line.lower() == "archive member included because of file (symbol)":
                    handler = reference_handler
                elif line.lower() == "linker script and memory map":
                    handler = mapping_handler
                else:
                    handler = handler(self,line)



    def generateHTMLIntro(self):
        toc     = []
        content = []

        toc.append('<a href="#intro">Introduction</a>')

        path = self.inputs[0].abspath()

        mtime     = time.localtime(os.path.getmtime(path))
        filesize  = os.path.getsize(path)

        content.append('<h2 id="intro">Introduction</h2>')
        content.append('<ul>')
        content.append('<li><b>Path:</b> %s</li>' % path)
        content.append('<li><b>Last Modification:</b> %s</li>' % time.strftime('%Y-%m-%d %H:%M:%S %Z',mtime))
        content.append('<li><b>Size:</b> %d bytes</li>' % filesize)
        content.append('</ul>')
        content.append
        return (toc,content)

    def generateHTMLSecByName(self):
        toc     = []
        content = []

        secbyname = []

        # generate section by name toc
        toc.append('<a href="#secbyname">Symbols by Section and Name</a><ol>')
        for sec in sorted(self.sections_by_name.values(),key = lambda y : y.get('idx',99)):
            # Skip all debugging symbols
            if 'DEBUGGING' in sec.get('flags',[]):
                continue

            symbols = tuple(x for x in self.symbols_by_section.get(sec.name,[]) if getattr(x,'size', 0))

            if symbols:
                secbyname.append(sec)
                toc.append('<li><a href="#secbyname_%s">Section &quot;%s&quot</a></li>' % (sec.name[1:],sec.name))

        toc.append('</ol>')


        content.append('<h2>Symbols by Section and Name</h2>')
        # iterate over all sections shown in toc
        for sec in secbyname:
            content.append('<h3 id="secbyname_%s">Section &quot;%s&quot</h3>' % (sec.name[1:],sec.name))

            if getattr(sec,'size',None) and getattr(sec,'lma',None):
                content.append('<p>Size: %(size)d, LMA %(lma)#010x</p>' % sec)

            content.append('<table>\n')
            content.append('<tr><th>Symbol Name</th><th>Symbol Address</th><th>Symbol size</th></tr>')

            symbols = tuple(x for x in self.symbols_by_section.get(sec.name,[]) if getattr(x,'size', 0))

            for sym in sorted(symbols,key = lambda x : x.name):
                content.append('<tr><td>%(name)s</td><td>%(value)#010x</td><td class="rightalign">%(size)d</td></tr>' % sym)

            content.append('</table>')

        return (toc,content)

    def generateHTMLModulesByName(self):
        toc     = []
        content = []

        secbyname = []

        # generate section by name toc
        toc.append('<a href="#modbyname">Symbols by Module and Name</a><ol>')
        content.append('<h2>Symbols by Module and Name</h2>')

        for module, symbols in sorted(self.symbols_by_module.items(), key = lambda x: x[0]):
            symbols = tuple(x for x in symbols if getattr(x,'size', 0))

            if symbols:
                anchor = module.translate(string.maketrans(".","_"))

                toc.append('<li><a href="#modbyname_%s">Module &quot;%s&quot</a></li>' % (anchor, module))
                content.append('<h3 id="modbyname_%s">Module &quot;%s&quot</h3>' % (anchor,module))

                content.append('<table>\n')
                content.append('<tr><th>Symbol Name</th><th>Size text/rodata</th><th>Size data/bss</th><th>Size xip</th></tr>')

                sum_data = 0
                sum_text = 0
                sum_xip = 0

                for sym in sorted(symbols,key = lambda x : x.name):
                    section = sym.section[1:].split(".")[0]

                    size_text = 0
                    size_data = 0
                    size_xip  = 0

                    address = sym.get("value",0)

                    if address >= 0x0c000000 and address < (0x0c000000 + 4*1024*1024):
                        size_xip = sym.size
                    elif section in ("bss", "data", "stacks"):
                        size_data = sym.size
                    else:
                        size_text = sym.size

                    sum_text += size_text
                    sum_data += size_data
                    sum_xip  += size_xip

                    content.append('<tr><td>%s</td><td class="rightalign">%d</td><td class="rightalign">%d</td><td class="rightalign">%d</td></tr>' % (sym.name, size_text, size_data, size_xip))

                content.append('<tr><th>SUM</th><th class="rightalign">%d</th><th class="rightalign">%d</th><th class="rightalign">%d</th></tr>' % (sum_text, sum_data, sum_xip))
                content.append('</table>')
        toc.append('</ol>')

        return (toc,content)

    def __str__(self):
      src_str = ' '.join([x.nice_path(self.env) for x in self.inputs])
      tgt_str = ' '.join([x.nice_path(self.env) for x in self.outputs])
      return '[SIZEREPORT] %s\n' % (src_str)

'''
     Autogeneration of wscript for a set of libraries
'''

@conf
def generatewscript(bld, target, use, *k, **kw):
    """
    Function to generate a 'wscript' for specified in 'use' libraries.
    <b> Mandatory parameter </b>
    @param target \b string: target file name
    @param use   <b> list of targets names:</b> name of the task (referenced by "use")
    \n\n  Optional parameters: \n\n
    @param name   \b string: name of the task
    @param install_path \b string: path to install generated component
    """
    output_node = bld.srcnode.get_bld().make_node(kw['install_path']).make_node(target)
    name        = output_node.path_from(bld.srcnode.get_bld())

    # mandatory parameters:
    kw['target'] = [output_node]
    kw['name']   = name

    try:
        tgen = bld.get_tgen_by_name(name)
    except bld.errors.WafError:
        tgen = bld(features="genwscript", hidden_from_list = True, **kw)
        tgen.use      = {}
        tgen.wscripts = []

    if getattr(tgen,"posted", None):
        bld.fatal("Wscript tgen already posted")

    if isinstance(use,dict):
        tgen.use.update(use)
    else:
        use = Utils.to_list(use)

        for x in use:
            tgen.use.setdefault(x,[])

    tgen.wscripts.append(bld.path.find_node(WSCRIPT_FILE))


@feature('genwscript')
@before_method('check_tgen_availability')
def build_wscript(self):
    self.genwscript_task = self.create_task('genwscript')
    self.genwscript_task.outputs = self.to_list(self.target)
    self.genwscript_task.inputs  = self.to_list(self.wscripts)
    self.genwscript_task.input   = []
    self.genwscript_task.forced_includes = {}

    new_use = self.use.copy()

    for x, includes in self.use.copy().items():
        try:
            tgen = self.bld.get_tgen_by_name(x)
        except Errors.WafError:
            del new_use[x]
        else:
            self.genwscript_task.input.append(tgen)

            if includes:
                self.genwscript_task.forced_includes[tgen] = self.to_list(includes)

    self.use = new_use
    self.genwscript_task.SELF    = self

    # add dependency of target-"wscript" on "wscript" that calls "generatewscript" function

    if getattr(self, 'install_path', None) is not None:
      self.bld.install_files(self.install_path, self.genwscript_task.outputs)


class genwscript(Task.Task):
    ''' Run objcopy on the target'''
    log_str   = "[WSCRIPT] $TARGET"
    color     = 'PINK'
    inst_to   = None
    cmdline   = None

    def run(self):
      input           = self.input
      forced_includes = self.forced_includes
      outputfile = self.outputs[0]
      outstring = ""

      # does exist an output file Node?
      if not outputfile:
        Logs.pprint('YELLOW', "warning: generatewscript - can not find o create a target file '%s'" % str(outputfile) )
        return

      # create target build directory, if not exist
      target_dir = outputfile.parent.abspath()
      on = os.path.exists(target_dir)
      if not os.path.exists(target_dir):
        os.makedirs(target_dir)

      # delete the file to raise an error (just on "waf install") on failure
      outputfile.delete()

      # get install_path fot generated wscript:
      if getattr(self.SELF, 'install_path', None):
        wscript_install_path = self.SELF.install_path
      else:
        # not defined, set default to 'dist'
        wscript_install_path = 'dist'

      # if user passed a dict, we need to used his forces includes, otherwise
      # we take them from public_includes of used components
      # dict: 'task_name1':['export_includes11', 'export_includes21', '...']
      #       'task_name2':['export_includes21', 'export_includes22', '...']
      for tgen in input:
        if not getattr(tgen, "posted", None):
            bld.fatal("Source tgen not yet posted")

        if getattr(tgen, 'SDK', False):
          # This is a SDK component only providing includes and defines
          comp_name = "    bld(name            = \"%s\",\n" % tgen.name
          _ljust = comp_name.rindex('(') + 1
          outstring += comp_name
          outstring += " "*_ljust + "SDK             = True,\n"
        else:

          if not getattr(tgen, 'install_path', None):
            # this component will not installed
            continue

          # This is a pre-compiled library
          comp_name = "    bld.externalcomponent(target          = \"%s\",\n" % tgen.target[tgen.target.rfind("/")+1:]
          _ljust = comp_name.rindex('(') + 1
          outstring += comp_name

          # get relative path to target
          rel_path = os.path.relpath(path  = tgen.install_path,
                                     start = wscript_install_path)

          outstring += " "*_ljust + "path            = \"%s\",\n" % rel_path.replace('\\', '/')
          outstring += " "*_ljust + "name            = \"%s\",\n" % tgen.name

        if getattr(tgen, 'description', None):
          outstring += " "*_ljust + "description     = \"%s\",\n" % tgen.description

        if getattr(tgen, 'displaygroup', None):
          outstring += " "*_ljust + "displaygroup    = \"%s\",\n" % tgen.displaygroup

        if getattr(tgen, 'version', None):
          outstring += " "*_ljust + "version         = \"%s\",\n" % tgen.version

        # export includes
        export_includes = forced_includes.get(tgen, None) or getattr(tgen, 'public_includes', None)

        if export_includes:
          outstring += " "*_ljust + "export_includes = ["
          for ei in export_includes:
            outstring += "\"%s\", " % ei
          outstring += "],\n"

        # If a component defines public_defines we will use them,
        # otherwise fall back to export defines
        export_defines = getattr(tgen, 'public_defines', None) or getattr(tgen, 'export_defines', None)
        if export_defines:
          outstring += " "*_ljust + "export_defines  = ["
          for ed in export_defines:
            outstring += "\"%s\", " % ed
          outstring += "],\n"

        # Make sure the closing brace is aligned with the opening brace
        # _ljust contains space to first char in parametername (so substract 1)
        outstring += " "*(_ljust - 1) + ")\n\n"

      if outstring:
          outstring = "def build(bld):\n\n" + outstring

      outputfile.write(outstring)
'''
     end of Autogeneration of wscript for a set of libraries
'''

'''
 svn stuff
'''
@conf
def svn_export(bld, target, svn_url, *k, **kw):
    """
    Function to export files from specified svn-location to local directory.
    <b> Mandatory parameter </b>
    @param target \b string: local destination directory to export files from svn
    @param svn_url \b string: svn url
    """
    # create target dir
    if not os.path.exists(target):
      os.makedirs(target)
    # get target node
    n = bld.srcnode.find_node(target)
    if n:
      bld(features="svnexport", target = n, svn_url= svn_url, hidden_from_list = True, **kw)
    else:
      Logs.pprint('YELLOW',"warning: can't create or find target directory \'%s\'." % (target) )

@feature('svnexport')
def build_svnexport(self):
    svnexport_tsk = self.create_task('svnexport', tgt = self.target)
    svnexport_tsk.url = self.svn_url
    svnexport_tsk.custom_options =  getattr(self,'custom_options',None)

    # try to run all 'svnexport's in series
    task_svnexport = getattr(self.bld,'task_svnexport',None)
    if task_svnexport:
        svnexport_tsk.set_run_after(task_svnexport)
    # save in context current task to run the next one after it
    self.bld.task_svnexport = svnexport_tsk;

    # add dependency of target on "wscript" that calls "svn_export" function
    current_wscript = self.path.find_node(WSCRIPT_FILE)
    if current_wscript:
      self.bld.add_manual_dependency(svnexport_tsk.outputs[0], current_wscript)


class svnexport(Task.Task):
    ''' perform an svn export of given url '''
    color   = 'PINK'
    log_str = "[SVNEXPORT] $url"

    def run(self):
        # user can define own export options, for example without "--ignore-externals"
        export_options = Utils.to_list(getattr(self,"custom_options", "--force --quiet --ignore-externals"))

        res = self.exec_command([self.env.SVN] + export_options + ["export", \
                               self.url, self.outputs[0].abspath()])
        if res:
            raise Errors.WafError("Failed to export url {self.url}".format(**locals()))



'''
   Firmware Info
'''
class Bunch(object):
    pass

'''
   Doc-utils
'''
class rst2html(Task.Task):
    ''' convert a rst file to a html file '''
    color   = 'CYAN'
    inst_to = None

    # run
    def run(self):
      # arguments for docutils call
      arguments = ['-d','-t', self.inputs[0].abspath(), self.outputs[0].abspath()]
      '''
      From the packed distributive the docutils can't extract style-sheet (html4css1.css) and template (template.txt) files.
      They are installed separately in "WAF_DIR\dist\docutils" directory.
      '''
      docutils_extras = sys.path[0] + '/dist/docutils'
      '''
      Check, if directory exist. In developer waf version this directory do not exist,
      default style-sheet and template will be used (from docutils installed python)
      '''
      try:
        os.stat(docutils_extras)
        arguments.insert(0, '--template=%s/template.txt'%docutils_extras)
        arguments.insert(0, '--stylesheet-path=%s/html4css1.css'%docutils_extras)
      except:
        pass
      # just warn on error:
      try:
        from docutils.core import publish_cmdline
        publish_cmdline(writer_name='html', argv=arguments)
      except:
        Logs.pprint('YELLOW',"warning: can't convert RST file to HTML")


from waflib.TaskGen import extension
@extension('_Insight.xml')
def insight_hook(self,node):
    root,ext = os.path.splitext(node.name)

    autogen_c_node = node.parent.get_src().find_or_declare('Sources/' + root + '_Autogen.c')
    autogen_h_node = node.parent.get_src().find_or_declare('Includes/'+ root + '_Autogen.h')

    try:
        task = self.create_task('insight_autogen',[node],[autogen_c_node,autogen_h_node])
    except:
        pass
    else:
        self.get_hook(autogen_c_node)(self,autogen_c_node)


'''
   Prefix for the component names
'''
@conf
def get_name_prefix(bld, toolchain = None, suffix = None, build_id = None):
    """
    Constructs a clean prefix for the component names (e.g. "arm-hitex-elf/4.0.3/rcX_V2.0/")
    <b> Mandatory parameter </b>
    @param toolchain \b string: identification of toolchain to use in build.
    @param suffix \b string: identification of the rcX (e.g. "rcX_V2.0", "rcX_V2.1" and so on).
    """
    try:
        if build_id:
            bld.setup_build(build_id)
        else:
            if toolchain in bld.all_envs:
                bld.setenv(toolchain)
            elif "toolchain_" + toolchain in bld.all_envs:
                bld.setenv("toolchain_" + toolchain)
            else:
                bld.fatal("Toolchain not found")

        prefix = bld.env['MACHINE'] or "unkown"

        return "/".join(x for x in (prefix, \
                                     ".".join(bld.env['CC_VERSION']), \
                                     suffix) if x) + "/"
    except bld.errors.WafError:
        return "[TOOLCHAIN not found]"
    finally:
        bld.setenv('')

class copy(Task.Task):
    color = "CYAN"

    def run(self):
        inputs  = self.inputs
        outputs = self.outputs

        for i,o in zip(inputs,outputs):
            o.parent.mkdir()
            shutil.copy(i.abspath(), o.abspath())


@feature("sourcezip")
@before_method("process_rule")
def process_sourcezip(self):
    while len(self.meths):
        del self.meths[0]

    svn_root_info = self.bld.svn_get_info(self.path)

    to_copy = {}

    root_node   = self.path.get_bld().make_node("SourceZip")

    use = self.to_list(self.use)

    if self.bld.cmd == "install":
        create_task = self.create_task
    else:
        def create_task(*args,**kw):
            class Namespace(object): pass
            return Namespace()

    while use:
        try:
            tgen = self.bld.get_tgen_by_name(use.pop(0))
        except self.bld.errors.WafError:
            continue

        svn_info = self.bld.svn_get_info(tgen.path)

        if "fake_lib" in tgen.features:
            source_node = root_node.make_node("Externals")
            export_node = source_node.make_node(tgen.path.name)

            if svn_info['Repository Root'] == "http://subversion01:8080/svn/FirmwareBuilds":
                exporturl = "http://subversion01:8080/svn/Fieldbus" + svn_info['URL'][len(svn_info['Repository Root']):] \
                   + "/Components"

                svn_export_info = self.bld.svn_get_info(exporturl)
                tsk = None

                if svn_export_info:
                    tsk     = create_task("svnexport", [], [export_node])
                    tsk.url = svn_export_info['URL']
                else:
                    exporturl = "http://subversion01:8080/svn/Fieldbus" + svn_info['URL'][len(svn_info['Repository Root']):] \
                       + "/Tasks"
                    svn_export_info = self.bld.svn_get_info(exporturl)

                    if svn_export_info:
                        export_node = root_node.make_node("Tasks")
                        tsk     = create_task("svnexport", [], [export_node])
                        tsk.url = svn_export_info['URL']

                        exporturl = "http://subversion01:8080/svn/Fieldbus" + svn_info['URL'][len(svn_info['Repository Root']):] \
                           + "/Targets"

                        svn_export_info = self.bld.svn_get_info(exporturl)

                        export_node = root_node.make_node("Targets")
                        tsk     = create_task("svnexport", [], [export_node])
                        tsk.url = svn_export_info['URL']

                if not tsk:
                    self.bld.fatal("Failed to detect export url properly")

            else:
                bld.fatal("Don't know how to handle sources of lib external {tgen.name}".format(**locals()))
        else:
            if tgen.path.parent.name == "Targets":
                source_node = root_node.make_node("Targets")
            else:
                source_node = root_node.make_node("Components")
            tgen_use = self.to_list(getattr(tgen,"use", []))

            source       = self.to_list(getattr(tgen,"source", []))
            source_arm   = self.to_list(getattr(tgen,"source_arm", []))
            source_thumb = self.to_list(getattr(tgen,"source_thumb", []))

            name = tgen.name
            if (name + "_arm_objs") in tgen_use:
                source_arm.extend(self.to_list(
                    self.bld.get_tgen_by_name(name + "_arm_objs").source))

            if (name + "_thumb_objs") in tgen_use:
                source_thumb.extend(self.to_list(
                    self.bld.get_tgen_by_name(name + "_thumb_objs").source))

            tgensource_node = source_node.make_node(tgen.path.name)
            for y in (source + source_thumb + source_arm):
                if isinstance(y,str):
                    y = tgen.path.find_or_declare(y)

                if y not in to_copy:
                    relpath = y.path_from(tgen.path)
                    dst_node = tgensource_node.make_node(relpath)
                    to_copy[y] = dst_node

            includes = self.to_list(getattr(tgen,"includes", []))

            for x in includes:
                if isinstance(x,str):
                  x = tgen.path.find_node(x)

                if x.is_bld():
                    continue

                for y in x.ant_glob("**/*"):
                    if y not in to_copy:
                        relpath = y.path_from(tgen.path)
                        dst_node = tgensource_node.make_node(relpath)
                        to_copy[y] = dst_node

            wscript_src_node = tgen.path.find_node("wscript")
            wscript_dst_node = tgensource_node.make_node("wscript")

            if wscript_src_node not in to_copy:
                to_copy[wscript_src_node] = wscript_dst_node
    if to_copy:
        wscript_src_node = self.path.find_node("wscript")
        wscript_dst_node = root_node.make_node("wscript")

        if wscript_src_node not in to_copy:
            to_copy[wscript_src_node] = wscript_dst_node


        create_task("copy", to_copy.keys(), to_copy.values())
