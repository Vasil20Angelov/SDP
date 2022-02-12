#pragma once
#include <string>
#include <list>
using namespace std;

const int INF = 2 * 24 * 60;

struct TransportLine
{
	int line_number; //!< Line number
	int stops_count; //!< Number of stops that the line contains
	list<int> bus_stops; //!< List of all stops numbers that the line contains, sorted (list.front() is where the line begins, list.back() is where the line ends)
	TransportLine(int line_number = 0, int stops_count = 0) : line_number(line_number), stops_count(stops_count) {};
};

//! Graph Edges
struct Bus
{
	int number; //!< Bus number = on the line it is moving
	int destination; //!< Number of a bus stop, where it is headed
	int time_begin; //!< The time when a bus starts travelling toward the destionation stop
	int time_end; //!< The time when a bus arrives at the destination stop

	Bus(int n = -1, int destination = -1, int time_begin = -1, int time_end = -1) :
		number(n),
		destination(destination),
		time_begin(time_begin),
		time_end(time_end) {};
};

//! Graph Vertices
struct BusStop
{
	int number = -1; //!< Bus stop number, equals to bus stops vector's index
	string name; //!< Bus stop name
	int busses_count = 0; //!< The count of all busses going through the bus stop
	unordered_map<int, list<Bus>> busses; //!< Key pointing to a line and value all busses from this line on this bus stop
	BusStop() = default;
};

//! A struct used to show the shortest path between 2 bus stops
struct Path
{
	int time = INF; //!< The time when a bus arrives at any bus stop
	int from = -1; //!< The number of the previous bus stop
	int bus = -1; //!< Bus number
	int cntStops = 0; //!< The count of the previous bus stops before reaching the current stop
	bool visited = false;
	Path(int t = INF, int f = -1, int b = -1, int c = 0) : time(t), from(f), bus(b), cntStops(c), visited(false) {};
};