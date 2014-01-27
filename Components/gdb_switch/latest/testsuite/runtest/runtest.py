import sys, os, getpass
import re
import socket
import copy
import serial
import signal


from pexpect import TIMEOUT
from pexpect import EOF

from time import gmtime, strftime, sleep

# ----------------------------------------------------------
child_openocd = None
child_gdbswitch = None
child_xpicgdb = None

printlevel = 0
loglevel = 0
log = None # This object will be created in the main() function
testcnt = 0
failures = 0
noncriticalerrorcounter = 0
MAX_NON_CRITICAL_ERRORS = 3
NORMAL_ERROR  = -1
CRITICAL_ERROR = -2
NON_CRITICAL_ERROR = -3

if sys.platform == "win32":
  from winpexpect import winspawn
  from winpexpect import ExceptionPexpect
else:
  from pexpect import spawn
  from pexpect import ExceptionPexpect

class TestLog:
  def write(self, str):
    self.f.write(str)
  def writeln(self, str):
    self.f.write(str+'\n')
  def __init__(self, filename):
    self.f = open(filename,'w')
    self.writeln("Test Log")
    self.writeln(strftime("Date and Time: %Y-%m-%d %H:%M:%S", gmtime()))
    self.writeln("User ID: " + getpass.getuser())
    self.writeln("Hostname: "+ socket.gethostname())
    self.writeln("Directory: " + os.getcwd())
    self.writeln("Script: " + __file__)
  def close(self):
    self.f.close();
    
def vprint(str, level=5):
  if printlevel >= level:
    print str
  if loglevel >= level:
    log.writeln(str)
    
def eprint(str):
  print "ERROR: " + str
  log.writeln("ERROR: " + str)

    
def _spawn(command):
  spawned = None
  if sys.platform == "win32":
    spawned = winspawn(command)
  else:
    spawned = spawn(command)
  return spawned

prefix = os.environ.get("XPIC_TOOLS", "")
if prefix != "":
  prefix = prefix + os.path.sep + "bin" + os.path.sep
default_config = {};
default_config["run_cmd"] =  prefix+"xpic-gdb"
default_config["exit_code"] = 0
default_config["exit_timeout"] = 5

def nonCriticalErrorCounter():
  global noncriticalerrorcounter
  global CRITICAL_ERROR
  global NON_CRITICAL_ERROR
  
  noncriticalerrorcounter = noncriticalerrorcounter + 1
  if noncriticalerrorcounter >= MAX_NON_CRITICAL_ERRORS:
    eprint("max non critical errors reached! quitting program.")
    return CRITICAL_ERROR
  else:
    return NON_CRITICAL_ERROR


def parseConfig(filename):
  
  config = {}
  
  file = open(filename,'r')
  lines = file.readlines()
    
  
  for line in lines:
    matches = re.findall("RUNTEST!\s*(\w*)\s*=\s*(\S+)", line)
    if not matches:
      return config
    for match in matches:
      config[match[0]] = match[1]  
  
  return config

###################################################################################
# Execute gdb Test 
###################################################################################

