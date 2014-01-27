#!/usr/bin/perl -w

##################################################################
# srec2c.pl
# This script generates an c source file containing a
# loadable xPIC program from an S-Record.
# 
##################################################################


use strict;

# xPIC Segment Definition Version 1.0
my $versionMajor = 1;
my $versionMinor = 0;

##################################################################
# DEFAULT PARAMETERS

# this is the source file that has to be processed
my $strSrc = "./xpic.srec";

# the target file
my $strTrg = "xpic.c";

# debug output switch
my $verbose = 0;

##################################################################
# HANDLE SCRIPT ARGUMENTS

my $arg = shift;

while( $arg ) { if( $arg eq '-help' or $arg eq '-?' ) {
    print("\nusage: srec2c.pl [-s source] [-t target]\n");
    print("\nOptions:\n");
    print("-s  'source' defines the file to translate\n");
    print("-t  'target' defines target file to store the result\n");
    print("-v  'verbose' print debug output\n");
	exit;
  }
  if( $arg eq '-s' ) { $strSrc = shift; }
  elsif( $arg eq '-t' ) { $strTrg = shift; }
  elsif( $arg eq '-v' ) {  $verbose = 1; }
  elsif( substr($arg,0,1) eq '-' ) {
    print("ERROR: Invalid option: $arg!\nUse -? for help\n");
	exit;
  }
  else { print("ERROR: Invalid argument: $arg!\nUse -? for help\n"); exit; }
  
  $arg = shift;
}


dprint("target: $strTrg\n");

my $segment = 0;

open(INFILE, "<", "$strSrc") or die "cannot open file \"$strSrc\": $!\n";
open(CDATA, ">", "$strTrg") or die "cannot create file \"$strTrg\": $!\n";

my @filestats = stat($strSrc);
my $modtime = $filestats[9];
dprintf("Last modification time: %s\n", $modtime);

printf(CDATA "// Timestamp: sec,min,hour,day,month,year-1900,seconds since 1.1.1970\n");
printf(CDATA "const unsigned long g_aulXpicPrgBuildTime[7] = {%d,%d,%d,%d,%d,%d,%d};\n\n", (localtime($modtime))[0,1,2,3,4,5], $modtime);
printf(CDATA "const unsigned long g_aulXpicPrg[] = {\n\t/* HEADER */\n\t0,0,0x43495058,0x%04x%04x,\n", $versionMajor,$versionMinor);

my $line = 0;
while( <INFILE> ) {
  dprint($_);
  $line++;
  
  if( /^S0/ ) {
    dprint("Block header\n");
  }
  elsif( /^S1/ ) {
    dprint("Data record, 2 address bytes\n");
    handleDataRecord($_,2);
  }
  elsif( /^S2/ ) {
    dprint("Data record, 3 address bytes\n");
    handleDataRecord($_,3);
  }
  elsif( /^S3/ ) {
    dprint("Data record, 4 address bytes\n");
    handleDataRecord($_,4);
  }
  elsif( /^S5/ ) {
    dprint("Record count\n");
  }
  elsif( /^S7/ ) {
    dprint("End of block, 4 address bytes\n");
  }
  elsif( /^S8/ ) {
    dprint("End of block, 3 address bytes\n");
  }
  elsif( /^S9/ ) {
    dprint("End of block, 2 address bytes\n");
  }
  else {
    printf("%s:%d: warning: Ignoring unknown block ID %s\n", $strSrc, $line, substr($_,0,2));
  }
  
  dprint("\n");
}

# print last segment
printSegment($segment);
# print end segment
print(CDATA "\t/* END SEGMENT */\n\t0, 0\n};\n");

close(CDATA);
close(INFILE);

sub handleDataRecord {
  my $input = shift;
  my $addrlen = shift;

  my $newrec = readDataRecord(substr($input,2),$addrlen);
  if( $segment eq 0 ) {
    # first record
    dprint("First segment\n");
    $segment= $newrec;
  } else {
    # concat the record to the last one if there is no gap between the sections
    if( $segment->{'nextaddr'} eq $newrec->{'addr'} ) {
      # no gap, append
      dprint("Continue segment\n");
      my @data = @{$newrec->{'data'}};
      unshift(@data, @{$segment->{'data'}});
      $segment->{'data'} = \@data;
      
      $segment->{'len'} += $newrec->{'len'};
      $segment->{'nextaddr'} = $newrec->{'nextaddr'};
    } else {
      # new section, print last section to c file
      dprint("Next segment\n");
      printSegment($segment);
      $segment= $newrec;
    }
  }
}

sub readDataRecord {
  my $srec = shift;
  my $addrlen = shift;
  
  my $rec = {};
  my $data = [];
  
  # get number of payload bytes, stripped checksum and address bytes
  my $bytecnt = hex(substr($srec, 0, 2)) - 1 - $addrlen;
  dprintf("bytecount: %d\n", $bytecnt);
  if($bytecnt & 3) { error("Invalid byte count in record: $bytecnt (not divisible by 4)"); }
  
  # get address
  my $addr = hex(substr($srec, 2, 2*$addrlen));
  dprintf("address: 0x%08x\n", $addr);
  
  $rec->{'addr'} = $addr;
  
  my $dwcnt = $bytecnt/4;
  $rec->{'len'} = $dwcnt;
  $rec->{'nextaddr'} = $addr + $bytecnt;
  
  $srec = substr($srec, 2+2*$addrlen);
  my $i;  
  for($i = 0; $i < $dwcnt; ++$i) {
    my $dw = hex(substr($srec, 8*$i, 2));
    $dw += (hex(substr($srec, 2+8*$i, 2))<<8);
    $dw += (hex(substr($srec, 4+8*$i, 2))<<16);
    $dw += (hex(substr($srec, 6+8*$i, 2))<<24);
    dprintf("dword: 0x%08x\n", $dw);
    push(@$data, $dw);
  }
  
  $rec->{'data'} = $data;
  
  return $rec;
}

sub printSegment {
  my $seg = shift;
  my $dw;
  my $cnt = -1;
  
  printf(CDATA "\t/* SEGMENT */\n\t0x%08x, // address\n\t0x%08x, // length\n\t",$seg->{'addr'}, $seg->{'len'});
  foreach $dw (@{$seg->{'data'}}) {
    if( ++$cnt eq 8 ) {
      $cnt = 0;
      printf(CDATA "\n\t");
    }
    printf(CDATA "0x%08x,",$dw); 
  }
  printf(CDATA "\n");
}

sub error {
  my $errorstring = shift;
  close(CDATA);
  close(INFILE);
  unlink($strTrg);
  die "$strSrc:$line: error: $errorstring\n";
}

sub dprint {
  if($verbose) { print($_); }
}

sub dprintf {
  if($verbose) { printf(@_); }
}
