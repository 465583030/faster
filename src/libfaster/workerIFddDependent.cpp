#include <list>
#include <iostream>


#include "_workerIFdd.h"
#include "indexedFddStorageExtern.cpp"


// --------- MAP
template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::map (workerFddBase * dest, ImapIFunctionP<K,T,L,U> mapFunc){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	L * ok = (L*) dest->getKeys();
	U * od = (U*) dest->getData();

	//std::cerr << "START " << id << " " << s << "  ";

	#pragma omp parallel for 
	for (int i = 0; i < s; ++i){
		std::pair<L,U> r = mapFunc(ik[i], d[i]);
		ok[i] = r.first;
		od[i] = r.second;
	}
	//std::cerr << "END ";
}		

template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::map (workerFddBase * dest, IPmapIFunctionP<K,T,L,U> mapFunc){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	size_t * ols = dest->getLineSizes() ;
	L * ok = (L*) dest->getKeys();
	U * od = (U*) dest->getData();

	//std::cerr << "START " << id << " " << s << "  ";

	#pragma omp parallel for 
	for (int i = 0; i < s; ++i){
		//mapFunc(ok[i], od[i], ols[i], d[i]);
		std::tuple <L,U,size_t> r = mapFunc(ik[i],  d[i]);
		ok[i] = std::get<0>(r);
		od[i] = std::get<1>(r);
		ols[i] = std::get<2>(r);
	}
	//std::cerr << "END ";
}		

template <typename K, typename T>
template < typename U>
void faster::_workerIFdd<K,T>::map (workerFddBase * dest, mapIFunctionP<K,T,U> mapFunc){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	U * od = (U*) dest->getData();

	//std::cerr << "START " << id << " " << s << "  ";

	#pragma omp parallel for 
	for (int i = 0; i < s; ++i){
		od[i] = mapFunc(ik[i], d[i]);
	}
	//std::cerr << "END ";
}		

template <typename K, typename T>
template <typename U>
void faster::_workerIFdd<K,T>::map (workerFddBase * dest, PmapIFunctionP<K,T,U> mapFunc){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	size_t * ols = dest->getLineSizes() ;
	U * od = (U*) dest->getData();

	//std::cerr << "START " << id << " " << s << "  ";

	#pragma omp parallel for 
	for (int i = 0; i < s; ++i){
		//mapFunc(od[i], ols[i], d[i]);
		std::pair<U, size_t> r = mapFunc(ik[i], d[i]);
		od[i] = r.first;
		ols[i] = r.second;
	}
	//std::cerr << "END ";
}		



// --------- BulkMAP
template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::bulkMap (workerFddBase * dest, IbulkMapIFunctionP<K,T,L,U> bulkMapFunc){
	bulkMapFunc((L*) (dest->getKeys()), (U *) dest->getData(), (K*) this->localData->getKeys(), (T *)this->localData->getData(), this->localData->getSize());
}
template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::bulkMap (workerFddBase * dest, IPbulkMapIFunctionP<K,T,L,U> bulkMapFunc){
	bulkMapFunc((L*) (dest->getKeys()), (U*) dest->getData(), dest->getLineSizes(), (K*) this->localData->getKeys(), (T *) this->localData->getData(), this->localData->getSize());
}
template <typename K, typename T>
template <typename U>
void faster::_workerIFdd<K,T>::bulkMap (workerFddBase * dest, bulkMapIFunctionP<K,T,U> bulkMapFunc){
	bulkMapFunc((U*) dest->getData(), (K*) this->localData->getKeys(), (T *)this->localData->getData(), this->localData->getSize());
}
template <typename K, typename T>
template <typename U>
void faster::_workerIFdd<K,T>::bulkMap (workerFddBase * dest, PbulkMapIFunctionP<K,T,U> bulkMapFunc){
	bulkMapFunc((U*) dest->getData(), dest->getLineSizes(), (K*) this->localData->getKeys(), (T *) this->localData->getData(), this->localData->getSize());
}


// --------- FlatMAP

