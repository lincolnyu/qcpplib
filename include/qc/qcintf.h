//
//  qcpplib v1.00
//  quanben's C++ template library
//  
//  Author Lincoln Yu
//
//  lincoln.yu@gmail.com
//  https://github.com/lincolnyu/qcpplib
//
//  The MIT License (MIT)
// 
//  Copyright (c) 2013 quanbenSoft
// 
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
// 
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
// 
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//


#if !defined (_QCINTF_H_)
#define _QCINTF_H_

#undef BOOL
#undef TRUE
#undef FALSE

#define BOOL	int
#define TRUE	(1)
#define FALSE	(0)

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
	int QcWildcardMatch(const char *pszSource, const char *pszPattern, int **paMatchRef);

	/// @brief Converts wildcard string to a regular expression
	/// @param pszWcPattern The wildcard pattern to convert to regular expression
	/// @param pszRegex The regular expression converted from the wildcard pattern. It cannot be NULL. It's
	///        the caller's responsibility to deallocate it.
	/// @return The number of characters in regular expression if conversion successful or -1
	int QcWildCardToRegex(const char *pszWcPattern, char **pszRegex);

	/// @brief Creates a split-ordered hash table
	/// @param The max-load which is the ratio of item count to table size at which the table should be expanded
	/// @param The function that dispose of the values added to the table
	/// @return The split-ordered hash table
	void* QcSoHashCreate(float maxLoad, void(*disposer)(void*));

	/// @brief Adds an item to a split-ordered hash table (an existing one will be replaced)
	/// @param pSoHash The hash table 
	/// @param key The key to the item to add
	/// @param pValue The value the key is mapped to
	void QcSoHashSet(void *pSoHash, unsigned int key, void *pValue);

	/// @brief Removes an item from a split-ordered hash table
	/// @param pSoHash The hash table
	/// @param key The key to the item to remove
	/// @return 0 if the item has been removed or -1 if it's not found
	int QcSoHashRemove(void *pSoHash, unsigned int key);

	/// @brief Looks for the item with the specified key in a split-ordered hash table
	/// @param pSoHash The hash table
	/// @param key The key to the item to remove
	/// @param pppValue To return the pointer to the pointer kept in the hash table to the value
	/// @return 0 if the item has been found or -1 if it's not found
	int QcSoHashFind(void *pSoHash, unsigned int key, void ***pppValue);

	/// @brief finalises a split-ordered hash table
	/// @param pSoHash The hash table to finalise
	void QcSoHashDestroy(void *pSoHash);

#if defined(__cplusplus)
}	/* extern "C" */
#endif

#endif
