#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <fstream>
#include <mysql++.h>
#include <string>
#include <sstream>

#include "DatabaseManager.h"
#include "Stat.h"

using namespace std;


DatabaseManager::DatabaseManager(string configFile) : conn(false) {
	string databaseS;
	string addressS;
	string usernameS;
	string passwordS;

	std::ifstream file(configFile);
	std::string str;
	while (std::getline(file, str))
	{
		string key = str.substr(0, str.find("="));
		string value = str.substr(str.find("=") + 1, str.find(";") - str.find("=") - 1);

		if (key == "database") databaseS = value;
		if (key == "address") addressS = value;
		if (key == "username") usernameS = value;
		if (key == "password") passwordS = value;
	}

	if (conn.connect(databaseS.c_str(), addressS.c_str(), usernameS.c_str(), passwordS.c_str())) {
		cout << "Connected to the database sucessfully!" << endl;
	}
	else {
		cout << "Database probelm in connecting to database: " << conn.error() << endl;
		throw ("Database Exception"); //it should be handeled in DNSExperimenter. 
	}
}
//true if it does something. 
bool DatabaseManager::prepareDB(){
	try {
		mysqlpp::StoreQueryResult res = storeQuery("SHOW TABLES LIKE 'names';");
		if (res) {

			if (res.begin() == res.end()) {
				cout << "Table does not exists" << endl;
				executeQuery("CREATE TABLE names (name VARCHAR(30), PRIMARY KEY (name));");
				executeQuery("CREATE TABLE records (name VARCHAR(30), timestamp BIGINT, latency INT, PRIMARY KEY (name,timestamp));");
				cout << "Tables created successfully." << endl;
				return true; //indicates prepareDB created tables. 
			}
		}
		return false; //indicates that prepareDB did nothing, becuase tables already exist. 
	}
	catch (char const* dbError){
		cout << "Database Error in preparing database: " << dbError << endl; 
		throw ("Database Exception"); //it should be handeled in DNSExperimenter. 
	}
}

void DatabaseManager::addExperiment(string name, unsigned long long timestamp, int newLatency) {
	try {
		std::ostringstream qStream;
		qStream << "INSERT INTO records VALUES (\"" << name << "\", " <<
			timestamp << ", " << newLatency << ");";
		executeQuery(qStream.str());
	}
	catch (char const* dbError){
		cout << "Database Error in adding experiment: " << dbError << endl;
	}
	catch (...){
		cout << "Database Error in adding experiment" << endl;
	}
}

Stat DatabaseManager::computeStatistics(string name) {
	//create query to compute all statistics for host with name name, and then create an object host
	//with that information and return it 
	Stat result(name);

	try {
		std::ostringstream qStream;
		qStream << "SELECT COUNT(name) as count,  AVG(latency), STDDEV_SAMP(latency), MIN(timestamp), MAX(timestamp) FROM records WHERE name = \"" << name << "\";";
		mysqlpp::StoreQueryResult res = storeQuery(qStream.str());

		if (res) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				result.averageTime = atof(res[i]["AVG(latency)"]);
				result.standardDeviation = atof(res[i]["STDDEV_SAMP(latency)"]);
				result.firstTimestamp = strtoull(res[i]["MIN(timestamp)"], NULL, 0);
				result.lastTimestamp = strtoull(res[i]["MAX(timestamp)"], NULL, 0);
				result.numberOfQueries = atoi(res[i]["count"]);
			}
		}
	}
	catch (char const* dbError){
		cout << "Database Error: " << dbError << endl;
	}
	catch (...){
		cout << "Error in computing statistics" << endl;
	}
	return result;
}

