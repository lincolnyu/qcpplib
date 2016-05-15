// qcpptest.cpp : Defines the entry point for the console application.
//

#if defined(_MSC_VER)
#	include "stdafx.h"
#else
#	define _TCHAR	char
#	define _tmain	main
#	include <stdio.h>
#endif

extern void TestLsz();
extern void TestStdStr();
extern void TestWcToRegexLsz();
extern void TestWcToRegexStdStr();
extern void TestBiPointer();

extern void SoHashTest();

extern "C" void QcTestWc();
extern "C" void QcTestWcToRegex();
extern "C" void QcSoHashTest();

int _tmain(int argc, _TCHAR* argv[])
{
	printf("main\n");
	TestBiPointer();
#if 0
#if 1
	TestLsz();
	TestStdStr();
	TestWcToRegexLsz();
	TestWcToRegexStdStr();

	QcTestWc();
	QcTestWcToRegex();

	SoHashTest();
#endif
	QcSoHashTest();
#endif
	return 0;
}

