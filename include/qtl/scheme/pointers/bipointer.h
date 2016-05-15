#if !defined (_BIPOINTER_H_)
#define _BIPOINTER_H_

#include <unordered_set>

using namespace std;

namespace Qtl { namespace Scheme { namespace Pointers {

	class BiPointedObject;

	class BackRef
	{
	private:
		// when the pointer is on the stack frame, this is null
		BiPointedObject * _source = nullptr;

		friend class BiPointedObject;
		
	public:
		BackRef(BiPointedObject *source)
		{
			_source = source;
		}

		bool operator==(const BackRef &other) const
		{
			return _source == other._source;
		}
		
	public:
		BiPointedObject * GetSourceObject() const { return _source ; }
	};
	
	class GarbageCollector
	{
	public:
		static GarbageCollector Instance;
		
	private:
		unordered_set<BiPointedObject*> _checkedObjects;
	
	public:
		int ClearThreshold = 32;
		
	public:
		void DisposeAll();

		~GarbageCollector()
		{
			DisposeAll();
		}
	
		void AddToCheck(BiPointedObject *obj)
		{
			if (_checkedObjects.size() < ClearThreshold)
			{
				_checkedObjects.insert(obj);
			}
			else
			{
				DisposeAll();
			}
		}
	};
	
	class BiPointedObject
	{
	private:
		typedef unordered_set<BackRef, _Bitwise_hash<BackRef>> InboundSet;

	private:
		bool _visitFlag;
		InboundSet _inbound;

		friend class BackRef;
		friend class BiPointerCore;
		
	private:		
		void DeleteThis()
		{
			delete this;
		}
		
		bool QuickCheck()
		{
			if (_inbound.size() == 0)
			{
				DeleteThis();
				return true;
			}
			return false;
		}
		
		bool IsStackFrameBased()
		{
			for (InboundSet::iterator itr = _inbound.begin(); itr !=
				_inbound.end(); ++itr)
			{
				const BackRef &ib = *itr;
				if (IsStackFrameBased(ib)) return true;
			}
			return false;
		}
			
		void RemoveInbound(BackRef &bp)
		{
			_inbound.erase(bp);
			if (!QuickCheck())
			{
				GarbageCollector::Instance.AddToCheck(this);
			}
		}
				
		void AddInbound(BackRef &bp)
		{
			_inbound.insert(bp);
		}

		static void ClearVisitFlags(const BackRef &ib)
		{
			if (ib._source != nullptr && ib._source->_visitFlag)
			{
				ib._source->ClearVisitFlags();
			}
		}

		bool IsStackFrameBased(const BackRef &ib) const
		{
			if (ib._source == nullptr) return true;
			ib._source->_visitFlag = true;
			for (InboundSet::iterator bpcitr = ib._source->_inbound.begin(); bpcitr != ib._source->_inbound.end(); ++bpcitr)
			{
				const BackRef &ib1 = *bpcitr;
				if (IsStackFrameBased(ib1)) return true;
			}
			return false;
		}

	protected:
		void ClearVisitFlags()
		{
			_visitFlag = false;
			for (InboundSet::iterator itr = _inbound.begin(); itr != _inbound.end(); ++itr)
			{
				const BackRef &ib = *itr;
				ClearVisitFlags(ib);
			}
		}
		
	public:
		virtual ~BiPointedObject()
		{

		}

		void UpdateIfIsolated()
		{
			bool isolated = CheckIfIsolated();
			if (isolated)
			{
				DeleteThis();
			}
		}
		
		bool CheckIfIsolated() 
		{
			if (_inbound.size() == 0) return true;
			bool isolated = !IsStackFrameBased();
			ClearVisitFlags();
			return isolated;
		}
	};

	class BiPointerCore
	{
	protected:
		BiPointedObject *_target;

		BiPointerCore() : _target(nullptr)
		{

		}

	public:
		void Link(BiPointedObject *source, BiPointedObject *target)
		{
			if (_target == target) return;
			Release(source);
			_target = target;
			if (_target != nullptr)
			{
				_target->AddInbound(BackRef(source)); // move constructor?
			}
		}

		void Release(BiPointedObject *source) 
		{
			if (_target != nullptr)
			{
				_target->RemoveInbound(BackRef(source));
			}
		}
	};

	template <class T>
	class BiPointer : public BiPointerCore
	{
	private:
		BiPointedObject *_source;

	public:
		BiPointer()
			: _source(nullptr)
		{

		}

		BiPointer(BiPointedObject *target)
			: _source(nullptr)
		{
			Link(_source, target);
		}

		~BiPointer()
		{
			Release(_source);
		}

		void SetSource(BiPointedObject *source)
		{
			_source = source;
		}

		void operator=(BiPointer<T> &other) 
		{
			Link(_source, other._target);
		}

		template <class T2>
		void operator=(BiPointer<T2> &other)
		{
			Link(_source, other._target);
		}

		void operator=(BiPointedObject &other) 
		{
			Link(_source, &other);
		}

		void operator=(BiPointedObject *other)
		{
			Link(_source, other);
		}

		T * operator->() const
		{
			return static_cast<T*>(_target);
		}
	};

	// TODO move to CPP
	void GarbageCollector::DisposeAll()
	{
		for (unordered_set<BiPointedObject*>::iterator itr = _checkedObjects.begin(); itr != _checkedObjects.end(); ++itr)
		{
			BiPointedObject *obj = *itr;
			obj->UpdateIfIsolated();
		}
		_checkedObjects.clear();
	}

	GarbageCollector GarbageCollector::Instance;
}}}

#endif

