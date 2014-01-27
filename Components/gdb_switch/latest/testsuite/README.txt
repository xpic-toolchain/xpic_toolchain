------------
start tests
------------
scons check

HINT: if the board in use is one of the early netx10 boards change the cfg file for openocd
-> nxhx10_etm.cfg
comment ft2232_device_desc "NXHX 10-ETM A"
uncomment #ft2232_device_desc "Dual RS232"

-----------
Errors
-----------

If you getting some error like this, it is highly possible
that openocd is already running. So close all openocd instances first!

-------- runtest.main --------
Launching run command: "openocd\openocd.exe" -f openocd\nxhx10_etm.cfg
ERROR: <class 'pexpect.EOF'>: End of file in read_nonblocking().
<winpexpect.winspawn object at 0x03A2AD90>
version: 2.3 ($Revision: 399 $)
command: openocd\openocd.exe
args: ['openocd\\openocd.exe', '-f', 'openocd\\nxhx10_etm.cfg']
searcher: searcher_re:
    0: re.compile("dcc downloads are enabled")
    1: TIMEOUT
buffer (last 100 chars):
before (last 100 chars): Error: ListDevices: 2

Error: 0: "Dual RS232 B"
Error: 1: "+"
Command handler execution failed

after: <class 'pexpect.EOF'>
match: None
match_index: None
exitstatus: None
flag_eof: False
pid: 4084
child_fd: 4
closed: False
timeout: 30
delimiter: <class 'pexpect.EOF'>
logfile: None
logfile_read: None
logfile_send: None
maxread: 2000
ignorecase: False
searchwindowsize: None
delaybeforesend: 0.05
delayafterclose: 0.1
delayafterterminate: 0.1
scons: *** [2011-10-06_11-53-31_test.log] AttributeError : 'NoneType' object has
 no attribute 'terminate'
Traceback (most recent call last):
  File "C:\Python26\Scripts\..\Lib\site-packages\scons-2.0.1\SCons\Action.py", l
ine 1046, in execute
    result = self.execfunction(target=target, source=rsources, env=env)
  File "runtest\xpic_test.py", line 14, in xpic_execute_runtest
    return runtest.main(file_names, logfile=report_name, configs=env["RUNTESTCON
FIGS"])
  File "runtest\runtest.py", line 368, in main
    child_xpicgdb.terminate()
AttributeError: 'NoneType' object has no attribute 'terminate'
scons: building terminated because of errors.
Exception TypeError: TypeError("'NoneType' object is not callable",) in <bound m
ethod winspawn.__del__ of <winpexpect.winspawn object at 0x03A2AD90>> ignored