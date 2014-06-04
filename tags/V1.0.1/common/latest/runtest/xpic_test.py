import runtest

from SCons             import *
from SCons.Script      import *
from SCons.Environment import *
g_gdb_connection = 'usb'
g_netx_chip      = 'netx10'

def xpic_execute_runtest(target, source, env):
  global g_gdb_connection
  global g_netx_chip
  file_names = []
  for file in source:
    file_names.append(str(file))
  report_name = str(target[0])

  return runtest.main(file_names, logfile=report_name, configs=env["RUNTESTCONFIGS"], gdb_connection = g_gdb_connection, netx = g_netx_chip)

bld = Builder(action = xpic_execute_runtest,
              suffix = '.log',
              src_suffix = '.elf')


xpic_env = Environment(BUILDERS = {'Check' : bld})
  

def xpic_runtest(logfile, files, configs={}, gdb_connection='usb', netx = 'netx10'):
  global g_gdb_connection
  global g_netx_chip
  g_gdb_connection = gdb_connection
  g_netx_chip      = netx
  tempenv = xpic_env.Clone()
  tempenv["RUNTESTCONFIGS"] = configs
  return tempenv.Check(logfile, files)