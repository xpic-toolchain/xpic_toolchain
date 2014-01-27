/*
 *******************************************************************************
 *                                                                             *
 * tester_testcases.c                                                          *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 *         Hilscher Gesellschaft für Systemautomation mbH                      *
 * date: 22.03.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/

#include <stdio.h>
// #include <stdlib.h>

#include "tester_testcases.h"
#include "tester_socket.h"

#include "../../../src/netx10gs_gdbproto.h"

//******************************************************************************
// TESTCASES                                                                   *
//******************************************************************************

// syntax:
	// case name
	// case number
	// msg xpicgdb is sending to gdbswitch
	// msg(s) openocd expects from gdbswitch
	// number of OpenocdRequest msgs
	// msg(s) openocd is sending to gdbswitch
	// msg xpicgdb expects from gdbswitch

	
//==============================================================================
// SPEICHER LESEN:
//==============================================================================
const char caseSpeicherLesen[] = { "Speicher lesen" };

//---------------------------------------------
// CASE 1
//
// Speicher lesen oberhalb 0x1fff
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherLesen1[]	= { "$m12345678,4#71" };
const char *OpenocdAnswer_caseSpeicherLesen1[]	= { "$12345678#42" };
const struct typeTestCase caseSpeicherLesen1 =
{
	caseSpeicherLesen,
	1,
	"$m12345678,4#71",
	OpenocdRequest_caseSpeicherLesen1,
	sizeof(OpenocdRequest_caseSpeicherLesen1)/sizeof(char *),
	OpenocdAnswer_caseSpeicherLesen1,
	"$12345678#42"
};

//---------------------------------------------
// CASE 2
//
// Speicher lesen unterhalb 0x2000
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherLesen2[] = { "$m1018d000,4#8b" };
const char *OpenocdAnswer_caseSpeicherLesen2[]	= { "$12345678#42" };
const struct typeTestCase caseSpeicherLesen2 =
{
	caseSpeicherLesen,
	2,
	"$m1000,4#42",
	OpenocdRequest_caseSpeicherLesen2,
	sizeof(OpenocdRequest_caseSpeicherLesen2)/sizeof(char *),
	OpenocdAnswer_caseSpeicherLesen2,
	"$12345678#42"
};

//---------------------------------------------
// CASE 3
//
// Speicher lesen oberhalb 0x1fff, Grenzfall
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherLesen3[] = { "$m00002000,4#4f" };
const char *OpenocdAnswer_caseSpeicherLesen3[]	= { "$12345678#42" };
const struct typeTestCase caseSpeicherLesen3 =
{
	caseSpeicherLesen,
	3,
	"$m2000,4#42",
	OpenocdRequest_caseSpeicherLesen3,
	sizeof(OpenocdRequest_caseSpeicherLesen3)/sizeof(char *),
	OpenocdAnswer_caseSpeicherLesen3,
	"$12345678#42"
};

//---------------------------------------------
// CASE 4
//
// Speicher lesen unterhalb 0x2000, Grenzfall
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherLesen4[] = { "$m1018dfff,4#2d" };
const char *OpenocdAnswer_caseSpeicherLesen4[]	= { "$12345678#42" };
const struct typeTestCase caseSpeicherLesen4 =
{
	caseSpeicherLesen,
	4,
	"$m1fff,4#42",
	OpenocdRequest_caseSpeicherLesen4,
	sizeof(OpenocdRequest_caseSpeicherLesen4)/sizeof(char *),
	OpenocdAnswer_caseSpeicherLesen4,
	"$12345678#42"
};

//---------------------------------------------
// CASE 5
//
// Fehler beim Lesen des Speichers
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherLesen5[] = { "$m1018d000,4#8b" };
const char *OpenocdAnswer_caseSpeicherLesen5[]	= { GDB_ERROR_0 };
const struct typeTestCase caseSpeicherLesen5 =
{
	caseSpeicherLesen,
	5,
	"$m1000,4#42",
	OpenocdRequest_caseSpeicherLesen5,
	sizeof(OpenocdRequest_caseSpeicherLesen5)/sizeof(char *),
	OpenocdAnswer_caseSpeicherLesen5,
	GDB_ERROR_0
};

//==============================================================================
// SPEICHER SCHREIBEN:
//==============================================================================
const char caseSpeicherSchreiben[] = { "Speicher schreiben" };

//---------------------------------------------
// CASE 1
//
// Speicher schreiben oberhalb 0x1fff
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherSchreiben1[]	= { "$M10188100,4:01000000#fb" };
const char *OpenocdAnswer_caseSpeicherSchreiben1[]	= { GDB_OK };
const struct typeTestCase caseSpeicherSchreiben1 =
{
	caseSpeicherSchreiben,
	1,
	"$M10188100,4:01000000#fb",
	OpenocdRequest_caseSpeicherSchreiben1,
	sizeof(OpenocdRequest_caseSpeicherSchreiben1)/sizeof(char *),
	OpenocdAnswer_caseSpeicherSchreiben1,
	GDB_OK
};

//---------------------------------------------
// CASE 2
//
// Speicher schreiben unterhalb 0x2000
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherSchreiben2[] = { "$M1018d000,4:01000000#26" };
const char *OpenocdAnswer_caseSpeicherSchreiben2[]	= { GDB_OK };
const struct typeTestCase caseSpeicherSchreiben2 =
{
	caseSpeicherSchreiben,
	2,
	"$M1000,4:01000000#fb",
	// "$M10188100,4:01000000#fb",
	OpenocdRequest_caseSpeicherSchreiben2,
	sizeof(OpenocdRequest_caseSpeicherSchreiben2)/sizeof(char *),
	OpenocdAnswer_caseSpeicherSchreiben2,
	GDB_OK
};

//---------------------------------------------
// CASE 3
//
// Speicher schreiben oberhalb 0x1fff, Grenzfall
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherSchreiben3[] = { "$M2000,4:12345678#42" };
const char *OpenocdAnswer_caseSpeicherSchreiben3[]	= { GDB_OK };
const struct typeTestCase caseSpeicherSchreiben3 =
{
	caseSpeicherSchreiben,
	3,
	"$M2000,4:12345678#42",
	OpenocdRequest_caseSpeicherSchreiben3,
	sizeof(OpenocdRequest_caseSpeicherSchreiben3)/sizeof(char *),
	OpenocdAnswer_caseSpeicherSchreiben3,
	GDB_OK
};

//---------------------------------------------
// CASE 4
//
// Speicher schreiben unterhalb 0x2000, Grenzfall
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherSchreiben4[] = { "$M1018dfff,4:12345678#eb" };
const char *OpenocdAnswer_caseSpeicherSchreiben4[]	= { GDB_OK };
const struct typeTestCase caseSpeicherSchreiben4 =
{
	caseSpeicherSchreiben,
	4,
	"$M1fff,4:12345678#42",
	OpenocdRequest_caseSpeicherSchreiben4,
	sizeof(OpenocdRequest_caseSpeicherSchreiben4)/sizeof(char *),
	OpenocdAnswer_caseSpeicherSchreiben4,
	GDB_OK
};

//---------------------------------------------
// CASE 5
//
// Fehler beim Schreiben des Speichers
//
//---------------------------------------------
const char *OpenocdRequest_caseSpeicherSchreiben5[] = { "$M1018d000,4:12345678#49" };
const char *OpenocdAnswer_caseSpeicherSchreiben5[]	= { GDB_ERROR_0 };
const struct typeTestCase caseSpeicherSchreiben5 =
{
	caseSpeicherSchreiben,
	5,
	"$M1000,4:12345678#42",
	OpenocdRequest_caseSpeicherSchreiben5,
	sizeof(OpenocdRequest_caseSpeicherSchreiben5)/sizeof(char *),
	OpenocdAnswer_caseSpeicherSchreiben5,
	GDB_ERROR_0
};

//==============================================================================
// REGISTER LESEN:
//==============================================================================
const char caseRegisterLesen[] = { "Register lesen" };

//---------------------------------------------
// CASE 1
//
// alle Register auslesen
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen1[]	= { XPIC_READ_REG_R0, XPIC_READ_REG_R1, XPIC_READ_REG_R2, XPIC_READ_REG_R3, XPIC_READ_REG_R4, XPIC_READ_REG_R5, XPIC_READ_REG_R6, XPIC_READ_REG_R7, XPIC_READ_REG_USR0, XPIC_READ_REG_USR1, XPIC_READ_REG_USR2, XPIC_READ_REG_USR3, XPIC_READ_REG_USR4, XPIC_READ_REG_PC, XPIC_READ_REG_SR };
const char *OpenocdAnswer_caseRegisterLesen1[]	= { "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42", "$12345678#42" };
const struct typeTestCase caseRegisterLesen1 =
{
	caseRegisterLesen,
	1,
	"$g#42",
	OpenocdRequest_caseRegisterLesen1,
	sizeof(OpenocdRequest_caseRegisterLesen1)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen1,
	"$12345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567800000000#1c"
};
	
//---------------------------------------------
// CASE 2
//
// Fehler beim Lesen aller Register
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen2[]	= { XPIC_READ_REG_R0, XPIC_READ_REG_R1, XPIC_READ_REG_R2, XPIC_READ_REG_R3, XPIC_READ_REG_R4, XPIC_READ_REG_R5, XPIC_READ_REG_R6, XPIC_READ_REG_R7, XPIC_READ_REG_USR0, XPIC_READ_REG_USR1, XPIC_READ_REG_USR2, XPIC_READ_REG_USR3, XPIC_READ_REG_USR4, XPIC_READ_REG_PC, XPIC_READ_REG_SR };
const char *OpenocdAnswer_caseRegisterLesen2[]	= { "$12345678#42", "$12345678#42", GDB_ERROR_0 };
const struct typeTestCase caseRegisterLesen2 =
{
	caseRegisterLesen,
	2,
	"$g#42",
	OpenocdRequest_caseRegisterLesen2,
	sizeof(OpenocdRequest_caseRegisterLesen2)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen2,
	GDB_ERROR_0
};
	
//---------------------------------------------
// CASE 3
//
// einzelne Register lesen: r0
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen3[]	= { XPIC_READ_REG_R0 };
const char *OpenocdAnswer_caseRegisterLesen3[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen3 =
{
	caseRegisterLesen,
	3,
	"$p0#42",
	OpenocdRequest_caseRegisterLesen3,
	sizeof(OpenocdRequest_caseRegisterLesen3)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen3,
	"$12345678#42"
};

//---------------------------------------------
// CASE 4
//
// einzelne Register lesen: r1
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen4[]	= { XPIC_READ_REG_R1 };
const char *OpenocdAnswer_caseRegisterLesen4[]	= { "$87654321#42" };
const struct typeTestCase caseRegisterLesen4 =
{
	caseRegisterLesen,
	4,
	"$p1#42",
	OpenocdRequest_caseRegisterLesen4,
	sizeof(OpenocdRequest_caseRegisterLesen4)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen4,
	"$87654321#42"
};

//---------------------------------------------
// CASE 5
//
// einzelne Register lesen: r2
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen5[]	= { XPIC_READ_REG_R2 };
const char *OpenocdAnswer_caseRegisterLesen5[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen5 =
{
	caseRegisterLesen,
	5,
	"$p2#42",
	OpenocdRequest_caseRegisterLesen5,
	sizeof(OpenocdRequest_caseRegisterLesen5)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen5,
	"$12345678#42"
};

//---------------------------------------------
// CASE 6
//
// einzelne Register lesen: r3
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen6[]	= { XPIC_READ_REG_R3 };
const char *OpenocdAnswer_caseRegisterLesen6[]	= { "$87654321#42" };
const struct typeTestCase caseRegisterLesen6 =
{
	caseRegisterLesen,
	6,
	"$p3#42",
	OpenocdRequest_caseRegisterLesen6,
	sizeof(OpenocdRequest_caseRegisterLesen6)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen6,
	"$87654321#42"
};

//---------------------------------------------
// CASE 7
//
// einzelne Register lesen: r4
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen7[]	= { XPIC_READ_REG_R4 };
const char *OpenocdAnswer_caseRegisterLesen7[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen7 =
{
	caseRegisterLesen,
	7,
	"$p4#42",
	OpenocdRequest_caseRegisterLesen7,
	sizeof(OpenocdRequest_caseRegisterLesen7)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen7,
	"$12345678#42"
};

//---------------------------------------------
// CASE 8
//
// einzelne Register lesen: r5
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen8[]	= { XPIC_READ_REG_R5 };
const char *OpenocdAnswer_caseRegisterLesen8[]	= { "$87654321#42" };
const struct typeTestCase caseRegisterLesen8 =
{
	caseRegisterLesen,
	8,
	"$p5#42",
	OpenocdRequest_caseRegisterLesen8,
	sizeof(OpenocdRequest_caseRegisterLesen8)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen8,
	"$87654321#42"
};

//---------------------------------------------
// CASE 9
//
// einzelne Register lesen: r6
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen9[]	= { XPIC_READ_REG_R6 };
const char *OpenocdAnswer_caseRegisterLesen9[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen9 =
{
	caseRegisterLesen,
	9,
	"$p6#42",
	OpenocdRequest_caseRegisterLesen9,
	sizeof(OpenocdRequest_caseRegisterLesen9)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen9,
	"$12345678#42"
};

//---------------------------------------------
// CASE 10
//
// einzelne Register lesen: r7
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen10[] = { XPIC_READ_REG_R7 };
const char *OpenocdAnswer_caseRegisterLesen10[] = { "$87654321#42" };
const struct typeTestCase caseRegisterLesen10 =
{
	caseRegisterLesen,
	10,
	"$p7#42",
	OpenocdRequest_caseRegisterLesen10,
	sizeof(OpenocdRequest_caseRegisterLesen10)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen10,
	"$87654321#42"
};

//---------------------------------------------
// CASE 11
//
// einzelne Register lesen: usr0
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen11[] = { XPIC_READ_REG_USR0 };
const char *OpenocdAnswer_caseRegisterLesen11[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen11 =
{
	caseRegisterLesen,
	11,
	"$p8#42",
	OpenocdRequest_caseRegisterLesen11,
	sizeof(OpenocdRequest_caseRegisterLesen11)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen11,
	"$12345678#42"
};

//---------------------------------------------
// CASE 12
//
// einzelne Register lesen: usr1
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen12[] = { XPIC_READ_REG_USR1 };
const char *OpenocdAnswer_caseRegisterLesen12[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen12 =
{
	caseRegisterLesen,
	12,
	"$p9#42",
	OpenocdRequest_caseRegisterLesen12,
	sizeof(OpenocdRequest_caseRegisterLesen12)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen12,
	"$12345678#42"
};

//---------------------------------------------
// CASE 13
//
// einzelne Register lesen: usr2
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen13[] = { XPIC_READ_REG_USR2 };
const char *OpenocdAnswer_caseRegisterLesen13[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen13 =
{
	caseRegisterLesen,
	13,
	"$pa#42",
	OpenocdRequest_caseRegisterLesen13,
	sizeof(OpenocdRequest_caseRegisterLesen13)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen13,
	"$12345678#42"
};

//---------------------------------------------
// CASE 14
//
// einzelne Register lesen: usr3
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen14[] = { XPIC_READ_REG_USR3 };
const char *OpenocdAnswer_caseRegisterLesen14[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen14 =
{
	caseRegisterLesen,
	14,
	"$pb#42",
	OpenocdRequest_caseRegisterLesen14,
	sizeof(OpenocdRequest_caseRegisterLesen14)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen14,
	"$12345678#42"
};

//---------------------------------------------
// CASE 15
//
// einzelne Register lesen: usr4
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen15[] = { XPIC_READ_REG_USR4 };
const char *OpenocdAnswer_caseRegisterLesen15[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen15 =
{
	caseRegisterLesen,
	15,
	"$pc#42",
	OpenocdRequest_caseRegisterLesen15,
	sizeof(OpenocdRequest_caseRegisterLesen15)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen15,
	"$12345678#42"
};

//---------------------------------------------
// CASE 16
//
// einzelne Register lesen: pc
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen16[] = { XPIC_READ_REG_PC };
const char *OpenocdAnswer_caseRegisterLesen16[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen16 =
{
	caseRegisterLesen,
	16,
	"$pd#42",
	OpenocdRequest_caseRegisterLesen16,
	sizeof(OpenocdRequest_caseRegisterLesen16)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen16,
	"$12345678#42"
};

//---------------------------------------------
// CASE 17
//
// einzelne Register lesen: sr
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen17[] = { XPIC_READ_REG_SR };
const char *OpenocdAnswer_caseRegisterLesen17[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen17 =
{
	caseRegisterLesen,
	17,
	"$pe#42",
	OpenocdRequest_caseRegisterLesen17,
	sizeof(OpenocdRequest_caseRegisterLesen17)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen17,
	"$12345678#42"
};

//---------------------------------------------
// CASE 18
//
// einzelne Register lesen: zero
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen18[] = { XPIC_READ_REG_ZERO };
const char *OpenocdAnswer_caseRegisterLesen18[]	= { "$12345678#42" };
const struct typeTestCase caseRegisterLesen18 =
{
	caseRegisterLesen,
	18,
	"$pf#42",
	OpenocdRequest_caseRegisterLesen18,
	sizeof(OpenocdRequest_caseRegisterLesen18)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen18,
	"$12345678#42"
};

//---------------------------------------------
// CASE 19
//
// einzelne Register lesen: usr0 nicht lesbar
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen19[] = { XPIC_READ_REG_USR0 };
const char *OpenocdAnswer_caseRegisterLesen19[]	= { GDB_ERROR_0 };
const struct typeTestCase caseRegisterLesen19 =
{
	caseRegisterLesen,
	19,
	"$p8#42",
	OpenocdRequest_caseRegisterLesen19,
	sizeof(OpenocdRequest_caseRegisterLesen19)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen19,
	GDB_ERROR_0
};

//---------------------------------------------
// CASE 20
//
// nicht zulaessiges Register
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterLesen20[] = { NULL };
const char *OpenocdAnswer_caseRegisterLesen20[]	= { NULL };
const struct typeTestCase caseRegisterLesen20 =
{
	caseRegisterLesen,
	20,
	"$pg#42",
	OpenocdRequest_caseRegisterLesen20,
	sizeof(OpenocdRequest_caseRegisterLesen20)/sizeof(char *),
	OpenocdAnswer_caseRegisterLesen20,
	GDB_ERROR_0
};


//==============================================================================
// REGISTER SCHREIBEN:
//==============================================================================
const char caseRegisterSchreiben[] = { "Register schreiben" };

//---------------------------------------------
// CASE 1
//
// alle Register schreiben -> nicht unterstuetzter Befehl
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben1[]	= { NULL };
const char *OpenocdAnswer_caseRegisterSchreiben1[]	= { NULL };
const struct typeTestCase caseRegisterSchreiben1 =
{
	caseRegisterSchreiben,
	1,
	"$G#42",
	OpenocdRequest_caseRegisterSchreiben1,
	sizeof(OpenocdRequest_caseRegisterSchreiben1)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben1,
	GDB_UNKWON
};

//---------------------------------------------
// CASE 2
//
// einzelne Register schreiben: r0
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben2[]	= { "$M10188000,4:12345678#1d" };
const char *OpenocdAnswer_caseRegisterSchreiben2[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben2 =
{
	caseRegisterSchreiben,
	2,
	"$P0=12345678#42",
	OpenocdRequest_caseRegisterSchreiben2,
	sizeof(OpenocdRequest_caseRegisterSchreiben2)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben2,
	GDB_OK
};

//---------------------------------------------
// CASE 3
//
// einzelne Register schreiben: r1
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben3[]	= { "$M10188004,4:12345678#21" };
const char *OpenocdAnswer_caseRegisterSchreiben3[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben3 =
{
	caseRegisterSchreiben,
	3,
	"$P1=12345678#42",
	OpenocdRequest_caseRegisterSchreiben3,
	sizeof(OpenocdRequest_caseRegisterSchreiben3)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben3,
	GDB_OK
};

//---------------------------------------------
// CASE 4
//
// einzelne Register schreiben: r2
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben4[]	= { "$M10188008,4:12345678#25" };
const char *OpenocdAnswer_caseRegisterSchreiben4[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben4 =
{
	caseRegisterSchreiben,
	4,
	"$P2=12345678#42",
	OpenocdRequest_caseRegisterSchreiben4,
	sizeof(OpenocdRequest_caseRegisterSchreiben4)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben4,
	GDB_OK
};

//---------------------------------------------
// CASE 5
//
// einzelne Register schreiben: r3
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben5[]	= { "$M1018800c,4:12345678#50" };
const char *OpenocdAnswer_caseRegisterSchreiben5[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben5 =
{
	caseRegisterSchreiben,
	5,
	"$P3=12345678#42",
	OpenocdRequest_caseRegisterSchreiben5,
	sizeof(OpenocdRequest_caseRegisterSchreiben5)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben5,
	GDB_OK
};

//---------------------------------------------
// CASE 6
//
// einzelne Register schreiben: r4
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben6[]	= { "$M10188010,4:12345678#1e" };
const char *OpenocdAnswer_caseRegisterSchreiben6[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben6 =
{
	caseRegisterSchreiben,
	6,
	"$P4=12345678#42",
	OpenocdRequest_caseRegisterSchreiben6,
	sizeof(OpenocdRequest_caseRegisterSchreiben6)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben6,
	GDB_OK
};

//---------------------------------------------
// CASE 7
//
// einzelne Register schreiben: r5
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben7[]	= { "$M10188014,4:12345678#22" };
const char *OpenocdAnswer_caseRegisterSchreiben7[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben7 =
{
	caseRegisterSchreiben,
	7,
	"$P5=12345678#42",
	OpenocdRequest_caseRegisterSchreiben7,
	sizeof(OpenocdRequest_caseRegisterSchreiben7)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben7,
	GDB_OK
};

//---------------------------------------------
// CASE 8
//
// einzelne Register schreiben: r6
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben8[]	= { "$M10188018,4:12345678#26" };
const char *OpenocdAnswer_caseRegisterSchreiben8[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben8 =
{
	caseRegisterSchreiben,
	8,
	"$P6=12345678#42",
	OpenocdRequest_caseRegisterSchreiben8,
	sizeof(OpenocdRequest_caseRegisterSchreiben8)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben8,
	GDB_OK
};

//---------------------------------------------
// CASE 9
//
// einzelne Register schreiben: r7
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben9[]	= { "$M1018801c,4:12345678#51" };
const char *OpenocdAnswer_caseRegisterSchreiben9[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben9 =
{
	caseRegisterSchreiben,
	9,
	"$P7=12345678#42",
	OpenocdRequest_caseRegisterSchreiben9,
	sizeof(OpenocdRequest_caseRegisterSchreiben9)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben9,
	GDB_OK
};

//---------------------------------------------
// CASE 10
//
// einzelne Register schreiben: usr0
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben10[] = { "$M10188020,4:12345678#1f" };
const char *OpenocdAnswer_caseRegisterSchreiben10[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben10 =
{
	caseRegisterSchreiben,
	10,
	"$P8=12345678#42",
	OpenocdRequest_caseRegisterSchreiben10,
	sizeof(OpenocdRequest_caseRegisterSchreiben10)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben10,
	GDB_OK
};

//---------------------------------------------
// CASE 11
//
// einzelne Register schreiben: usr1
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben11[] = { "$M10188024,4:12345678#23" };
const char *OpenocdAnswer_caseRegisterSchreiben11[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben11 =
{
	caseRegisterSchreiben,
	11,
	"$P9=12345678#42",
	OpenocdRequest_caseRegisterSchreiben11,
	sizeof(OpenocdRequest_caseRegisterSchreiben11)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben11,
	GDB_OK
};

//---------------------------------------------
// CASE 12
//
// einzelne Register schreiben: usr2
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben12[] = { "$M10188028,4:12345678#27" };
const char *OpenocdAnswer_caseRegisterSchreiben12[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben12 =
{
	caseRegisterSchreiben,
	12,
	"$Pa=12345678#42",
	OpenocdRequest_caseRegisterSchreiben12,
	sizeof(OpenocdRequest_caseRegisterSchreiben12)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben12,
	GDB_OK
};

//---------------------------------------------
// CASE 13
//
// einzelne Register schreiben: usr3
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben13[] = { "$M1018802c,4:12345678#52" };
const char *OpenocdAnswer_caseRegisterSchreiben13[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben13 =
{
	caseRegisterSchreiben,
	13,
	"$Pb=12345678#42",
	OpenocdRequest_caseRegisterSchreiben13,
	sizeof(OpenocdRequest_caseRegisterSchreiben13)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben13,
	GDB_OK
};

//---------------------------------------------
// CASE 14
//
// einzelne Register schreiben: usr4
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben14[]	= { "$M10188030,4:12345678#20" };
const char *OpenocdAnswer_caseRegisterSchreiben14[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben14 =
{
	caseRegisterSchreiben,
	14,
	"$Pc=12345678#42",
	OpenocdRequest_caseRegisterSchreiben14,
	sizeof(OpenocdRequest_caseRegisterSchreiben14)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben14,
	GDB_OK
};

//---------------------------------------------
// CASE 15
//
// einzelne Register schreiben: pc
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben15[] = { "$M10188034,4:12345678#24" };
const char *OpenocdAnswer_caseRegisterSchreiben15[]	= { GDB_OK };
const struct typeTestCase caseRegisterSchreiben15 =
{
	caseRegisterSchreiben,
	15,
	"$Pd=12345678#42",
	OpenocdRequest_caseRegisterSchreiben15,
	sizeof(OpenocdRequest_caseRegisterSchreiben15)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben15,
	GDB_OK
};

//---------------------------------------------
// CASE 16
//
// einzelne Register schreiben: pc -> konnte nicht schreiben
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben16[] = { "$M10188000,4:12345678#1d" };
const char *OpenocdAnswer_caseRegisterSchreiben16[]	= { GDB_ERROR_0 };
const struct typeTestCase caseRegisterSchreiben16 =
{
	caseRegisterSchreiben,
	16,
	"$P0=12345678#42",
	OpenocdRequest_caseRegisterSchreiben16,
	sizeof(OpenocdRequest_caseRegisterSchreiben16)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben16,
	GDB_ERROR_0
};

//---------------------------------------------
// CASE 17
//
// nicht zulaessiges Register
//
//---------------------------------------------
const char *OpenocdRequest_caseRegisterSchreiben17[] = { NULL };
const char *OpenocdAnswer_caseRegisterSchreiben17[]	= { NULL };
const struct typeTestCase caseRegisterSchreiben17 =
{
	caseRegisterSchreiben,
	17,
	"$Pe=12345678#42",
	OpenocdRequest_caseRegisterSchreiben17,
	sizeof(OpenocdRequest_caseRegisterSchreiben17)/sizeof(char *),
	OpenocdAnswer_caseRegisterSchreiben17,
	GDB_ERROR_0
};


//==============================================================================
// SOFTWARE BREAKPOINTS:
//==============================================================================
const char caseSoftwareBreakpoints[] = { "Software Breakpoints" };

//---------------------------------------------
// CASE 1
//
// Software Breakpoint setzen, oberhalb 0x1fff
//
//---------------------------------------------
const char *OpenocdRequest_caseSoftwareBreakpoints1[]	= { "$m01234567,4#69", "$M01234567,4:100000c4#3b" };
const char *OpenocdAnswer_caseSoftwareBreakpoints1[]	= { "$affe0001#42", GDB_OK };
const struct typeTestCase caseSoftwareBreakpoints1 =
{
	caseSoftwareBreakpoints,
	1,
	"$Z0,1234567,4#42",
	OpenocdRequest_caseSoftwareBreakpoints1,
	sizeof(OpenocdRequest_caseSoftwareBreakpoints1)/sizeof(char *),
	OpenocdAnswer_caseSoftwareBreakpoints1,
	GDB_OK
};

//---------------------------------------------
// CASE 2
//
// Software Breakpoint setzen, unterhalb 0x2000
//
//---------------------------------------------
const char *OpenocdRequest_caseSoftwareBreakpoints2[]	= { "$m1018d000,4#8b", "$M1018d000,4:100000c4#5d" };
const char *OpenocdAnswer_caseSoftwareBreakpoints2[]	= { "$affe0001#42", GDB_OK };
const struct typeTestCase caseSoftwareBreakpoints2 =
{
	caseSoftwareBreakpoints,
	2,
	"$Z0,1000,4#42",
	OpenocdRequest_caseSoftwareBreakpoints2,
	sizeof(OpenocdRequest_caseSoftwareBreakpoints2)/sizeof(char *),
	OpenocdAnswer_caseSoftwareBreakpoints2,
	GDB_OK
};

//---------------------------------------------
// CASE 3
//
// gleichen Software Breakpoint nochmal setzen
// TODO: switch checken! vorher "$Z0,1234567,4#42",
//---------------------------------------------
const char *OpenocdRequest_caseSoftwareBreakpoints3[]	= { NULL };
const char *OpenocdAnswer_caseSoftwareBreakpoints3[]	= { NULL };
const struct typeTestCase caseSoftwareBreakpoints3 =
{
	caseSoftwareBreakpoints,
	3,
	"$Z0,1000,4#42",
	OpenocdRequest_caseSoftwareBreakpoints3,
	sizeof(OpenocdRequest_caseSoftwareBreakpoints3)/sizeof(char *),
	OpenocdAnswer_caseSoftwareBreakpoints3,
	GDB_ERROR_0
};

//---------------------------------------------
// CASE 4
//
// Software Breakpoint wieder leoschen, oberhalb 0x1fff
//
//---------------------------------------------
const char *OpenocdRequest_caseSoftwareBreakpoints4[]	= { "$M01234567,4:affe0001#d6" };
const char *OpenocdAnswer_caseSoftwareBreakpoints4[]	= { GDB_OK };
const struct typeTestCase caseSoftwareBreakpoints4 =
{
	caseSoftwareBreakpoints,
	4,
	"$z0,1234567,4#42",
	OpenocdRequest_caseSoftwareBreakpoints4,
	sizeof(OpenocdRequest_caseSoftwareBreakpoints4)/sizeof(char *),
	OpenocdAnswer_caseSoftwareBreakpoints4,
	GDB_OK
};

//---------------------------------------------
// CASE 5
//
// Software Breakpoint wieder leoschen, unterhalb 0x2000
//
//---------------------------------------------
const char *OpenocdRequest_caseSoftwareBreakpoints5[]	= { "$M1018d000,4:affe0001#f8" };
const char *OpenocdAnswer_caseSoftwareBreakpoints5[]	= { GDB_OK };
const struct typeTestCase caseSoftwareBreakpoints5 =
{
	caseSoftwareBreakpoints,
	5,
	"$z0,1000,4#42",
	OpenocdRequest_caseSoftwareBreakpoints5,
	sizeof(OpenocdRequest_caseSoftwareBreakpoints5)/sizeof(char *),
	OpenocdAnswer_caseSoftwareBreakpoints5,
	GDB_OK
};

//---------------------------------------------
// CASE 6
//
// nicht vorhandenen Software Breakpoint loeschen
//
//---------------------------------------------
const char *OpenocdRequest_caseSoftwareBreakpoints6[]	= { NULL };
const char *OpenocdAnswer_caseSoftwareBreakpoints6[]	= { NULL };
const struct typeTestCase caseSoftwareBreakpoints6 =
{
	caseSoftwareBreakpoints,
	6,
	"$z0,1234567,4#42",
	OpenocdRequest_caseSoftwareBreakpoints6,
	sizeof(OpenocdRequest_caseSoftwareBreakpoints6)/sizeof(char *),
	OpenocdAnswer_caseSoftwareBreakpoints6,
	GDB_ERROR_0
};


//==============================================================================
// HARDWARE BREAKPOINTS:
//==============================================================================
const char caseHardwareBreakpoints[] = {"Hardware Breakpoints"};

//---------------------------------------------
// CASE 1
//
// ersten Hardware Breakpoint setzen, oberhalb 0x1fff
//
//---------------------------------------------
const char *OpenocdRequest_caseHardwareBreakpoints1[]	= { "$M10188104,4:67452301#1a", "$M10188114,4:00010000#00" };
const char *OpenocdAnswer_caseHardwareBreakpoints1[]	= { GDB_OK, GDB_OK };
const struct typeTestCase caseHardwareBreakpoints1 =
{
	caseHardwareBreakpoints,
	1,
	"$Z1,1234567,4#42",
	OpenocdRequest_caseHardwareBreakpoints1,
	sizeof(OpenocdRequest_caseHardwareBreakpoints1)/sizeof(char *),
	OpenocdAnswer_caseHardwareBreakpoints1,
	GDB_OK
};

//---------------------------------------------
// CASE 2
//
// zweiten Hardware Breakpoint setzen, unterhalb 0x2000
//
//---------------------------------------------
const char *OpenocdRequest_caseHardwareBreakpoints2[]	= { "$M1018811c,4:00100000#2f", "$M1018812c,4:00010000#30" };
const char *OpenocdAnswer_caseHardwareBreakpoints2[]	= { GDB_OK, GDB_OK };
const struct typeTestCase caseHardwareBreakpoints2 =
{
	caseHardwareBreakpoints,
	2,
	"$Z1,1000,4#42",
	OpenocdRequest_caseHardwareBreakpoints2,
	sizeof(OpenocdRequest_caseHardwareBreakpoints2)/sizeof(char *),
	OpenocdAnswer_caseHardwareBreakpoints2,
	GDB_OK
};

//---------------------------------------------
// CASE 3
//
// dritten Hardware Breakpoint setzen -> error
//
//---------------------------------------------
const char *OpenocdRequest_caseHardwareBreakpoints3[]	= { NULL };
const char *OpenocdAnswer_caseHardwareBreakpoints3[]	= { NULL };
const struct typeTestCase caseHardwareBreakpoints3 =
{
	caseHardwareBreakpoints,
	3,
	"$Z1,1000,4#42",
	OpenocdRequest_caseHardwareBreakpoints3,
	sizeof(OpenocdRequest_caseHardwareBreakpoints3)/sizeof(char *),
	OpenocdAnswer_caseHardwareBreakpoints3,
	GDB_ERROR_0
};

//---------------------------------------------
// CASE 4
//
// Hardware Breakpoint wieder leoschen
//
//---------------------------------------------
const char *OpenocdRequest_caseHardwareBreakpoints4[]	= { "$M10188114,4:00000000#ff", "$M1018812c,4:00000000#2f" };
const char *OpenocdAnswer_caseHardwareBreakpoints4[]	= { GDB_OK, GDB_OK };
const struct typeTestCase caseHardwareBreakpoints4 =
{
	caseHardwareBreakpoints,
	4,
	"$z1,1234567,4#42",
	OpenocdRequest_caseHardwareBreakpoints4,
	sizeof(OpenocdRequest_caseHardwareBreakpoints4)/sizeof(char *),
	OpenocdAnswer_caseHardwareBreakpoints4,
	GDB_OK
};

//---------------------------------------------
// CASE 5
//
// Hardware Breakpoint leoschen
//
//---------------------------------------------
const char *OpenocdRequest_caseHardwareBreakpoints5[]	= { "$M10188114,4:00000000#ff", "$M1018812c,4:00000000#2f" };
const char *OpenocdAnswer_caseHardwareBreakpoints5[]	= { GDB_OK, GDB_OK };
const struct typeTestCase caseHardwareBreakpoints5 =
{
	caseHardwareBreakpoints,
	5,
	"$z1,1000,4#42",
	OpenocdRequest_caseHardwareBreakpoints5,
	sizeof(OpenocdRequest_caseHardwareBreakpoints5)/sizeof(char *),
	OpenocdAnswer_caseHardwareBreakpoints5,
	GDB_OK
};

//---------------------------------------------
// CASE 6
//
// nicht vorhandenen Hardware Breakpoint loeschen
//
//---------------------------------------------
const char *OpenocdRequest_caseHardwareBreakpoints6[]	= { NULL };
const char *OpenocdAnswer_caseHardwareBreakpoints6[]	= { NULL };
const struct typeTestCase caseHardwareBreakpoints6 =
{
	caseHardwareBreakpoints,
	6,
	"$z1,1000,4#42",
	OpenocdRequest_caseHardwareBreakpoints6,
	sizeof(OpenocdRequest_caseHardwareBreakpoints6)/sizeof(char *),
	OpenocdAnswer_caseHardwareBreakpoints6,
	GDB_ERROR_0
};

// TODO: gleichen Hardware Breakpoint nochmal setzen - XXX sollte das einen Fehler melden!?


//==============================================================================
// SINGLE STEP:
//==============================================================================
const char caseSingleStep[] = { "Singlestep" };


//---------------------------------------------
// CASE 1
//
// single step ohne MSK_SW_BREAKPOINT oder MSK_HW_BREAKPOINTS
//
//---------------------------------------------
const char *OpenocdRequest_caseSingleStep1[]	= { XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP, XPIC_READ_BREAK_STATUS };
const char *OpenocdAnswer_caseSingleStep1[]	= { GDB_OK, "$00000000#42", "$21000000#42", GDB_OK, GDB_OK, "$03000000#42" };
const struct typeTestCase caseSingleStep1 =
{
	caseSingleStep,
	1,
	"$s#73",
	OpenocdRequest_caseSingleStep1,
	sizeof(OpenocdRequest_caseSingleStep1)/sizeof(char *),
	OpenocdAnswer_caseSingleStep1,
	GDB_SIGTRAP
};


//---------------------------------------------
// CASE 2
//
// single step mit MSK_SW_BREAKPOINT
//
//---------------------------------------------
const char *OpenocdRequest_caseSingleStep2[]	= { XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP, XPIC_READ_BREAK_STATUS, XPIC_RESET_IRQ_SBREAK, XPIC_READ_REG_PC, "$M10188034,4:04000000#04", XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP, XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP };
const char *OpenocdAnswer_caseSingleStep2[]	= { GDB_OK, "$00000000#42", "$31000000#42", GDB_OK, GDB_OK, "$13000000#42", GDB_OK, "$0c000000#42", GDB_OK, GDB_OK, "$21000000#42", GDB_OK, GDB_OK, GDB_OK, "$21000000#42", GDB_OK, GDB_OK };
const struct typeTestCase caseSingleStep2 =
{
	caseSingleStep,
	2,
	"$s#73",
	OpenocdRequest_caseSingleStep2,
	sizeof(OpenocdRequest_caseSingleStep2)/sizeof(char *),
	OpenocdAnswer_caseSingleStep2,
	GDB_SIGTRAP
};

//---------------------------------------------
// CASE 3
//
// single step mit hw bp 0
//
//---------------------------------------------
const char *OpenocdRequest_caseSingleStep3[]	= { XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP, XPIC_READ_BREAK_STATUS, XPIC_RESET_IRQ_HBREAK0 };
const char *OpenocdAnswer_caseSingleStep3[]	= { GDB_OK, "$00000000#42", "$25000000#42", GDB_OK, GDB_OK, "$07000000#42", GDB_OK };
const struct typeTestCase caseSingleStep3 =
{
	caseSingleStep,
	3,
	"$s#73",
	OpenocdRequest_caseSingleStep3,
	sizeof(OpenocdRequest_caseSingleStep3)/sizeof(char *),
	OpenocdAnswer_caseSingleStep3,
	GDB_SIGTRAP
};

//---------------------------------------------
// CASE 4
//
// single step mit hw bp 1
//
//---------------------------------------------
const char *OpenocdRequest_caseSingleStep4[]	= { XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP, XPIC_READ_BREAK_STATUS, XPIC_RESET_IRQ_HBREAK1 };
const char *OpenocdAnswer_caseSingleStep4[]	= { GDB_OK, "$00000000#42", "$29000000#42", GDB_OK, GDB_OK, "$0b000000#42", GDB_OK };
const struct typeTestCase caseSingleStep4 =
{
	caseSingleStep,
	4,
	"$s#73",
	OpenocdRequest_caseSingleStep4,
	sizeof(OpenocdRequest_caseSingleStep4)/sizeof(char *),
	OpenocdAnswer_caseSingleStep4,
	GDB_SIGTRAP
};

//---------------------------------------------
// CASE 5
//
// single step mit g_bDoSingleStepAfterBreakpoint TRUE - Teil A
// gleichzeitig Test fuer continue mit sw bp
//
//---------------------------------------------
const char *OpenocdRequest_caseSingleStep5[]	= { XPIC_CMD_START, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SBREAK, XPIC_READ_REG_PC, "$M10188034,4:08000000#08" };
const char *OpenocdAnswer_caseSingleStep5[]	= { GDB_OK, "$00000000#42", "$11000000#42", GDB_OK, GDB_OK, "$0c000000#42", GDB_OK};
const struct typeTestCase caseSingleStep5 =
{
	caseSingleStep,
	5,
	"$c#42",
	OpenocdRequest_caseSingleStep5,
	sizeof(OpenocdRequest_caseSingleStep5)/sizeof(char *),
	OpenocdAnswer_caseSingleStep5,
	GDB_SIGTRAP
};

//---------------------------------------------
// CASE 6
//
// single step mit g_bDoSingleStepAfterBreakpoint TRUE - Teil B
//
//---------------------------------------------
const char *OpenocdRequest_caseSingleStep6[]	= { XPIC_READ_REG_PC, "$M10188034,4:08000000#08", XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP, XPIC_SINGLESTEP, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SSTEP, XPIC_READ_BREAK_STATUS };
const char *OpenocdAnswer_caseSingleStep6[]	= { "$0c000000#42", GDB_OK, GDB_OK, "$00000000#42", "$21000000#42", GDB_OK, GDB_OK, GDB_OK, "$21000000#42", GDB_OK, GDB_OK, "$03000000#42" };
const struct typeTestCase caseSingleStep6 =
{
	caseSingleStep,
	6,
	"$s#73",
	OpenocdRequest_caseSingleStep6,
	sizeof(OpenocdRequest_caseSingleStep6)/sizeof(char *),
	OpenocdAnswer_caseSingleStep6,
	GDB_SIGTRAP
};


//==============================================================================
// CONTINUE:
//==============================================================================
const char caseContinue[] = { "Continue" };


//---------------------------------------------
// CASE 1
//
// continue mit CTRL-C - Teil A
//
//---------------------------------------------
const char *OpenocdRequest_caseContinue1[]	= { XPIC_CMD_START, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS };
const char *OpenocdAnswer_caseContinue1[]	= { GDB_OK, "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42", "$00000000#42" };
const struct typeTestCase caseContinue1 =
{
	caseContinue,
	1,
	"$c#42",
	OpenocdRequest_caseContinue1,
	sizeof(OpenocdRequest_caseContinue1)/sizeof(char *),
	OpenocdAnswer_caseContinue1,
	NULL
};

//---------------------------------------------
// CASE 2
//
// continue mit CTRL-C - Teil B
//
//---------------------------------------------
const char *OpenocdRequest_caseContinue2[]	= { XPIC_CMD_START, XPIC_READ_BREAK_STATUS };
const char *OpenocdAnswer_caseContinue2[]	= { GDB_OK, "$00000000#42" };
const struct typeTestCase caseContinue2 =
{
	caseContinue,
	1,
	GDB_CTRL_C,
	OpenocdRequest_caseContinue2,
	sizeof(OpenocdRequest_caseContinue2)/sizeof(char *),
	OpenocdAnswer_caseContinue2,
	GDB_SIGTRAP
};



// //---------------------------------------------
// // CASE 1
// //
// // continue, mit sw breakpoint
// //
// //---------------------------------------------
// const char *OpenocdRequest_caseSingleStep1[]	= { XPIC_CMD_START, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SBREAK, XPIC_READ_REG_PC, "$M10188034,4:f8ffffff#82" };
// const char *OpenocdAnswer_caseSingleStep1[]	= { GDB_OK, "$00000000#42", "$13000000#42", GDB_OK, GDB_OK, "$fcffffff#42", GDB_OK };
// const struct typeTestCase caseSingleStep1 =
// {
	// caseSingleStep,
	// 1,
	// "$c#42",
	// OpenocdRequest_caseSingleStep1,
	// sizeof(OpenocdRequest_caseSingleStep1)/sizeof(char *),
	// OpenocdAnswer_caseSingleStep1,
	// GDB_SIGTRAP
// };

// //==============================================================================
// // CONTINUE:
// //==============================================================================
// const char caseSonst[] = { "Sonstiges" };

// //---------------------------------------------
// // CASE 1
// //
// // continue, mit sw breakpoint
// //
// //---------------------------------------------
// const char *OpenocdRequest_caseSonst1[]	= { XPIC_CMD_START, XPIC_READ_BREAK_STATUS, XPIC_READ_BREAK_STATUS, XPIC_CMD_STOP, XPIC_RESET_IRQ_SBREAK, XPIC_READ_REG_PC, "$M10188034,4:f8ffffff#82" };
// const char *OpenocdAnswer_caseSonst1[]	= { GDB_OK, "$00000000#42", "$13000000#42", GDB_OK, GDB_OK, "$fcffffff#42", GDB_OK };
// const struct typeTestCase caseSonst1 =
// {
	// caseSingleStep,
	// 1,
	// "$c#42",
	// OpenocdRequest_caseSingleStep1,
	// sizeof(OpenocdRequest_caseSingleStep1)/sizeof(char *),
	// OpenocdAnswer_caseSingleStep1,
	// GDB_SIGTRAP
// };


/*	
	
	// case 47
	// 
		GDB_CTRL_C,
		NULL,
		NULL,
		GDB_SIGINT,
	
	// case 48
	// vom Gdb Switch nicht unterstuetzter Befehl
		"$X1000,4:ab#42",
		NULL,
		NULL,
		GDB_UNKWON,
	

	
	// case 7
	// Aktivierung von NoAckMode
		"$QStartNoAckMode#b0",
		"$QStartNoAckMode#b0",
		GDB_OK,
		GDB_OK,
	
	//-------------------------------
	// ab hier NoAckMode
	//-------------------------------
	
	// case 7
	// Speicher lesen unterhalb 0x2000, nochmal wegen NoAckMode
		"$m1000,4#42",
		"$m1018d000,4#8b",
		"$12345678#42",
		"$12345678#42",
	
	
	
	//-------------------------------
	// Fehlerfaelle:
	//-------------------------------
	
	// case 8
	// unbekannter qRcmd Befehl
		"$qRcmd,706f6c6c#58",
		NULL,
		NULL,
		"$O4552524f523a20696e76616c696420636f6d6d616e640a#65",
	
	
	// case 10
	// 
		"-",
		"-",
		GDB_OK,
		GDB_OK,
	
	// EXIT
		"EXIT",
		"EXIT",
		"EXIT",
		"EXIT"
		
};
*/

