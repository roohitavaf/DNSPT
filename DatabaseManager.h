#include <vector>
#include <mysql++.h>
#include <string>
#include "Stat.h"


#ifndef DatabaseManager_H
#define DatabaseManager_H

/**
This class performs all database related operations.
*/
class DatabaseManager {
private:
	mysqlpp::Connection conn; /**< connection to the database */
	/**This function execute queries. It is used for queries such as INSERT that do not return data.
	@param queryString The query to execute.
	@return true if query was successfull. false otherwise.
	*/
	bool executeQuery(std::string queryString); 

	/**This function executes queries. It is used for queries such as SELECT that return data.
	@param queryString The query to execute.
	@return The retrieved data from the database.
	*/
	mysqlpp::StoreQueryResult storeQuery(std::string queryString); 

public:
	/**
	Constructor. This function creates connection to the database. 
	@param configFile database config file.
	*/
	DatabaseManager(std::string configFile);

	/**
	This function creates necessary tables in the database if they are missing. 
	@return ture if it created tables, false otherwise.
	*/
	bool prepareDB() ;

	/**
	This function adds a new experiment record into the records table.
	@param name The domian name of the experiment. 
	@param timestamp The timestamp of the experiment. 
	@param newLatency The latency observed in the experiment.
	*/
	void addExperiment(std::string name, unsigned long long timestamp, int newLatency);

	/**
	This function removes a domain name from the database.
	@param name The domain name to be removed. 
	*/
	void removeName(std::string name);

	/**
	This function adds a new domain name to the database. 
	@param name The domain name to be added.
	*/
	void addName(std::string name);

	/**
	This function removes all domainas and records from the database. 
	*/
	void removeAll();

	/**
	This function gets the domain names from the database. 
	@return The set of domain names. 
	*/
	std::vector<std::string> getNames();

	/**
	This function computes the overall statistics for a specific domain name from the records of the databse.
	@param The domain name. 
	@see computeStatistics(std::string name, unsigned long long startTimestamp, unsigned long long endTimestamp)
	@return The statistics. 
	*/
	Stat computeStatistics(std::string name);

	/**
	This function computes the statistics for a specific domain name for a limited time period from the records of the databse.
	@param The domain name.
	@see computeStatistics(std::string name)
	@return The statistics.
	*/
	Stat computeStatistics(std::string name, unsigned long long startTimestamp, unsigned long long endTimestamp);

	/**
	This function computes the overall statistics for all domain names from the records. 
	@see computeStatistics(unsigned long long startTimestamp, unsigned long long endTimestamp)
	@return A set of statistics, one for each domain name. 
	*/
	std::vector<Stat> computeStatistics();

	/**
	This function computes statistics for all domain names for a limited time period from the records.
	@see computeStatistics()
	@return A set of statistics, one for each domain name.
	*/
	std::vector<Stat> computeStatistics(unsigned long long startTimestamp, unsigned long long endTimestamp);
};

#endif