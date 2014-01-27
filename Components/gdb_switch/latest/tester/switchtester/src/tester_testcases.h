/*
 *******************************************************************************
 *                                                                             *
 * tester_testcases.h                                                          *
 *                                                                             *
 * author: Daniel Kramlinger                                                   *
 * Hilscher Gesellschaft für Systemautomation mbH                              *
 * date: 25.02.2011                                                            *
 *                                                                             *
 *******************************************************************************
*/



#ifndef TESTER_TESTCASES_H
#define TESTER_TESTCASES_H

#define TEST_CASES_COUNT	64


char
	g_cVerbose,
	cNoAckModeStartedXpic,
	g_cTestCases,
	**ppTestCases;

unsigned int
	uiNumberOfCases;

void InitTestCases( void );

struct typeTest
{
	char **psTest;
};

struct typeTestCase
{
	const char *psTestCaseName;
	const unsigned char ucTestCaseNumber;
	const char *psXpicgdbRequest;
	const char **ppsOpenocdRequest;
	const unsigned char ucOpenocdRequestElements;
	const char **ppsOpenocdAnswer;
	// const unsigned char ucOpenocdAnswerElements;
	const char *psGdbswitchAnswer;
};

// struct typeTestCase *pTestCases;
const struct typeTestCase **pTestCases;
#endif