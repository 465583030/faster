#ifndef LIBFASTER_WORKERFDD_H
#define LIBFASTER_WORKERFDD_H

#include <list>
#include <omp.h>

template <class T>
class fddStorage;

template <class T>
class workerFdd;

template <class K, class T>
class workerIFdd;

#include "workerFddBase.h"

template <class T>
class workerFddCore : public workerFddBase{
	protected:
		fddStorage <T> * localData;

	public:
		workerFddCore(unsigned int ident, fddType t);
		workerFddCore(unsigned int ident, fddType t, size_t size);
		~workerFddCore();

		void setDataRaw(void * keys UNUSED, void * data UNUSED, size_t size UNUSED) override{}
		void setDataRaw(void * keys UNUSED, void ** data UNUSED, size_t * lineSizes UNUSED, size_t size UNUSED) override{}

		fddType getType() override ;
		fddType getKeyType() override ;

		T & operator[](size_t address);
		void * getData() override;
		size_t getSize() override;
		size_t itemSize() override;
		size_t baseSize() override;
		void deleteItem(void * item) override ;
		void shrink();

		void groupByKey(fastComm *comm UNUSED) override {}
		void countByKey(fastComm *comm UNUSED) override {}
};

// Worker side FDD
template <class T>
class workerFdd : public workerFddCore<T>{
	private:
		// Not Pointer -> Not Pointer
		template <typename U>
		void _apply(void * func, fddOpType op, workerFdd<U> * dest);
		template <typename U>
		void _applyP(void * func, fddOpType op, workerFdd<U> * dest);
		template <typename L, typename U>
		void _applyI(void * func, fddOpType op, workerIFdd<L,U> * dest);
		template <typename L, typename U>
		void _applyIP(void * func, fddOpType op, workerIFdd<L,U> * dest);
		void _applyReduce(void * func, fddOpType op, void *& result, size_t & rSize);

		template <typename L>
		void _preApplyI(void * func, fddOpType op, workerFddBase * dest);
		void _preApply(void * func, fddOpType op, workerFddBase * dest);

		// --------- FUNCTIONS ----------

		template <typename U>
		void map (workerFdd<U> & dest, mapFunctionP<T,U> mapFunc);
		template <typename U>
		void map (workerFdd<U> & dest, PmapFunctionP<T,U> mapFunc);
		template <typename L, typename U>
		void map (workerIFdd<L,U> & dest, ImapFunctionP<T,L,U> mapFunc);
		template <typename L, typename U>
		void map (workerIFdd<L,U> & dest, IPmapFunctionP<T,L,U> mapFunc);

		template <typename U>
		void bulkMap (workerFdd<U> & dest, bulkMapFunctionP<T,U> bulkMapFunc);
		template <typename U>
		void bulkMap (workerFdd<U> & dest, PbulkMapFunctionP<T,U> bulkMapFunc);
		template <typename L, typename U>
		void bulkMap (workerIFdd<L,U> & dest, IbulkMapFunctionP<T,L,U> bulkMapFunc);
		template <typename L, typename U>
		void bulkMap (workerIFdd<L,U> & dest, IPbulkMapFunctionP<T,L,U> bulkMapFunc);

		template <typename U>
		void flatMap(workerFdd<U> & dest,  flatMapFunctionP<T,U> flatMapFunc );
		template <typename U>
		void flatMap(workerFdd<U> & dest,  PflatMapFunctionP<T,U> flatMapFunc );
		template <typename L, typename U>
		void flatMap(workerIFdd<L,U> & dest,  IflatMapFunctionP<T,L,U> flatMapFunc );
		template <typename L, typename U>
		void flatMap(workerIFdd<L,U> & dest,  IPflatMapFunctionP<T,L,U> flatMapFunc );

		template <typename U>
		void bulkFlatMap(workerFdd<U> & dest,  bulkFlatMapFunctionP<T,U> bulkFlatMapFunc );
		template <typename U>
		void bulkFlatMap(workerFdd<U> & dest,  PbulkFlatMapFunctionP<T,U> bulkFlatMapFunc );
		template <typename L, typename U>
		void bulkFlatMap(workerIFdd<L,U> & dest,  IbulkFlatMapFunctionP<T,L,U> bulkFlatMapFunc );
		template <typename L, typename U>
		void bulkFlatMap(workerIFdd<L,U> & dest,  IPbulkFlatMapFunctionP<T,L,U> bulkFlatMapFunc );




		// REDUCE
		T reduce (reduceFunctionP<T> reduceFunc);
		T bulkReduce (bulkReduceFunctionP<T> bulkReduceFunc);


	public:

		workerFdd(unsigned int ident, fddType t) : workerFddCore<T>(ident, t) {}
		workerFdd(unsigned int ident, fddType t, size_t size) : workerFddCore<T>(ident, t, size) {}
		~workerFdd(){}

