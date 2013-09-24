#include "qtl/scheme/hash/sohash.h"

#include <vector>
#include <map>

using namespace Qtl::Scheme::Hash;

void SoHashTest()
{
	std::map<int, int> mapref;
	SoHashLinear<int> sohash(4);
	SoHashLinear<int>::ValueType *pVal;
	typedef SoHashLinear<int>::ValueType ValueType;
	typedef SoHashLinear<int>::KeyType KeyType;
	for (int i = 0; i < 100; i++)
	{
		KeyType key = rand()%200;
		ValueType value = rand()%200;
		bool addOrDel = (rand()%10)>3;
		if (addOrDel)
		{
			sohash.AddKeyValuePair(key, value);
			mapref[key] = value;
			printf("added (%d,%d) to the hash\n", key, value);
		}
		else
		{
			int deleted = sohash.DeleteKey(key);
			mapref.erase(key);
			printf("deleted (%d) from the hash (%d)\n", key, deleted);
		}
	}

	for (int i = 0; i < 1000; i++)
	{
		KeyType key = rand()%200;
		std::map<int,int>::iterator iterRef = mapref.find(key);
		bool refFound = (iterRef != mapref.end());
		bool sohashFound = sohash.FindFirst(key, &pVal);
		if (refFound != sohashFound)
		{
			printf("error in so-hash mapping\n");
			break;
		}
		else if (refFound)
		{
			int valRef = iterRef->second;
			int valSoHash = *pVal;
			if (valRef != valSoHash)
			{
				printf("error in so-hash mapping\n");
				break;
			}
		}
		printf("item hit\n");
	}
}

