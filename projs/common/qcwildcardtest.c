#include "qc/qcintf.h"
#include "qtl/system/system.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

void QcTestWc()
{
	const char *pattern = "ab(*)C(?)dd\\(t(f(*)abc)q";
	const char *s = "abcdarrCgdd(tfabtabcq";
	int *matchRef;
	int matchCount = QcWildcardMatch(s, pattern, &matchRef);

	printf("+%s...\n", _QTL_FUNC);

	if (matchCount>0)
	{
		char display[128];
		int i;
		for (i = 0; i < matchCount; i++)
		{
			int matchBegin = matchRef[i*2];
			int matchEnd = matchRef[i*2+1];
			memset(display, 0, sizeof(display));
			strncpy(display, s, matchEnd-matchBegin);
			printf("%s\n", display);
		}
	}
	else
	{
		printf("unmatched\n");
	}

	printf("\n");
	free(matchRef);
}

void QcTestWcToRegex()
{
	const char *pattern = "ab(*)C(?)dd\\(t(f(*)abc)q";
	char *regex;

	printf("+%s...\n", _QTL_FUNC);

	QcWildCardToRegex(pattern, &regex);
	printf("%s\n", regex);
	printf("\n");
	free(regex);
}