template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::flatMap(workerFddBase * dest,  IflatMapIFunctionP<K,T,L,U> flatMapFunc ){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	std::list<std::pair<L,U>> resultList;

	#pragma omp parallel 
	{
		std::list<std::pair<L,U>> partResultList;

		#pragma omp for 
		for (int i = 0; i < s; ++i){
			std::list<std::pair<L,U>> r = flatMapFunc(ik[i], d[i]);

			partResultList.insert(partResultList.end(), r.begin(), r.end());
		}

		#pragma omp critical
		resultList.insert(resultList.end(), partResultList.begin(), partResultList.end() );

	}
	dest->insertl(&resultList);
}
template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::flatMap(workerFddBase * dest,  IPflatMapIFunctionP<K,T,L,U> flatMapFunc ){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	std::list< std::tuple<L, U, size_t> > resultList;

	#pragma omp parallel 
	{
		std::list<std::tuple<L, U, size_t>> partResultList;

		#pragma omp for 
		for (int i = 0; i < s; ++i){
			std::list<std::tuple<L, U, size_t>> r = flatMapFunc(ik[i], d[i]);

			partResultList.insert(partResultList.end(), r.begin(), r.end());
		}

		#pragma omp critical
		resultList.insert(resultList.end(), partResultList.begin(), partResultList.end() );

	}
	dest->insertl(&resultList);
}
template <typename K, typename T>
template <typename U>
void faster::_workerIFdd<K,T>::flatMap(workerFddBase * dest,  flatMapIFunctionP<K,T,U> flatMapFunc ){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	std::list<U> resultList;

	#pragma omp parallel 
	{
		std::list<U> partResultList;

		#pragma omp for 
		for (int i = 0; i < s; ++i){
			std::list<U> r = flatMapFunc(ik[i], d[i]);

			partResultList.insert(partResultList.end(), r.begin(), r.end());
		}

		#pragma omp critical
		resultList.insert(resultList.end(), partResultList.begin(), partResultList.end() );

	}
	dest->insertl(&resultList);
}
template <typename K, typename T>
template <typename U>
void faster::_workerIFdd<K,T>::flatMap(workerFddBase * dest,  PflatMapIFunctionP<K,T,U> flatMapFunc ){
	T * d = this->localData->getData();
	size_t s = this->localData->getSize();
	K * ik = (K*) this->localData->getKeys();
	std::list< std::pair<U, size_t> > resultList;

	#pragma omp parallel 
	{
		std::list<std::pair<U, size_t>> partResultList;

		#pragma omp for 
		for (int i = 0; i < s; ++i){
			std::list<std::pair<U, size_t>> r = flatMapFunc(ik[i], d[i]);

			partResultList.insert(partResultList.end(), r.begin(), r.end());
		}

		#pragma omp critical
		resultList.insert(resultList.end(), partResultList.begin(), partResultList.end() );

	}
	dest->insertl(&resultList);
}

template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::bulkFlatMap(workerFddBase * dest,  IbulkFlatMapIFunctionP<K,T,L,U> bulkFlatMapFunc ){
	L * ok;
	U * result;
	size_t rSize;

	bulkFlatMapFunc(ok, result, rSize, this->localData->getKeys(), this->localData->getData(), this->localData->getSize());
	dest->setData(ok, result, rSize);
}
template <typename K, typename T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::bulkFlatMap(workerFddBase * dest,  IPbulkFlatMapIFunctionP<K,T,L,U> bulkFlatMapFunc ){
	L * ok;
	U * result;
	size_t * rDataSizes = NULL;
	size_t rSize;

	bulkFlatMapFunc(ok, result, rDataSizes, rSize, this->localData->getKeys(), (T*) this->localData->getData(), this->localData->getSize());
	dest->setData( ok, result, rDataSizes, rSize);
}
template <typename K, typename T>
template <typename U>
void faster::_workerIFdd<K,T>::bulkFlatMap(workerFddBase * dest,  bulkFlatMapIFunctionP<K,T,U> bulkFlatMapFunc ){
	U * result;
	size_t rSize;

	bulkFlatMapFunc(result, rSize, this->localData->getKeys(), this->localData->getData(), this->localData->getSize());
	dest->setData(result, rSize);
}
template <typename K, typename T>
template <typename U>
void faster::_workerIFdd<K,T>::bulkFlatMap(workerFddBase * dest,  PbulkFlatMapIFunctionP<K,T,U> bulkFlatMapFunc ){
	U * result;
	size_t * rDataSizes = NULL;
	size_t rSize;

	bulkFlatMapFunc( result, rDataSizes, rSize, this->localData->getKeys(), (T*) this->localData->getData(), this->localData->getSize());
	dest->setData( result, rDataSizes, rSize);
}



