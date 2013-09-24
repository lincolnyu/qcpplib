#if !defined(_SOHASH_H_)
#define _SOHASH_H_

#include <vector>
#include <mutex>

namespace Qtl { namespace Scheme { namespace Hash {

/// @brief The default disposer that does nothing
template <class TValue>
struct DefaultDisposer
{
	void operator()(TValue &value)
	{
	}
};

/// @brief Split-ordered hash base class
template <class TValue, class TDisposer=DefaultDisposer<TValue>>
class SoHash
{
public:
	/// @brief The type of the key
	typedef unsigned int KeyType;
	
	/// @brief Accessible type of the value
	typedef TValue	ValueType;
	
protected:

	/// @brief The base node used for dummy node and for normal node to inherit
	struct BaseNode
	{
		/// @brief SO-key for the node (bit-reversal) plus 1 if non-dummy
		KeyType Key;
		
		/// @brief The pointer that points to the next node 
		BaseNode *Next;
		
		/// @brief Instantiates a BaseNode with the specific SO-key
		/// @param key The SO-key to the node
		BaseNode(KeyType key) : Key(key), Next(NULL)
		{
		}
		
		/// @brief Finalises the BaseNode
		virtual ~BaseNode()
		{
		}
	};

	/// @brief Normal node as an extension of BaseNode
	class Node : public BaseNode
	{
	private:
		/// @brief The base class
		typedef BaseNode	Base;

	public:
		/// @brief The value this node contains
		ValueType Value;

	public:
		/// @brief Instantiates a Node with the specified key and value
		Node(KeyType key, ValueType value) : Base(key), Value(value)
		{
		}
	};

	/// @brief A functor that accepts all key matches
	struct AllwaysTruePredicate
	{
		bool operator()(ValueType key)
		{
			return true;
		}
	};
	
public:	// Nested types

	/// @brief The iterator of the class
	class Iterator
	{
	private:
		/// @brief The node the iterator is based on
		BaseNode *_node;
		
	private:
		/// @brief Moves the iterator to the next
		void MoveNext()
		{
			do
			{
				_node = _node->Next;
			} while (_node != NULL && dynamic_cast<Node*>(_node) == NULL);
		}
	
	private:
		/// @brief Instantiates an iterator with the specified node
		/// @param node The node the iterator to bind to
		Iterator(BaseNode *node) : _node(node)
		{
		}
	
	public:
		/// @brief Instantiates an iterator associated to default (NULL) node
		Iterator() : _node(NULL)
		{
		}
		
		/// @brief Instantiates an iterator with another iterator to copy from
		/// @param other The iterator to copy from
		Iterator(const Iterator &other) : _node(other._node)
		{
		}
		
		/// @brief Assign the spcified iterator to this
		/// @param other The itertator to copy from
		/// @return This iterator after the assignment
		Iterator &operator=(const Iterator &other)
		{
			_node = other._node;
			return (*this);
		}
		
		/// @brief Determines if the two iterators are the same
		/// @param other The iterator to compare this one to
		/// @return true if they are considered the same or false
		bool operator==(const Iterator &other)
		{
			return (_node == other._node);
		}
	
		/// @brief Moves the iterator to the next node and returns the iterator itself after the move
		/// @return The iterator
		Iterator &operator++()
		{
			MoveNext();
			return (*this);
		}
		
		/// @brief Moves the iterator to the next node and returns a copy of the iterator before the move
		/// @return A copy of the iterator before the move
		Iterator operator++(int)
		{
			Iterator result(this);
			MoveNext();
			return result;
		}
		
		/// @brief Returns the value the iterator references
		/// @return The value
		ValueType &operator*()
		{
			return (_node != NULL)?dynamic_cast<Node*>(_node)->Value:ValueType();
		}
	};
	
	/// @brief The constant iterator of the class
	struct ConstIterator : public Iterator
	{
		/// @brief Returns the readonly value the iterator references
		/// @return The value
		const ValueType &operator*()
		{
			return (_node != NULL)?dynamic_cast<Node*>(_node)->Value:ValueType();
		}
	};	

	/// @brief Options when adding duplicate item
	struct AddStrategy
	{
		enum Enum
		{
			ReplaceExisting,
			ReturnFalseOnExisting,
			AddDuplicate
		};
	};	
protected:
	/// @brief The number of items
	int _count;
	
	/// @brief The number of bits needed at minimum to address a bucket in the table, corresponding to table size
	int _tableIndexBits;

