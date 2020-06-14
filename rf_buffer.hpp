#include <vector>
#include <stack>

using namespace std;

//
// Implementation of a recycled RF buffer
//
// Used for storing RF that is transmitted in a buffer.
// The buffer can be used to simulate the delay that
// occurs when a signal travels through space.
// The buffer is implemented using recycleble vectors.
// The purpose of recyclable vectors is to allow re-use
// of vectors to optimize the chance of cache hits 
// and to create vectors with the correct size.
//


// Recycling Bin Implementation
// uses singleton design pattern
template<typename T>
class RFBufferRecyclingBinData
{

	stack<vector<T>> vectors;
	int is_empty;

public:

	RFBufferRecyclingBinData()
	{
		is_empty = 1;
	}

	void add_vector(vector<T> in_vect)
	{
		vectors.push(move(in_vect));
		is_empty = 0;
	}

	vector<T> get_vector()
	{		
		vector<T> temp = vectors.top();
		vectors.pop();
		if (vectors.empty())
			is_empty = 1;
		return temp;
	}

	int check_is_empty()
	{
		return is_empty;
	}
};

template<typename T>
class RFBufferRecyclingBin
{
	static RFBufferRecyclingBinData<T>* data;

	RFBufferRecyclingBin();

public:

	static RFBufferRecyclingBinData<T>* get_instance(){
		if (data == NULL){
			data = new RFBufferRecyclingBinData<T>();
		} 
		return data;
	}
};

template<typename T>
RFBufferRecyclingBinData<T>* RFBufferRecyclingBin<T>::data = NULL;

template<typename T>
class RecycledRFBuffer
{

	vector<T> vect;
	RFBufferRecyclingBinData<T> * recycling_bin;

public:

	RecycledRFBuffer()
	{
		// check if vectors exist in the recycling bin
		// if yes, then reuse vector
		recycling_bin = RFBufferRecyclingBin<T>::get_instance();
		if (recycling_bin->check_is_empty() == 0)
		{
			vect = recycling_bin->get_vector();
			vect.resize(0);
		}
	}

	T operator[](int idx ){ return vect[idx]; }

	void push_back(T val){ vect.push_back(val); }

	T back() { return vect.back(); }
    void resize(int new_size){ return vect.resize(new_size); }
	int size() { return vect.size(); }
	int capacity() { return vect.capacity(); }

	~RecycledRFBuffer(){ recycling_bin->add_vector(vect); }

};
