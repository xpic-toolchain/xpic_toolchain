import sys, os, getpass
import re
import socket
import copy
import serial


from pexpect import TIMEOUT
from pexpect import EOF

from time import gmtime, strftime, sleep

# ----------------------------------------------------------
g_gdb_connection = 'usb'
g_netx_chip      = 'netx10'
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

    
def _spawn(command, logfile=None):
  spawned = None
  if sys.platform == "win32":
    spawned = winspawn(command,logfile=logfile)
  else:
    spawned = spawn(command,logfile=logfile)
  return spawned

prefix = os.environ.get("XPIC_TOOLS", "")
if prefix == "":
    prefix = os.environ.get("PATH_GNU_XPIC", "")

if prefix != "":
  prefix = prefix + os.path.sep + "bin" + os.path.sep
        
default_config = {};
default_config["run_cmd"] =  '"' + prefix + 'xpic-gdb"'
default_config["exit_code"] = 0
default_config["exit_timeout"] = 5
default_config["prefix"] = prefix

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

# ----------------------------------------------------------
###################################################################################
# Execute Open OCD 
###################################################################################
def executeOpenOcd(config):
  global child_openocd
  global g_netx_chip
  
  if g_netx_chip == 'netx51':
      # openocd     = 'openocd_new.exe'
      openocd     = '..\\..\\openocd\\openocd.exe'
      config_search = '..\\..\\netx_openocd_scripts'
      config_file_if = '..\\..\\netx_openocd_scripts\\interface\\hilscher_nxhx51_etm.cfg'
      config_file_board = '..\\..\\netx_openocd_scripts\\board\\hilscher_nxhx51.cfg'
      #  -s C:\Daten\netx_openocd_scripts.git -f c:\Daten\netx_openocd_scripts.git\interface\hilscher_nxhx51_etm.cfg -f c:\Daten\netx_openocd_scripts.git\board\hilscher_nxhx51.cfg
      run_cmd = '"' + config["prefix"] + openocd + '" -s "' + config["prefix"] + config_search + '"' + ' -f' + ' "' + config["prefix"] + config_file_if + '" -f "' + config["prefix"] + config_file_board + '"' + ' "-c puts oocd-started"' 
      print("Launching run command: " + run_cmd)
      child_openocd = _spawn (run_cmd)
        
      #match = child_openocd.expect(["target halted in ARM state due to debug-request", TIMEOUT], timeout=15)
      match = child_openocd.expect(["oocd-started", TIMEOUT], timeout=15)	  
      
      if match != 0:
        eprint("Unable to run openocd: " + child_openocd.before)
        child_openocd.terminate()
        return CRITICAL_ERROR
    
  elif g_netx_chip == 'netx51_usb2jtag':
      openocd     = 'openocd_sven.exe'
      config_file = 'netx56.cfg'
      
      run_cmd = '"' + config["prefix"] + openocd + '" -f' + '"' + config["prefix"] + '\\' + config_file + '"'
      print("Launching run command: " + run_cmd)
      child_openocd = _spawn (run_cmd)
        
      match = child_openocd.expect(["target0: hardware has 2 breakpoint", TIMEOUT], timeout=15)
      if match != 0:
        eprint("Unable to run openocd: " + child_openocd.before)
        child_openocd.terminate()
        return CRITICAL_ERROR    
    
  else:
      openocd     = 'openocd.exe'
      config_file = 'nxhx10_etm.cfg'
      
      run_cmd = '"' + config["prefix"] + openocd + '" -f' + '"' + config["prefix"] + '\\' + config_file + '"'
      print("Launching run command: " + run_cmd)
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
def executeGdbSwitch(config):
  global child_gdbswitch
  global g_netx_chip

  # C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201602011609\gdb_switch\gdb_switch_v2.exe
  gdbswitch = '..\\..\\gdb_switch\\gdb_switch_v2.exe'
  if g_netx_chip == 'netx51' or g_netx_chip == 'netx51_usb2jtag':
        netx      = ' -c netX51'
  else:
        netx      = ' -c netX10'
      
  run_cmd = '"' + config["prefix"] + gdbswitch + '"' + netx
  print("Launching run command: " + run_cmd)
  child_gdbswitch = _spawn (run_cmd)
  return 0