	/// @brief The mutex used to make code reentrant
	std::mutex _mutex;

private:
	TDisposer _disposer;

protected:	// it's an abstract class so we make its constructor non-public
	/// @brief Instantiates a SoHash
	SoHash() : _count(0), _tableIndexBits(1)
	{
	}

	/// @brief Instantiates a SoHash with the specified disposer
	/// @param disposer The functor that finalizes the value
	SoHash(const TDisposer &disposer) : _disposer(disposer), _count(0), _tableIndexBits(1)
	{
	}

public:

	/// @brief destructor
	virtual ~SoHash()
	{
	}

public:	// properties
	/// @brief Returns the number of total items the hash contains
	/// @return The number of items
	int GetCount() const
	{
		return _count;
	}

	/// @brief Returns the number of bits required at minimum to represent a table index
	/// @return The number of bits required
	int GetTableIndexBits() const
	{
		return _tableIndexBits;
	}

public:
	/// @brief Returns the iterator to the first non-dummy item
	/// @return The iterator
	Iterator GetBegin()
	{
		Iterator begin(GetBucket(0));
		return ++begin;
	}

	/// @brief Returns the iterator to the tail (NULL)
	/// @return The iterator
	Iterator GetEnd()
	{
		return Iterator(NULL);
	}

	/// @brief Returns the iterator the first non-dummy item
	/// @return The iterator
	ConstIterator GetBegin() const
	{
		ConstIterator begin(GetBucket(0));
		return ++begin;
	}

	/// @brief Returns the iterator to the tail (NULL)
	/// @return The iterator
	ConstIterator GetEnd() const
	{
		return ConstIterator(NULL);
	}

	/// @brief Adds a key value pair to the hash table
	/// @param key The numeric key to the value
	/// @param value The value associated with the key
	/// @param addStrategy How to deal with duplication
	/// @return true if the pair is added
	bool AddKeyValuePair(KeyType key, ValueType value, enum AddStrategy::Enum addStrategy=AddStrategy::ReplaceExisting)
	{
		KeyType soDummyKey = Reverse(key);
		KeyType soKey = soDummyKey | 0x1;
		Node *node = new Node(soKey, value);

		// lock
		std::lock_guard<std::mutex> lock(_mutex);
		
		int indexBucket = (int) (key & ((KeyType) GetTableSize() - 1));
		BaseNode *cp = GetBucket(indexBucket);
		if (cp == NULL)
		{
			cp = InitializeBucket(indexBucket);
		}
		for (; cp->Next != NULL && cp->Next->Key < soKey; cp = cp->Next)
        {
        }
		if (cp->Next != NULL && cp->Next->Key == soKey)
		{
			switch (addStrategy)
            {
			case AddStrategy::ReplaceExisting:
                ((Node*)cp->Next)->Value = value;
                return true;
			case AddStrategy::ReturnFalseOnExisting:
                return false;
            }
		}

		node->Next = cp->Next;
        cp->Next = node;

        _count++;

        ExpandIfNeeded();
		
		return true;
		// unlock
	}

	/// @brief Removes all the contents of the hash and reinitializes it
	void Clear()
	{
		// lock
		std::lock_guard<std::mutex> lock(_mutex);

		BaseNode *cp = GetBucket(0);
        if (cp == NULL) return;

		BaseNode *cpNext;
		for (; cp != NULL; cp = cpNext)
        {
			cpNext = cp->Next;
			Node *node = dynamic_cast<Node*>(cp);
			if (node != NULL)
			{
				_disposer(node->Value);
			}
			delete cp;
        }

        ResetBuckets();
        _tableIndexBits = 1;
        _count = 0;
		// unlock
	}

	/// @brief Gets the first item with the key
	/// @param key The key to find the item with
	/// @param ppValue To return the pointer to the pointer to the value. Pass in NULL to ignore the retrieval
	/// @param pSoKey The SOkey of the item
	/// @return true if found or false
	bool FindFirst(KeyType key, ValueType **ppValue=NULL, KeyType *pSoKey=NULL) const
	{
		KeyType soKey;
		Node *node = _FindFirstPtr(key, soKey);
		if (pSoKey != NULL)
		{
			*pSoKey = soKey;
		}
        if (node == NULL)
        {
            return false;
        }
		if (ppValue != NULL)
		{
			*ppValue = &node->Value;
		}
        return true;
	}