// Not Pointer -> Not Pointer
template <class K, class T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::_applyI(void * func, fddOpType op, workerFddBase * dest){
	switch (op){
		case OP_Map:
			map<L,U>(dest, (ImapIFunctionP<K,T,L,U>) func);
			std::cerr << "Map ";
			break;
		case OP_BulkMap:
			bulkMap<L,U>(dest, ( IbulkMapIFunctionP<K,T,L,U> ) func);
			std::cerr << "BulkMap ";
			break;
		case OP_FlatMap:
			flatMap<L,U>(dest, ( IflatMapIFunctionP<K,T,L,U> ) func);
			std::cerr << "FlatMap ";
			break;
		case OP_BulkFlatMap:
			bulkFlatMap<L,U>(dest, ( IbulkFlatMapIFunctionP<K,T,L,U> ) func);
			std::cerr << "BulkFlatMap ";
			break;
	}
}

// Not Pointer -> Pointer
template <class K, class T>
template <typename L, typename U>
void faster::_workerIFdd<K,T>::_applyIP(void * func, fddOpType op, workerFddBase * dest){
	switch (op){
		case OP_Map:
			map<L,U>(dest, (IPmapIFunctionP<K,T,L,U>) func);
			std::cerr << "Map ";
			break;
		case OP_BulkMap:
			bulkMap<L,U>(dest, ( IPbulkMapIFunctionP<K,T,L,U> ) func);
			std::cerr << "BulkMap ";
			break;
		case OP_FlatMap:
			flatMap<L,U>(dest, ( IPflatMapIFunctionP<K,T,L,U> ) func);
			std::cerr << "FlatMap ";
			break;
		case OP_BulkFlatMap:
			bulkFlatMap<L,U>(dest, ( IPbulkFlatMapIFunctionP<K,T,L,U> ) func);
			std::cerr << "BulkFlatMap ";
			break;
	}
}
// Not Pointer -> Not Pointer
template <class K, class T>
template <typename U>
void faster::_workerIFdd<K,T>::_apply(void * func, fddOpType op, workerFddBase * dest){
	switch (op){
		case OP_Map:
			map<U>(dest, (mapIFunctionP<K,T,U>) func);
			std::cerr << "Map ";
			break;
		case OP_BulkMap:
			bulkMap<U>(dest, ( bulkMapIFunctionP<K,T,U> ) func);
			std::cerr << "BulkMap ";
			break;
		case OP_FlatMap:
			flatMap<U>(dest, ( flatMapIFunctionP<K,T,U> ) func);
			std::cerr << "FlatMap ";
			break;
		case OP_BulkFlatMap:
			bulkFlatMap<U>(dest, ( bulkFlatMapIFunctionP<K,T,U> ) func);
			std::cerr << "BulkFlatMap ";
			break;
	}
}

// Not Pointer -> Pointer
template <class K, class T>
template <typename U>
void faster::_workerIFdd<K,T>::_applyP(void * func, fddOpType op, workerFddBase * dest){
	switch (op){
		case OP_Map:
			map<U>(dest, (PmapIFunctionP<K,T,U>) func);
			std::cerr << "Map ";
			break;
		case OP_BulkMap:
			bulkMap<U>(dest, ( PbulkMapIFunctionP<K,T,U> ) func);
			std::cerr << "BulkMap ";
			break;
		case OP_FlatMap:
			flatMap<U>(dest, ( PflatMapIFunctionP<K,T,U> ) func);
			std::cerr << "FlatMap ";
			break;
		case OP_BulkFlatMap:
			bulkFlatMap<U>(dest, ( PbulkFlatMapIFunctionP<K,T,U> ) func);
			std::cerr << "BulkFlatMap ";
			break;
	}
}


