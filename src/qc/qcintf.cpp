#include "qc/qcintf.h"

// implementation in qtl
#include "qtl/string/wildcard.h"

#if defined(_MSC_VER)
#	define STRCPY(dst, src, bufsize)	strcpy_s(dst, bufsize, src)
#else
#	define STRCPY(dst, src, bufsize)	strcpy(dst, src)
#endif

/// @brief Matches wildcards to source string
/// @param pszSource The source string
/// @param pszPattern The wildcard pattern
/// @param aMatchRef The list of match entries that reference to the beginning and end of the matching
///        organized in order and alternately. The variable is allocated internally unless it's NULL 
///        and it's the responsibility of the caller to deallocate it.
/// @return The number of match entries if matching or -1
int QcWildcardMatch(char *pszSource, char *pszPattern, int **paMatchRef)
{
	using namespace Qtl::String::Wildcard;
	Matcher<> matcher;
	Matcher<>::MatchResultType matchResult;
	Pattern<> pattern(pszPattern);
	if (!matcher.Match(pszSource, pattern, matchResult))
	{
		return -1;
	}
	int matchCount = matchResult.Matches.size();
	if (paMatchRef != NULL)
	{
		int* aMatchRef = (int*)malloc(sizeof(int)*matchCount*2);
		for (int i = 0; i < matchCount; i++)
		{
			aMatchRef[i*2] = matchResult.Matches[i].Begin-pszPattern;
			aMatchRef[i*2+1] = matchResult.Matches[i].End-pszPattern;
		}
		*paMatchRef = aMatchRef;
	}
	return matchCount;
}

/// @brief Converts wildcard string to a regular expression
/// @param pszWcPattern The wildcard pattern to convert to regular expression
/// @param pszRegex The regular expression converted from the wildcard pattern. It cannot be NULL. It's
///        the caller's responsibility to deallocate it.
/// @return The number of characters in regular expression if conversion successful or -1
int QcWildCardToRegex(char *pszWcPattern, char **pszRegex)
{
	using namespace Qtl::String::Wildcard;
	Pattern<> pattern(pszWcPattern);
	WildCardToRegex<> wc2regex;
	char *regexBuf = (char*)alloca(pattern.GetLength()*4);	// should be enough?
	char *iterRegex=regexBuf;
	wc2regex.Convert(pattern, regexBuf, iterRegex);
	*iterRegex = '\0';
	int len = iterRegex-regexBuf;
	*pszRegex = (char*)malloc(sizeof(char)*(len+1));
	STRCPY(*pszRegex, regexBuf, len+1);
	return len;
}
