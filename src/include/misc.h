#ifndef LIBFASTER_MISC_H
#define LIBFASTER_MISC_H

#define FDD_TYPE_NULL 	0x00
#define FDD_TYPE_INT 	0x01

#include <typeinfo>
#include <string>
#include <math.h>

#include "definitions.h"

namespace faster{
	fddType decodeType(size_t typeCode);
	
	const std::string decodeOptype(fddOpType op);

	template < typename T >
	double mean(std::vector<T> v){
		T sum = 0;
		for ( size_t i = 0; i < v.size(); ++i)
			sum += v[i];

		return sum/(double)(v.size());
	}
	template < typename T >
	double stdDev(std::vector<T> v, double mean){
		double sum = 0;
		for ( size_t i = 0; i < v.size(); ++i)
			sum += pow(v[i] - (double) mean, 2);

		return ( sqrt( sum/(v.size() - 1) ) );
	}
}

#endif
