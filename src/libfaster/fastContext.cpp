#include <fstream>

#include "worker.h"
#include "fastContext.h"
#include "misc.h"

// Create a context with local as master
fastContext::fastContext(const fastSettings & s){

	settings = new fastSettings(s);
	comm = new fastComm( s.getMaster() );
	numFDDs = 0;
	numTasks = 0;

}


fastContext::~fastContext(){ 
	// Tell workers to go home!
	std::cerr << "    S:FINISH! ";
	comm->sendFinish();

	// Clean process
	fddList.clear();
	taskList.clear();
	delete comm;
	delete settings;
}

void fastContext::registerFunction(void * funcP){
	//std::cerr << "  Register " << funcP ;
	funcTable.insert(funcTable.end(), funcP);
	//std::cerr << ".\n";
}

void fastContext::startWorkers(){
	// Create a Worker context and exit after finished
	if ( ! comm->isDriver() ){
		// Start worker role
		worker worker(comm, funcTable.data());

		worker.run();

		// Clean process
		delete comm; 
		delete settings; 
		exit(0);
	}// */

}

int fastContext::findFunc(void * funcP){
	//std::cerr << "  Find Function " << funcP ;
	for( size_t i = 0; i < funcTable.size(); ++i){
		if (funcTable[i] == funcP)
			return i;
	}
	return -1;
}

unsigned long int fastContext::_createFDD(fddBase * ref, fddType type, size_t size){
	
	for (int i = 1; i < comm->numProcs; ++i){
		size_t dataPerProc = size / (comm->numProcs - 1);
		int rem = size % (comm->numProcs -1);
		if (i <= rem)
			dataPerProc += 1;
		comm->sendCreateFDD(numFDDs, type, dataPerProc, i);

		std::cerr << "    S:CreateFdd ID:" << numFDDs << " T:" << (int) type << " S:" << dataPerProc << '\n';
	}
	fddList.insert(fddList.begin(), ref);
	
	return numFDDs++;
}

unsigned long int fastContext::_createIFDD(fddBase * ref, fddType kType, fddType tType, size_t size){

	for (int i = 1; i < comm->numProcs; ++i){
		size_t dataPerProc = size / (comm->numProcs - 1);
		int rem = size % (comm->numProcs -1);
		if (i <= rem)
			dataPerProc += 1;
		comm->sendCreateIFDD(numFDDs, kType, tType, dataPerProc, i);

		std::cerr << "    S:CreateIFdd ID:" << numFDDs << " K:" << kType << " T:" << tType << " S:" << dataPerProc <<'\n';
	}
	fddList.insert(fddList.begin(), ref);
	return numFDDs++;
}

// TODO CHANGE THIS!
unsigned long int fastContext::createFDD(fddBase * ref, size_t typeCode, size_t size){
	return _createFDD(ref, decodeType(typeCode), size);
}
unsigned long int fastContext::createFDD(fddBase * ref, size_t typeCode){
	return _createFDD(ref, decodeType(typeCode), 0);
}
unsigned long int fastContext::createPFDD(fddBase * ref, size_t typeCode, size_t size){
	return _createFDD(ref, POINTER | decodeType(typeCode), size);
}
unsigned long int fastContext::createPFDD(fddBase * ref, size_t typeCode){
	return _createFDD(ref, POINTER | decodeType(typeCode), 0);
}
unsigned long int fastContext::createIFDD(fddBase * ref, size_t kTypeCode, size_t tTypeCode, size_t size){
	return _createIFDD(ref, decodeType(kTypeCode), decodeType(tTypeCode), size);
}
unsigned long int fastContext::createIFDD(fddBase * ref, size_t kTypeCode, size_t tTypeCode){
	return _createIFDD(ref, decodeType(kTypeCode), decodeType(tTypeCode), 0);
}
unsigned long int fastContext::createIPFDD(fddBase * ref, size_t kTypeCode, size_t tTypeCode, size_t size){
	return _createIFDD(ref, decodeType(kTypeCode), POINTER | decodeType(tTypeCode), size);
}
unsigned long int fastContext::createIPFDD(fddBase * ref, size_t kTypeCode, size_t tTypeCode){
	return _createIFDD(ref, decodeType(kTypeCode), POINTER | decodeType(tTypeCode), 0);
}

unsigned long int fastContext::createFddGroup(fddBase * ref, std::vector<fddBase*> & fddV){
	std::vector<unsigned long int> idV(fddV.size());
	std::vector<fddType> kV(fddV.size());
	std::vector<fddType> tV(fddV.size());

	for( size_t i = 0; i < fddV.size(); ++i){
		idV[i] = fddV[i]->getId();
		tV[i] = fddV[i]->tType();
		kV[i] = fddV[i]->kType();
	}

	//comm->sendCreateFDDGroup( numFDDs,  idV, kV, tV);
	// >>>>>>>>>>>>>>>>>>>>>>>>>> RESUME HERE <<<<<<<<<<<<<<<<<<<<<<<<<< //

	std::cerr << "    S:CreateFddGroup ID:" << numFDDs << '\n';

	fddList.insert(fddList.begin(), ref);
	return numFDDs++;
}



// Propagate FDD destruction to other machines
size_t findFileSize(const char* filename)
{
	std::ifstream in(filename, std::ifstream::in | std::ifstream::binary);
	in.seekg(0, std::ifstream::end);
	return in.tellg(); 
}

unsigned long int fastContext::readFDD(fddBase * ref, const char * fileName){
	//send read fdd n. numFdds from file fileName
	size_t fileSize = findFileSize(fileName);
	size_t offset = 0;

	for (int i = 1; i < comm->numProcs; ++i){
		size_t dataPerProc = fileSize / (comm->numProcs - 1);
		int rem = fileSize % (comm->numProcs -1);
		if (i <= rem)
			dataPerProc += 1;
		comm->sendReadFDDFile(ref->getId(), std::string(fileName), dataPerProc, offset, i);
		offset += dataPerProc;
	}

	std::cerr << "    S:ReadFdd";
	comm->waitForReq(comm->numProcs - 1);
	std::cerr << '\n';
	
	return numFDDs++;
}

void fastContext::getFDDInfo(size_t & s){
	s = 0;
	for (int i = 1; i < comm->numProcs; ++i){
		size_t size;
		comm->recvFDDInfo(size);
		s += size;
	}
}



unsigned long int fastContext::enqueueTask(fddOpType opT, unsigned long int idSrc, unsigned long int idRes, int funcId){
	fastTask * newTask = new fastTask();
	newTask->id = numTasks++;
	newTask->srcFDD = idSrc;
	newTask->destFDD = idRes;
	newTask->operationType = opT;
	newTask->functionId = funcId;
	newTask->workersFinished = 0;

	// TODO do this later on a shceduler?
	comm->sendTask(*newTask);
	std::cerr << "    S:Task ID:" << newTask->id << " FDD:" << idSrc << " F:" << funcId << '\n';

	taskList.insert(taskList.end(), newTask);

	return newTask->id;
}
unsigned long int fastContext::enqueueTask(fddOpType opT, unsigned long int id){
	return enqueueTask(opT, id, 0, -1);
}

void * fastContext::recvTaskResult(unsigned long int &id, size_t & size){
	double time;
	int proc;

	void * result = comm->recvTaskResult(id, proc, size, time);
	std::cerr << "    R:TaskResult P:" << proc << " ID:" << id << " Result:"  << * (int*) result << '\n';

	taskList[id]->workersFinished++;

	return result;
}


void fastContext::destroyFDD(unsigned long int id){
	comm->sendDestroyFDD(id);
}


