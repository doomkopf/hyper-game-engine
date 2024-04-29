#ifndef STACK_H_
#define STACK_H_

template <class T>
class CStack // LIFO
{
public:
	typedef T *TP;

	CStack(int s)
	{
		buf = NULL;
		p = 0;

		SetSize(s);
	}

	~CStack()
	{
		Destruct();
	}

	void Destruct()
	{
		if(buf)
		{
			for(int i = 0; i < size; i++)
			{
				if(buf[i])
				{
					delete buf[i];
					buf[i] = NULL;
				}
			}

			delete [] buf;
			buf = NULL;
		}

		p = 0;
		size = 0;
	}

	void SetSize(int s)
	{
		Destruct();
		size = s;
		buf = new TP[size];
		for(int i = 0; i < size; i++)
			buf[i] = NULL;
	}

	void Push(const T &d)
	{
		if(buf[p])
			delete buf[p];
		buf[p] = new T(d);
		p++;
		if(p >= size)
			p = 0;
	}

	void Push(T *d)
	{
		if(buf[p])
			delete buf[p];
		buf[p] = d;
		p++;
		if(p >= size)
			p = 0;
	}

	void Pop(T *d)
	{
		p--;
		if(p < 0)
			p = size - 1;
		if(buf[p])
		{
			*d = *buf[p];
			delete buf[p];
			buf[p] = NULL;
		}
	}

	void Pop(T **d)
	{
		p--;
		if(p < 0)
			p = size - 1;
		if(buf[p])
		{
			*d = buf[p];
			buf[p] = NULL;
		}
	}

	TP *GetBuffer() const
	{
		return buf;
	}

	int GFDI() const // GetFirstDataIndex
	{
		if(!buf[p])
		{
			if(!buf[(p <= 0) ? size - 1 : p - 1])
				return -1;
		}

		return (p <= 0) ? size - 1 : p - 1;
	}

	int GNDI(int i) const // GetNextDataIndex
	{
		if(!buf[(i <= 0) ? size - 1 : i - 1])
			return -1;

		if(i == p)
			return -1;

		return (i <= 0) ? size - 1 : i - 1;
	}

protected:
	int size, p;
	TP *buf;
};

#endif