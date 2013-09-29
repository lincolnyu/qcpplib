// wildcardtest.cpp : Tests the wildcard module.
//

#if defined(_MSC_VER)
#	include "stdafx.h"
#endif

#if defined(_MSC_VER)
#	define STRNCPY(dst, src, len)	strncpy_s(dst, src, len)
#else
#	define STRNCPY(dst, src, len)	strncpy(dst, src, len)
#endif

#include "qtl/system/system.h"

#include <cstdio>
#if _QTL_MINGW
#	include <malloc.h>
#else
#	include <stdlib.h>
#endif
#include <string>

#include "qtl/string/wildcard.h"

using namespace Qtl::String::Wildcard;

void TestLsz()
{	
	printf("+%s...\n", _QTL_FUNC);
	Matcher<> matcher;
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	typedef MatchResult<> MatchR;
	MatchR matchResult;
	const char *s = "abcdarrCgdd(tfabtabcq";
	bool matched = matcher.Match(s, pattern, matchResult);
	if (matched)
	{
		char display[128];
		for (MatchR::MatchList::iterator iter = matchResult.Matches.begin(); iter != matchResult.Matches.end(); ++iter)
		{
			memset(display, 0, sizeof(display));
			STRNCPY(display, iter->Begin, pattern.GetQuotedLength(iter->Begin, iter->End));
			printf("%s\n", display);
		}
	}
}

void TestStdStr()
{
	printf("+%s...\n", _QTL_FUNC);
	typedef MatcherTraits<const std::string&, std::string::const_iterator, size_t> Traits;
	Matcher<Traits> matcher;
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	typedef MatchResult<std::string::const_iterator, size_t> MatchR;
	MatchR matchResult;
	std::string s = "abcdarrCgdd(tfabtabcq";
	bool matched = matcher.Match(s, pattern, matchResult);
	if (matched)
	{
		for (MatchR::MatchList::iterator iter = matchResult.Matches.begin(); iter != matchResult.Matches.end(); ++iter)
		{
			for (MatchR::MatchType::CharIter citer = iter->Begin; citer!=iter->End; ++citer)
			{
				printf("%c", *citer);
			}
			printf("\n");
		}
	}
	printf("\n");
}

void TestWcToRegexLsz()
{
	printf("+%s...\n", _QTL_FUNC);
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	WildCardToRegex<Pattern<> > wc2regex;
	const int regexBufSize=128;
	char *regex = (char*)alloca(regexBufSize);
	char *iterRegex = regex;
	memset(regex, 0, regexBufSize*sizeof(*regex));
	wc2regex.Convert(pattern, regex, iterRegex);
	printf("%s\n", regex);
	printf("\n");
}

void TestWcToRegexStdStr()
{
	printf("+%s...\n", _QTL_FUNC);
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	WildCardToRegex<Pattern<>, std::string&, std::string::iterator> wc2regex;
	std::string regex;
	std::string::iterator dummy = regex.begin();
	wc2regex.Convert(pattern, regex, dummy);
	printf("%s\n", regex.c_str());
	printf("\n");
}
