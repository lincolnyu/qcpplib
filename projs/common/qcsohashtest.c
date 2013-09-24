#include "qc/qcintf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#define BOOL	int

typedef struct _ValueType
{
	int Value;
} ValueType;

void DeleteValue(void *v)
{
	ValueType *pNode = (ValueType*)v;
	free(pNode);
}

void QcSoHashTest()
{
	typedef unsigned int KeyType;
	void *pSH = QcSoHashCreate(4, DeleteValue);
	int i;
	BOOL addOrDel;

	for (i = 0; i < 100; i++)
	{
		KeyType key = rand()%200;
		ValueType* value = (ValueType*)malloc(sizeof(ValueType));
		value->Value = rand()%200;
		addOrDel = (rand()%10)>3;
		if (addOrDel)
		{
			QcSoHashSet(pSH, key, value);
			printf("added (%d,%d) to the hash\n", key, value);
		}
		else
		{
			int res = QcSoHashRemove(pSH, key);
			printf("deleted (%d) from the hash (res=%d)\n", key, res);
		}
	}

	for (i = 0; i < 1000; i++)
	{
		void **pp;
		KeyType key = rand()%200;
		int res = QcSoHashFind(pSH, key, &pp);
		void *p = *pp;
		ValueType *pVal = (ValueType*)p;
		if (res == 0)
		{
			printf("found for key %d, %d\n", key, pVal->Value);
		}
		else
		{
			printf("not found for key %d\n", key);
		}
	}

	QcSoHashDestroy(pSH);
}