	/// @brief Gets the first item with the key
	/// @param key The key to find the item with
	/// @param values All the values with the key (multiple values if duplicate values allowed)
	/// @return true if at least one is found or false
	bool Find(KeyType key, std::vector<ValueType> &values) const
	{
		KeyType soKey;
		Node *cp = _FindFirstPtr(key, soKey);
		values.clear();
		for (; cp != NULL && cp->Key == soKey; cp = cp->Next)
        {
			values.push_back(dynamic_cast<Node*>(cp)->Value);
        }
		
		return (values.size() > 0);
	}
	
	/// @brief Gets the iterator to the first item with the key
	/// @param key The key to find the item with
	/// @param pSoKey The SOkey of the item
	/// @return The iterator
	Iterator FindFirst(KeyType key, KeyType *pSoKey=NULL)
	{
		KeyType soKey;
		Iterator iter = _FindFirstIter(key, soKey);
		if (pSoKey != NULL)
		{
			*pSoKey = soKey;
		}
		return (*iter);
	}
	
	/// @brief Gets the constant to the first item with the key
	/// @param key The key to find the item with
	/// @param pSoKey The SOkey of the item
	/// @return The iterator
	ConstIterator FindFirst(KeyType key, KeyType *pSoKey=NULL) const
	{
		KeyType soKey;
		ConstIterator iter = _FindFirstCIter(key, soKey);
		if (pSoKey != NULL)
		{
			*pSoKey = soKey;
		}
		return (*iter);
	}

	/// @brief Deletes all the items with the specified key
	/// @param key The key to delete items with
	/// @return The number of items deleted
	int DeleteKey(KeyType key)
	{
		AllwaysTruePredicate alwaysTrue;
		return DeleteKeyValuePairs<AllwaysTruePredicate&>(key, alwaysTrue);
	}

	/// @brief Delete all the items with the specified key and satisfying the predicate
	/// @param key The key to delete items with
	/// @param isTarget The predicate to determine if the item with the key should be deleted
	/// @return The number of items deleted
	template <class TPredicate>
	int DeleteKeyValuePairs(KeyType key, TPredicate isTarget)
	{
		KeyType soDummyKey = Reverse(key);
        KeyType soKey = soDummyKey | 0x1;

        // lock
		std::lock_guard<std::mutex> lock(_mutex);
        
        int indexBucket = (int) (key & ((KeyType) GetTableSize() - 1));
        BaseNode *cp = GetBucket(indexBucket);
        if (cp == NULL) return 0;

        int numDeleted = 0;

        for (; cp->Next != NULL && cp->Next->Key < soKey; cp = cp->Next)
        {
        }

        for (; cp->Next != NULL && cp->Next->Key == soKey;)
        {
            Node* toDelete = dynamic_cast<Node*>(cp->Next);	// note the key ensures that it's of Node type
            if (isTarget(toDelete->Value))
            {
                cp->Next = toDelete->Next;
				_disposer(toDelete->Value);
                delete toDelete;
                numDeleted++;
                _count--;
            }
            else
            {
                cp = cp->Next;
            }
        }
        return numDeleted;
        // unlock
	}

protected:	// pure virtual (abstract) methods

	/// @brief Gets the specified bucket of the bucket table for the hash algorithm to access
	/// @param index The index of the bucket
	virtual BaseNode * GetBucket(int index) const = 0;
	
	/// @brief Sets node for the specified bucket of the bucket table for the hash algorithm to access
	/// @param index The index of the bucket
	/// @param bucket The node to set to the specified bucket
	virtual void SetBucket(int index, BaseNode *bucket) = 0;

	/// @brief Sets the base node for the bucket that has been created by doubling the bucket table
	virtual void AddBucket(int index, BaseNode *node) = 0;

	/// @brief Calls the Double() method if the implementation reckons it should
	virtual void ExpandIfNeeded() = 0;

	/// @brief The size of the bucket table. It's provided by the implementer 
	/// @return The table size
	/// @remarks The current design requires it to start at 2 and double only after a double (expansion) operation, 
	///          But there's no restriction on the allocation of the internal memory for the bucket table
	virtual int GetTableSize() const = 0;
	
	/// @brief Resets the buckets to the initial state, normally part of the clear up process
	virtual void ResetBuckets() = 0;

protected:

	/// @brief Find the first item with the specified key
	/// @param key The key to find the item with
	/// @param soKey the SO-key of the item corresponding to the key
	/// @return The first node with the key
	Node* _FindFirstPtr(KeyType key, KeyType &soKey) const
	{
		int indexBucket = (int)(key & ((KeyType)GetTableSize() - 1));
        KeyType soDummyKey = Reverse(key);
        soKey = soDummyKey | 0x1;

        BaseNode *cp = GetBucket(indexBucket);
		if (cp == NULL) return NULL;

        for (; cp != NULL && cp->Key < soKey; cp = cp->Next)
        {
        }

        if (cp != NULL && cp->Key == soKey)
        {
            return dynamic_cast<Node*>(cp);
        }
        return NULL;
	}
	
