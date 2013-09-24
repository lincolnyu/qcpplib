// qcpptest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern void TestLsz();
extern void TestStdStr();
extern void TestWcToRegexLsz();
extern void TestWcToRegexStdStr();

extern void SoHashTest();

extern "C" void QcTestWc();
extern "C" void QcTestWcToRegex();
extern "C" void QcSoHashTest();

int _tmain(int argc, _TCHAR* argv[])
{
#if 0
	TestLsz();
	TestStdStr();
	TestWcToRegexLsz();
	TestWcToRegexStdStr();

	QcTestWc();
	QcTestWcToRegex();

	SoHashTest();
#endif

	QcSoHashTest();
}

