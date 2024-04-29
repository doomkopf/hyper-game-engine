#ifndef FASTQUEUE_H_
#define FASTQUEUE_H_

// data types with dynamic instances must have copy constructor and the =operator overloaded in order to work with this class
template <class T>
class CFastQueue // FIFO
{
public:
	CFastQueue(int s)
	{
		size = s;
		p = 0;
		f = 0;
		count = 0;
		data = new T[size];
	}

	~CFastQueue()
	{
		Destruct();
	}

	void Destruct()
	{
		delete [] data;
	}

	void Add(const T &d)
	{
		if(count < size)
			count++;
		else
		{
			if(f >= size - 1)
				f = 0;
			else
				f++;
		}

		if(p >= size)
			p = 0;
		data[p++] = d;
	}

	bool Get(T *d)
	{
		if(count < 1)
			return false;
		count--;

		if(f >= size)
			f = 0;
		*d = data[f++];

		return true;
	}

	int count;

protected:
	T *data;
	int size, p, f;
};

#endif