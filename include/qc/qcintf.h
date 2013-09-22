#if !defined (_QCINTF_H_)
#define _QCINTF_H_

#if defined(__cplusplus)
extern "C" {
#endif

	/// @brief Matches wildcards to source string
	/// @param pszSource The source string
	/// @param pszPattern The wildcard pattern
	/// @param aMatchRef The list of match entries that reference to the beginning and end of the matching
	///        organized in order and alternately. The variable is allocated internally unless it's NULL 
	///        and it's the responsibility of the caller to deallocate it.
	/// @return The number of match entries if matching or -1
	int QcWildcardMatch(char *pszSource, char *pszPattern, int **paMatchRef);

	/// @brief Converts wildcard string to a regular expression
	/// @param pszWcPattern The wildcard pattern to convert to regular expression
	/// @param pszRegex The regular expression converted from the wildcard pattern. It cannot be NULL. It's
	///        the caller's responsibility to deallocate it.
	/// @return The number of characters in regular expression if conversion successful or -1
	int QcWildCardToRegex(char *pszWcPattern, char **pszRegex);

#if defined(__cplusplus)
}	/* extern "C" */
#endif

#endif
