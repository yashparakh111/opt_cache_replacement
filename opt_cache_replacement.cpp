#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <climits>

#define MAX_CACHE_SIZE 1000

using namespace std;

int ASSOCIATIVITY;

unordered_map<int, queue<int>> access_time_pattern;	// stores time stamp queue for each unique line access
queue<int> access_pattern;	// stores the access pattern in a simple queue

int cache_set[MAX_CACHE_SIZE];	// simulate a single cache line
int cache_len;	// current number of lines in the cache

/* loads access pattern from the input file and parses
 * them to into two data structures: map between access
 * line and a time stamp queue for when the respective
 * line is accessed, and a simple queue for the access
 * pattern.
 */

int load_access_pattern(char* filename) {
	string line;
	ifstream myfile(filename);	// open input file
	if(myfile.is_open()) {
		int access_num = 0;	// time stamp counter

		while(getline(myfile, line)) {	// get next line number if possible
			int access_line_num = stoi(line);

			// push line in access pattern queue
			access_pattern.push(access_line_num);

			// push time stamp line into its respective queue in the map
			auto access_line = access_time_pattern.find(access_line_num);
			if(access_line == access_time_pattern.end()) {	// create new time stamp queue	
				queue<int> q;
				q.push(access_num);
				access_time_pattern[access_line_num] = q;
			} else {	// time stamp queue already exists
				(access_line->second).push(access_num);
			}
			access_num++;
		}

		myfile.close();
		return 1;
	} else {	// error in opening file
		return 0;
	}
}

/* return index of line_num in the 
 * cache (if it exists) */
int find_cache_line(int line_num) {
	for(int i = 0; i < ASSOCIATIVITY; i++) {
		if(cache_set[i] == line_num) {
			return i;
		}
	}
	return -1;
}

/* clear cache of all its contents */
void flush_cache() {
	cache_len = 0;
	for(int i = 0; i < ASSOCIATIVITY; i++) {
		cache_set[i] = -1;
	}
}

int queue_print(queue<int> q) {
	while(!q.empty()) {
		cout << q.front() << " ";
		q.pop();
	}
}

void print_cache_set() {
	for(int x = 0; x < ASSOCIATIVITY; x++) {
		int line = cache_set[x];
		(line == -1 ? cout << '-' : cout << line) << "\t";
	}
}

void print_access_time_pattern() {
	unordered_map<int, queue<int>> access_time_pattern_cpy(access_time_pattern);
	for (auto x : access_time_pattern_cpy) {
		cout << x.first << ": ";
		queue_print(x.second);
		cout << endl;
	}
	cout << endl;
}

/* run belady's algo (OPT) on single cache set */
void run_opt() {
	// initialize cache and other parameters
	int counter = 0;
	int cache_hit = 0;
	flush_cache();

	while(!access_pattern.empty()) {
		int demand_line = access_pattern.front();	// current line required to be accessed

		cout << counter << ")\t" << "(" << demand_line << ")\t";
		print_cache_set();
		//print_access_time_pattern();

		access_time_pattern[demand_line].pop();	// remove current time stamp for demand line

		if(find_cache_line(demand_line) < 0) {	// cache line miss
			cout << "\t(M)" << endl;
			if(cache_len != ASSOCIATIVITY) {	// cache set has empty space
				cache_set[cache_len] = demand_line;
				cache_len++;
			} else {	// cache set is full, evict line with longest rereference interval
				int longest_rereference_interval_index = 0;
				int longest_rereference_interval = -1;

				// scan through cache to search for line with longest rereference interval
				for(int index = 0; index < ASSOCIATIVITY; index++) {
					if(access_time_pattern[cache_set[index]].empty()) {	// line never accessed again, inf rereference interval
						longest_rereference_interval_index = index;
						longest_rereference_interval = INT_MAX;
					} else if(access_time_pattern[cache_set[index]].front() 
							> longest_rereference_interval) {	// found a line with longer reference interval
						longest_rereference_interval_index = index;
						longest_rereference_interval = access_time_pattern[cache_set[index]].front();
					}
				}
				
				// insert demand line
				cache_set[longest_rereference_interval_index] = demand_line;
			}
		} else {	// cache line hit
			cout << "\t(H)" << endl;
			cache_hit++;
		}

		access_pattern.pop();
		counter++;
	}

	// print hit ratio info
	cout << endl << "Cache Hit Ratio: " << cache_hit << "/" << counter << " = " << (double)cache_hit / counter * 100 << "%" << endl;
}

int main(int argc, char** argv) {
	load_access_pattern(argv[1]);
	string s(argv[2]);
	ASSOCIATIVITY = stoi(s);
	if(ASSOCIATIVITY > MAX_CACHE_SIZE || ASSOCIATIVITY < 1)
		cout << "Illegal Cache Set Size" << endl;
	else
		run_opt();

	return 0;
}

