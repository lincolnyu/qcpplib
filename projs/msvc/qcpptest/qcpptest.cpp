// qcpptest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern void TestLsz();
extern void TestStdStr();
extern void TestWcToRegexLsz();
extern void TestWcToRegexStdStr();

extern "C" void QcTestWc();
extern "C" void QcTestWcToRegex();

int _tmain(int argc, _TCHAR* argv[])
{
	TestLsz();
	TestStdStr();
	TestWcToRegexLsz();
	TestWcToRegexStdStr();

	QcTestWc();
	QcTestWcToRegex();
}

