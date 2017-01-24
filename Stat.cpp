#include <string>
#include "Stat.h"
using namespace std;

Stat::Stat(string name){
	this->name = name;
	this->firstTimestamp = 0;
	this->lastTimestamp = 0;
	this->averageTime = 0;
	this->numberOfQueries = 0;
	this->standardDeviation = 0;
}