	/// @brief Return the iterator to the first item with the specified key
	/// @param key The key to find the item with
	/// @param soKey the SO-key of the item corresponding to the key
	/// @return The iterator to the first item that matches
	Iterator _FindFirstItr(KeyType key, KeyType &soKey) const
	{
		Node *cp = _FindFirstPtr(key, soKey);
		return Iterator(cp);
	}
	
	/// @brief Return the constant iterator to the first item with the specified key
	/// @param key The key to find the item with
	/// @param soKey the SO-key of the item corresponding to the key
	/// @return The constant iterator to the first item that matches
	ConstIterator _FindFirstCItr(KeyType key, KeyType &soKey) const
	{
		Node *cp = _FindFirstPtr(key, soKey);
		return ConstIterator(cp);
	}

	/// @brief Returns the parent of the specified bucket
	/// @param indexBucket the index of the bucket to return the parent of
	/// @return The index of the parent of the bucket
	int GetParent(int indexBucket) const
    {
        int bval[] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
        unsigned int x = (unsigned int) indexBucket;
        int r = -1; // this implementation can handle 0 bucket index
        if ((x & 0xFFFF0000) != 0)
        {
            r += 16/1;
            x >>= 16/1;
        }
        if ((x & 0x0000FF00) != 0)
        {
            r += 16/2;
            x >>= 16/2;
        }
        if ((x & 0x000000F0) != 0)
        {
            r += 16/4;
            x >>= 16/4;
        }
        r += bval[x];
        unsigned int mask = 1U << r;	// NOTE left shift by -1 equates right shift by 1
        indexBucket = (int) (((unsigned int) indexBucket) & ~mask);
        return indexBucket;
    }

	/// @brief Doubles the bucket table
	void Double()
	{
        // expands the bucket list
        int oldSize = GetTableSize();
        for (int i = 0; i < oldSize; i++)
        {
            BaseNode *cp = GetBucket(i);
            if (cp == NULL)
            {   // parent uninitialized
                AddBucket(oldSize + i, NULL); 
                continue; 
            }
            KeyType msb = cp->Key >> (32 - _tableIndexBits);
            KeyType testbit = 1 << (31 - _tableIndexBits);
            for (; cp->Next != NULL; cp = cp->Next)
            {
                if ((cp->Next->Key & testbit) != 0 || cp->Next->Key >> (32 - _tableIndexBits) != msb)
                {
                    break;
                }
            }
            if (cp->Next != NULL && cp->Next->Key >> (32 - _tableIndexBits) == msb)
            {
                BaseNode * dummyNode = new BaseNode((cp->Next->Key >> (31 - _tableIndexBits)) << (31 - _tableIndexBits));

                AddBucket(oldSize + i, dummyNode);

                // this order to ensure readers are unaffected
                dummyNode->Next = cp->Next;
                cp->Next = dummyNode;
            }
            else
            {
                AddBucket(oldSize + i, NULL);
            }
        }

        _tableIndexBits++;
	}

	/// @brief Initialise a non-initialized (with a null pointer) bucket
	/// @param indexBucket The index of the bucket
	/// @return A dummy node the bucket now points to
	BaseNode * InitializeBucket(int indexBucket)
	{
		KeyType soDummyKey = Reverse((KeyType)indexBucket);
        BaseNode * dummyNode;
        if (indexBucket == 0)
        {
            // NOTE the bucket 0 is always by itself or recursively initialised before any other buckets
            dummyNode = new BaseNode(soDummyKey);
        }
        else
        {
            int indexParent = GetParent(indexBucket);
			BaseNode *parent = GetBucket(indexParent);
			if (parent == NULL)
			{
				parent = InitializeBucket(indexParent);
			}
            dummyNode = new BaseNode(soDummyKey);
            ListInsert(dummyNode, parent);
        }
		SetBucket(indexBucket, dummyNode);
        return dummyNode;
	}