//==============================================================================
void InitTestCases( void )
{
	// int i = 0x0;
	// unsigned int uiNumberOfCases2 = 0x0;
	static const struct typeTestCase *astructTestCases[] =
	{
		// Speicher lesen
		&caseSpeicherLesen1,
		&caseSpeicherLesen2,
		&caseSpeicherLesen3,
		&caseSpeicherLesen4,
		&caseSpeicherLesen5,
		
		// Speicher schreiben
		&caseSpeicherSchreiben1,
		&caseSpeicherSchreiben2,
		&caseSpeicherSchreiben3,
		&caseSpeicherSchreiben4,
		&caseSpeicherSchreiben5,
		
		// Register lesen
		&caseRegisterLesen1,
		&caseRegisterLesen2,
		&caseRegisterLesen3,
		&caseRegisterLesen4,
		&caseRegisterLesen5,
		&caseRegisterLesen6,
		&caseRegisterLesen7,
		&caseRegisterLesen8,
		&caseRegisterLesen9,
		&caseRegisterLesen10,
		&caseRegisterLesen11,
		&caseRegisterLesen12,
		&caseRegisterLesen13,
		&caseRegisterLesen14,
		&caseRegisterLesen15,
		&caseRegisterLesen16,
		&caseRegisterLesen17,
		&caseRegisterLesen18,
		&caseRegisterLesen19,
		&caseRegisterLesen20,
		
		// Register schreiben
		&caseRegisterSchreiben1,
		&caseRegisterSchreiben2,
		&caseRegisterSchreiben3,
		&caseRegisterSchreiben4,
		&caseRegisterSchreiben5,
		&caseRegisterSchreiben6,
		&caseRegisterSchreiben7,
		&caseRegisterSchreiben8,
		&caseRegisterSchreiben9,
		&caseRegisterSchreiben10,
		&caseRegisterSchreiben11,
		&caseRegisterSchreiben12,
		&caseRegisterSchreiben13,
		&caseRegisterSchreiben14,
		&caseRegisterSchreiben15,
		&caseRegisterSchreiben16,
		&caseRegisterSchreiben17,
		
		// Software Breakpoints
		&caseSoftwareBreakpoints1,
		&caseSoftwareBreakpoints2,
		// &caseSoftwareBreakpoints3,
		&caseSoftwareBreakpoints4,
		&caseSoftwareBreakpoints5,
		&caseSoftwareBreakpoints6,
		
		// Hardware Breakpoints
		&caseHardwareBreakpoints1,
		&caseHardwareBreakpoints2,
		&caseHardwareBreakpoints3,
		&caseHardwareBreakpoints4,
		&caseHardwareBreakpoints5,
		&caseHardwareBreakpoints6,
		
		// singlestep
		&caseSingleStep1,
		&caseSingleStep2,
		&caseSingleStep3,
		&caseSingleStep4,
		&caseSingleStep5,
		&caseSingleStep6,
		
		// // continue
		// &caseContinue1,
		// &caseContinue2
		
	};
	// struct typeTestCase *astructTC = &case_1;
	//g_cVerbose = 0x0;
	// ppTestCases = pTestCases;
	cNoAckModeStartedXpic = 0x0;
	g_cTestCases = 2;
	pTestCases = astructTestCases;
	// fprintf( stdout, "a: %i\n", sizeof(astructTC) );
	// fprintf( stdout, "b: %i\n", sizeof(char *) );
	uiNumberOfCases = sizeof(astructTestCases) / sizeof(struct typeTestCase *);
	// fprintf ( stdout, "elemente: %i\n", uiNumberOfCases );
	// fprintf ( stdout, "gaga: %i\n", pTestCases[10]->ucOpenocdRequestElements );

	// for ( i = 0x0; i < 2; i++ )
	// {
		// fprintf ( stdout, "gaga: %s\n", pTestCases[i]->ppsOpenocdRequest[0] );
		
	// }
	// printf ( "%s\n", pTC[1].psGdbswitchAnswer );
}