def executeGdbTest(file, config):
  global CRITICAL_ERROR
  global NON_CRITICAL_ERROR
  global NORMAL_ERROR
  global child_xpicgdb
  
  run_cmd = config["run_cmd"] + " -q -nw " + file
  vprint("Launching run command: " + run_cmd)
  child_xpicgdb = _spawn (run_cmd)
    
  match = child_xpicgdb.expect(["\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run gdb: " + child_xpicgdb.before)
    child_xpicgdb.terminate()
    return CRITICAL_ERROR
    
  # Connect
  vprint("Connecting with hardware")
  child_xpicgdb.sendline ('target remote :3335')
  match = child_xpicgdb.expect(["Remote debugging using :\\d.*\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to connect with target board: " + child_xpicgdb.before)
    child_xpicgdb.terminate()
    return CRITICAL_ERROR
    
  # Load
  vprint("Loading program")
  child_xpicgdb.sendline ("load")
  match = child_xpicgdb.expect(["\\(gdb\\)", TIMEOUT], timeout=15)
  if match != 0:
    eprint("Unable to load program file: " + child_xpicgdb.before)
    child_xpicgdb.terminate()
    return nonCriticalErrorCounter()
  
  # Set breakpoint
  vprint("Setting breakpoint")
  child_xpicgdb.sendline('break _exit')
  match = child_xpicgdb.expect(["Breakpoint 1 at.*\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to set breakpoint: " + child_xpicgdb.before)
    child_xpicgdb.terminate()
    return nonCriticalErrorCounter()
  
  # Open serial port for stdin/stdout of the program under test
  if "check_stdout" in config.keys():
    ser = serial.Serial()
    ser.baudrate = 115200
    ser.port = "COM21"
    ser.timeout=1 # After hitting the breakpoint we wait another second for the output stream
    ser.open()
  
  # Start
  vprint("Starting program")
  child_xpicgdb.sendline ('c')
  match = child_xpicgdb.expect(["Continuing.*", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run program on target: " + child_xpicgdb.before) 
    child_xpicgdb.terminate()
    return nonCriticalErrorCounter()
    
   
  # Wait for breakpoint hit
  match = child_xpicgdb.expect(["Breakpoint.*", TIMEOUT], timeout=config["exit_timeout"])
  if match != 0:
    eprint("No breakpoint reached!")
    child_xpicgdb.terminate()
    # child_gdbswitch.expect([".*", TIMEOUT], timeout=5)
    # vprint("g: " + str(child_gdbswitch.after))
    # vprint("o: " + str(child_openocd.before))
    return nonCriticalErrorCounter()
  
  # Check stdout
  if "check_stdout" in config.keys():
    vprint("Check: stdout", 3)
    out = ser.read(1024)
    checkOutput = config["check_stdout"]
    retcode, msg = checkOutput(out)
    if retcode != 0:
      eprint("Check of stdout failed: " + msg)
      child_xpicgdb.terminate()
      return retcode
    else:
      vprint(msg, 3)
  
  # Check exit code
  if "exit_code" in config.keys():
    vprint("Check: exit_code " + str(config["exit_code"]), 3)
    child_xpicgdb.sendline ('info registers r2')
    match = child_xpicgdb.expect(["r2.*\\(gdb\\)", TIMEOUT], timeout=5)
    if match != 0:
      eprint("Unable to receive exit code")
      child_xpicgdb.terminate()
      return nonCriticalErrorCounter()

  # Get exit code
  r2_string = child_xpicgdb.after
  programExitValueStr = re.match(".*0x([0-9a-f]+)\s", r2_string).group(1)
  programExitValue = int(programExitValueStr, 16)
  vprint("Exit code: 0x" + programExitValueStr)
  if programExitValue != int(config["exit_code"]):
    eprint("Expected program exit code " + str(config["exit_code"]) + ", found: " + programExitValueStr)
    child_xpicgdb.terminate()
    return NORMAL_ERROR
  
  # Close gdb 
  child_xpicgdb.terminate()
  # Win7 may not terminate a process immediateley
  sleep(1)
  return 0

###################################################################################
# Run gdb tests
###################################################################################

def runGdbTest(file, config):
  return_code = executeGdbTest(file, config)
  return return_code

def runTests(files, configs):
  global CRITICAL_ERROR
  global testcnt
  global failures
  failcnt = 0
  
  common_config = default_config.copy()
  common_config.update(configs["0"])
  
  vprint("runtest configuration: " + str(common_config))
  
  for file in files:
    testcnt += 1    
    vprint("\n<Test %d>" % (testcnt), 2)
    vprint("File: " + file, 2)
    
    config = common_config.copy()
    if file in configs.keys():
      config.update(configs[file])
      vprint("Override configuration " + str(configs[file]))
    
    retcode = runGdbTest(file, config)
    
    if retcode == CRITICAL_ERROR:
        failures += failcnt + 1
        return failcnt
    
    if retcode != 0:
      vprint("Test result: ERROR", 2)
      failcnt += 1
    else:  
      vprint("Test result: SUCCESS", 2)
  
  failures += failcnt
  return failcnt

###################################################################################
# Execute Open OCD 
###################################################################################
def executeOpenOcd():
  global child_openocd
  
  run_cmd = '"openocd\\openocd.exe" -f openocd\\nxhx10_etm.cfg'
  vprint("Launching run command: " + run_cmd)
  child_openocd = _spawn (run_cmd)
    
  match = child_openocd.expect(["dcc downloads are enabled", TIMEOUT], timeout=15)
  if match != 0:
    eprint("Unable to run openocd: " + child_openocd.before)
    child_openocd.terminate()
    return CRITICAL_ERROR
  return 0

###################################################################################
# Execute gdb-switch
###################################################################################

def executeGdbSwitch():
  global child_gdbswitch
  
  run_cmd = '"..\\bin\\netx10_gdb_switch.exe"'
  vprint("Launching run command: " + run_cmd)
  child_gdbswitch = _spawn (run_cmd)
    
  # match = child_openocd.expect(["Info : accepting 'gdb' connection from 0", TIMEOUT], timeout=15)
  # if match != 0:
    # eprint("Unable to run gdb-switch: " + child_openocd.before)
    # child_gdbswitch.terminate()
    # child_openocd.terminate()
    # return CRITICAL_ERROR
  
  return 0

###################################################################################
# internal main function
###################################################################################

def main(files, logfile="test.log", verbosity=5, logverbosity=4, configs={}):
  print("\n-------- runtest.main --------")
  
  global noncriticalerrorcounter
  noncriticalerrorcounter = 0
  global MAX_NON_CRITICAL_ERRORS
  MAX_NON_CRITICAL_ERRORS = 3
  global NORMAL_ERROR
  NORMAL_ERROR  = -1
  global CRITICAL_ERROR
  CRITICAL_ERROR = -2
  global NON_CRITICAL_ERROR
  NON_CRITICAL_ERROR = -3
  
  global log
  log = TestLog(logfile)
  global testcnt
  testcnt = 0
  global failures
  failures = 0
  global printlevel
  printlevel = verbosity
  global loglevel
  loglevel = logverbosity
  
  if "0" not in configs.keys():
    configs["0"] = {}
  
  try:
    # starting openocd:
    result = executeOpenOcd()    
    if result != 0:
      eprint( "could not start OpenOCD!" )
      return -1
    
    # waiting for openocd be ready
    sleep(4)
    
    # starting gdb-switch:
    result = executeGdbSwitch()    
    if result != 0:
      eprint( "could not start gdb-switch!" )
      child_openocd.terminate()
      return -1
    
    # waiting for gdb-switch be ready
    sleep(4)
    
    # starting tests
    result = runTests(files, configs)
      
    vprint("\n<Summary>", 2)
    vprint("Executed tests: %d" % (testcnt), 1)
    vprint("Passed tests:   %d" % (testcnt - failures), 1)
    vprint("Failed tests:   %d" % (failures), 1)
    
  except Exception as ex:
    eprint(str(sys.exc_info()[0]) + ": " + str(ex))
    result = -1
    child_xpicgdb.terminate()
    child_openocd.terminate()
    child_gdbswitch.terminate()
  
  child_gdbswitch.terminate()
  child_openocd.terminate()
  
  
  if result == 0:
    vprint("Overall result: PASSED", 1)
  else:
    vprint("Overall result: FAILED", 1)
    
  log.close()
  return result

 
###################################################################################
# main function
###################################################################################

if __name__ == '__main__':
  return_code = main(sys.argv[1:])
  print return_code
  

    