	/// @brief Inserts the node at the appropriate position after 'start'
	/// @param The node to insert
	/// @param The starting point guaranteed to be before where the node is to be inserted
	/// @return Inserted node if insertion is successful or the existing node
	BaseNode * ListInsert(BaseNode * node, BaseNode * start)
    {
        // move to the one after the insertion point
        for (; start->Next != NULL && start->Next->Key < node->Key; start = start->Next)
        {
        }
        if (start->Next != NULL && start->Next->Key == node->Key)
        {
            return start->Next;
        }
        node->Next = start->Next;
        start->Next = node;
            
        return node;
    }

	/// @brief returns the bit-reversal of the specified key
	/// @param The key to bit-reverse
	/// @return The bit-reversal of the key
	/// @remarks The current implementation is based on the 3-operation approach from
	///   1. http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
    ///   2. http://stackoverflow.com/questions/1688532/how-to-reverse-bits-of-a-byte
	KeyType Reverse(KeyType key) const
	{
		KeyType b0 = key & 0xff;
        KeyType b1 = (key >> 8) & 0xff;
        KeyType b2 = (key >> 16) & 0xff;
        KeyType b3 = (key >> 24) & 0xff;

        // reverse the bytes
        if (b0 != 0)
        {
            b0 = (KeyType) ((b0*0x0202020202UL & 0x010884422010UL)%1023);
        }
        if (b1 != 0)
        {
            b1 = (KeyType) ((b1*0x0202020202UL & 0x010884422010UL)%1023);
        }
        if (b2 != 0)
        {
            b2 = (KeyType) ((b2*0x0202020202UL & 0x010884422010UL)%1023);
        }
        if (b3 != 0)
        {
            b3 = (KeyType) ((b3*0x0202020202UL & 0x010884422010UL)%1023);
        }

        return ((b0 << 24) | (b1 << 16) | (b2 << 8) | b3);
	}
};

template <class TValue, class TDisposer=DefaultDisposer<TValue>>
class SoHashLinear : public SoHash<TValue, TDisposer>
{
private:
	typedef SoHash<TValue, TDisposer> Base;

private:
	BaseNode **_buckets;

	float _maxLoad;

	int _tableSize;

public:
	SoHashLinear(float maxLoad) : _buckets(NULL), _maxLoad(maxLoad)
	{
		ResetBuckets();
	}

	SoHashLinear(float maxLoad, TDisposer disposer) : Base(disposer), _buckets(NULL), _maxLoad(maxLoad)
	{
		ResetBuckets();
	}

	virtual ~SoHashLinear()
	{
		Clear();
		delete _buckets;
	}
 	
public:
	float GetMaxLoad() const
	{
		return _maxLoad;
	}

protected: 	// SoHash<TValue> members

	/// @brief Gets the specified bucket of the bucket table for the hash algorithm to access
	/// @param index The index of the bucket
	virtual BaseNode *GetBucket(int indexBucket) const
	{
		return _buckets[indexBucket];
	}
	
	/// @brief Sets node for the specified bucket of the bucket table for the hash algorithm to access
	/// @param index The index of the bucket
	/// @param bucket The node to set to the specified bucket
	virtual void SetBucket(int indexBucket, BaseNode *node)
	{
		_buckets[indexBucket] = node;
	}

	/// @brief The size of the bucket table; it's provided by the implementor however it should always hold that
    ///        it starts at 2 and doubles only after a CAS
	/// @return The table size
	virtual int GetTableSize() const
	{
		return _tableSize;
	}

	/// @brief Calls the Double() method if the implementation reckons it should
	virtual void ExpandIfNeeded()
	{
		if (GetCount()<=GetMaxLoad()*GetTableSize())
		{
			return;
		}

		// NOTE this pre-allocates memory which is essential and doesn't increase the TableSize
		_buckets = (BaseNode**)realloc(_buckets, sizeof(*_buckets)*GetTableSize()*2);

		// Note all the new buckets have been committed by the Double() method
		// That's why _tableSize is by definition to be doubled and the buckets array was not initialized to zero
		Double();

		_tableSize *= 2;
	}

	/// @brief Adds a node to the specified bucket as part of the CAS expanding process;
	///        The adding is always starting from the previous table end and performed in order.
	/// @param indexBucket The location of the bucket (in some implementation might be ignored
	virtual void AddBucket(int indexBucket, BaseNode *node)
    {
    	_buckets[indexBucket] = node;
    }

    /// <summary>
    ///  sets buckets to initial state after clear up the contents of the hash
    /// </summary>
    virtual void ResetBuckets()
    {
		_tableSize = 2;
        _buckets = (BaseNode **)realloc(_buckets, sizeof(*_buckets)*_tableSize);
		_buckets[0] = NULL;
		_buckets[1] = NULL;
    }
};

}}}

#endif
