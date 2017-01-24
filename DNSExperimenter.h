#include <vector>
#include <string>
#include "Stat.h"
#include "DatabaseManager.h"


#ifndef DNSExperimenter_H
#define DNSExperimenter_H
/**
This class performs all DNS experiments.
*/
class DNSExperimenter {

private: 
	int frequency;  /**< Frequency of performing experiments. */
	bool useDatabase; /**< If useDatabase is true, the DNSExperimenter store data in the database. */
	DatabaseManager* dbManager; /**< The DatabaseManager to work with database.*/
	static std::string randomString(int len); /**< A routine to create random string of size len*/
	static int getLatency(const char name[]); /**< A routine to perform an experiment for a host with specified domain name.*/
	static int getRandomLatency(const char name[]); /**< Adds the random string, then perform the experiment.*/

	bool runExperiments; /**< If set to true means experiments are running.*/
	std::vector<Stat> stats; /**< Overall statistics so far.*/
	std::vector<std::string> names; /**< The set of all names of which the program is tracking performance.*/

	/**
	This function performs the actual experiments. This function will run in another thread.
	*/
	void doExperiments();

public:
	/**
	This function adds alexa top 10 websites' name to the experiment set. 
	*/
	void addTop10();

	/**
	Constructor. 
	*/
	DNSExperimenter();

	/**
	This function sets the database base config file. 
	@param configFile The address to the database config file. 
	*/
	void setConfigFile(std::string configFile);
	
	/**
	This function adds a new domain name to the experiment set.
	@param name The domain name to be added in the future experiments.
	*/
	void addName(std::string name);

	/**
	This function removes a domain name from the experiment set.
	@param name The domain name to be removed from the future experiments.
	*/
	void removeName(std::string name);

	/**
	This function removes all names and statistics. It also clear database tables.
	*/
	void removeAll ();

	/**
	This function  sets the frequency of performing the experiments.
	@param timesPerMinute Number of experiments per mintue.
	*/
	void setFrequency(int timesPerMinute);

	/**
	This function gets the frequency of performing the experiments.
	@return THe current frequency.
	*/
	int getFrequency() const;

	/**
	This function starts the experiments. 
	*/
	void startExperiment(); 

	/**
	This function stops the experiemtns. 
	*/
	void stopExperiment(); 

	/**
	This function checks if the experiments are running. 
	@return If experiments are running returns true. Otherwise returns false. 
	*/
	bool getRunExperiments() const;

	/**
	This function returns the overal statistics obtained so far.
	@return The overall statistics. 
	*/
	std::vector<Stat> getStats() const;


	/**
	This function returns the set of name in the experiment set. 
	@return The set of names.
	*/
	std::vector<std::string> getNames() const;
};
#endif