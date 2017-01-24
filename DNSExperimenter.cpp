#include <iostream> 
#include <thread>
#include <vector>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <ldns/ldns.h>
#include <ctime>
#include <cstring>
#include <chrono>
#include <cmath>
#include <time.h>
#include <mutex> 

#include "DNSExperimenter.h"

using namespace std;

//Welford's method to update average and standard deviation.-----
//see: http://jonisalonen.com/2013/deriving-welfords-method-for-computing-variance/
double getNewAvg(double oldAvg, double newValue, double newNumber) {
	return oldAvg + (newValue - oldAvg) / newNumber;
}

double getNewSd(double oldAvg, double oldSd, double newValue, double newNumber){
	double newAvg = getNewAvg(oldAvg, newValue, newNumber);
	return sqrt(((newNumber - 2)* pow(oldSd, 2) + (newValue - newAvg)*(newValue - oldAvg)) / (newNumber - 1));
}

mutex mtx;

//-----------------------------------------------------------------
void DNSExperimenter::doExperiments(){
	while (runExperiments) {
		mtx.lock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60 / frequency));
		for (string h : names){
			int latency = getRandomLatency(h.c_str());

			unsigned long long millis =
				std::chrono::duration_cast<std::chrono::milliseconds>
				(std::chrono::system_clock::now().time_since_epoch()).count();

			if (useDatabase) 
				dbManager->addExperiment(h, millis, latency);
			auto s = std::find_if(stats.begin(), stats.end(), [h](const Stat s) { return  s.name == h; });
			if (s == stats.end()){
				Stat newS(h); 
				newS.firstTimestamp = millis;
				newS.lastTimestamp = millis;
				newS.numberOfQueries = 1;
				newS.averageTime = latency;
				newS.standardDeviation = 0;
				stats.push_back(newS);
			}
			else {
				(*s).lastTimestamp = millis;
				(*s).numberOfQueries++;
				int oldAvg = (*s).averageTime;
				(*s).averageTime = getNewAvg(oldAvg, latency, (*s).numberOfQueries);
				(*s).standardDeviation = getNewSd(oldAvg, (*s).standardDeviation, latency, (*s).numberOfQueries);
			}
		}
		mtx.unlock();
	}
	
}


void DNSExperimenter::startExperiment(){
	if (!runExperiments) {
		runExperiments = true;
		if (frequency >= 1){
			std::thread t1(&DNSExperimenter::doExperiments, this);
			t1.detach();
		}
		else
			cout << "Please set frequency to an integer greater than 0. Use setFrequency command." << endl;
	}
}

void DNSExperimenter::stopExperiment(){
	runExperiments = false;
}

void DNSExperimenter::setFrequency(int f){
	if (f >= 0)
		frequency = f;
}

int DNSExperimenter::getFrequency() const{
	return frequency;
}


void DNSExperimenter::addName(string newUrl){
	if (std::find_if(names.begin(), names.end(), [newUrl](const string h) { return h == newUrl; }) == names.end())
	{
		names.push_back(newUrl);
		if (useDatabase)
			dbManager->addName(newUrl);
	}
}

void DNSExperimenter::removeName(string urlToRemove){
	mtx.lock();
	stats.erase(std::remove_if(stats.begin(), stats.end(), [urlToRemove](const Stat w) { return w.name == urlToRemove; }), stats.end());
	names.erase(std::remove(names.begin(), names.end(), urlToRemove), names.end());
	if (names.empty())
		runExperiments = false;
	mtx.unlock();
	if (useDatabase)
		dbManager->removeName(urlToRemove);
}

void DNSExperimenter::removeAll(){
	mtx.lock();
	runExperiments = false;
	stats.clear();
	names.clear();
	mtx.unlock();


	if (useDatabase)
		dbManager->removeAll();
}

bool DNSExperimenter::getRunExperiments() const{
	return runExperiments;
}

vector<Stat> DNSExperimenter::getStats() const{
	return  stats;
}

string DNSExperimenter::randomString(int len){
	srand(time(0));
	string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int pos;
	while (str.size() != len) {
		pos = ((rand() % (str.size() - 1)));
		str.erase(pos, 1);
	}
	return str;
}

int DNSExperimenter::getLatency(const char host[]){
	ldns_resolver *res;
	ldns_rdf *domain;
	ldns_pkt *p;
	ldns_rr_list *a;
	ldns_status s;

	p = NULL;
	a = NULL;
	domain = NULL;
	res = NULL;
	/* create a new resolver from /etc/resolv.conf */
	s = ldns_resolver_new_frm_file(&res, NULL);

	if (s != LDNS_STATUS_OK) {
		cout << "s != LDNS_STATUS_OK" << endl;
		//exit(EXIT_FAILURE);
	}

	double latency = 0;
	domain = ldns_dname_new_frm_str(host);
	if (!domain) {
		return -1;
	}
	else
	{
		if (!ldns_dname_str_absolute(host) &&
			ldns_dname_absolute(domain)) {
			ldns_rdf_set_size(domain, ldns_rdf_size(domain) - 1);
		}
		unsigned long long stMillis =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count();
		p = ldns_resolver_query(res,
			domain,
			LDNS_RR_TYPE_A,
			LDNS_RR_CLASS_IN,
			LDNS_RD);
		unsigned long long etMillis =
			std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count();
		latency = (int)(etMillis - stMillis);
		ldns_rdf_deep_free(domain);
	}
	ldns_pkt_free(p);
	ldns_resolver_deep_free(res);
	return latency;
}

int DNSExperimenter::getRandomLatency(const char host[]){
	string hostToCheck = host;
	string randomStr = randomString(10);
	hostToCheck.insert(0, ".");
	hostToCheck.insert(0, randomStr);
	int latency = getLatency(hostToCheck.c_str());
	return latency;
}


DNSExperimenter::DNSExperimenter(){
	runExperiments = false;
	frequency = 60;
	useDatabase = false;
}

void DNSExperimenter::setConfigFile(string configFile){
	try {
		dbManager = new DatabaseManager(configFile);
		useDatabase = true;
		if (dbManager->prepareDB()){
			addTop10();
		}

		names = dbManager->getNames();
		stats = (dbManager->computeStatistics());
	}
	catch (...) {
		cout << "Probelm with databse. Program now continues without using databse." << endl;
		useDatabase = false;
		addTop10();
	}
}

void DNSExperimenter::addTop10(){
	addName("google.com");
	addName("facebook.com");
	addName("youtube.com");
	addName("yahoo.com");
	addName("live.com");
	addName("wikipedia.org");
	addName("baidu.com");
	addName("blogger.com");
	addName("msn.com");
	addName("qq.com");
}

vector<string> DNSExperimenter::getNames() const{
	return names;
}