#include <string> 

#ifndef Stat_H
#define Stat_H

/**
Objects instantiated from this class represent a set of statistics for a specific domain name. 
*/
class Stat {
public:
	std::string name; 
	double averageTime;
	double standardDeviation;
	int numberOfQueries;
	unsigned long long firstTimestamp;
	unsigned long long lastTimestamp;

	/**
	Constructor. 
	@param name The domain name. 
	*/
	Stat(std::string name);
};

#endif