Stat DatabaseManager::computeStatistics(string name, unsigned long long startTimestamp, unsigned long long endTimestamp){
	Stat result(name);

	try {
		std::ostringstream qStream;
		qStream << "SELECT COUNT(name) as count,  AVG(latency), STDDEV_SAMP(latency), MIN(timestamp), MAX(timestamp) FROM records WHERE name = \"" << name << "\" AND timestamp <= " << endTimestamp << " AND timestamp >= " << startTimestamp << ";";
		mysqlpp::StoreQueryResult res = storeQuery(qStream.str());
		if (res) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				result.averageTime = atof(res[i]["AVG(latency)"]);
				result.standardDeviation = atof(res[i]["STDDEV_SAMP(latency)"]);
				result.firstTimestamp = strtoull(res[i]["MIN(timestamp)"], NULL, 0);
				result.lastTimestamp = strtoull(res[i]["MAX(timestamp)"], NULL, 0);
				result.numberOfQueries = atoi(res[i]["count"]);
			}
		}
	}
	catch (char const* dbError){
		cout << "Database Error in computing statistics: " << dbError << endl;
	}
	catch (...){
		cout << "Error in computing statistics" << endl;
	}
	return result;
}
vector<Stat> DatabaseManager::computeStatistics(){
	vector<Stat> result;

	try {
		std::ostringstream qStream;
		qStream << "SELECT name, COUNT(name) as count,  AVG(latency), STDDEV_SAMP(latency), MIN(timestamp), MAX(timestamp) FROM records GROUP BY name;";
		mysqlpp::StoreQueryResult res = storeQuery(qStream.str());

		if (res) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				string s(res[i]["name"]);
				Stat host(s);
				host.averageTime = atof(res[i]["AVG(latency)"]);
				host.standardDeviation = atof(res[i]["STDDEV_SAMP(latency)"]);
				host.firstTimestamp = strtoull(res[i]["MIN(timestamp)"], NULL, 0);
				host.lastTimestamp = strtoull(res[i]["MAX(timestamp)"], NULL, 0);
				host.numberOfQueries = atoi(res[i]["count"]);
				result.push_back(host);
			}
		}
	}
	catch (char const* dbError){
		cout << "Database Error in computing statistics: " << dbError << endl;
	}
	catch (...){
		cout << "Error in computing statistics" << endl;
	}
	return result;
}

vector<Stat> DatabaseManager::computeStatistics(unsigned long long startTimestamp, unsigned long long endTimestamp){
	vector<Stat> result;

	try {
		std::ostringstream qStream;
		qStream << "SELECT name, COUNT(name) as count,  AVG(latency), STDDEV_SAMP(latency), MIN(timestamp), MAX(timestamp) FROM records WHERE  timestamp <= " << endTimestamp << " AND timestamp >= " << startTimestamp << " GROUP BY name; ";

		mysqlpp::StoreQueryResult res = storeQuery(qStream.str());

		if (res) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				string s(res[i]["name"]);
				Stat host(s);
				host.averageTime = atof(res[i]["AVG(latency)"]);
				host.standardDeviation = atof(res[i]["STDDEV_SAMP(latency)"]);
				host.firstTimestamp = strtoull(res[i]["MIN(timestamp)"], NULL, 0);
				host.lastTimestamp = strtoull(res[i]["MAX(timestamp)"], NULL, 0);
				host.numberOfQueries = atoi(res[i]["count"]);
				result.push_back(host);
			}
		}
	}
	catch (char const* dbError){
		cout << "Database Error in computing statistics: " << dbError << endl;
	}
	catch (...){
		cout << "Error in computing statistics" << endl;
	}
	return result;
}

void DatabaseManager::removeName(string name){
	try {
		std::ostringstream qStream;
		qStream << "DELETE FROM records WHERE name = \"" << name << "\";";
		executeQuery(qStream.str());

		qStream.str("");
		qStream << "DELETE FROM names WHERE name = \"" << name << "\";";
		executeQuery(qStream.str());
	}
	catch (char const* dbError){
		cout << "Database Error in removing name: " << dbError << endl;
	}
	catch (...){
		cout << "Error in removing name" << endl;
	}
}

void DatabaseManager::addName(string name){
	try {
		std::ostringstream qStream;
		qStream << "INSERT INTO names VALUES (\"" << name << "\");";
		executeQuery(qStream.str());
	}
	catch (char const* dbError){
		cout << "Database Error in adding name: " << dbError << endl;
	}
	catch (...){
		cout << "Error in adding name" << endl;
	}
}
void DatabaseManager::removeAll(){
	try {
		executeQuery("DELETE FROM records;");
		executeQuery("DELETE FROM names;");
	}
	catch (char const* dbError){
		cout << "Database Error in removing all: " << dbError << endl;
	}
	catch (...){
		cout << "Error in removing all" << endl;
	}
}

vector<string> DatabaseManager::getNames(){
	vector<string> result;

	try {
		mysqlpp::StoreQueryResult res = storeQuery("SELECT * FROM names;");
		if (res) {
			for (size_t i = 0; i < res.num_rows(); ++i) {
				string s(res[i]["name"]);
				result.push_back(s);
			}
		}
	}
	catch (char const* dbError){
		cout << "Database Error in getting host names: " << dbError << endl;
	}
	catch (...){
		cout << "Error in getting host names" << endl;
	}
	return result;
}

bool DatabaseManager::executeQuery(string queryString){
	mysqlpp::Query query = conn.query(queryString.c_str());
	bool res = query.exec();
	if (!res)
		throw query.error();
	return res;
}

mysqlpp::StoreQueryResult DatabaseManager::storeQuery(string queryString){
	mysqlpp::Query query = conn.query(queryString.c_str());
	mysqlpp::StoreQueryResult res = query.store();
	if (!res)
		throw query.error();
	return res;
}