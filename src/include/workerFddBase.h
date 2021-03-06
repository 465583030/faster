#ifndef LIBFASTER_WORKERFDDBASE_H
#define LIBFASTER_WORKERFDDBASE_H

#include <cstdlib>

#include "definitions.h"

namespace faster{

	class fastCommBuffer;
	class fastComm;
	class workerFddBase;


	class workerFddBase{
		protected:
			unsigned long int id;
			fddType type;
			fddType keyType;
		public:
			workerFddBase() ;
			workerFddBase(unsigned int ident, fddType t);
			virtual ~workerFddBase() = 0;

			virtual fddType getType() = 0;
			virtual fddType getKeyType() = 0;

			virtual void setData( void *, size_t) = 0;
			virtual void setData( void *, size_t *, size_t) = 0;
			virtual void setData( void *, void *, size_t) = 0;
			virtual void setData( void *, void *, size_t *, size_t) = 0;

			virtual void setDataRaw( void *, size_t) = 0;
			virtual void setDataRaw( void *, size_t *, size_t) = 0;
			virtual void setDataRaw( void *, void *, size_t) = 0;
			virtual void setDataRaw( void *, void *, size_t *, size_t) = 0;

			virtual void * getItem(size_t) = 0;

			virtual void * getKeys() = 0;
			virtual void * getData() = 0;
			virtual size_t getSize() = 0;
			virtual size_t * getLineSizes() = 0;
			virtual void   setSize(size_t s) = 0;

			virtual size_t itemSize() = 0;
			virtual size_t baseSize() = 0;

			virtual void deleteItem(void * item) = 0;

			virtual void shrink() = 0;
			virtual void insertl(void * v) = 0;
			virtual void insert(void * k, void * v, size_t s) = 0;

			virtual void preapply(unsigned long int id, void * func, fddOpType op, workerFddBase * dest, fastComm * comm) = 0;
			virtual void apply(void * func, fddOpType op, workerFddBase * dest, fastCommBuffer & buffer) = 0;

			virtual void collect(fastComm * comm) = 0;
			virtual void exchangeDataByKey(fastComm *comm) = 0;
			virtual std::vector< std::vector<void*> > * getKeyLocations() = 0;
			virtual void * getUKeys() = 0;
			virtual void  setUKeys(void * uk) = 0;
			virtual void * getKeyMap() = 0;
			virtual void  setKeyMap(void * km) = 0;

			virtual void writeToFile(void * path, size_t procId, void * sufix) = 0;
	};
}

#endif
