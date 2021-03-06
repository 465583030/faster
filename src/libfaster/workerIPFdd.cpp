#include <tuple>
#include <iostream>

#include "_workerIFdd.h"
#include "indexedFddStorageExtern.cpp"
#include "fastComm.h"

// -------------------------- worker<K,T*> specialization -------------------------- //

// REDUCE
template <typename K, typename T>
std::tuple<K,T*,size_t> faster::_workerIFdd<K,T*>::reduce (IPreduceIPFunctionP<K,T> reduceFunc){
	T ** d = this->localData->getData();
	std::tuple<K,T*,size_t>  resultT;
	size_t s = this->localData->getSize();
	size_t * ils = this->localData->getLineSizes();
	K * ik = this->localData->getKeys();
	//std::cerr << "START " << id << " " << s << " | ";

	#pragma omp parallel 
	{
		int nT = omp_get_num_threads();
		int tN = omp_get_thread_num();
		std::tuple<K,T*,size_t>  partResult (ik[tN], d[tN], ils[tN]);
		T * b, * a;
		size_t aSize, bSize;
		K aKey, bKey;

		#pragma omp for 
		for (int i = nT; i < s; ++i){
			aKey = std::get<0>(partResult);
			a = std::get<1>(partResult);
			aSize = std::get<2>(partResult);
			b = d[i];
			bKey = ik[i];
			bSize = ils[i];

			partResult = reduceFunc( aKey, a, aSize, bKey, b, bSize );

			delete [] a;
			delete [] b;
		}
		#pragma omp master
		resultT = partResult;

		#pragma omp barrier

		#pragma omp critical
		if (omp_get_thread_num() != 0){
			aKey = std::get<0>(resultT);
			a = std::get<1>(resultT);
			aSize = std::get<2>(resultT);

			bKey = std::get<0>(partResult);
			b = std::get<1>(partResult);
			bSize = std::get<2>(partResult);

			resultT = reduceFunc(aKey, a, aSize, bKey, b, bSize);

			delete [] a;
			delete [] b;
		}
	}
	//std::cerr << "END ";
	return resultT;
}

template <typename K, typename T>
std::tuple<K,T*,size_t> faster::_workerIFdd<K,T*>::bulkReduce (IPbulkReduceIPFunctionP<K,T> bulkReduceFunc){
	K * ik = this->localData->getKeys();
	return bulkReduceFunc(ik, this->localData->getData(), this->localData->getLineSizes(), this->localData->getSize());
}


template <typename K, typename T>
void faster::_workerIFdd<K,T*>::applyIndependent(void * func, fddOpType op, fastCommBuffer & buffer){ 
	std::tuple<K,T*,size_t> r;

	switch (op){
		case OP_Reduce:
			r = reduce(( IPreduceIPFunctionP<K,T> ) func);
			//std::cerr << "Reduce " ;
			break;
		case OP_BulkReduce:
			r = bulkReduce(( IPbulkReduceIPFunctionP<K,T> ) func);
			//std::cerr << "BulkReduce ";
			break;
	}
	
	buffer <<  r;
}


// -------------------------- Public Functions ------------------------ //



template <typename K, typename T>
void faster::_workerIFdd<K,T*>::setData(K * keys, T ** data, size_t *lineSizes, size_t size){
	this->localData->setData(keys, data, lineSizes, size);
}


template <typename K, typename T>
void faster::_workerIFdd<K,T*>::setDataRaw(void * keys, void * data, size_t *lineSizes, size_t size) {
	this->localData->setDataRaw(keys, data, lineSizes, size);
}

template <typename K, typename T>
size_t * faster::_workerIFdd<K,T*>::getLineSizes(){ 
	return this->localData->getLineSizes();
}


template <typename K, typename T>
void faster::_workerIFdd<K,T*>::insert(void * k, void * in, size_t s){ 
	this->localData->insert(*(K *) k, (T*&) in, s);
}
template <typename K, typename T>
void faster::_workerIFdd<K,T*>::insertl(void * in){ 
	insert( *(std::deque<std::tuple<K,T*, size_t>>*) in);
}




template <typename K, typename T>
void faster::_workerIFdd<K,T*>::insert(K & key, T* & in, size_t s){ 
	this->localData->insert(key, in, s); 
}

template <typename K, typename T>
void faster::_workerIFdd<K,T*>::insert(std::deque< std::tuple<K, T*, size_t> > & in){ 
	typename std::deque< std::tuple<K, T*, size_t> >::iterator it;

	if (this->localData->getSize() < in.size())
		this->localData->grow(in.size());

	for ( it = in.begin(); it != in.end(); it++)
		this->localData->insert(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it)); 
}


template <typename K, typename T>
void faster::_workerIFdd<K,T*>::apply(void * func, fddOpType op, workerFddBase * dest, fastCommBuffer & buffer){ 
	if (op & OP_GENERICMAP)
		applyDependent(func, op, dest);
	else
		applyIndependent(func, op, buffer);
}



template <typename K, typename T>
void faster::_workerIFdd<K,T*>::collect(fastComm * comm) {
	comm->sendFDDDataCollect(this->id, this->localData->getKeys(), this->localData->getData(), this->localData->getLineSizes(), this->localData->getSize());
};


