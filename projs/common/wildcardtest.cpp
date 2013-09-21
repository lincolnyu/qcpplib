// wildcardtest.cpp : Tests the wildcard module.
//

#if defined(_MSC_VER)
#include "stdafx.h"
#endif

#include <string>

#include "string/wildcard.h"

using namespace Qtl::String::Wildcard;

void TestLsz()
{	
	printf("+%s()...\n", __FUNCTION__);
	Matcher<> matcher;
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	MatchResult<> matchResult;
	char *s = "abcdarrCgdd(tfabtabcq";
	bool matched = matcher.Match(s, pattern, matchResult);
	if (matched)
	{
		char display[128];
		for (auto iter = matchResult.Matches.begin(); iter != matchResult.Matches.end(); ++iter)
		{
			memset(display, 0, sizeof(display));
			strncpy(display, iter->Begin, pattern.GetQuotedLength(iter->Begin, iter->End));
			printf("%s\n", display);
		}
	}
}

void TestStdStr()
{
	printf("+%s()...\n", __FUNCTION__);
	typedef MatcherTraits<const std::string&, std::string::const_iterator, size_t> Traits;
	Matcher<Traits> matcher;
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	MatchResult<std::string::const_iterator, size_t> matchResult;
	std::string s = "abcdarrCgdd(tfabtabcq";
	bool matched = matcher.Match(s, pattern, matchResult);
	if (matched)
	{
		for (auto iter = matchResult.Matches.begin(); iter != matchResult.Matches.end(); ++iter)
		{
			for (auto citer = iter->Begin; citer!=iter->End; ++citer)
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
	printf("+%s()...\n", __FUNCTION__);
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	WildCardToRegex<Pattern<>> wc2regex;
	const int regexBufSize=128;
	char *regex = (char*)alloca(regexBufSize);
	memset(regex, 0, regexBufSize*sizeof(*regex));
	wc2regex.Convert(pattern, regex, regex);
	printf("%s\n", regex);
	printf("\n");
}

void TestWcToRegexStdStr()
{
	printf("+%s()...\n", __FUNCTION__);
	Pattern<> pattern("ab(*)C(?)dd\\(t(f(*)abc)q");
	WildCardToRegex<Pattern<>, std::string&, std::string::iterator> wc2regex;
	std::string regex;
	wc2regex.Convert(pattern, regex, regex.begin());
	printf("%s\n", regex.c_str());
	printf("\n");
}
