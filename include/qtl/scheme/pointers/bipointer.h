#if !defined (_BIPOINTER_H_)
#define _BIPOINTER_H_

#include <hash_set>

using namespace std;

namespace Qtl { namespace Scheme { namespace Pointers {

	class BiPointedObject;
	
	class BiPointerCore
	{
	private:
		// when the pointer is on the stack frame, this is null
		BiPointedObject * _source = nullptr;
		BiPointedObject * _target = nullptr;
		
		friend class BiPointedObject;
		
	public:
		~BiPointerCore()
		{
			if (_target != nullptr)
			{
				_target->RemoveInbound(*this);
			}
		}
		
		bool IsStackFrameBased() const
		{
			if (_source == nullptr) return true;
			_source->_visitFlag = true;
			for (hash_set<BiPointerCore>::iterator bpcitr = _source->_inbound.begin(); bpcitr != _source->_inbound.end(); ++bpcitr)
			{
				const BiPointerCore &ib = *bpcitr;
				if (ib.IsStackFrameBased()) return true;
			}
			return false;
		}
		
		void ClearVisitFlags() const
		{
			if (_source != nullptr && _source->_visitFlag)
			{
				_source->ClearVisitFlags();
			}
		}
	
	public:
		template <class T> T & GetSource() const { return _source->getContent(); }
		template <class T> const T &  GetSource() const { return _source->getContent(); }
		template <class T>  & GetTarget() const { return _target->getContent(); }
		template <class T>  T &  GetTarget() const { return _target->getContent(); }
		
		template <class T> void SetSource(T &t) { t.AddOutbound(*this); }
		template <class T> void SetTarget(T &t) { t.AddInbound(*this);}
	};
	
	class GarbageCollector
	{
	public:
		static GarbageCollector Instance;
		
	private:
		hash_set<BiPointedObject*> _checkedObjects;
	
	public:
		int ClearThreshold = 32;
		
	public:
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
		
		void DisposeAll()
		{
			for (hash_set<BiPointedObject*>::iterator itr = _checkedObjects.begin(); itr != _checkedObjects.end(); ++itr)
			{
				BiPointedObject *obj = *itr;
				obj->UpdateIfIsolated();
			}
			_checkedObjects.clear();
		}
	};
	
	class BiPointedObject
	{
	private:
		// TODO can change this to set
		hash_set<BiPointerCore> _inbound;
		hash_set<BiPointerCore> _outbound;
		bool _visitFlag;
		
		friend class BiPointerCore;
		
	private:
		void DeleteThis()
		{
			delete this;
		}
		
		void QuickCheck()
		{
			if (_inbound.size() == 0)
			{
				DeleteThis();
			}
		}
		
		bool IsStackFrameBased()
		{
			for (hash_set<BiPointerCore>::iterator itr = _inbound.begin(); itr != 
				_inbound.end(); ++itr)
			{
				const BiPointerCore &ib = *itr;
				if (ib.IsStackFrameBased()) return true;
			}
			return false;
		}
			
		void ClearVisitFlags()
		{
			_visitFlag = false;
			for (hash_set<BiPointerCore>::iterator itr = _inbound.begin(); itr != _inbound.end(); ++itr)
			{
				const BiPointerCore &ib = *itr;
				ib.ClearVisitFlags();
			}
		}

		void RemoveInbound(BiPointerCore &bp)
		{
			_inbound.erase(bp);
			QuickCheck();
			GarbageCollector::Instance.AddToCheck(this);
		}
		
		void AddOutbound(BiPointerCore &bp)
		{
			_outbound.insert(bp);
		}
		
		void AddInbound(BiPointerCore &bp)
		{
			_inbound.insert(bp);
		}
		
	public:
		//virtual T & GetContent<T>() = 0;
		//virtual const T & GetContent<T>() const = 0;
		
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
	/*
	template <class T> class BiPointer : private BiPointerCore
	{
		
	}*/
}}}

#endif