		// For known types
		void setData(T * data, size_t size) ;

		// For anonymous types
		void setDataRaw(void * data, size_t size) override;
		void setDataRaw(void ** data UNUSED, size_t * listSizes UNUSED, size_t size UNUSED ) override{}

		void insert(T & in);
		void insert(std::list<T> & in);

		// Apply task functions to FDDs
		void apply(void * func, fddOpType op, workerFddBase * dest, void *& result, size_t & rSize);

		void collect(fastComm * comm) override;


};

// Pointer specialization
template <class T>
class workerFdd<T*> : public workerFddCore<T*>{
	private:
		// Pointer -> Not Pointer
		template <typename U>
		void _apply(void * func, fddOpType op, workerFdd<U> * dest);
		template <typename U>
		void _applyP(void * func, fddOpType op, workerFdd<U> * dest);
		template <typename L, typename U>
		void _applyI(void * func, fddOpType op, workerIFdd<L,U> * dest);
		template <typename L, typename U>
		void _applyIP(void * func, fddOpType op, workerIFdd<L,U> * dest);
		void _applyReduce(void * func, fddOpType op, void *& result, size_t & rSize);

		template <typename L>
		void _preApplyI(void * func, fddOpType op, workerFddBase * dest);
		void _preApply(void * func, fddOpType op, workerFddBase * dest);
		// --------- FUNCTIONS ----------

		template <typename U>
		void map (workerFdd<U> & dest, mapPFunctionP<T,U> mapFunc);
		template <typename U>
		void map (workerFdd<U> & dest, PmapPFunctionP<T,U> mapFunc);
		template <typename L, typename U>
		void map (workerIFdd<L,U> & dest, ImapPFunctionP<T,L,U> mapFunc);
		template <typename L, typename U>
		void map (workerIFdd<L,U> & dest, IPmapPFunctionP<T,L,U> mapFunc);

		template <typename U>
		void bulkMap (workerFdd<U> & dest, bulkMapPFunctionP<T,U> bulkMapFunc);
		template <typename U>
		void bulkMap (workerFdd<U> & dest, PbulkMapPFunctionP<T,U> bulkMapFunc);
		template <typename L, typename U>
		void bulkMap (workerIFdd<L,U> & dest, IbulkMapPFunctionP<T,L,U> bulkMapFunc);
		template <typename L, typename U>
		void bulkMap (workerIFdd<L,U> & dest, IPbulkMapPFunctionP<T,L,U> bulkMapFunc);

		template <typename U>
		void flatMap(workerFdd<U> & dest,  flatMapPFunctionP<T,U> flatMapFunc );
		template <typename U>
		void flatMap(workerFdd<U> & dest,  PflatMapPFunctionP<T,U> flatMapFunc );
		template <typename L, typename U>
		void flatMap(workerIFdd<L,U> & dest,  IflatMapPFunctionP<T,L,U> flatMapFunc );
		template <typename L, typename U>
		void flatMap(workerIFdd<L,U> & dest,  IPflatMapPFunctionP<T,L,U> flatMapFunc );

		template <typename U>
		void bulkFlatMap(workerFdd<U> & dest,  bulkFlatMapPFunctionP<T,U> bulkFlatMapFunc );
		template <typename U>
		void bulkFlatMap(workerFdd<U> & dest,  PbulkFlatMapPFunctionP<T,U> bulkFlatMapFunc );
		template <typename L, typename U>
		void bulkFlatMap(workerIFdd<L,U> & dest,  IbulkFlatMapPFunctionP<T,L,U> bulkFlatMapFunc );
		template <typename L, typename U>
		void bulkFlatMap(workerIFdd<L,U> & dest,  IPbulkFlatMapPFunctionP<T,L,U> bulkFlatMapFunc );


		// REDUCE
		std::pair<T*,size_t> reduce (PreducePFunctionP<T> reduceFunc);
		std::pair<T*,size_t> bulkReduce (PbulkReducePFunctionP<T> bulkReduceFunc);
		

	public:
		workerFdd(unsigned int ident, fddType t) : workerFddCore<T*>(ident, t) {}
		workerFdd(unsigned int ident, fddType t, size_t size) : workerFddCore<T*>(ident, t, size) {}
		~workerFdd(){}


		// For known types
		void setData(T ** data, size_t *lineSizes, size_t size) ;

		// For anonymous types
		void setDataRaw(void * data UNUSED, size_t size UNUSED) override {}
		void setDataRaw(void ** data, size_t *lineSizes, size_t size) override;

		size_t * getLineSizes();

		void insert(T* & in, size_t s);
		void insert(std::list< std::pair<T*, size_t> > & in);

		// Apply task functions to FDDs
		void apply(void * func, fddOpType op, workerFddBase * dest, void *& result, size_t & rSize);
		void collect(fastComm * comm) override;

};

#endif
