import runtest

from SCons             import *
from SCons.Script      import *
from SCons.Environment import *

def xpic_execute_runtest(target, source, env):
  file_names = []
  for file in source:
    file_names.append(str(file))
  
  report_name = str(target[0])

  return runtest.main(file_names, logfile=report_name, configs=env["RUNTESTCONFIGS"])

bld = Builder(action = xpic_execute_runtest,
              suffix = '.log',
              src_suffix = '.elf')


xpic_env = Environment(BUILDERS = {'Check' : bld})
  

def xpic_runtest(logfile, files, configs={}):
  tempenv = xpic_env.Clone()
  tempenv["RUNTESTCONFIGS"] = configs
  return tempenv.Check(logfile, files)