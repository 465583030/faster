#ifndef LIBFASTER_WORKER_H
#define LIBFASTER_WORKER_H

#include <vector>
#include <string>

#include "workerFddBase.h"
#include "fastTask.h"

namespace faster{

	class worker;
	class fastContext;
	class fastComm;


	// Worker class
	// Responsible for receiving and executing tasks and createing workerFDDs .
	class worker{
		friend class fastContext;
		// TODO Fix this!!! not every function should be private !
		private:
			unsigned long int id;
			std::vector< workerFddBase * > fddList;
			fastComm * comm;
			char tag;
			bool finished;
			char * buffer;
			void ** funcTable;
			std::vector< std::tuple<void*, size_t, int> > * globalTable;

			worker(fastComm * c, void ** ft, std::vector< std::tuple<void*, size_t, int> > & globalTable);
			~worker();

			void run();

			// Worker functions
			void createFDD (unsigned long int id, fddType type, size_t size);
			template <typename K>
			void _createIFDD (unsigned long int id, fddType type, size_t size);
			void createIFDD (unsigned long int id, fddType kType, fddType tType, size_t size);
			void createFDDGroup(unsigned long int id, fddType kType, std::vector<unsigned long int> & members);

			void discardFDD(unsigned long int id);

			// 1D Data
			void setFDDData(unsigned long int id, void * data, size_t size);
			void setFDDIData(unsigned long int id, void * kays, void * data, size_t size);
			// 2D Data
			void setFDDData(unsigned long int id, void * data, size_t * lineSizes, size_t size);
			void setFDDIData(unsigned long int id, void * kays, void * data, size_t * lineSizes, size_t size);

			//void getFDDData(unsigned long int id, void *& data, size_t &size);

			void setFDDOwnership(unsigned long int id, size_t low, size_t up);

			void readFDDFile(unsigned long int id, std::string &filename, size_t size, size_t offset);
			void writeFDDFile(unsigned long int id, std::string &path, std::string &sufix);

			void updateGlobals(fastTask &task);
			void solve(fastTask & task);

			void collect(unsigned long int id);

			void calibrate();
	};
}
#endif
