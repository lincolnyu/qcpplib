#include <vector>

#include "qtl/scheme/pointers/bipointer.h"

using namespace std;

namespace Qtl 
{
	namespace Scheme 
	{
		namespace Pointers 
		{
			GarbageCollector GarbageCollector::Instance;

			void GarbageCollector::DisposeAll()
			{
				_isColleting = true;
				
				vector<BiPointedObject *> delvec;
				for (unordered_set<BiPointedObject*>::iterator itr = _checkedObjects.begin(); itr != _checkedObjects.end(); ++itr)
				{
					BiPointedObject *obj = *itr;
					if (obj->CheckIfIsolated())
					{
						delvec.push_back(obj);
					}
				}
				for (vector<BiPointedObject *>::iterator itr = delvec.begin(); itr != delvec.end(); ++itr)
				{
					BiPointedObject * obj = *itr;
					delete obj;
				}
				_checkedObjects.clear();
				_isColleting = false;
			}
		}
	}
}