template <class K, class T>
template <typename L>
void faster::_workerIFdd<K,T>::_preApplyI(void * func, fddOpType op, workerFddBase * dest){
	switch (dest->getType()){
		case Null: break;
		case Char:      _applyI<L,char>	(func, op, dest); break;
		case Int:       _applyI<L,int> 	(func, op,  dest); break;
		case LongInt:   _applyI<L,long int>(func, op,  dest); break;
		case Float:     _applyI<L,float>   (func, op,  dest); break;
		case Double:    _applyI<L,double>  (func, op,  dest); break;
		case CharP:    _applyIP<L,char *>  (func, op, dest); break;
		case IntP:     _applyIP<L,int *>   (func, op, dest); break;
		case LongIntP: _applyIP<L,long int *> (func, op, dest); break;
		case FloatP:   _applyIP<L,float *>    (func, op, dest); break;
		case DoubleP:  _applyIP<L,double *>   (func, op, dest); break;
		case String:   _applyI<L,std::string>(func, op,  dest); break;
		//case Custom:  _applyI(func, op, dest); break;
		case CharV:    _applyI<L,std::vector<char>>    (func, op, dest); break;
		case IntV:     _applyI<L,std::vector<int>>     (func, op, dest); break;
		case LongIntV: _applyI<L,std::vector<long int>>(func, op, dest); break;
		case FloatV:   _applyI<L,std::vector<float>>   (func, op, dest); break;
		case DoubleV:  _applyI<L,std::vector<double>>  (func, op, dest); break;
	}

}

template <class K, class T>
void faster::_workerIFdd<K,T>::_preApply(void * func, fddOpType op, workerFddBase * dest){
	switch (dest->getType()){
		case Null: break;
		case Char:      _apply<char> 	(func, op, dest); break;
		case Int:       _apply<int> 	(func, op, dest); break;
		case LongInt:   _apply<long int> (func, op, dest); break;
		case Float:     _apply<float> 	(func, op, dest); break;
		case Double:    _apply<double> 	(func, op, dest); break;
		case CharP:    _applyP<char *> 	(func, op, dest); break;
		case IntP:     _applyP<int *> 	(func, op, dest); break;
		case LongIntP: _applyP<long int *> (func, op, dest); break;
		case FloatP:   _applyP<float *> (func, op, dest); break;
		case DoubleP:  _applyP<double *> (func, op, dest); break;
		case String:    _apply<std::string>(func, op, dest); break;
		//case Custom:  _apply<void *>(func, op, dest); break;
		case CharV:   _apply<std::vector<char>>  (func, op,  dest); break;
		case IntV:    _apply<std::vector<int>> 	 (func, op,  dest); break;
		case LongIntV:_apply<std::vector<long int>>(func, op,  dest); break;
		case FloatV:  _apply<std::vector<float>>  (func, op,  dest); break;
		case DoubleV: _apply<std::vector<double>> (func, op,  dest); break;
	}

}
template <typename K, typename T>
void faster::_workerIFdd<K,T>::applyDependent(void * func, fddOpType op, workerFddBase * dest){ 
	switch (dest->getKeyType()){
		case Null:     _preApply(func, op, dest);break;
		case Char:     _preApplyI<char>(func, op, dest); break;
		case Int:      _preApplyI<int>(func, op, dest); break;
		case LongInt:  _preApplyI<long int>(func, op, dest); break;
		case Float:    _preApplyI<float>(func, op, dest); break;
		case Double:   _preApplyI<double>(func, op, dest); break;
		case String:   _preApplyI<std::string>(func, op, dest); break;
	}
}



