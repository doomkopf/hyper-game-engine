#ifndef LIST_H_
#define LIST_H_

#ifndef NULL
#define NULL 0
#endif

#ifdef CList
#undef CList
#endif

template <class T>
class CList;

template <class T>
class CRootNode;

template <class T>
class CNode
{
	friend class CList;
	friend class CRootNode;
public:
	CNode()
	{
		data = NULL;
		next = NULL;
		prev = NULL;
	}

	~CNode()
	{
		Destruct();
	}

	void Destruct()
	{
		if(data)
		{
			delete data;
			data = NULL;
		}
		if(next)
		{
			delete next;
			next = NULL;
		}
		prev = NULL;
	}

	void Add(const T &d)
	{
		if(!data)
			data = new T(d);
		else
		{
			if(!next)
			{
				next = new CNode<T>;
				next->prev = this;
			}
			next->Add(d);
		}
	}

	void Add(T *d)
	{
		if(!data)
			data = d;
		else
		{
			if(!next)
			{
				next = new CNode<T>;
				next->prev = this;
			}
			next->Add(d);
		}
	}

	virtual void Delete()
	{
		if(data)
		{
			delete data;
			data = NULL;
		}
		if(next)
			next->prev = prev;
		if(prev)
			prev->next = next;

		next = NULL;
		prev = NULL;

		delete this;
	}

	T *GetData() const
	{
		return data;
	}

	void SetData(T *d)
	{
		data = d;
	}

	CNode<T> *next, *prev;

protected:
	T *data;
};

template <class T>
class CRootNode : public CNode<T>
{
public:
	virtual void Delete()
	{
		if(data)
		{
			delete data;
		}

		if(next)
		{
			CNode<T> *n = next;

			data = next->data;
			if(next->next)
				next->next->prev = this;
			next = next->next;

			n->data = NULL;
			n->next = NULL;
			n->prev = NULL;

			delete n;
		}
		else
		{
			data = NULL;
		}
	}
};

template <class T>
class CList
{
public:
	CList()
	{
		node = NULL;
	}

	CList(const CList<T> &l)
	{
		node = NULL;
		Copy(l);
	}

	CList &operator=(const CList<T> &l)
	{
		Copy(l);

		return *this;
	}

	void Copy(const CList<T> &l)
	{
		Clear();
		for(CNode<T> *n = l.GFDN(); n; n = n->next)
			Add(*n->data);
	}

	~CList()
	{
		Clear();
	}

	void Clear()
	{
		if(node)
		{
			delete node;
			node = NULL;
		}
	}

	void Add(const T &d)
	{
		if(!node)
			node = new CRootNode<T>;
		node->Add(d);
	}

	void Add(T *d)
	{
		if(!node)
			node = new CRootNode<T>;
		node->Add(d);
	}

	void AddList(const CList<T> &l)
	{
		for(CNode<T> *n = l.GFDN(); n; n = n->next)
			Add(*n->data);
	}

	int GetCount() const
	{
		int c = 0;

		for(CNode<T> *n = GFDN(); n; n = n->next)
			c++;

		return c;
	}

	CNode<T> *GFDN() const
	{
		if(node)
		{
			if(node->data)
				return node;
		}

		return NULL;
	}

	CNode<T> *GLDN() const
	{
		for(CNode<T> *n = GFDN(); n; n = n->next)
		{
			if(!n->next)
				return n;
		}

		return NULL;
	}

	void Reverse()
	{
		CNode<T> *n1, *n2;
		T *t;
		int c, i;

		c = GetCount();

		if(c < 2)
			return;

		c /= 2;

		for(i = 0, n1 = GFDN(), n2 = GLDN(); i < c ; n1 = n1->next, n2 = n2->prev, i++)
		{
			t = n1->data;
			n1->data = n2->data;
			n2->data = t;
		}
	}

	int GetIndex(T *d) const
	{
		int i = 0;
		for(CNode<T> *n = GFDN(); n; n = n->next, i++)
		{
			if(n->GetData() == d)
				return i;
		}

		return -1;
	}

	T *GetData(int idx) const
	{
		int i = 0;
		for(CNode<T> *node = GFDN(); node; node = node->next, i++)
		{
			if(i == idx)
				return node->GetData();
		}

		return NULL;
	}

	CNode<T> *node;
};

#endif