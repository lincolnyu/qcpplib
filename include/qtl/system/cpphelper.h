#if !defined(_CPP_HELPER_H_)
#define _CPP_HELPER_H_

template <class T>
bool IsOfType(void *pQuery)
{
	return (dynamic_cast<T>(pQuery> != NULL);
}

#endif
