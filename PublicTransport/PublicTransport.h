#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "Structs.h"
using namespace std;

class PublicTransport
{
public:

	static string file_name; //!< Default file

	//! Default constructor
	PublicTransport();
	//! Reads data from a text file
	PublicTransport(ifstream& file);

	//! Finds a bus stop's index, by given bus stop name
	int find_bus_stop(string name) const;
	//! Finds the fastest path by using modified Dijkstra algorithm
	void find_fastest_path() const;

	//! Adds 1 bus stop on front or on back of a currently existing bus line
	bool extend_line(int line, const string& name1, const string& name2);
	//! Sets a travelling time between 2 stops if the line route isn't a cycle
	void set_course_nonSpecial(int time1, int& time2, int stopper, int index, int line, int bs, list<Bus>::iterator& bus, const list<int>::iterator& it);
	//! Sets a travelling time between 2 stops if the line route is a cycle and the current stop is the last
	int set_course_special(int time1, int index, int line, list<Bus>::iterator& bus, const list<int>::iterator& it);
	//! Adds a new course to a currently existing bus line (adds another bus from the same line going through the line in other time)
	bool add_new_course(int line);
	//! Adds a new line
	bool add_new_line();

	//! Removes 1 bus stop
	void remove_bus_stop();
	//! Removes 1 course from a line
	bool remove_course(int line);
	//! Removes a whole line and all courses from that line
	bool remove_line(int line);

	//! Checks which lines can be removed to optimize the transport network
	void reduce_transport_lines(int** matrix) const;

	//! Loads data from a text file
	void load_data(ifstream& file);
	//! Saves data to a text file
	void save_data(ofstream& file) const;

	//! Shows bus stops and a list with the busses that go through the bus stop
	void print_bus_stops_schedule() const;
	//! Shows all courses from a selected line or all lines
	void print_courses() const;
	//! Shows the bus stops from a selected line or all lines
	void print_line() const;

	//! Type any symbol to continue the program execution	
	void type_to_continue() const;
	//! Executes user's order
	void users_option(int option);
	//! Main menu
	void menu();

private:	
	
	//! Creates the edges from a cyclic line
	bool set_busses_times_CyclicLine(pair<int, TransportLine>& p_tl);
	//! Creates edges from non-Cyclic line and fot the middle stops for a cyclic line
	bool set_busses_times_middle_stops(pair<int, TransportLine>& p_tl, list<int>::iterator& start_stop_it, bool cyclic);
	//! After loading the data from a text file - validates and creates the graph edges
	bool set_busses_times();
	
	//! Returns the first bus from a given line on given bus stop
	list<Bus>::iterator find_busses(int line, int bs);
	//! Returns invalid bus
	list<Bus>::iterator find_busses_end(int line, int bs);

	//! Shows how to reach from one stop to another 
	void print_path(int bs1, int bs2, const unordered_map<int, Path>& dist) const;
	//! Relaxing edges
	bool update_shortest_time(unordered_map<int, Path>& dist, priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>& pqueue, const Bus& bus, int current_stop) const;

	int** create_adj_matrix();
	//! Tries to add a bus stop to a line
	bool add_bus_stops_to_line(int stops_count, vector<string>& bs_names);	
	//! Creates the 1st course of a new line
	void create_first_course(int newline, const vector<string>& bs_names);
	//! Sets the time when a bus travells to (arrives at) a bus stop
	void set_course_time(queue<int>& arrival_time, int line, int bs1, int bs2, int count, list<Bus>::iterator& bus, const string& name1, const string& name2, bool new_start);
	//! Creates a new bus stop with a given name
	int create_new_bs(const string& name, int line);
	//! Calls "create_new_bs(...)" function and tries to set a course going through the new bus stop
	void create_bs_set_time(queue<int>& arrival_time, int line, int bs1, int bs2, int count, bool new_start, const string& name1, const string& name2);
	//! Adds a stop to a line
	void add_stop_to_line(queue<int>& arrival_time, int line, int bs1, int bs2, int count, const string& name1, const string& name2, bool new_start);
	

	//! When removing a bus stop from the middle of a line, it redirects the courses from the previous and the next bus stop
	void redirect_courses(int bs, int index);
	//! Removes a bus stop
	void delete_bs(int bs);
	//! Removes one course from a line
	void delete_course(int line, int count);
	//! Zeroes class members
	void clear();


	int bs_count = 0; //!< Bus stops count
	int lines_count = 0; //!< Lines count
	unordered_map<int, BusStop> bus_stops; //!< Vertices of the graph. The key is a bus stop number
	unordered_map<string, int> bs_index; //!< Key - bus stop name, value - bus stop number
	unordered_map<int, TransportLine> lines; //!< Keeps all bus lines. Key - line number
};

