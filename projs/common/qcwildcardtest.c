#include "qc/qcintf.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>


void QcTestWc()
{
	char *pattern = "ab(*)C(?)dd\\(t(f(*)abc)q";
	char *s = "abcdarrCgdd(tfabtabcq";
	int *matchRef;
	int matchCount = QcWildcardMatch(s, pattern, &matchRef);

	printf("+%s()...\n", __FUNCTION__);

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
	char *pattern = "ab(*)C(?)dd\\(t(f(*)abc)q";
	char *regex;

	printf("+%s()...\n", __FUNCTION__);

	QcWildCardToRegex(pattern, &regex);
	printf("%s\n", regex);
	printf("\n");
	free(regex);
}