###################################################################################
# Execute gdb reset (uart) 
###################################################################################   
def executeGdbReset(config):
  global CRITICAL_ERROR
  
  # open gdb
  vprint("Resetting target")
  child = _spawn(config["run_cmd"] + " -q -nw -b 115200")   
  match = child.expect(["\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run gdb: " + child.before)
    child.terminate()
    return CRITICAL_ERROR
  
  # reset server
  child.sendline ('target remote com1')
  match = child.expect(["Remote debugging using com\\d.*\\(gdb\\)", TIMEOUT], timeout=5)
  
  if match != 0:
    eprint("Unable to connect with target board: " + child.before)
    child.terminate()
    return CRITICAL_ERROR
  child.sendline ('quit')
  match = child.expect(['\[answered Y; input not from terminal\]', TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unexpected gdb output: " + child.before)
  child.terminate()
  sleep(0.5)
  return match


###################################################################################
# Execute gdb Test 
###################################################################################

def executeGdbTest(file, config):
  global CRITICAL_ERROR
  global NON_CRITICAL_ERROR
  global NORMAL_ERROR
  
  run_cmd = config["run_cmd"] + " -q -nw -b 115200 " + file
  vprint("Launching run command: " + run_cmd)
  child = _spawn (run_cmd)
    
  match = child.expect(["\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run gdb: " + child.before)
    child.terminate()
    return CRITICAL_ERROR
    
  # Connect
  vprint("Connecting with hardware")
  child.sendline ('target remote com1')
  match = child.expect(["Remote debugging using com\\d.*\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to connect with target board: " + child.before)
    child.terminate()
    return CRITICAL_ERROR

  # Load
  vprint("Loading program")
  child.sendline ("load")
  match = child.expect(["\\(gdb\\)", TIMEOUT], timeout=15)
  if match != 0:
    eprint("Unable to load program file: " + child.before)
    child.terminate()
    return nonCriticalErrorCounter()
  
  # Set breakpoint
  vprint("Setting breakpoint")
  child.sendline('break _exit')
  match = child.expect(["Breakpoint 1 at.*\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to set breakpoint: " + child.before)
    child.terminate()
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
  child.sendline ('c')
  match = child.expect(["Continuing.*", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run program on target: " + child.before) 
    child.terminate()
    return nonCriticalErrorCounter()
    
   
  # Wait for breakpoint hit
  match = child.expect(["Breakpoint.*", TIMEOUT], timeout=config["exit_timeout"])
  if match != 0:
    eprint("No breakpoint reached!")
    child.terminate()
    return nonCriticalErrorCounter()
  
  # Check stdout
  if "check_stdout" in config.keys():
    vprint("Check: stdout", 3)
    out = ser.read(1024)
    checkOutput = config["check_stdout"]
    retcode, msg = checkOutput(out)
    if retcode != 0:
      eprint("Check of stdout failed: " + msg)
      child.terminate()
      return retcode
    else:
      vprint(msg, 3)
  
  # Check exit code
  if "exit_code" in config.keys():
    vprint("Check: exit_code " + str(config["exit_code"]), 3)
    child.sendline ('info registers r2')
    match = child.expect(["r2.*\\(gdb\\)", TIMEOUT], timeout=5)
    if match != 0:
      eprint("Unable to receive exit code")
      child.terminate()
      return nonCriticalErrorCounter()

  # Get exit code
  r2_string = child.after
  programExitValueStr = re.match(".*0x([0-9a-f]+)\s", r2_string).group(1)
  programExitValue = int(programExitValueStr, 16)
  vprint("Exit code: 0x" + programExitValueStr)
  if programExitValue != int(config["exit_code"]):
    eprint("Expected program exit code " + str(config["exit_code"]) + ", found: " + programExitValueStr)
    child.terminate()
    return NORMAL_ERROR
  
  # # Close gdb 
  # child.sendline('q')
  # match = child.expect(['\[answered Y; input not from terminal\]', TIMEOUT], timeout=5)
  # if match != 0:
    # eprint("Unexpected gdb output: " + child.before)
    # child.terminate()
    # return nonCriticalErrorCounter()
  
  # Close gdb 
  child.terminate()
  # Win7 may not terminate a process immediateley
  sleep(1)
  return 0

###################################################################################
# Run gdb tests (usb)
###################################################################################
def sendGDBcmd(gdb, Commands):
    for cmd in Commands:
      gdb.sendline (cmd)
      match = gdb.expect(["\\(gdb\\)", TIMEOUT], timeout=15)
      if match != 0:
        eprint("sendGDBcmd error")
        return 1
    return 0

def runGdbTestUsb(file, config):
  global CRITICAL_ERROR
  global NON_CRITICAL_ERROR
  global NORMAL_ERROR
  
  run_cmd = config["run_cmd"] + " -q -nw " + file
  vprint("Launching run command: " + run_cmd)
  child_gdb = _spawn (run_cmd)
    
  match = child_gdb.expect(["\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run gdb: " + child_gdb.before)
    child_gdb.terminate()
    return CRITICAL_ERROR
    
  # Connect
  vprint("Connecting with hardware")
  child_gdb.sendline ('target remote :3335')
  match = child_gdb.expect(["Remote debugging using :\\d.*\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to connect with target board: " + child_gdb.before)
    child_gdb.terminate()
    return CRITICAL_ERROR

  # Init SDRAM
  Commands =["x/1w 0x101c0070",
             "set {int}0x101c0070 = $__",
             "set {int}0x101C0C40 = 0x00000050",
             "set {int}0x101C0140 = 0",
             "set {int}0x101C0144 = 0x00a13262",
             "set {int}0x101C0148 = 0x00000033",
             "set {int}0x101C0140 = 0x030d0121"           
            ]
  if 0 != sendGDBcmd(child_gdb, Commands): 
    eprint("Unable to init sdram")
    child_gdb.terminate()
    return nonCriticalErrorCounter()
  
  # Load
  vprint("Loading program")
  child_gdb.sendline ("load")
  match = child_gdb.expect(["\\(gdb\\)", TIMEOUT], timeout=15)
  if match != 0:
    eprint("Unable to load program file: " + child_gdb.before)
    child_gdb.terminate()
    return nonCriticalErrorCounter()
  
  # Set breakpoint
  vprint("Setting breakpoint")
  child_gdb.sendline('break _exit')
  match = child_gdb.expect(["Breakpoint 1 at.*\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to set breakpoint: " + child_gdb.before)
    child_gdb.terminate()
    return nonCriticalErrorCounter()
  
  # Open serial port for stdin/stdout of the program under test
  if "check_stdout" in config.keys():
    ser = serial.Serial()
    ser.baudrate = 115200
    ser.port = "COM1"
    ser.timeout=1 # After hitting the breakpoint we wait another second for the output stream
    ser.open()
  
  # Start
  vprint("Starting program")
  child_gdb.sendline ('c')
  match = child_gdb.expect(["Continuing.*", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run program on target: " + child_gdb.before) 
    child_gdb.terminate()
    return nonCriticalErrorCounter()
    
   
  # Wait for breakpoint hit
  match = child_gdb.expect(["Breakpoint.*", TIMEOUT], timeout=config["exit_timeout"])
  if match != 0:
    eprint("No breakpoint reached!")
    child_gdb.terminate()
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
      child_gdb.terminate()
      return retcode
    else:
      vprint(msg, 3)
  
  # Check exit code
  if "exit_code" in config.keys():
    vprint("Check: exit_code " + str(config["exit_code"]), 3)
    child_gdb.sendline ('info registers r2')
    match = child_gdb.expect(["r2.*\\(gdb\\)", TIMEOUT], timeout=5)
    if match != 0:
      eprint("Unable to receive exit code")
      child_xpicgdb.terminate()
      return nonCriticalErrorCounter()

  # Get exit code
  r2_string = child_gdb.after
  programExitValueStr = re.match(".*0x([0-9a-f]+)\s", r2_string).group(1)
  programExitValue = int(programExitValueStr, 16)
  vprint("Exit code: 0x" + programExitValueStr)
  if programExitValue != int(config["exit_code"]):
    eprint("Expected program exit code " + str(config["exit_code"]) + ", found: " + programExitValueStr)
    child_gdb.terminate()
    return NORMAL_ERROR
  
  # Close gdb 
  child_gdb.terminate()
  # Win7 may not terminate a process immediateley
  sleep(0.5)
  return 0

def runGdbTestUsb_StartGDB(config):
  global g_netx_chip
  add_opt = ''  
  if g_netx_chip == 'netx51' or g_netx_chip == 'netx51_usb2jtag':
      add_opt = '--xpic-int-dram-addr 0x10280000'
      
      SDRAM_Commands =["set {int}0x101C0140 = 0",
                       "set {int}0x101C0148 = 0x00000023",
                       "set {int}0x101C0144 = 0x00A22151",
                       "set {int}0x101C0140 = 0x030D0001",
                      ]      
      
  else:
      SDRAM_Commands =["x/1w 0x101c0070",
                       "set {int}0x101c0070 = $__",
                       "set {int}0x101C0C40 = 0x00000050",
                       "set {int}0x101C0140 = 0",
                       "set {int}0x101C0144 = 0x00a13262",
                       "set {int}0x101C0148 = 0x00000033",
                       "set {int}0x101C0140 = 0x030d0121"           
                      ]          
  run_cmd = config["run_cmd"] + " -q -nw " + add_opt # + file
  vprint("Launching run command: " + run_cmd)
  #file = open("C:\\Daten\\gdb.log","w")
  #child_gdb = _spawn (run_cmd, file) # to create a logfile with output/input for xpic-GDB
  child_gdb = _spawn (run_cmd)
  
  match = child_gdb.expect(["\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run gdb: " + child_gdb.before)
    child_gdb.terminate()
    return None
    
  # Connect
  vprint("Connecting with hardware")
  child_gdb.sendline ('target remote :3335')
  match = child_gdb.expect(["Remote debugging using :\\d.*\\(gdb\\)", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to connect with target board: " + child_gdb.before)
    child_gdb.terminate()
    return None
  
  # Init SDRAM
  if 0 != sendGDBcmd(child_gdb, SDRAM_Commands): 
    eprint("Unable to init sdram")
    child_gdb.terminate()
    return None

  return child_gdb

def runGdbTestUsb_StopGDB(gdb):
  # Close gdb 
  gdb.terminate()
  # Win7 may not terminate a process immediateley
  sleep(0.5)
  
exit_bkpt_not_set=True  
def runGdbTestUsbV2(file=None, gdb=None, config=None):
  global CRITICAL_ERROR
  global NON_CRITICAL_ERROR
  global NORMAL_ERROR
  global exit_bkpt_not_set
  
  if gdb==None:
      eprint("GDB not started!!! Run gbd in test first!")
      return CRITICAL_ERROR;

  child_gdb = gdb
        
  # File
  import string  
  _file = string.replace(file,"\\","/")
  
  # reset mxpic
  #child_gdb.sendline ("mon reset")
  #sleep(0.5)
  
  vprint("Loading file: " + _file)
  child_gdb.sendline ("file " + _file)
  match = child_gdb.expect(["\\(gdb\\)", TIMEOUT], timeout=15)
  if match != 0:
    eprint("Unable to load program file: " + child_gdb.before)
    return nonCriticalErrorCounter()

  if 0 != sendGDBcmd(child_gdb, ['mon reset']): 
    eprint("Unable to reset xpic!")

  # Load
  vprint("Loading program")
  child_gdb.sendline ("load")
  match = child_gdb.expect(["\\(gdb\\)", TIMEOUT], timeout=60)
  if match != 0:
    eprint("Unable to load program file: " + child_gdb.before)
    return nonCriticalErrorCounter()

  if exit_bkpt_not_set:
      # Set breakpoint
      vprint("Setting breakpoint")
      child_gdb.sendline('break _exit')
      match = child_gdb.expect(["Breakpoint 1 at.*\\(gdb\\)", TIMEOUT], timeout=5)
      if match != 0:
        eprint("Unable to set breakpoint: " + child_gdb.before)
        return nonCriticalErrorCounter()
      exit_bkpt_not_set = False
  
  # Open serial port for stdin/stdout of the program under test
  if "check_stdout" in config.keys():
    ser = serial.Serial()
    ser.baudrate = 115200
    ser.port = "COM1"
    ser.timeout=1 # After hitting the breakpoint we wait another second for the output stream
    ser.open()
  
  # Start
  vprint("Starting program")
  child_gdb.sendline ('c')
  match = child_gdb.expect(["Continuing.*", TIMEOUT], timeout=5)
  if match != 0:
    eprint("Unable to run program on target: " + child_gdb.before) 
    return nonCriticalErrorCounter()
    
   
  # Wait for breakpoint hit
  match = child_gdb.expect(["Breakpoint.*", TIMEOUT], timeout=config["exit_timeout"])
  if match != 0:
    eprint("No breakpoint reached!")
    #child_gdb.sendcontrol('c')
    #eprint("sendcontrol")
    #sleep(3)
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
      return retcode
    else:
      vprint(msg, 3)
  
  # Check exit code
  if "exit_code" in config.keys():
    vprint("Check: exit_code " + str(config["exit_code"]), 3)
    child_gdb.sendline ('info registers r2')
    match = child_gdb.expect(["r2.*\\(gdb\\)", TIMEOUT], timeout=5)
    if match != 0:
      eprint("Unable to receive exit code")
      return nonCriticalErrorCounter()

  # Get exit code
  r2_string = child_gdb.after
  programExitValueStr = re.match(".*0x([0-9a-f]+)\s", r2_string).group(1)
  programExitValue = int(programExitValueStr, 16)
  vprint("Exit code: 0x" + programExitValueStr)
  if programExitValue != int(config["exit_code"]):
    eprint("Expected program exit code " + str(config["exit_code"]) + ", found: " + programExitValueStr)
    return NORMAL_ERROR
  
  return 0
###################################################################################
# Run gdb tests
###################################################################################

def runGdbTest(file, config):
  retcode = executeGdbReset(config)
  if retcode != 0:
    return retcode
  return_code = executeGdbTest(file, config)
  return return_code

def runTests(files, configs):
  global CRITICAL_ERROR
  global testcnt
  global failures
  global child_openocd
  global child_gdbswitch
  global g_gdb_connection
  failcnt = 0
  
  common_config = default_config.copy()
  common_config.update(configs["0"])
  
  vprint("runtest configuration: " + str(common_config))
    
  # gdb_connection='usb': run usb-tools
  if g_gdb_connection == 'usb':    
     # starting openocd:
     
     result = executeOpenOcd(common_config)    
     if result != 0:
       eprint( "could not start OpenOCD!" )
       return CRITICAL_ERROR
     
     # waiting for openocd be ready
     sleep(0.5)
     
     # starting gdb-switch:
     result = executeGdbSwitch(common_config)    
     if result != 0:
       eprint( "could not start gdb-switch!" )
       child_openocd.terminate()
       return CRITICAL_ERROR
     
     # waiting for gdb-switch be ready
     sleep(0.5)
  # end of 'usb' 
     
  for file in files:
    testcnt += 1    
    vprint("\n<Test %d>" % (testcnt), 2)
    vprint("File: " + file, 2)
    
    config = common_config.copy()
    if file in configs.keys():
      config.update(configs[file])
      vprint("Override configuration " + str(configs[file]))

    if g_gdb_connection != 'usb':
      retcode = runGdbTest(file, config)
    else:
      retcode = runGdbTestUsb(file, config)
    
    if retcode == CRITICAL_ERROR:
        if g_gdb_connection == 'usb':     
            child_openocd.terminate()
            child_gdbswitch.terminate()
            sleep(1)
                    
        failures += failcnt + 1
        if failcnt == 0:
            return 1 
        return failcnt

    if retcode != 0:
      sleep(1)
      vprint("Test result: ERROR", 2)
      failcnt += 1
    else:  
      vprint("Test result: SUCCESS", 2)
  
  failures += failcnt
  
  # gdb_connection='usb': close gdb and usb-tools
  
  if g_gdb_connection == 'usb':     
    child_openocd.terminate()
    child_gdbswitch.terminate()
    # Win7 may not terminate a process immediately
    sleep(1)
  
  # end of 'usb' 
       
  return failcnt

def runTestsV2(files, configs):
  global CRITICAL_ERROR
  global testcnt
  global failures
  global child_openocd
  global child_gdbswitch
  global g_gdb_connection
  global exit_bkpt_not_set
  failcnt = 0
  
  common_config = default_config.copy()
  common_config.update(configs["0"])
  
  vprint("runtest configuration: " + str(common_config))
    
  # gdb_connection='usb': run usb-tools
  if g_gdb_connection != 'usb':
     eprint( "test runs just only with USB connection! Use option: gdb_connection=usb" )
     return CRITICAL_ERROR    
     # starting openocd:
  
  result = executeOpenOcd(common_config)    
  if result != 0:
     eprint( "could not start OpenOCD!" )
     return CRITICAL_ERROR
     
  # waiting for openocd be ready
  sleep(0.5)
  # starting gdb-switch:
  result = executeGdbSwitch(common_config)    
  if result != 0:
    eprint( "could not start gdb-switch!" )
    child_openocd.terminate()
    return CRITICAL_ERROR

  # waiting for gdb-switch be ready

  sleep(0.5)

  # start gdb at first
  child_gdb = runGdbTestUsb_StartGDB(common_config)
  if not child_gdb:
      return CRITICAL_ERROR;
  
  exit_bkpt_not_set=True
  num_test = len(files)   
  for file in files:
    testcnt += 1    
    vprint("\n<Test %d/%d>" % (testcnt, num_test), 2)
    vprint("File: " + file, 2)
    
    config = common_config.copy()
    if file in configs.keys():
      config.update(configs[file])
      vprint("Override configuration " + str(configs[file]))
    
    # The TEST!
    retcode = runGdbTestUsbV2(file=file, gdb = child_gdb, config=config)    
    
    if retcode == CRITICAL_ERROR:
        child_openocd.terminate()
        child_gdbswitch.terminate()
                    
        failures += failcnt + 1
        if failcnt == 0:
            return 1 
        return failcnt

    if retcode != 0:
      vprint("Test result: ERROR", 2)
      failcnt += 1
    else:  
      vprint("Test result: SUCCESS", 2)
  
  failures += failcnt
  
  # Close gdb
  runGdbTestUsb_StopGDB(child_gdb)
  
      
  # gdb_connection='usb': close gdb and usb-tools
  
  child_openocd.terminate()
  child_gdbswitch.terminate()
  # Win7 may not terminate a process immediately
  sleep(1)
  
  # end of 'usb' 
       
  return failcnt
  
###################################################################################
# internal main function
###################################################################################

def main(files, logfile="test.log", verbosity=4, logverbosity=4, configs={}, gdb_connection = 'usb', netx = 'netx10'):
  print("\n-------- runtest.main --------")
  global g_gdb_connection
  g_gdb_connection= gdb_connection
  global g_netx_chip
  g_netx_chip      = netx
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
  printlevel = 5
  global loglevel
  loglevel = logverbosity
  
  if "0" not in configs.keys():
    configs["0"] = {}
  
  try:
    if g_gdb_connection != 'usb':
        result = runTests(files, configs) # gdb start/stop
    else:
        result = runTestsV2(files, configs) # gdb starts ones
    vprint("\n<Summary>", 2)
    vprint("Executed tests: %d" % (testcnt), 1)
    vprint("Passed tests:   %d" % (testcnt - failures), 1)
    vprint("Failed tests:   %d" % (failures), 1)
  except Exception as ex:
    eprint(str(sys.exc_info()[0]) + ": " + str(ex))
    result = -1
  
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
