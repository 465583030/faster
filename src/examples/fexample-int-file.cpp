#include <iostream>
#include "libfaster.h"

#define NUMITEMS 100*1000

using namespace std;
using namespace faster;

int map1(int & a){
	return a / 2;
}


int reduce1(int &a, int &b){
	return a + b;
}


int main(int argc, char ** argv){
	// Init Faster Framework
	cout << "Init FastLib" << '\n';
	fastContext fc(argc,argv);
	fc.registerFunction((void*) &map1);
	fc.registerFunction((void*) &reduce1);

	fc.startWorkers();
	if (!fc.isDriver())
		return 0;

	cout << "Import Data" << '\n';
	fdd<int> data(fc, "../res/testM.txt");

	cout << "Process Data" << '\n';
	int result = data.map<int>(&map1)->reduce(&reduce1);

	cout << "DONE!" << '\n';
	std::cout << result << "\n";

	return 0;
}