/*
extern template class faster::_workerIFdd<char, char>;
extern template class faster::_workerIFdd<char, int>;
extern template class faster::_workerIFdd<char, long int>;
extern template class faster::_workerIFdd<char, float>;
extern template class faster::_workerIFdd<char, double>;
template class faster::_workerIFdd<char, char*>;
template class faster::_workerIFdd<char, int*>;
template class faster::_workerIFdd<char, long int*>;
template class faster::_workerIFdd<char, float*>;
template class faster::_workerIFdd<char, double*>;
extern template class faster::_workerIFdd<char, std::string>;
extern template class faster::_workerIFdd<char, std::vector<char>>;
extern template class faster::_workerIFdd<char, std::vector<int>>;
extern template class faster::_workerIFdd<char, std::vector<long int>>;
extern template class faster::_workerIFdd<char, std::vector<float>>;
extern template class faster::_workerIFdd<char, std::vector<double>>;

extern template class faster::_workerIFdd<int, char>;
extern template class faster::_workerIFdd<int, int>;
extern template class faster::_workerIFdd<int, long int>;
extern template class faster::_workerIFdd<int, float>;
extern template class faster::_workerIFdd<int, double>;
template class faster::_workerIFdd<int, char*>;
template class faster::_workerIFdd<int, int*>;
template class faster::_workerIFdd<int, long int*>;
template class faster::_workerIFdd<int, float*>;
template class faster::_workerIFdd<int, double*>;
extern template class faster::_workerIFdd<int, std::string>;
extern template class faster::_workerIFdd<int, std::vector<char>>;
extern template class faster::_workerIFdd<int, std::vector<int>>;
extern template class faster::_workerIFdd<int, std::vector<long int>>;
extern template class faster::_workerIFdd<int, std::vector<float>>;
extern template class faster::_workerIFdd<int, std::vector<double>>;

extern template class faster::_workerIFdd<long int, char>;
extern template class faster::_workerIFdd<long int, int>;
extern template class faster::_workerIFdd<long int, long int>;
extern template class faster::_workerIFdd<long int, float>;
extern template class faster::_workerIFdd<long int, double>;
template class faster::_workerIFdd<long int, char*>;
template class faster::_workerIFdd<long int, int*>;
template class faster::_workerIFdd<long int, long int*>;
template class faster::_workerIFdd<long int, float*>;
template class faster::_workerIFdd<long int, double*>;
extern template class faster::_workerIFdd<long int, std::string>;
extern template class faster::_workerIFdd<long, std::vector<char>>;
extern template class faster::_workerIFdd<long, std::vector<int>>;
extern template class faster::_workerIFdd<long, std::vector<long int>>;
extern template class faster::_workerIFdd<long, std::vector<float>>;
extern template class faster::_workerIFdd<long, std::vector<double>>;

extern template class faster::_workerIFdd<float, char>;
extern template class faster::_workerIFdd<float, int>;
extern template class faster::_workerIFdd<float, long int>;
extern template class faster::_workerIFdd<float, float>;
extern template class faster::_workerIFdd<float, double>;
template class faster::_workerIFdd<float, char*>;
template class faster::_workerIFdd<float, int*>;
template class faster::_workerIFdd<float, long int*>;
template class faster::_workerIFdd<float, float*>;
template class faster::_workerIFdd<float, double*>;
extern template class faster::_workerIFdd<float, std::string>;
extern template class faster::_workerIFdd<float, std::vector<char>>;
extern template class faster::_workerIFdd<float, std::vector<int>>;
extern template class faster::_workerIFdd<float, std::vector<long int>>;
extern template class faster::_workerIFdd<float, std::vector<float>>;
extern template class faster::_workerIFdd<float, std::vector<double>>;

extern template class faster::_workerIFdd<double, char>;
extern template class faster::_workerIFdd<double, int>;
extern template class faster::_workerIFdd<double, long int>;
extern template class faster::_workerIFdd<double, float>;
extern template class faster::_workerIFdd<double, double>;
template class faster::_workerIFdd<double, char*>;
template class faster::_workerIFdd<double, int*>;
template class faster::_workerIFdd<double, long int*>;
template class faster::_workerIFdd<double, float*>;
template class faster::_workerIFdd<double, double*>;
extern template class faster::_workerIFdd<double, std::string>;
extern template class faster::_workerIFdd<double, std::vector<char>>;
extern template class faster::_workerIFdd<double, std::vector<int>>;
extern template class faster::_workerIFdd<double, std::vector<long int>>;
extern template class faster::_workerIFdd<double, std::vector<float>>;
extern template class faster::_workerIFdd<double, std::vector<double>>;

extern template class faster::_workerIFdd<std::string, char>;
extern template class faster::_workerIFdd<std::string, int>;
extern template class faster::_workerIFdd<std::string, long int>;
extern template class faster::_workerIFdd<std::string, float>;
extern template class faster::_workerIFdd<std::string, double>;
template class faster::_workerIFdd<std::string, char*>;
template class faster::_workerIFdd<std::string, int*>;
template class faster::_workerIFdd<std::string, long int*>;
template class faster::_workerIFdd<std::string, float*>;
template class faster::_workerIFdd<std::string, double*>;
extern template class faster::_workerIFdd<std::string, std::string>;
extern template class faster::_workerIFdd<std::string, std::vector<char>>;
extern template class faster::_workerIFdd<std::string, std::vector<int>>;
extern template class faster::_workerIFdd<std::string, std::vector<long int>>;
extern template class faster::_workerIFdd<std::string, std::vector<float>>;
extern template class faster::_workerIFdd<std::string, std::vector<double>>;
*/
