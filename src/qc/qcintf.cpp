#include "qtl/system/system.h"
#include <malloc.h>

// implementations in qtl
#include "qtl/string/wildcard.h"
#include "qtl/scheme/hash/sohash.h"

#include "qc/qcintf.h"

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
int QcWildcardMatch(const char *pszSource, const char *pszPattern, int **paMatchRef)
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
int QcWildCardToRegex(const char *pszWcPattern, char **pszRegex)
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

/// @brief Creates a split-ordered hash table
/// @param The max-load which is the ratio of item count to table size at which the table should be expanded
/// @param The function that dispose of the values added to the table
/// @return The split-ordered hash table
void* QcSoHashCreate(float maxLoad, void(*disposer)(void*))
{
	using namespace Qtl::Scheme::Hash;
	typedef SoHashLinear<void*, void(*)(void*)> SoHashType;
	SoHashType *pSoHashLinear = new SoHashType(maxLoad, disposer);
	return pSoHashLinear;
}

/// @brief Adds an item to a split-ordered hash table (an existing one will be replaced)
/// @param key The key to the item to add
/// @param pValue The value the key maps to
void QcSoHashSet(void *pSoHash, unsigned int key, void *pValue)
{
	using namespace Qtl::Scheme::Hash;
	typedef SoHashLinear<void*, void(*)(void*)> SoHashType;
	SoHashType *pSH = (SoHashType*)pSoHash;
	pSH->AddKeyValuePair(key, pValue);
}


/// @brief Removes an item from a split-ordered hash table
/// @param pSoHash The hash table
/// @param key The key to the item to remove
/// @return 0 if the item has been removed or -1 if it's not found
int QcSoHashRemove(void *pSoHash, unsigned int key)
{
	using namespace Qtl::Scheme::Hash;
	typedef SoHashLinear<void *, void(*)(void*)> SoHashType;
	SoHashType *pSH = (SoHashType*)pSoHash;
	int numDeleted = pSH->DeleteKey(key);
	return (numDeleted>0)? 0 : -1;
}

/// @brief Looks for the item with the specified key in a split-ordered hash table
/// @param pSoHash The hash table
/// @param key The key to the item to remove
/// @param pppValue To return the pointer to the pointer kept in the hash table to the value
/// @return 0 if the item has been found or -1 if it's not found
int QcSoHashFind(void *pSoHash, unsigned int key, void ***pppValue)
{
	using namespace Qtl::Scheme::Hash;
	typedef SoHashLinear<void *, void(*)(void*)> SoHashType;
	SoHashType *pSH = (SoHashType*)pSoHash;
	bool found = pSH->FindFirst(key, pppValue);
	return (found)? 0 : -1;
}

/// @brief finalises a split-ordered hash table
/// @param pSoHash The hash table to finalise
void QcSoHashDestroy(void *pSoHash)
{
	using namespace Qtl::Scheme::Hash;
	typedef SoHashLinear<void *, void(*)(void*)> SoHashType;
	SoHashType *pSH = (SoHashType*)pSoHash;
	delete pSH;
}
