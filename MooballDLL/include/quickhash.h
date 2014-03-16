
#ifndef __QHASH_H_
#define __QHASH_H_

#include <vector>


template <class T>
class CHashTable
{
	struct ItemStruct
	{
		std::string indexName;
		T data;
	};

	public:

		void Clear()
		{
			stack.clear();
		}

		long Size()
		{
			return stack.size();
		}

		void Add(std::string indexName, T data)
		{
			for(size_t i=0; i<stack.size(); i++)
				if(stack[i].indexName == indexName)
				{
					stack[i].data=data;
					return;
				}
				ItemStruct obj;
				obj.indexName=indexName;
				obj.data = data;
				stack.push_back(obj);
		}

		std::string GetIndexName(long index)
		{
			if(index < 0) index=0;
			return stack[index].indexName;
		}

		T  &operator [] (long index)
		{
			if(index < 0) index=0;
			return stack[index].data;
		}

		T &operator [](std::string indexName)
		{

			for(size_t i=0; i<stack.size(); i++)
				if(stack[i].indexName == std::string(indexName))
					return   stack[i].data;

			long idx = stack.size();
			ItemStruct obj;
			obj.indexName=indexName;
			stack.push_back(obj);
			return stack[idx].data;
		}


	private:
		std::vector <ItemStruct> stack;

};


#endif
