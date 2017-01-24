#include <iostream> 
#include <string>
#include <iomanip> 
#include <algorithm>
#include <cstdlib> 

#include "DNSExperimenter.h"

using namespace std;

void printOverall(const vector<Stat>& stats);
void printHosts(vector<string> hosts);
void printCommands();

int main(int argc, char* argv[]){
	DNSExperimenter dnsExp;
	if (argc < 2) {
		cout << "You have not provided a database config file.\nContinue without using database? (y/n):";
		string r;
		cin >> r;
		if (r != "y")
			return 1;
		dnsExp.addTop10();
	}
	else {
		dnsExp.setConfigFile(argv[1]);
	}


	while (true){
		if (dnsExp.getRunExperiments())
			cout << "(~)>: ";
		else 
			cout << ">: ";
		string command;
		cin >> command;
		if (command == "exit") {
			break;
		}
		else if (command == "add") {
			string u;
			cin >> u;
			dnsExp.addName(u);
		}
		else if (command == "add_top10") {
			dnsExp.addTop10();
		}
		else if (command == "remove"){
			string u;
			cin >> u;
			dnsExp.removeName(u);
		}
		else if (command == "names"){
			vector<string> hosts = dnsExp.getNames();
			if (!hosts.empty())
				printHosts(dnsExp.getNames());
			else
				cout << "There is no host to show. You can add new hosts using \"add host_name\" command." << endl;
		}
		else if (command == "remove_all") { 
			dnsExp.removeAll();
		}
		else if (command == "set_freq"){
			int f;
			cin >> f;
			dnsExp.setFrequency(f);
		}
		else if (command == "get_freq"){
			cout << dnsExp.getFrequency() << endl;
		}
		else if (command == "start"){
			if (!dnsExp.getNames().empty())
				dnsExp.startExperiment();
			else
				cout << "The list of hosts are empty. Please add at least one host using add command." << endl;
		}
		else if (command == "stop"){
			dnsExp.stopExperiment();
		}
		else if (command == "stat"){
			if (!dnsExp.getStats().empty())
				printOverall(dnsExp.getStats());
			else cout << "No statistics to show. You can run experiments using \"start\" command." << endl;
		}
		else if (command == "stat_s"){
			if (!dnsExp.getStats().empty()) {
				vector<Stat> websites = dnsExp.getStats();
				sort(websites.begin(), websites.end(),
					[](const Stat  a, const Stat  b) -> bool
				{
					return a.averageTime < b.averageTime;
				});
				printOverall(websites);
			}
			else 
				cout << "No statistics to show. Please start experiments using \"start\" command." << endl;
		}
		else if (command == "help"){
			printCommands();
		}
		else if (command == "clear"){
			system("clear");
		}
		else{
			cout << "unkonwn command! Type help for help." << endl;
		}
	}

	return 0;
}

void printOverall(const vector<Stat>& stats) {
	int filedLength = 11;
	auto minmax_width = std::max_element(stats.begin(), stats.end(),
		[](Stat const& lhs, Stat const& rhs) {
		return lhs.name.size() < rhs.name.size(); });
		if (minmax_width != stats.end()) {
			cout << setw((*minmax_width).name.size()) << "Name" << setw(filedLength+5) << "Avg. time (ms)" << setw(filedLength) << "Std. dev." << setw(filedLength) << "#queries" << setw(filedLength + 4) << "first query" << setw(filedLength + 5) << "last query" << endl << endl;
			for (Stat w : stats){
				cout << setw((*minmax_width).name.size()) << w.name << setw(filedLength + 5) << w.averageTime << setw(filedLength) << w.standardDeviation << setw(filedLength) << w.numberOfQueries << setw(filedLength + 4) << w.firstTimestamp << setw(filedLength + 5) << w.lastTimestamp << endl;
			}
		}
		else
			cout << "List is empty. Add new host names using add [host name] command." << endl;
}

void printHosts(vector<string> hosts){
	for_each(hosts.begin(), hosts.end(), [](string& s) {cout << s << endl; });
}

void printCommands(){
	int firstFiledLength = 20;
	int filedLength =50;
	cout << "Use the following commands:" << endl << endl;
	cout << std::left << setw(firstFiledLength) << "add <name>"  << setw(filedLength) << "addes a new domain name." << endl;
	cout << setw(firstFiledLength) << "remove <name>" << setw(filedLength) << "removes a new domain name." << endl;
	cout << setw(firstFiledLength) << "add_top10" << setw(filedLength) << "adds Alexa top 10 domains." << endl;
	cout << setw(firstFiledLength) << "remove_all" << setw(filedLength) << "removes all names and statistics." << endl;
	cout << setw(firstFiledLength) << "names" << setw(filedLength) << "shows current domain names." << endl;
	cout << setw(firstFiledLength) << "set_freq <freq.>" << setw(filedLength) << "sets frequency." << endl;
	cout << setw(firstFiledLength) << "get_freq" << setw(filedLength) << "gets current frequency." << endl;
	cout << setw(firstFiledLength) << "start" << setw(filedLength) << "starts performing experiments." << endl;
	cout << setw(firstFiledLength) << "stop" << setw(filedLength) << "stops performing experiments." << endl;
	cout << setw(firstFiledLength) << "stat" << setw(filedLength) << "shows the statistics." << endl;
	cout << setw(firstFiledLength) << "stat_s" << setw(filedLength) << "shows the statistics in ascending order of average latencies." << endl;
	cout << setw(firstFiledLength) << "clear" << setw(filedLength) << "clears the screen." << endl;
	cout << setw(firstFiledLength) << "help" << setw(filedLength) << "shows commands" << endl << std::internal;



}